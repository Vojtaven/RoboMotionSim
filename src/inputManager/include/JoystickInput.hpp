#ifndef JOYSTICK_INPUT_HPP
#define JOYSTICK_INPUT_HPP
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>

class JoystickInput {
public:
	JoystickInput(const ControllerMapping& controllerMapping);
	void update(RobotState& state, const  float maxSpeed, const float maxRotationSpeed);
	void updateAfterSettingsChange() {}
private:
	float getInput(const JoystickControll& control, const float maxValue);
	const ControllerMapping& _controllerMapping;
};

#endif // !JOYSTICK_INPUT_HPP