#ifndef IPC_INPUT_HPP
#define IPC_INPUT_HPP
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include <zmq.hpp>
#include "Protocol.hpp"
#include "Commands.hpp"
#include <queue>
#include <chrono>
#include <optional>
class IPCInput {
public:
	IPCInput(const IPCMapping& ipcMapping);
	void update(RobotState& state);
	void checkForInputCompletion(const RobotState& state, const float dt);
	void updateAfterSettingsChange();

private:
	void handleCommandStart();
	std::unique_ptr<Command> stackMotorCommands();
	void handleHandshake(zmq::message_t& id, const MsgHeader& header);
	void heartbeatCheck();
	void handleCommand(const MsgHeader& header, const uint8_t* data, size_t size);
	void sendTelemetry(const RobotState& state);
	void sendResponse(MsgType type, uint32_t id, const std::vector<uint8_t>& payload = {});
	void sendMotorCount(uint32_t id,uint16_t motorCount);
	void sendHandshakeAck(uint32_t id);
	void sendHeartbeatAck(uint32_t id);
	void clearCommandQueue();
	void handleDisconnect(uint32_t id);
	void disconnectClient();
	void sendDisconnectAck(uint32_t id);
	void sendCommandAck(uint32_t id) { sendResponse(MsgType::CMD_ACK, id); }
	void sendCommandError(uint32_t id) { sendResponse(MsgType::CMD_ERROR, id); }
	void sendCommandComplete(uint32_t id) { sendResponse(MsgType::CMD_COMPLETE, id); }


	std::chrono::steady_clock clock;
	std::chrono::steady_clock::time_point _lastHeartbeatReceived;
	const IPCMapping& _ipcMapping;
	zmq::context_t _context;
	uint32_t _lastID = 0;
	uint32_t _outID = 0;
	std::optional<zmq::message_t> _connectedClientID;
	int _motorCount =-1;
	std::queue<std::unique_ptr<Command>> _commandQueue;
	std::unique_ptr<Command> _currentCommand = nullptr;
    std::vector<uint8_t> _telemetryBuf;
    zmq::socket_t _telemetry_out;  // high freq, conflate=1
    zmq::socket_t _command_router;   // commands in/responses out, router
};

#endif // !IPC_INPUT_HPP