#ifndef KEYBOARD_INPUT_HPP
#define KEYBOARD_INPUT_HPP
#include "AppConfig.hpp"
#include <SFML/Window/Keyboard.hpp>
#include "RobotState.hpp"
class KeyboardInput {
public:
	KeyboardInput(const KeyboardMapping& keyboardMapping);
	void update(RobotState& state, const float maxSpeed, const float maxRotationSpeed) const;
	void updateAfterSettingsChange();
private:
	const KeyboardMapping& _keyboardMapping;
	sf::Keyboard::Key _upKey;
	sf::Keyboard::Key _downKey;
	sf::Keyboard::Key _leftKey;
	sf::Keyboard::Key _rightKey;
	sf::Keyboard::Key _chassisRotateLeftKey;
	sf::Keyboard::Key _chassisRotateRightKey;
	sf::Keyboard::Key _frontRotateLeftKey;
	sf::Keyboard::Key _frontRotateRightKey;
};

#endif // !KEYBOARD_INPUT_HPP
