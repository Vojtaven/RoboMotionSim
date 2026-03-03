#ifndef JOYSTICK_INPUT_HPP
#define JOYSTICK_INPUT_HPP
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>

class JoystickInput {
public:
	JoystickInput(const ControllerMapping& controllerMapping);
	void update(RobotState& state, const  float maxSpeed, const float maxRotationSpeed);
	void updateAfterSettingsChange();
private:
	const ControllerMapping& _controllerMapping;
	sf::Joystick::Axis _moveXAxis;
	sf::Joystick::Axis _moveYAxis;
	sf::Joystick::Axis _chassisRotateAxis;
};

#endif // !JOYSTICK_INPUT_HPP