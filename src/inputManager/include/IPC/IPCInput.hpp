#ifndef IPC_INPUT_HPP
#define IPC_INPUT_HPP
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>
#include <zmq.hpp>
#include "Protocol.hpp"
#include "Commands.hpp"
#include <queue>
#include <chrono>

class IPCInput {
public:
	IPCInput(const IPCMapping& ipcMapping);
	void update(RobotState& state);
	void updateAfterSettingsChange();

private:
	void HandleHandshake(const MsgHeader& header);
	void HeartBeatCheck();
	void HandleCommand(const MsgHeader& header, const uint8_t* data, size_t size);
	void SentTelemetry(const RobotState& state);
	void SendResponse(MsgType type, uint32_t id, const std::vector<uint8_t>& payload = {});
	void SendMotorCount(uint32_t id,uint32_t motorCount);
	void SendHandshakeAck(uint32_t id);
	void SendHeartbeatAck(uint32_t id);
	void ClearCommandQueue();
	void HandleDisconnect(uint32_t id);
	void DisconnectClient();
	void SendDisconnectAck(uint32_t id);

	std::chrono::steady_clock clock;
	std::chrono::steady_clock::time_point _lastHeartbeatReceived;
	const IPCMapping& _ipcMapping;
	zmq::context_t _context;
	uint32_t _lastID = 0;
	uint32_t _outID = 0;
	bool _handshakeComplete = false;
	int _motorCount =-1;
	std::queue<Command> _commandQueue;
	Command _currentCommand = Command{};
    zmq::socket_t _telemetry_out;  // high freq, conflate=1
    zmq::socket_t _response_out;  // low freq, no conflate
    zmq::socket_t _command_in;   // commands in
};

#endif // !IPC_INPUT_HPP