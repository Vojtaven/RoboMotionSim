#include "IPC/IPCInput.hpp"
#include "IPC/CommandParameters.hpp"
#include <zmq.hpp>
#include <iostream>
#include <cstring>


IPCInput::IPCInput(const IPCMapping& ipcMapping)
	: _ipcMapping(ipcMapping),
	_context(1),
	_telemetry_out(_context, zmq::socket_type::pub),
	_command_router(_context, zmq::socket_type::router)
{
	updateAfterSettingsChange();
}
void IPCInput::update(RobotState& state) {
	if (_motorCount < 0 || _motorCount != state.wheelCount) {
		_motorCount = state.wheelCount;
		if (_connectedClientID.has_value())
			SendMotorCount(_outID++, _motorCount);
	}

	// Reset motor state each frame - active commands will set values during execute
	state.fromWheelSpeeds = false;
	for (auto& wheel : state.wheels)
		wheel.speed = 0;

	// Poll for incoming messages (non-blocking)
	zmq::message_t id, msg;
	while (_command_router.recv(id, zmq::recv_flags::dontwait)) {
		if (!_command_router.recv(msg, zmq::recv_flags::dontwait))
			continue;

		if (msg.size() < sizeof(MsgHeader))
			continue;

		const uint8_t* raw = static_cast<const uint8_t*>(msg.data());
		MsgHeader header;
		std::memcpy(&header, raw, sizeof(MsgHeader));
		const uint8_t* payload = raw + sizeof(MsgHeader);
		size_t payloadSize = msg.size() - sizeof(MsgHeader);

		if (header.type == MsgType::HANDSHAKE)
		{
			HandleHandshake(id, header);
			continue;
		}

		if (!_connectedClientID.has_value() || id != _connectedClientID)
			continue; // Ignore messages from non-connected clients

		switch (header.type) {
		case MsgType::HEARTBEAT:
			_lastHeartbeatReceived = clock.now();
			SendHeartbeatAck(header.id);
			break;
		case MsgType::COMMAND:
			HandleCommand(header, payload, payloadSize);
			break;
		case MsgType::DISCONNECT:
			HandleDisconnect(header.id);
			break;
		case MsgType::CLEAR_COMMAND_QUEUE:
			ClearCommandQueue();
			_currentCommand = nullptr;
			break;
		default:
			break;
		}
		_lastID = header.id;
	}

	if (!_connectedClientID.has_value())
		return;

	HeartBeatCheck();

	// Start next command from queue if no current command
	HandleCommandStart();

	// Execute and update current command
	if (_currentCommand) {
		_currentCommand->execute(state);
	}

	// Send telemetry
	SentTelemetry(state);
}

void IPCInput::checkForInputCompletion(const RobotState& state, const float dt) {
	if (_currentCommand && _currentCommand->updateAndCheckCompletion(state, dt)) {
		if (auto* wrapper = dynamic_cast<MotorCommandWrapper*>(_currentCommand.get()))
		{
			wrapper->checkInnerCommandCompletion([this](uint32_t cmdId) {
				SendCommandComplete(cmdId);
				});
			if (wrapper->isMoveCompleted())
				_currentCommand = nullptr;
		}
		else {
			SendCommandComplete(_currentCommand->getId());
			_currentCommand = nullptr;
		}
	}
}

void IPCInput::updateAfterSettingsChange() {
	// Close existing sockets before rebinding/reconnecting
	_telemetry_out.close();
	_command_router.close();

	// Recreate sockets
	_telemetry_out = zmq::socket_t(_context, zmq::socket_type::pub);
	_command_router = zmq::socket_t(_context, zmq::socket_type::router);

	_telemetry_out.set(zmq::sockopt::conflate, 1);
	_telemetry_out.set(zmq::sockopt::sndhwm, 1);
	_telemetry_out.set(zmq::sockopt::linger, 0);

	_telemetry_out.bind(_ipcMapping.address + ":" + std::to_string(_ipcMapping.telemetry_port));
	_command_router.bind(_ipcMapping.address + ":" + std::to_string(_ipcMapping.command_port));
}


void IPCInput::HandleCommandStart() {
	if (_commandQueue.empty())
		return;

	if (!_currentCommand) {
		if (dynamic_cast<RawMotorCommand*>(_commandQueue.front().get()))
		{
			_currentCommand = StackMotorCommands();
		}
		else {
			auto temp = std::move(_commandQueue.front());
			_commandQueue.pop();
			_currentCommand = std::move(temp);
		}
	}

}

std::unique_ptr<Command> IPCInput::StackMotorCommands() {
	auto wrapper = std::make_unique<MotorCommandWrapper>(_motorCount);
	while (!_commandQueue.empty() && dynamic_cast<RawMotorCommand*>(_commandQueue.front().get()))
	{
		auto nextRawMotor = dynamic_cast<RawMotorCommand*>(_commandQueue.front().get());
		wrapper->addMotorCommand(nextRawMotor->motor_id, std::move(_commandQueue.front()));
		_commandQueue.pop();
	}

	return wrapper;
}


void IPCInput::HandleHandshake(zmq::message_t& id, const MsgHeader& header) {
	if (_connectedClientID.has_value())
		return; // Ignore handshake if already complete

	_lastHeartbeatReceived = clock.now();
	_connectedClientID = std::move(id);
	SendHandshakeAck(header.id);
	_lastID = header.id;
	SendMotorCount(_outID++, _motorCount);
}
void IPCInput::HeartBeatCheck() {
	if (_lastHeartbeatReceived + std::chrono::milliseconds(static_cast<int>(_ipcMapping.heartbeatTimeout * 1000)) < clock.now()) {
		// Heartbeat timeout, consider connection lost
		std::cout << "Heartbeat timeout, disconnecting client" << std::endl;
		ClearCommandQueue();
		_currentCommand = nullptr;
		_connectedClientID.reset();
		_motorCount = -1;
	}
}
void IPCInput::HandleCommand(const MsgHeader& header, const uint8_t* data, size_t size) {
	try {
		CommandType cmdType = Command::getCommandType(data, size);
		const uint8_t* cmdData = data + sizeof(CommandType);
		size_t cmdSize = size - sizeof(CommandType);

		// STOP: skip queue, cancel current command immediately
		if (cmdType == CommandType::STOP) {
			_currentCommand = nullptr;
			SendCommandAck(header.id);
			SendCommandComplete(header.id);
			return;
		}

		// STOP_MOTOR: skip queue, stop specific motor in active wrapper
		if (cmdType == CommandType::STOP_MOTOR) {
			auto params = CommandParameters::ParseParams<StopMotorParams>(cmdData, cmdSize);
			auto* wrapper = dynamic_cast<MotorCommandWrapper*>(_currentCommand.get());
			if (wrapper)
				wrapper->removeCommand(params.motor_id);
			SendCommandAck(header.id);
			SendCommandComplete(header.id);
			return;
		}

		// Regular command: create and queue
		auto cmd = Command::Create(header.id, cmdType, cmdData, cmdSize);
		SendCommandAck(header.id);
		_commandQueue.push(std::move(cmd));
	}
	catch (const std::exception&) {
		SendCommandError(header.id);
	}
}



void IPCInput::SentTelemetry(const RobotState& state) {
	uint8_t wheelCount = state.wheelCount;
	size_t payload_size = sizeof(TelemetryOdometry)
		+ sizeof(TelemetryWheelState) * wheelCount;

	_telemetryBuf.resize(sizeof(MsgHeader) + payload_size);
	uint8_t* ptr = _telemetryBuf.data();

	// Header
	MsgHeader header;
	header.id = _outID++;
	header.type = MsgType::TELEMETRY;
	header.payload_size = static_cast<uint16_t>(payload_size);
	std::memcpy(ptr, &header, sizeof(MsgHeader));
	ptr += sizeof(MsgHeader);

	// Fixed part
	TelemetryOdometry odo{};
	odo.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		clock.now().time_since_epoch()).count();
	odo.distanceTraveledX = state.distanceTraveled.x;
	odo.distanceTraveledY = state.distanceTraveled.y;
	odo.localVelocityX = state.localVelocity.x;
	odo.localVelocityY = state.localVelocity.y;
	odo.frontAngle = state.frontAngle;
	odo.chassisAngle = state.chassisAngle;
	odo.chassisAngularVelocity = state.angularVelocity;
	odo.wheelCount = wheelCount;
	std::memcpy(ptr, &odo, sizeof(TelemetryOdometry));
	ptr += sizeof(TelemetryOdometry);

	// Variable part - wheels one by one
	for (int i = 0; i < wheelCount; i++)
	{
		TelemetryWheelState ws;
		ws.speed = state.wheels[i].speed;
		ws.distanceTraveled = state.wheels[i].distanceTraveled;
		std::memcpy(ptr, &ws, sizeof(TelemetryWheelState));
		ptr += sizeof(TelemetryWheelState);
	}

	_telemetry_out.send(zmq::buffer(_telemetryBuf), zmq::send_flags::dontwait);
}
void IPCInput::SendResponse(MsgType type, uint32_t id, const std::vector<uint8_t>& payload) {
	if (!_connectedClientID.has_value())
		return;

	std::vector<uint8_t> buf(sizeof(MsgHeader) + payload.size());
	uint8_t* ptr = buf.data();

	// Header
	MsgHeader header;
	header.id = id;
	header.type = type;
	header.payload_size = static_cast<uint16_t>(payload.size());
	std::memcpy(ptr, &header, sizeof(MsgHeader));
	ptr += sizeof(MsgHeader);

	// Payload
	if (!payload.empty()) {
		std::memcpy(ptr, payload.data(), payload.size());
	}

	zmq::message_t id_copy;
	id_copy.copy(_connectedClientID.value());
	_command_router.send(id_copy, zmq::send_flags::sndmore);
	_command_router.send(zmq::buffer(buf), zmq::send_flags::none);
}
void IPCInput::SendMotorCount(uint32_t id, uint16_t motorCount) {
	SendResponse(MsgType::MOTOR_COUNT, id, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&motorCount), reinterpret_cast<uint8_t*>(&motorCount) + sizeof(uint16_t)));
}

void IPCInput::SendHandshakeAck(uint32_t id) { SendResponse(MsgType::HANDSHAKE_ACK, id); }
void IPCInput::SendHeartbeatAck(uint32_t id) { SendResponse(MsgType::HEARTBEAT_ACK, id); }
void IPCInput::ClearCommandQueue() {
	_commandQueue = std::queue<std::unique_ptr<Command>>();
}
void IPCInput::DisconnectClient() {
	SendResponse(MsgType::DISCONNECT, _outID++);
}

void IPCInput::HandleDisconnect(uint32_t id) {
	std::cout << "Disconnecting client" << std::endl;
	ClearCommandQueue();
	_currentCommand = nullptr;
	SendDisconnectAck(id);
	_connectedClientID.reset();
	_motorCount = -1;
}
void IPCInput::SendDisconnectAck(uint32_t id) { SendResponse(MsgType::DISCONNECT_ACK, id); }