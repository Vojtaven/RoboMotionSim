#include "IPC/IPCInput.hpp"
#include <zmq.hpp>
#include <iostream>


IPCInput::IPCInput(const IPCMapping& ipcMapping)
	: _ipcMapping(ipcMapping),
	_context(1), 
	_telemetry_out(_context, zmq::socket_type::pub),
	_response_out(_context, zmq::socket_type::pub),
	_command_in(_context, zmq::socket_type::sub)
{
	updateAfterSettingsChange();
}
void IPCInput::update(RobotState& state) {
	if (_motorCount < 0 || _motorCount != state.wheelCount) {
		_motorCount = state.wheelCount;

	}


}
void IPCInput::updateAfterSettingsChange() {
	// Close existing sockets before rebinding/reconnecting
	_telemetry_out.close();
	_response_out.close();
	_command_in.close();

	// Recreate sockets
	_telemetry_out = zmq::socket_t(_context, zmq::socket_type::pub);
	_response_out = zmq::socket_t(_context, zmq::socket_type::pub);
	_command_in = zmq::socket_t(_context, zmq::socket_type::sub);

	// Bind/connect to new addresses/ports
	_telemetry_out.bind(_ipcMapping.address + ":" + std::to_string(_ipcMapping.telemetry_port));
	_telemetry_out.set(zmq::sockopt::conflate, 1);
	_response_out.bind(_ipcMapping.address + ":" + std::to_string(_ipcMapping.response_port));
	_command_in.connect(_ipcMapping.address + ":" + std::to_string(_ipcMapping.command_port));
}



void IPCInput::HandleHandshake(const MsgHeader& header) {
	if(_handshakeComplete)
		return; // Ignore handshake if already complete

	_lastHeartbeatReceived = clock.now();
	_handshakeComplete = true;
	SendHandshakeAck(header.id);
	_lastID = header.id;
}
void IPCInput::HeartBeatCheck() {
	if (_lastHeartbeatReceived + std::chrono::milliseconds(static_cast<int>(_ipcMapping.heartbeatTimeout * 1000)) < clock.now()) {
		// Heartbeat timeout, consider connection lost
		std::cout << "Heartbeat timeout, disconnecting client" << std::endl;
		ClearCommandQueue();
		_handshakeComplete = false;
		_motorCount = -1;
	}
}
void IPCInput::HandleCommand(const MsgHeader& header, const uint8_t* data, size_t size) {
	//Command command = Command::Create(header, data, size);
	//_commandQueue.push(command);
}

void IPCInput::SentTelemetry(const RobotState& state) {
	uint8_t wheelCount = state.wheelCount;
	size_t payload_size = sizeof(TelemetryOdometry)
		+ sizeof(TelemetryWheelState) * wheelCount;

	std::vector<uint8_t> buf(sizeof(MsgHeader) + payload_size);
	uint8_t* ptr = buf.data();

	// Header
	MsgHeader header;
	header.id = _outID++;
	header.type = MsgType::TELEMETRY;
	header.payload_size = payload_size;
	std::memcpy(ptr, &header, sizeof(MsgHeader));
	ptr += sizeof(MsgHeader);

	// Fixed part
	TelemetryOdometry odo;
	odo.distanceTraveledX = state.distanceTraveled.x;
	odo.distanceTraveledY = state.distanceTraveled.y;
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

	_telemetry_out.send(zmq::buffer(buf), zmq::send_flags::none);
}
void IPCInput::SendResponse(MsgType type, uint32_t id, const std::vector<uint8_t>& payload) {
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

	_response_out.send(zmq::buffer(buf), zmq::send_flags::none);
}
void IPCInput::SendMotorCount(uint32_t id,uint32_t motorCount) {
	MsgHeader header;
	header.id = id;
	header.type = MsgType::MOTOR_COUNT;
	header.payload_size = sizeof(uint32_t);

	std::vector<uint8_t> buf(sizeof(MsgHeader) + sizeof(uint32_t));
	uint8_t* ptr = buf.data();

	std::memcpy(ptr, &header, sizeof(MsgHeader));
	ptr += sizeof(MsgHeader);

	std::memcpy(ptr, &motorCount, sizeof(uint32_t));

	_response_out.send(zmq::buffer(buf), zmq::send_flags::none);
}

void IPCInput::SendHandshakeAck(uint32_t id) {SendResponse(MsgType::HANDSHAKE_ACK, id); }
void IPCInput::SendHeartbeatAck(uint32_t id) {SendResponse(MsgType::HEARTBEAT_ACK, id); }
void IPCInput::ClearCommandQueue() {
	_commandQueue = std::queue<std::unique_ptr<Command>>();
}
void IPCInput::DisconnectClient() {
	SendResponse(MsgType::DISCONNECT, _outID++);
}

void IPCInput::HandleDisconnect(uint32_t id) {
	std::cout << "Disconnecting client" << std::endl;
	ClearCommandQueue();
	_handshakeComplete = false;
	_motorCount = -1;
	SendDisconnectAck(id);
}
void IPCInput::SendDisconnectAck(uint32_t id) {SendResponse(MsgType::DISCONNECT_ACK, id); }