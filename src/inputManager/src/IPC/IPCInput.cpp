// Author: Vojtech Venzara
// Date: 2026-04-03
// Description: Inter-process communication input handler for receiving commands from external applications

#include "IPC/IPCInput.hpp"
#include "IPC/CommandParameters.hpp"
#include <zmq.hpp>
#include <iostream>
#include <cstring>


IPCInput::IPCInput(ConfigSection<IPCMapping>& ipcMapping)
	: _ipcMapping(ipcMapping.get()),
	_context(1),
	_telemetry_out(_context, zmq::socket_type::pub),
	_command_router(_context, zmq::socket_type::router)
{
	_subscription = ipcMapping.scopedSubscribe([this](const IPCMapping&) {
		updateAfterSettingsChange();
	});
	updateAfterSettingsChange();
}
std::optional<std::string> IPCInput::update(RobotState& state) {
	if( _errorMessage.has_value())
		return _errorMessage;

	if (_motorCount < 0 || _motorCount != state.wheelCount) {
		_motorCount = state.wheelCount;
		if (_connectedClientID.has_value())
			sendMotorCount(_outID++, _motorCount);
	}

	// Reset motor state each frame - active commands will set values during execute
	state.fromWheelSpeeds = false;

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
			handleHandshake(id, header);
			continue;
		}

		if (!_connectedClientID.has_value() || id != _connectedClientID)
			continue; // Ignore messages from non-connected clients

		switch (header.type) {
		case MsgType::HEARTBEAT:
			_lastHeartbeatReceived = clock.now();
			sendHeartbeatAck(header.id);
			break;
		case MsgType::COMMAND:
			handleCommand(header, payload, payloadSize);
			break;
		case MsgType::DISCONNECT:
			handleDisconnect(header.id);
			break;
		case MsgType::CLEAR_COMMAND_QUEUE:
			clearCommandQueue();
			_currentCommand = nullptr;
			break;
		default:
			break;
		}
		_lastID = header.id;
	}

	if (!_connectedClientID.has_value())
		return "No client connected";

	heartbeatCheck();

	// Start next command from queue if no current command
	handleCommandStart();

	// Execute and update current command
	if (_currentCommand) {
		_currentCommand->execute(state);
	}

	// Send telemetry
	sendTelemetry(state);

	return _errorMessage;
}

void IPCInput::checkForInputCompletion(const RobotState& state, const double dt) {
	if (_currentCommand && _currentCommand->updateAndCheckCompletion(state, dt)) {
		if (auto* wrapper = dynamic_cast<MotorCommandWrapper*>(_currentCommand.get()))
		{
			wrapper->checkInnerCommandCompletion([this](uint32_t cmdId) {
				sendCommandComplete(cmdId);
				});
			if (wrapper->isMoveCompleted())
				_currentCommand = nullptr;
		}
		else {
			sendCommandComplete(_currentCommand->getId());
			_currentCommand = nullptr;
		}
	}
}

bool IPCInput::validateConfig() const {
	const auto& addr = _ipcMapping.address;
	if (addr.empty())
		return false;

	// Must use a supported ZMQ transport
	if (addr.find("tcp://") != 0 && addr.find("ipc://") != 0)
		return false;

	// Validate TCP host portion (e.g. "tcp://127.0.0.1")
	if (addr.find("tcp://") == 0) {
		std::string host = addr.substr(6);
		if (host.empty())
			return false;

		// Reject whitespace or obviously invalid characters
		for (char c : host) {
			if (std::isspace(static_cast<unsigned char>(c)) || c == ':' || c == '/')
				return false;
		}

		// If it looks like an IP (starts with digit), validate the octets
		if (std::isdigit(static_cast<unsigned char>(host[0]))) {
			int octets = 0;
			size_t start = 0;
			for (size_t i = 0; i <= host.size(); i++) {
				if (i == host.size() || host[i] == '.') {
					if (i == start)
						return false; // empty segment
					std::string segment = host.substr(start, i - start);
					for (char c : segment) {
						if (!std::isdigit(static_cast<unsigned char>(c)))
							return false;
					}
					int val = std::stoi(segment);
					if (val < 0 || val > 255)
						return false;
					octets++;
					start = i + 1;
				}
			}
			if (octets != 4)
				return false;
		}
	}

	// Validate ports
	if (_ipcMapping.command_port < 1024 || _ipcMapping.telemetry_port < 1024)
		return false;
	if (_ipcMapping.command_port == _ipcMapping.telemetry_port)
		return false;

	return true;
}

void IPCInput::updateAfterSettingsChange() {
	_errorMessage.reset();
	if (!validateConfig()) {
		_errorMessage = "Invalid IPC configuration: check address scheme and port values";
		return;
	}
	try{
	// Close existing sockets before rebinding/reconnecting
	_telemetry_out.close();
	_command_router.close();

	// Recreate sockets
	_telemetry_out = zmq::socket_t(_context, zmq::socket_type::pub);
	_command_router = zmq::socket_t(_context, zmq::socket_type::router);

	_telemetry_out.set(zmq::sockopt::conflate, 1); // only keep the latest telemetry message
	_telemetry_out.set(zmq::sockopt::sndhwm, 1);
	_telemetry_out.set(zmq::sockopt::linger, 0);

	_telemetry_out.bind(_ipcMapping.address + ":" + std::to_string(_ipcMapping.telemetry_port));
	_command_router.bind(_ipcMapping.address + ":" + std::to_string(_ipcMapping.command_port));
	}
	catch (const zmq::error_t& e) {
		_errorMessage = std::string("ZeroMQ error during socket setup: ") + e.what();
		std::cerr << _errorMessage.value() << std::endl;
	}
}


void IPCInput::handleCommandStart() {
	if (_commandQueue.empty())
		return;

	if (!_currentCommand) {
		if (dynamic_cast<RawMotorCommand*>(_commandQueue.front().get()))
		{
			_currentCommand = stackMotorCommands();
		}
		else {
			auto temp = std::move(_commandQueue.front());
			_commandQueue.pop();
			_currentCommand = std::move(temp);
		}
	}

}

// Merge consecutive per-motor commands into a single wrapper so they execute in parallel
std::unique_ptr<Command> IPCInput::stackMotorCommands() {
	auto wrapper = std::make_unique<MotorCommandWrapper>(_motorCount);
	while (!_commandQueue.empty() && dynamic_cast<RawMotorCommand*>(_commandQueue.front().get()))
	{
		auto nextRawMotor = dynamic_cast<RawMotorCommand*>(_commandQueue.front().get());
		wrapper->addMotorCommand(nextRawMotor->motor_id, std::move(_commandQueue.front()));
		_commandQueue.pop();
	}

	return wrapper;
}


void IPCInput::handleHandshake(zmq::message_t& id, const MsgHeader& header) {
	if (_connectedClientID.has_value())
		return; // Ignore handshake if already complete

	_lastHeartbeatReceived = clock.now();
	_connectedClientID = std::move(id);
	sendHandshakeAck(header.id);
	_lastID = header.id;
	sendMotorCount(_outID++, _motorCount);
}
void IPCInput::heartbeatCheck() {
	if (_lastHeartbeatReceived + std::chrono::milliseconds(static_cast<int>(_ipcMapping.heartbeatTimeout * 1000)) < clock.now()) {
		// Heartbeat timeout, consider connection lost
		std::cout << "Heartbeat timeout, disconnecting client" << std::endl;
		clearCommandQueue();
		_currentCommand = nullptr;
		_connectedClientID.reset();
		_motorCount = -1;
	}
}
void IPCInput::handleCommand(const MsgHeader& header, const uint8_t* data, size_t size) {
	try {
		CommandType cmdType = Command::getCommandType(data, size);
		const uint8_t* cmdData = data + sizeof(CommandType);
		size_t cmdSize = size - sizeof(CommandType);

		// STOP: skip queue, cancel current command immediately
		if (cmdType == CommandType::STOP) {
			_currentCommand = nullptr;
			sendCommandAck(header.id);
			sendCommandComplete(header.id);
			return;
		}

		// STOP_MOTOR: skip queue, stop specific motor in active wrapper
		if (cmdType == CommandType::STOP_MOTOR) {
			auto params = CommandParameters::parseParams<StopMotorParams>(cmdData, cmdSize);
			if (params.motor_id >= _motorCount)
				throw std::runtime_error("Motor ID out of range");
			auto* wrapper = dynamic_cast<MotorCommandWrapper*>(_currentCommand.get());
			if (wrapper)
				wrapper->removeCommand(params.motor_id);
			sendCommandAck(header.id);
			sendCommandComplete(header.id);
			return;
		}

		// Regular command: create, validate and queue
		auto cmd = Command::create(header.id, cmdType, cmdData, cmdSize);
		cmd->validate(_motorCount);
		sendCommandAck(header.id);
		_commandQueue.push(std::move(cmd));
	}
	catch (const std::exception&) {
		sendCommandError(header.id);
	}
}



void IPCInput::sendTelemetry(const RobotState& state) {
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
	odo.distanceTraveledX = static_cast<float>(state.distanceTraveled.x);
	odo.distanceTraveledY = static_cast<float>(state.distanceTraveled.y);
	odo.localVelocityX = static_cast<float>(state.localVelocity.x);
	odo.localVelocityY = static_cast<float>(state.localVelocity.y);
	odo.frontAngle = static_cast<float>(state.frontAngle);
	odo.chassisAngle = static_cast<float>(state.chassisAngle);
	odo.chassisAngularVelocity = static_cast<float>(state.angularVelocity);
	odo.wheelCount = wheelCount;
	std::memcpy(ptr, &odo, sizeof(TelemetryOdometry));
	ptr += sizeof(TelemetryOdometry);

	// Variable part - wheels one by one
	for (int i = 0; i < wheelCount; i++)
	{
		TelemetryWheelState ws;
		ws.speed = state.wheels[i].speed;
		ws.distanceTraveled = static_cast<float>(state.wheels[i].distanceTraveled);
		std::memcpy(ptr, &ws, sizeof(TelemetryWheelState));
		ptr += sizeof(TelemetryWheelState);
	}

	_telemetry_out.send(zmq::buffer(_telemetryBuf), zmq::send_flags::dontwait);
}
void IPCInput::sendResponse(MsgType type, uint32_t id, const std::vector<uint8_t>& payload) {
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
void IPCInput::sendMotorCount(uint32_t id, uint16_t motorCount) {
	sendResponse(MsgType::MOTOR_COUNT, id, std::vector<uint8_t>(reinterpret_cast<uint8_t*>(&motorCount), reinterpret_cast<uint8_t*>(&motorCount) + sizeof(uint16_t)));
}

void IPCInput::sendHandshakeAck(uint32_t id) { sendResponse(MsgType::HANDSHAKE_ACK, id); }
void IPCInput::sendHeartbeatAck(uint32_t id) { sendResponse(MsgType::HEARTBEAT_ACK, id); }
void IPCInput::clearCommandQueue() {
	_commandQueue = std::queue<std::unique_ptr<Command>>();
}
void IPCInput::disconnectClient() {
	sendResponse(MsgType::DISCONNECT, _outID++);
}

void IPCInput::handleDisconnect(uint32_t id) {
	std::cout << "Disconnecting client" << std::endl;
	clearCommandQueue();
	_currentCommand = nullptr;
	sendDisconnectAck(id);
	_connectedClientID.reset();
	_motorCount = -1;
}
void IPCInput::sendDisconnectAck(uint32_t id) { sendResponse(MsgType::DISCONNECT_ACK, id); }
