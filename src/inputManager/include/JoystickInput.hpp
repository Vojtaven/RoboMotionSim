#ifndef JOYSTICK_INPUT_HPP
#define JOYSTICK_INPUT_HPP
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>
#include <optional>
#include <string>

class JoystickInput {
public:
	JoystickInput(const ControllerMapping& controllerMapping);
	std::optional<std::string> update(RobotState& state, const float maxSpeed, const float maxRotationSpeed);
	void updateAfterSettingsChange() {}
private:
	float getInput(const JoystickControl& control, const float maxValue);
	const ControllerMapping& _controllerMapping;
};

#endif // !JOYSTICK_INPUT_HPP