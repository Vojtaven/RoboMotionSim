#include "windows/SettingsWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>
#include <cstdint>
#include "SFMLHelper.hpp"
SettingsWindow::SettingsWindow(const AppConfig& config) {
	_windowConfig = config.settingsWindowConfig;
	_settings = config.renderSettings;
	_gridSpacingRatio = (_settings.gridSpacing.x != 0.0f) ? _settings.gridSpacing.y / _settings.gridSpacing.x : 1.0f;
	if (_windowConfig.open) {
		open();
	}
}

SettingsWindow::~SettingsWindow() {
	close();
}

void SettingsWindow::open(const RenderSettings& settings) {
	_settings = settings;
	_gridSpacingRatio = (_settings.gridSpacing.x != 0.0f) ? _settings.gridSpacing.y / _settings.gridSpacing.x : 1.0f;
	open();
}

void SettingsWindow::open() {
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
		"Settings",
		sf::Style::Titlebar | sf::Style::Close| sf::Style::Resize);
	_window->setFramerateLimit(_windowConfig.frameRateLimit);
	_window->setPosition({ _windowConfig.position.x, _windowConfig.position.y });

	if (!ImGui::SFML::Init(*_window)) {
		_window.reset();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	_windowConfig.open = true;
	_isOpen = true;
}

void SettingsWindow::firstTimeSetup() {
	auto screenSize = sf::VideoMode::getDesktopMode().size;

	_windowConfig.size = { 420, 600 };
	_windowConfig.position = { ((int)screenSize.x - _windowConfig.size.x) / 2, ((int)screenSize.y - _windowConfig.size.y) / 2 };
	_windowConfig.frameRateLimit = 60;
	_windowConfig.resizable = true;
	_windowConfig.open = false;
	_windowConfig.wasOpenedBefore = true;
}

// Close the settings window. 
// If closeFromRoot is true, it indicates that the close action was initiated from the main window,
// so we should not set open to false in the config to allow reopening the settings window without issues.
void SettingsWindow::close(bool closeFromRoot) {
	if (!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	ImGui::SFML::Shutdown(*_window);
	saveConfig();
	_windowConfig.open = closeFromRoot;
	_window->close();
	_window.reset();
	_pendingClose = false;
	_isOpen = false;
}

bool SettingsWindow::isOpen() const {
	return _isOpen;
}

void SettingsWindow::update(sf::Time dt) {
	if (!isOpen()) return;

	// Switch to settings window context
	ImGui::SFML::SetCurrentWindow(*_window);

	// Process events BEFORE starting the ImGui frame
	while (const std::optional event = _window->pollEvent()) {
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

void SettingsWindow::draw() {
	if(!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	_window->clear(sf::Color(30, 30, 30));
	ImGui::SFML::Render(*_window);
	_window->display();
}

void SettingsWindow::saveConfig() {
	_windowConfig.frameRateLimit = _windowConfig.frameRateLimit;
	_windowConfig.position = FromSFMLVector(_window->getPosition());
	_windowConfig.size = FromSFMLVector( _window->getSize());
	_windowConfig.resizable = true;
}

void SettingsWindow::renderContent() {
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(windowSize);

	ImGui::Begin("##SettingsContent", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);

	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Visualization Settings");
	ImGui::Separator();
	ImGui::Spacing();

	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	bool changed = false;

	// Scale factor
	ImGui::Text("Scale Factor");
	if (ImGui::SliderFloat("##Scale", &_settings.scaleFactor, 0.1f, 5.0f, "%.2f", sliderFlags)) {
		changed = true;
	}

	ImGui::Checkbox("Lock on Robot Center While Scaling", &_settings.lockOnRobotCenterWhileScaling);

	ImGui::Spacing();

	// Grid spacing
	ImGui::Text("Grid Spacing (mm)");
	bool xChanged = ImGui::SliderFloat("X##GridSpacingX", &_settings.gridSpacing.x, 10.0f, 200.0f, "%.1f", sliderFlags);
	bool yChanged = ImGui::SliderFloat("Y##GridSpacingY", &_settings.gridSpacing.y, 10.0f, 200.0f, "%.1f", sliderFlags);
	if (xChanged || yChanged) {
		if (_settings.lockGridSpacingRatio) {
			if (xChanged && _settings.gridSpacing.x > 0.0f) {
				_settings.gridSpacing.y = std::clamp(_settings.gridSpacing.x * _gridSpacingRatio, 10.0f, 200.0f);
			} else if (yChanged && _gridSpacingRatio > 0.0f) {
				_settings.gridSpacing.x = std::clamp(_settings.gridSpacing.y / _gridSpacingRatio, 10.0f, 200.0f);
			}
		} else if (_settings.gridSpacing.x > 0.0f) {
			_gridSpacingRatio = _settings.gridSpacing.y / _settings.gridSpacing.x;
		}
		changed = true;
	}

	ImGui::Spacing();
	ImGui::Checkbox("Lock Grid Spacing Ratio", &_settings.lockGridSpacingRatio);
	if (ImGui::Checkbox("Show Grid", &_settings.showGrid)) {
		changed = true;
	}
	ImGui::Spacing();
	ImGuiColorEditFlags colorFlags =
		ImGuiColorEditFlags_DisplayRGB |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_InputRGB;
	// Grid color
	ImGui::Text("Grid Color");
	if (ImGui::ColorPicker3("##GridColor", _settings.gridColor.data(), colorFlags)) {
		changed = true;
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Reset button
	if (ImGui::Button("Reset to Defaults", ImVec2(-1, 30))) {
		_settings = RenderSettings();
		_gridSpacingRatio = 1.0f;
		changed = true;
	}

	if (changed && _onSettingsChanged) {
		_onSettingsChanged(_settings);
	}

	ImGui::End();
}