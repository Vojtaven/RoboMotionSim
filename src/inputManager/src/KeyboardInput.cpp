#include "KeyboardInput.hpp"
#include "RobotState.hpp"
#include <SFML/Window/Keyboard.hpp>
#include "MathUtils.hpp"
KeyboardInput::KeyboardInput(const KeyboardMapping& keyboardMapping)
	: _keyboardMapping(keyboardMapping)
{
	updateAfterSettingsChange();
}
void KeyboardInput::update(RobotState& state, const float maxSpeed, const float maxRotationSpeed) const{
	Vec2f& speed = state.localVelocity;
	float& rotationSpeed = state.angularVelocity;
	float& frontRotationSpeed = state.frontAngularVelocity;

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