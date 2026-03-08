#ifndef IPC_INPUT_HPP
#define IPC_INPUT_HPP
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>
#include <zmq.hpp>


class IPCInput {
public:
	IPCInput();
	void update(RobotState& state, const  float maxSpeed, const float maxRotationSpeed);
	void updateAfterSettingsChange();

private:
	zmq::context_t _ctx;
	zmq::socket_t  _socket;
};

#endif // !IPC_INPUT_HPP