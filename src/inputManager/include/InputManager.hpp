#ifndef INOPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP	
#include "AppConfig.hpp"
#include <memory>
#include <optional>
#include <string>
#include "KeyboardInput.hpp"
#include "JoystickInput.hpp"
#include "RobotState.hpp"
#include "IPC/IPCInput.hpp"

class InputManager {
public :
	InputManager(const InputSettings& inputSettings);
	std::optional<std::string> update(RobotState& state, bool hasFocus) const;
	void checkForInputCompletion(const RobotState& state, const float dt);
	void updateAfterSettingsChange();
private:
	const InputSettings& _inputSettings;
	float _maxRotationSpeedRadians;
	std::unique_ptr<KeyboardInput> _keyboardInput;
	std::unique_ptr<JoystickInput> _joystickInput;
	std::unique_ptr<IPCInput> _ipcInput;
};

#endif // !INPUT_MANAGER_HPP
