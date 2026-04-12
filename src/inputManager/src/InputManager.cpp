// Author: Vojtech Venzara
// Date: 2026-04-02
// Description: Central input handling system that aggregates inputs from keyboard, joystick, serial and IPC sources

#include "InputManager.hpp"
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include "KeyboardInput.hpp"
#include "JoystickInput.hpp"
InputManager::InputManager(ConfigSection<InputSettings>& inputSettings)
	: _inputSettings(inputSettings.get()),
	_keyboardInput(std::make_unique<KeyboardInput>(inputSettings.getMutable().keyboardMapping)),
	_joystickInput(std::make_unique<JoystickInput>(inputSettings.getMutable().controllerMapping)),
	_ipcInput(std::make_unique<IPCInput>(inputSettings.getMutable().ipcMapping)),
	_serialInput(std::make_unique<SerialInput>(inputSettings.getMutable().serialMapping))
{
	_settingsSubscription = inputSettings.scopedSubscribe([this](const InputSettings&) {
		updateAfterSettingsChange();
	});
	updateAfterSettingsChange();
}
std::optional<std::string> InputManager::update(RobotState& state, bool hasFocus) const {
	state.localVelocity = { 0,0 };
	state.angularVelocity = 0;
	state.frontAngularVelocity = 0;


	// assume all wheels are powered; input sources will override individually if needed
	for (auto& wheel : state.wheels)
		wheel.powered = true;


	switch (_inputSettings.inputType)
	{
	case InputType::Keyboard:
		if (_inputSettings.registerInputWithoutFocus || hasFocus)
			_keyboardInput->update(state, _inputSettings.maxSpeed, _maxRotationSpeedRadians);
		return std::nullopt;
	case InputType::Controller:
		if (_inputSettings.registerInputWithoutFocus || hasFocus) {
			auto result = _joystickInput->update(state, _inputSettings.maxSpeed, _maxRotationSpeedRadians);
			return result;
		}
		return std::nullopt;
	case InputType::IPC:
		return _ipcInput->update(state);
	case InputType::Serial:
		return _serialInput->update(state);
	default:
		return "Unsupported input type";
	}
}

void InputManager::checkForInputCompletion(const RobotState& state, const double dt) {
	switch (_inputSettings.inputType)
	{
	case InputType::Keyboard:
		break;
	case InputType::Controller:
		break;
	case InputType::IPC:
		_ipcInput->checkForInputCompletion(state, dt);
		break;
	case InputType::Serial:
		break;
	default:
		throw std::runtime_error("Unsupported input type");
	}

}

void InputManager::updateAfterSettingsChange() {
	_maxRotationSpeedRadians = static_cast<float>(DegreesToRadians(_inputSettings.maxRotationSpeed));
}
