#include "windows/InputSettingsWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <string>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"

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
				} else {
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
	_window->clear(sf::Color(30, 30, 30));
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
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Input Settings");
	ImGui::Separator();
	ImGui::Spacing();

	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	bool changed = false;

	// Input type selector
	const char* inputTypeItems[] = { "Keyboard", "Controller", "IPC" };
	_inputTypeIndex = (int)_settings.inputType;
	if (ImGui::Combo("Input Type", &_inputTypeIndex, inputTypeItems, IM_ARRAYSIZE(inputTypeItems))) {
		_settings.inputType = (InputType)_inputTypeIndex;
		_waitingForKey = nullptr;
		_waitingForButton = nullptr;
		_waitingForAxis = nullptr;
		changed = true;
	}
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Common settings
	ImGui::Text("Max Speed (mm/s)");
	changed |= ImGui::SliderFloat("##MaxSpeed", &_settings.maxSpeed, 1.0f, 2000.0f, "%.1f", sliderFlags);
	ImGui::Spacing();

	ImGui::Text("Max Rotation Speed (deg/s)");
	changed |= ImGui::SliderFloat("##MaxRotSpeed", &_settings.maxRotationSpeed, 1.0f, 360.0f, "%.1f", sliderFlags);
	ImGui::Spacing();

	changed |= ImGui::Checkbox("Register Input Without Focus", &_settings.registerInputWithoutFocus);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Show keyboard or controller mapping depending on input type
	if (_inputTypeIndex == 0) {
		ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Keyboard Mapping");
		ImGui::Separator();
		ImGui::Spacing();
		changed |= renderKeyboardMapping();
	}
	else {
		ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Controller Mapping");
		ImGui::Separator();
		ImGui::Spacing();
		changed |= renderControllerMapping();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Reset button
	if (ImGui::Button("Reset to Defaults", ImVec2(-1, 30))) {
		_settings = _defaultSettings;
		_inputTypeIndex = (_settings.inputType == InputType::Controller) ? 1 : 0;
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

bool InputSettingsWindow::renderKeyboardMapping() {
	bool changed = _bindingChanged;
	KeyboardMapping& km = _settings.keyboardMapping;

	auto renderKeyBind = [&](const char* label, int& keyCode) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%s", label);
		ImGui::TableNextColumn();

		std::string btnId = std::string("##") + label;
		if (_waitingForKey == &keyCode) {
			ImGui::Button(("Press a key..." + btnId).c_str(), ImVec2(-1, 0));
		} else {
			std::string keyName = getKeyName(keyCode);
			if (ImGui::Button((keyName + btnId).c_str(), ImVec2(-1, 0))) {
				_waitingForKey = &keyCode;
				_waitingForButton = nullptr;
				_waitingForAxis = nullptr;
			}
		}
	};

	ImGui::TextDisabled("Click a button and press a key to rebind. Escape to cancel.");
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
	bool changed = _bindingChanged;
	ControllerMapping& cm = _settings.controllerMapping;
	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	ImGui::Text("Controller ID");
	changed |= ImGui::InputInt("##ControllerId", &cm.controllerId);
	cm.controllerId = std::clamp(cm.controllerId, 0, 7);
	ImGui::Spacing();

	auto renderAxisBind = [&](const char* label, int& axisCode) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%s", label);
		ImGui::TableNextColumn();

		std::string btnId = std::string("##") + label;
		if (_waitingForAxis == &axisCode) {
			ImGui::Button(("Move an axis..." + btnId).c_str(), ImVec2(-1, 0));
		} else {
			std::string axisName = std::string("Axis ") + getAxisName(axisCode);
			if (ImGui::Button((axisName + btnId).c_str(), ImVec2(-1, 0))) {
				_waitingForAxis = &axisCode;
				_waitingForKey = nullptr;
				_waitingForButton = nullptr;
			}
		}
	};

	auto renderButtonBind = [&](const char* label, int& buttonCode) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%s", label);
		ImGui::TableNextColumn();

		std::string btnId = std::string("##") + label;
		if (_waitingForButton == &buttonCode) {
			ImGui::Button(("Press a button..." + btnId).c_str(), ImVec2(-1, 0));
		} else {
			std::string buttonName = "Button " + std::to_string(buttonCode);
			if (ImGui::Button((buttonName + btnId).c_str(), ImVec2(-1, 0))) {
				_waitingForButton = &buttonCode;
				_waitingForKey = nullptr;
				_waitingForAxis = nullptr;
			}
		}
	};

	ImGui::TextDisabled("Click a button to rebind. Escape to cancel.");
	ImGui::Spacing();

	if (ImGui::BeginTable("##ControllerBindings", 2, ImGuiTableFlags_BordersInnerH)) {
		ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 200);
		ImGui::TableSetupColumn("Binding", ImGuiTableColumnFlags_WidthStretch);

		renderAxisBind("Move X Axis", cm.moveXAxis);
		renderAxisBind("Move Y Axis", cm.moveYAxis);
		renderAxisBind("Chassis Rotate Axis", cm.chassisRotateAxis);
		renderButtonBind("Front Rotate Left", cm.frontRotateLeftButton);
		renderButtonBind("Front Rotate Right", cm.frontRotateRightButton);

		ImGui::EndTable();
	}

	ImGui::Spacing();
	ImGui::Text("Deadzone (%%)");
	changed |= ImGui::SliderInt("##Deadzone", &cm.deadzone, 0, 100, "%d", sliderFlags);

	return changed;
}
