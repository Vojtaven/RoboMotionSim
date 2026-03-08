#include "IPCInput.hpp"
#include <zmq.hpp>
#include <iostream>
IPCInput::IPCInput()
	: _ctx(1), _socket(_ctx, zmq::socket_type::pull)
{

	_socket.bind("tcp://127.0.0.1:5555");
	std::cout << "Receiver listening on port 5555...\n";

}
void IPCInput::update(RobotState& state, const  float maxSpeed, const float maxRotationSpeed) {
	zmq::message_t msg;
	auto result = _socket.recv(msg, zmq::recv_flags::dontwait);

	if (result)
		std::cout << "Received: " << msg.to_string() << "\n";

}
void IPCInput::updateAfterSettingsChange() {

}
