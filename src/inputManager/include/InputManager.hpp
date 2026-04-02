#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP
#include "AppConfig.hpp"
#include "ConfigSection.hpp"
#include <memory>
#include <optional>
#include <string>
#include "KeyboardInput.hpp"
#include "JoystickInput.hpp"
#include "RobotState.hpp"
#include "IPC/IPCInput.hpp"
#include "SerialInput.hpp"

class InputManager {
public :
	InputManager(ConfigSection<InputSettings>& inputSettings);
	// Returns an optional error message if something went wrong (e.g. controller disconnected)
	std::optional<std::string> update(RobotState& state, bool hasFocus) const;
	// For input types that have action with duration (e.g. "move forward for 2 seconds"), check if any of those actions have completed and update state accordingly
	void checkForInputCompletion(const RobotState& state, const double dt);
private:
	void updateAfterSettingsChange();
	const InputSettings& _inputSettings;
	ScopedSubscription<InputSettings> _settingsSubscription;
	float _maxRotationSpeedRadians;
	std::unique_ptr<KeyboardInput> _keyboardInput;
	std::unique_ptr<JoystickInput> _joystickInput;
	std::unique_ptr<IPCInput> _ipcInput;
	std::unique_ptr<SerialInput> _serialInput;
};

#endif // !INPUT_MANAGER_HPP
