#include "windows/SettingsWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdint>

SettingsWindow::SettingsWindow() = default;

SettingsWindow::~SettingsWindow() {
	close();
}

void SettingsWindow::open(const VisualizationSettings& currentSettings, sf::RenderWindow& callerWindow) {
	if (_isOpen) {
		_window->requestFocus();
		return;
	}

	_settings = currentSettings;
	_pendingClose = false;
	_callerWindow = &callerWindow;

	_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode({ 420, 600 }),
		"Settings",
		sf::Style::Titlebar | sf::Style::Close| sf::Style::Resize);
	_window->setFramerateLimit(60);

	if (!ImGui::SFML::Init(*_window)) {
		_window.reset();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Restore caller's context — Init() switches the active context internally
	ImGui::SFML::SetCurrentWindow(callerWindow);

	_deltaClock.restart();
	_isOpen = true;
}

void SettingsWindow::close() {
	if (!_isOpen) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	ImGui::SFML::Shutdown(*_window);
	_window->close();
	_window.reset();
	_isOpen = false;
	_pendingClose = false;

	// Restore caller's context so the main window isn't left with a dangling context
	if (_callerWindow)
		ImGui::SFML::SetCurrentWindow(*_callerWindow);
}

bool SettingsWindow::isOpen() const {
	return _isOpen;
}

void SettingsWindow::update() {
	if (!_isOpen) return;

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

	// Now start the ImGui frame
	ImGui::SFML::Update(*_window, _deltaClock.restart());

	// Render ImGui content
	renderContent();

	// Draw
	_window->clear(sf::Color(30, 30, 30));
	ImGui::SFML::Render(*_window);
	_window->display();
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

	ImGui::Spacing();

	// Grid spacing
	ImGui::Text("Grid Spacing (mm)");
	if (ImGui::SliderFloat2("##GridSpacing", _settings.gridSpacing, 10.0f, 200.0f, "%.1f", sliderFlags)) {
		changed = true;
	}

	ImGui::Spacing();
	ImGuiColorEditFlags colorFlags =
		ImGuiColorEditFlags_DisplayRGB |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_InputRGB;
	// Grid color
	ImGui::Text("Grid Color");
	if (ImGui::ColorPicker3("##GridColor", _settings.gridColor, colorFlags)) {
		changed = true;
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Reset button
	if (ImGui::Button("Reset to Defaults", ImVec2(-1, 30))) {
		_settings.scaleFactor = 1.0f;
		_settings.gridSpacing[0] = 50.0f;
		_settings.gridSpacing[1] = 50.0f;
		_settings.gridColor[0] = 80.0f / 255.0f;
		_settings.gridColor[1] = 80.0f / 255.0f;
		_settings.gridColor[2] = 80.0f / 255.0f;
		changed = true;
	}

	if (changed && _onSettingsChanged) {
		_onSettingsChanged(_settings);
	}

	ImGui::End();
}