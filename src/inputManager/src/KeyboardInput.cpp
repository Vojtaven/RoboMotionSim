// Author: Vojtech Venzara
// Date: 2026-04-02
// Description: Input handler for capturing keyboard events and converting them to robot control commands

#include "KeyboardInput.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Keyboard.hpp>
#include "MathUtils.hpp"
KeyboardInput::KeyboardInput(ConfigSection<KeyboardMapping>& keyboardMapping)
	: _keyboardMapping(keyboardMapping.get())
{
	_subscription = keyboardMapping.scopedSubscribe([this](const KeyboardMapping&) {
		updateAfterSettingsChange();
	});
	updateAfterSettingsChange();
}
void KeyboardInput::update(RobotState& state, const float maxSpeed, const float maxRotationSpeed) const{
	Vec2d& speed = state.localVelocity;
	double& rotationSpeed = state.angularVelocity;
	double& frontRotationSpeed = state.frontAngularVelocity;

	if (sf::Keyboard::isKeyPressed(_leftKey))
		speed.y -= maxSpeed;
	if (sf::Keyboard::isKeyPressed(_rightKey))
		speed.y += maxSpeed;
	if (sf::Keyboard::isKeyPressed(_downKey))
		speed.x -= maxSpeed;
	if (sf::Keyboard::isKeyPressed(_upKey))
		speed.x += maxSpeed;
	if (sf::Keyboard::isKeyPressed(_chassisRotateLeftKey))
		rotationSpeed -= maxRotationSpeed;
	if (sf::Keyboard::isKeyPressed(_chassisRotateRightKey))
		rotationSpeed += maxRotationSpeed;
	if (sf::Keyboard::isKeyPressed(_frontRotateLeftKey))
		frontRotationSpeed -= maxRotationSpeed;
	if (sf::Keyboard::isKeyPressed(_frontRotateRightKey))
		frontRotationSpeed += maxRotationSpeed;
}
void KeyboardInput::updateAfterSettingsChange() {
	_upKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.upKey);
	_downKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.downKey);
	_leftKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.leftKey);
	_rightKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.rightKey);
	_chassisRotateLeftKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.chassisRotateLeftKey);
	_chassisRotateRightKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.chassisRotateRightKey);
	_frontRotateLeftKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.frontRotateLeftKey);
	_frontRotateRightKey = static_cast<sf::Keyboard::Key>(_keyboardMapping.frontRotateRightKey);
}
