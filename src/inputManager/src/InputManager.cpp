#include "InputManager.hpp"
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include "KeyboardInput.hpp"
#include "JoystickInput.hpp"
InputManager::InputManager(const InputSettings& inputSettings)
	: _inputSettings(inputSettings),
	_keyboardInput(std::make_unique<KeyboardInput>(_inputSettings.keyboardMapping)),
	_joystickInput(std::make_unique<JoystickInput>(_inputSettings.controllerMapping)),
	_ipcInput(std::make_unique<IPCInput>(inputSettings.ipcMapping))
{
	updateAfterSettingsChange();
}
void InputManager::update(RobotState& state, bool hasFocus) const {
	state.localVelocity = { 0,0 };
	state.angularVelocity = 0;
	state.frontAngularVelocity = 0;

	if (!_inputSettings.registerInputWithoutFocus && !hasFocus)
		return;


	switch (_inputSettings.inputType)
	{
	case InputType::Keyboard:
		_keyboardInput->update(state, _inputSettings.maxSpeed, _maxRotationSpeedRadians);
		break;
	case InputType::Controller:
		_joystickInput->update(state, _inputSettings.maxSpeed, _maxRotationSpeedRadians);
		break;

	case InputType::IPC:
		_ipcInput->update(state);
		break;
	default:
		throw std::runtime_error("Unsupported input type");
	}
}

void InputManager::checkForInputCompletion(const RobotState& state, const float dt) {
	switch (_inputSettings.inputType)
	{
	case InputType::Keyboard:
		break;
	case InputType::Controller:
		break;
	case InputType::IPC:
		_ipcInput->checkForInputCompletion(state,dt);
		break;
	default:
		throw std::runtime_error("Unsupported input type");
	}

}

void InputManager::updateAfterSettingsChange() {
	_keyboardInput->updateAfterSettingsChange();
	_joystickInput->updateAfterSettingsChange();
	_ipcInput->updateAfterSettingsChange();
	_maxRotationSpeedRadians = DegreesToRadians(_inputSettings.maxRotationSpeed);
}
