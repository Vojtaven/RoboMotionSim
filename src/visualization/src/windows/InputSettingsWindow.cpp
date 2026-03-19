#include "windows/InputSettingsWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"
#include "ColorConstants.hpp"

static std::string getKeyName(int keyCode) {
	if (keyCode < 0) return "Unknown";
	auto key = static_cast<sf::Keyboard::Key>(keyCode);
	auto scancode = sf::Keyboard::delocalize(key);
	sf::String desc = sf::Keyboard::getDescription(scancode);
	std::string name = desc.toAnsiString();
	if (name.empty()) return "Key " + std::to_string(keyCode);
	return name;
}

static const char* getAxisName(int axisCode) {
	switch (axisCode) {
	case 0: return "X";
	case 1: return "Y";
	case 2: return "Z";
	case 3: return "R";
	case 4: return "U";
	case 5: return "V";
	case 6: return "PovX";
	case 7: return "PovY";
	default: return "Unknown";
	}
}

static void updateControllerList(std::vector<std::string>& controllerNames, int& selectedController) {
	controllerNames.clear();

	for (int i = 0; i < sf::Joystick::Count; ++i) {  // Count = 8
		if (sf::Joystick::isConnected(i)) {
			sf::Joystick::Identification id = sf::Joystick::getIdentification(i);
			std::string name = "[" + std::to_string(i) + "] " + id.name.toAnsiString();
			controllerNames.push_back(name);
		}
	}

	// Reset selection if it's now out of range
	if (selectedController >= (int)controllerNames.size())
		selectedController = 0;

}

InputSettingsWindow::InputSettingsWindow(const AppConfig& config)
{
	_windowConfig = config.inputSettingsWindow;
	_settings = config.inputSettings;
	if (_windowConfig.open) {
		open();
	}
}

InputSettingsWindow::~InputSettingsWindow() {
	close();
}

void InputSettingsWindow::open(const InputSettings& settings) {
	_settings = settings;
	open();
}

void InputSettingsWindow::open() {
	if (isOpen()) {
		_window->requestFocus();
		return;
	}

	if (!_windowConfig.wasOpenedBefore) {
		firstTimeSetup();
	}


	_pendingClose = false;

	_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode({ (uint32_t)_windowConfig.size.x, (uint32_t)_windowConfig.size.y }),
		"Input Settings",
		sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
	_window->setPosition({ _windowConfig.position.x, _windowConfig.position.y });

	if (!ImGui::SFML::Init(*_window)) {
		_window.reset();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	WindowHelper::SetScaledFont(io, DEFAULT_FONT_DATA, DEFAULT_FONT_DATA_SIZE, _windowConfig.defaultFontSize);
	_windowConfig.open = true;
	_isOpen = true;
}

void InputSettingsWindow::firstTimeSetup() {
	auto screenSize = sf::VideoMode::getDesktopMode().size;

	_windowConfig.size = { 420, 600 };
	_windowConfig.position = { ((int)screenSize.x - _windowConfig.size.x) / 2, ((int)screenSize.y - _windowConfig.size.y) / 2 };
	_windowConfig.resizable = true;
	_windowConfig.open = false;
	_windowConfig.wasOpenedBefore = true;
}

// Close the settings window. 
// If closeFromRoot is true, it indicates that the close action was initiated from the main window,
// so we should not set open to false in the config to allow reopening the settings window without issues.
void InputSettingsWindow::close(bool closeFromRoot) {
	if (!isOpen()) return;

	saveConfig();
	_windowConfig.open = closeFromRoot;
	ImGui::SFML::SetCurrentWindow(*_window);
	ImGui::SFML::Shutdown(*_window);
	_window->close();
	_window.reset();
	_pendingClose = false;
	_isOpen = false;
}

bool InputSettingsWindow::isOpen() const {
	return _isOpen;
}

void InputSettingsWindow::update(sf::Time dt) {
	if (!isOpen()) return;

	_controllerRefreshAccumulator += dt;
	_bindingChanged = false;

	// Switch to settings window context
	ImGui::SFML::SetCurrentWindow(*_window);

	// Process events BEFORE starting the ImGui frame
	while (const std::optional event = _window->pollEvent()) {
		// Intercept key press for keyboard key binding
		if (_waitingForKey) {
			if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
				if (keyEvent->code == sf::Keyboard::Key::Escape) {
					_waitingForKey = nullptr;
				}
				else {
					*_waitingForKey = static_cast<int>(keyEvent->code);
					_waitingForKey = nullptr;
					_bindingChanged = true;
				}
				continue;
			}
		}

		// Intercept joystick button press for controller button binding
		if (_waitingForButton) {
			if (const auto* btnEvent = event->getIf<sf::Event::JoystickButtonPressed>()) {
				*_waitingForButton = static_cast<int>(btnEvent->button);
				_waitingForButton = nullptr;
				_bindingChanged = true;
				continue;
			}
			if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
				if (keyEvent->code == sf::Keyboard::Key::Escape) {
					_waitingForButton = nullptr;
					continue;
				}
			}
		}

		// Intercept joystick axis movement for controller axis binding
		if (_waitingForAxis) {
			if (const auto* axisEvent = event->getIf<sf::Event::JoystickMoved>()) {
				if (std::abs(axisEvent->position) > 50.0f) {
					*_waitingForAxis = static_cast<int>(axisEvent->axis);
					_waitingForAxis = nullptr;
					_bindingChanged = true;
					continue;
				}
			}
			if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
				if (keyEvent->code == sf::Keyboard::Key::Escape) {
					_waitingForAxis = nullptr;
					continue;
				}
			}
		}

		ImGui::SFML::ProcessEvent(*_window, *event);

		if (event->is<sf::Event::Closed>()) {
			_pendingClose = true;
		}
	}

	if (_pendingClose) {
		close();
		return;
	}

	ImGui::SFML::Update(*_window, dt);

	renderContent();
}

void InputSettingsWindow::draw() {
	if (!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	_window->clear(Colors::WindowClearColor);
	ImGui::SFML::Render(*_window);
	_window->display();
}

void InputSettingsWindow::saveConfig() {
	_windowConfig.position = FromSFMLVector(_window->getPosition());
	_windowConfig.size = FromSFMLVector(_window->getSize());
	_windowConfig.resizable = true;
}

void InputSettingsWindow::renderContent() {
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(windowSize);

	ImGui::Begin("##InputSettingsContent", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);

	ImGui::PushItemWidth(-FLT_MIN);
	ImGui::TextColored(Colors::WindowHeader, "Input Settings");
	ImGui::Separator();
	ImGui::Spacing();

	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	bool changed = false;

	// Input type selector
	const char* inputTypeItems[] = { "Keyboard", "Controller", "IPC", "Serial" };
	_inputTypeIndex = (int)_settings.inputType;
	if (ImGui::Combo("##", &_inputTypeIndex, inputTypeItems, IM_ARRAYSIZE(inputTypeItems))) {
		_settings.inputType = (InputType)_inputTypeIndex;
		_waitingForKey = nullptr;
		_waitingForButton = nullptr;
		_waitingForAxis = nullptr;
		changed = true;
	}
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	switch (_inputTypeIndex) {
	case 0:
		changed |= renderKeyboardMapping();
		break;
	case 1:
		changed |= renderControllerMapping();
		break;

	case 2:
		changed |= renderIPCMapping();
		break;
	case 3:
		changed |= renderSerialMapping();
		break;
	default:
		throw std::runtime_error("Invalid input type index");
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Reset button
	if (ImGui::Button("Reset to Defaults", ImVec2(-1, 30))) {
		_settings = _defaultSettings;
		_inputTypeIndex = (int)_settings.inputType;
		_waitingForKey = nullptr;
		_waitingForButton = nullptr;
		_waitingForAxis = nullptr;
		changed = true;
	}

	if (changed && _onSettingsChanged) {
		_onSettingsChanged(_settings);
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

bool InputSettingsWindow::renderCommonSettings() {
	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;
	bool changed = false;
	// Common settings
	ImGui::Text("Max Speed (mm/s)");
	changed |= ImGui::SliderFloat("##MaxSpeed", &_settings.maxSpeed, 1.0f, 2000.0f, "%.1f", sliderFlags);
	ImGui::Spacing();

	ImGui::Text("Max Rotation Speed (deg/s)");
	changed |= ImGui::SliderFloat("##MaxRotSpeed", &_settings.maxRotationSpeed, 1.0f, 360.0f, "%.1f", sliderFlags);
	ImGui::Spacing();

	changed |= ImGui::Checkbox("Register Input Without Focus", &_settings.registerInputWithoutFocus);
	ImGui::Spacing();
	ImGui::Spacing();
	return changed;
}

bool InputSettingsWindow::renderKeyboardMapping() {
	bool changed = _bindingChanged || renderCommonSettings();
	ImGui::TextColored(Colors::WindowHeader, "Keyboard Mapping");
	ImGui::Separator();
	ImGui::Spacing();
	KeyboardMapping& km = _settings.keyboardMapping;

	auto renderKeyBind = [&](const char* label, int& keyCode) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%s", label);
		ImGui::TableNextColumn();

		std::string btnId = std::string("##") + label;
		if (_waitingForKey == &keyCode) {
			ImGui::Button(("Press a key..." + btnId).c_str(), ImVec2(-1, 0));
		}
		else {
			std::string keyName = getKeyName(keyCode);
			if (ImGui::Button((keyName + btnId).c_str(), ImVec2(-1, 0))) {
				_waitingForKey = &keyCode;
				_waitingForButton = nullptr;
				_waitingForAxis = nullptr;
			}
		}
		};

	ImGui::PushTextWrapPos(0.0f);
	ImGui::TextDisabled("Click a button and press a key to rebind. Escape to cancel.");
	ImGui::PopTextWrapPos();
	ImGui::Spacing();

	if (ImGui::BeginTable("##KeyboardBindings", 2, ImGuiTableFlags_BordersInnerH)) {
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 200);
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);

		renderKeyBind("Move Forward", km.upKey);
		renderKeyBind("Move Backward", km.downKey);
		renderKeyBind("Move Left", km.leftKey);
		renderKeyBind("Move Right", km.rightKey);
		renderKeyBind("Chassis Rotate Left", km.chassisRotateLeftKey);
		renderKeyBind("Chassis Rotate Right", km.chassisRotateRightKey);
		renderKeyBind("Front Rotate Left", km.frontRotateLeftKey);
		renderKeyBind("Front Rotate Right", km.frontRotateRightKey);

		ImGui::EndTable();
	}

	return changed;
}

bool InputSettingsWindow::renderControllerMapping() {
	bool changed = _bindingChanged || renderCommonSettings();
	ImGui::TextColored(Colors::WindowHeader, "Controller Mapping");
	ImGui::Separator();
	ImGui::Spacing();
	ControllerMapping& cm = _settings.controllerMapping;
	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	if (_controllerNames.empty() || _controllerRefreshAccumulator >= sf::seconds(1.0f)) {
		_controllerRefreshAccumulator = sf::Time::Zero;
		updateControllerList(_controllerNames, cm.controllerId);
	}

	ImGui::Text("Controllers:");

	if (_controllerNames.empty()) {
		ImGui::BeginDisabled();
		if (ImGui::BeginCombo("##controllers", "No controllers found"))
			ImGui::EndCombo();
		ImGui::EndDisabled();
	}
	else {
		const char* preview = _controllerNames[cm.controllerId].c_str();

		if (ImGui::BeginCombo("##controllers", preview)) {
			for (int i = 0; i < (int)_controllerNames.size(); ++i) {
				bool isSelected = (cm.controllerId == i);
				if (ImGui::Selectable(_controllerNames[i].c_str(), isSelected)) {
					cm.controllerId = i;
					changed = true;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	ImGui::Spacing();

	auto renderJoystickControl = [&](const char* label, JoystickControl& ctrl) {
		ImGui::PushID(label);

		ImGui::TextColored(Colors::JoystickControlLabel, "%s", label);

		const char* modeItems[] = { "Buttons", "Axis" };
		int modeIndex = ctrl.isAxis ? 1 : 0;
		if (ImGui::Combo("", &modeIndex, modeItems, IM_ARRAYSIZE(modeItems))) {
			ctrl.isAxis = (modeIndex == 1);
			changed = true;
		}

		if (ctrl.isAxis) {
			// Axis binding
			std::string axisBtnId = std::string("##Axis_") + label;
			if (_waitingForAxis == &ctrl.axisId) {
				ImGui::Button(("Move an axis..." + axisBtnId).c_str(), ImVec2(-1, 0));
			}
			else {
				std::string axisName = std::string("Axis ") + getAxisName(ctrl.axisId);
				if (ImGui::Button((axisName + axisBtnId).c_str(), ImVec2(-1, 0))) {
					_waitingForAxis = &ctrl.axisId;
					_waitingForKey = nullptr;
					_waitingForButton = nullptr;
				}
			}
			changed |= ImGui::Checkbox("Invert", &ctrl.invert);
		}
		else {
			// Button bindings
			std::string btn1Id = std::string("##Btn1_") + label;
			ImGui::Text("Button 1 (Negative)");
			if (_waitingForButton == &ctrl.buttonId1) {
				ImGui::Button(("Press a button..." + btn1Id).c_str(), ImVec2(-1, 0));
			}
			else {
				std::string buttonName = "Button " + std::to_string(ctrl.buttonId1);
				if (ImGui::Button((buttonName + btn1Id).c_str(), ImVec2(-1, 0))) {
					_waitingForButton = &ctrl.buttonId1;
					_waitingForKey = nullptr;
					_waitingForAxis = nullptr;
				}
			}

			std::string btn2Id = std::string("##Btn2_") + label;
			ImGui::Text("Button 2 (Positive)");
			if (_waitingForButton == &ctrl.buttonId2) {
				ImGui::Button(("Press a button..." + btn2Id).c_str(), ImVec2(-1, 0));
			}
			else {
				std::string buttonName = "Button " + std::to_string(ctrl.buttonId2);
				if (ImGui::Button((buttonName + btn2Id).c_str(), ImVec2(-1, 0))) {
					_waitingForButton = &ctrl.buttonId2;
					_waitingForKey = nullptr;
					_waitingForAxis = nullptr;
				}
			}
		}

		ImGui::PopID();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		};
	ImGui::Text("Deadzone (%%)");
	changed |= ImGui::SliderInt("##Deadzone", &cm.deadzone, 0, 100, "%d", sliderFlags);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::TextDisabled("Click a button to rebind. Escape to cancel.");
	ImGui::Spacing();

	renderJoystickControl("Move X", cm.xAxisControl);
	renderJoystickControl("Move Y", cm.yAxisControl);
	renderJoystickControl("Chassis Rotate", cm.chassisRotateControl);
	renderJoystickControl("Front Rotate", cm.frontRotateControl);
	return changed;
}
bool InputSettingsWindow::renderIPCMapping() {
	ImGui::TextColored(Colors::WindowHeader, "IPC Mapping");
	ImGui::Separator();
	ImGui::Spacing();
	bool changed = false;
	IPCMapping& ipc = _settings.ipcMapping;
	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	ImGui::Text("Address");
	char addressBuf[256] = {};
	auto addrLen = ipc.address.copy(addressBuf, sizeof(addressBuf) - 1);
	addressBuf[addrLen] = '\0';
	if (ImGui::InputText("##IPCAddress", addressBuf, sizeof(addressBuf))) {
		ipc.address = addressBuf;
		changed = true;
	}
	ImGui::Spacing();

	// Port helper - InputInt clamped to valid uint16_t range
	auto renderPort = [&](const char* label, uint16_t& port) {
		ImGui::Text("%s", label);
		std::string id = std::string("##") + label;
		int portVal = static_cast<int>(port);
		if (ImGui::InputInt(id.c_str(), &portVal)) {
			port = static_cast<uint16_t>(std::clamp(portVal, 1, 65535));
			changed = true;
		}
		ImGui::Spacing();
		};

	renderPort("Command Port", ipc.command_port);
	renderPort("Response Port", ipc.response_port);
	renderPort("Telemetry Port", ipc.telemetry_port);

	ImGui::Text("Heartbeat Timeout (s)");
	changed |= ImGui::SliderFloat("##HeartbeatTimeout", &ipc.heartbeatTimeout, 0.5f, 30.0f, "%.1f", sliderFlags);
	ImGui::Spacing();

	return changed;
}

bool InputSettingsWindow::renderSerialMapping() {
	ImGui::TextColored(Colors::WindowHeader, "Serial Mapping");
	ImGui::Separator();
	ImGui::Spacing();
	bool changed = false;
	SerialMapping& serial = _settings.serialMapping;

	ImGui::Text("Port Name");
	char portBuf[64] = {};
	auto portLen = serial.portName.copy(portBuf, sizeof(portBuf) - 1);
	portBuf[portLen] = '\0';
	if (ImGui::InputText("##SerialPort", portBuf, sizeof(portBuf))) {
		serial.portName = portBuf;
		changed = true;
	}
	ImGui::Spacing();

	ImGui::Text("Baud Rate");
	static const unsigned int baudRates[] = { 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 };
	static const char* baudRateNames[] = { "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600" };
	int currentBaudIndex = 0;
	for (int i = 0; i < IM_ARRAYSIZE(baudRates); ++i) {
		if (baudRates[i] == serial.baudRate) {
			currentBaudIndex = i;
			break;
		}
	}
	if (ImGui::Combo("##BaudRate", &currentBaudIndex, baudRateNames, IM_ARRAYSIZE(baudRateNames))) {
		serial.baudRate = baudRates[currentBaudIndex];
		changed = true;
	}
	ImGui::Spacing();

	ImGui::TextDisabled("Binary protocol: [uint8 motorCount][float x motorCount]");
	ImGui::Spacing();

	return changed;
}
