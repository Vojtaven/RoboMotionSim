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
	void HandleCommandStart();
	std::unique_ptr<Command> StackMotorCommands();
	void HandleHandshake(zmq::message_t& id, const MsgHeader& header);
	void HeartBeatCheck();
	void HandleCommand(const MsgHeader& header, const uint8_t* data, size_t size);
	void SentTelemetry(const RobotState& state);
	void SendResponse(MsgType type, uint32_t id, const std::vector<uint8_t>& payload = {});
	void SendMotorCount(uint32_t id,uint16_t motorCount);
	void SendHandshakeAck(uint32_t id);
	void SendHeartbeatAck(uint32_t id);
	void ClearCommandQueue();
	void HandleDisconnect(uint32_t id);
	void DisconnectClient();
	void SendDisconnectAck(uint32_t id);                                              
	void SendCommandAck(uint32_t id) { SendResponse(MsgType::CMD_ACK, id); }
	void SendCommandError(uint32_t id) { SendResponse(MsgType::CMD_ERROR, id); }
	void SendCommandComplete(uint32_t id) { SendResponse(MsgType::CMD_COMPLETE, id); }


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
    zmq::socket_t _telemetry_out;  // high freq, conflate=1
    zmq::socket_t _command_router;   // commands in/responses out, router
};

#endif // !IPC_INPUT_HPP