#include "JoystickInput.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Joystick.hpp>

JoystickInput::JoystickInput(const ControllerMapping& controllerMapping)
	: _controllerMapping(controllerMapping)
{
	updateAfterSettingsChange();
}
void JoystickInput::update(RobotState& state, const float maxSpeed, const float maxRotationSpeed) {
	const int id = _controllerMapping.controllerId;
	const float deadzone = _controllerMapping.deadzone;
	if (!sf::Joystick::isConnected(id))
	{
		throw std::runtime_error("Joystick not connected");
	}

	sf::Joystick::update();

	float x = sf::Joystick::getAxisPosition(id, _moveXAxis);
	float y = sf::Joystick::getAxisPosition(id, _moveYAxis);
	float rotation = sf::Joystick::getAxisPosition(id, _chassisRotateAxis);
	bool rotateFrontLeft = sf::Joystick::isButtonPressed(id, _controllerMapping.frontRotateLeftButton);
	bool rotateFrontRight = sf::Joystick::isButtonPressed(id, _controllerMapping.frontRotateRightButton);

	if (std::abs(x) > deadzone) {
		state.localVelocity.x = -(x / 100.0f) * maxSpeed;
	}
	if (std::abs(y) > deadzone) {
		state.localVelocity.y = (y / 100.0f) * maxSpeed;
	}
	if (std::abs(rotation) > deadzone) {
		state.angularVelocity = (rotation / 100.0f) * maxRotationSpeed;
	}

	state.frontAngularVelocity -= rotateFrontLeft ? maxRotationSpeed : 0;
	state.frontAngularVelocity += rotateFrontRight ? maxRotationSpeed : 0;

}
void JoystickInput::updateAfterSettingsChange() {
	_moveXAxis = static_cast<sf::Joystick::Axis>(_controllerMapping.moveXAxis);
	_moveYAxis = static_cast<sf::Joystick::Axis>(_controllerMapping.moveYAxis);
	_chassisRotateAxis = static_cast<sf::Joystick::Axis>(_controllerMapping.chassisRotateAxis);
}
