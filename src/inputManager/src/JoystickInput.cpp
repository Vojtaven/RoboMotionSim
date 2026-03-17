#include "JoystickInput.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>
JoystickInput::JoystickInput(const ControllerMapping& controllerMapping)
	: _controllerMapping(controllerMapping)
{
	updateAfterSettingsChange();
}
std::optional<std::string> JoystickInput::update(RobotState& state, const float maxSpeed, const float maxRotationSpeed) {
	const int id = _controllerMapping.controllerId;
	const bool currentlyConnected = sf::Joystick::isConnected(id);

	if(!currentlyConnected)
		return "Joystick disconnected";

	state.localVelocity.x = getInput(_controllerMapping.xAxisControl, maxSpeed);
	state.localVelocity.y = getInput(_controllerMapping.yAxisControl, maxSpeed);
	state.angularVelocity = getInput(_controllerMapping.chassisRotateControl, maxRotationSpeed);
	state.frontAngularVelocity = getInput(_controllerMapping.frontRotateControl, maxRotationSpeed);
	return std::nullopt;
}
float JoystickInput::getInput(const JoystickControll& control, const float maxValue) {
	float result = 0;
	const int id = _controllerMapping.controllerId;
	if (control.isAxis) {
		const float deadzone = _controllerMapping.deadzone;
		float rawValue = sf::Joystick::getAxisPosition(id, static_cast<sf::Joystick::Axis>(control.axisId));
		rawValue = (std::abs(rawValue) > deadzone) ? rawValue : 0; // Apply deadzone

		result = maxValue * (rawValue / 100.0f);
	}
	else {
		result = (sf::Joystick::isButtonPressed(id, control.buttonId1) ? maxValue : 0.0f) - (sf::Joystick::isButtonPressed(id, control.buttonId2) ? maxValue : 0.0f);
	}

	return result * (control.invert ? -1.0f : 1.0f);
}
