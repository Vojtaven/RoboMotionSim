// Author: Vojtech Venzara
// Date: 2026-04-02
// Description: Input handler for gamepad/joystick input with support for multiple axes and button mapping

#ifndef JOYSTICK_INPUT_HPP
#define JOYSTICK_INPUT_HPP
#include "AppConfig.hpp"
#include "ConfigSection.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>
#include <optional>
#include <string>

class JoystickInput {
public:
	JoystickInput(ConfigSection<ControllerMapping>& controllerMapping);
	// Returns error string if controller is disconnected
	std::optional<std::string> update(RobotState& state, const float maxSpeed, const float maxRotationSpeed);
private:
	float getInput(const JoystickControl& control, const float maxValue);
	const ControllerMapping& _controllerMapping;
};

#endif // !JOYSTICK_INPUT_HPP
