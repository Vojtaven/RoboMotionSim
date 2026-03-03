#ifndef INOPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP	
#include "AppConfig.hpp"
#include <memory>
#include "KeyboardInput.hpp"
#include "JoystickInput.hpp"
#include "RobotState.hpp"

class InputManager {
public :
	InputManager(const InputSettings& inputSettings);
	void update(RobotState& state, bool hasFocus) const;
	void updateAfterSettingsChange();
private:
	const InputSettings& _inputSettings;
	float _maxRotationSpeedRadians;
	std::unique_ptr<KeyboardInput> _keyboardInput;
	std::unique_ptr<JoystickInput> _joystickInput;
};

#endif // !INPUT_MANAGER_HPP
