#include "windows/RenderSettingsWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>
#include <cstdint>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"
#include "ColorConstants.hpp"
RenderSettingsWindow::RenderSettingsWindow(const AppConfig& config, const sf::Image& icon)
	: _icon(icon)
{
	_windowConfig = config.renderSettingsWindow;
	_settings = config.renderSettings;
	if (_windowConfig.open) {
		open();
	}
}

RenderSettingsWindow::~RenderSettingsWindow() {
	close();
}

void RenderSettingsWindow::open(const RenderSettings& settings) {
	_settings = settings;
	open();
}

void RenderSettingsWindow::open() {
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
		"Render Settings",
		sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
	_window->setPosition({ _windowConfig.position.x, _windowConfig.position.y });
	if (_icon.getSize().x > 0 && _icon.getSize().y > 0)
		_window->setIcon(_icon);
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

void RenderSettingsWindow::firstTimeSetup() {
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
void RenderSettingsWindow::close(bool closeFromRoot) {
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

bool RenderSettingsWindow::isOpen() const {
	return _isOpen;
}

void RenderSettingsWindow::update(sf::Time dt) {
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

void RenderSettingsWindow::draw() {
	if (!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	_window->clear(Colors::WindowClearColor);
	ImGui::SFML::Render(*_window);
	_window->display();
}

void RenderSettingsWindow::saveConfig() {
	_windowConfig.position = FromSFMLVector(_window->getPosition());
	_windowConfig.size = FromSFMLVector(_window->getSize());
	_windowConfig.resizable = true;
}

void RenderSettingsWindow::renderContent() {
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(windowSize);

	ImGui::Begin("##SettingsContent", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);

	ImGui::PushItemWidth(-FLT_MIN);
	ImGui::TextColored(Colors::WindowHeader, "Visualization Settings");
	ImGui::Separator();
	ImGui::Spacing();

	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;

	bool changed = false;

	// Scale factor
	ImGui::Text("Scale Factor");
	changed |= ImGui::SliderFloat("##Scale", &_settings.scaleFactor, 0.1f, 5.0f, "%.2f", sliderFlags);
	changed |= ImGui::Checkbox("Lock on view on robot", &_settings.lockViewOnRobot);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	GridSettings& gridSettings = _settings.gridSettings;
	ImGui::Spacing();
	ImGui::Text("Grid Settings");
	ImGui::Spacing();
	changed |= ImGui::Checkbox("Show Grid", &_settings.showGrid);
	if (_settings.showGrid) {
		changed |= ImGui::Checkbox("Auto Grid Spacing", &gridSettings.autoSpacing);
		if (!gridSettings.autoSpacing) {
			ImGui::Spacing();
			ImGui::Text("Grid Spacing (mm)");
			changed |= ImGui::SliderInt("##GridSpacing", &gridSettings.spacing, 10, 2000, "%d", sliderFlags);
		}
		ImGui::Spacing();
		ImGui::Text("Grid subdivision count");
		changed |= ImGui::SliderInt("##GridSubdivision", &gridSettings.subdivisionsCount, 0, 10, "%d", sliderFlags);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGuiColorEditFlags colorFlags =
		ImGuiColorEditFlags_DisplayRGB |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_InputRGB;

	const char* colorTypeItems[] = { "Grid color", "SubGrid color", "TrailColor" };
	ImGui::Combo("##ColorSelecterType", &colorTypeIndex, colorTypeItems, IM_ARRAYSIZE(colorTypeItems));
	float* dataToUse = nullptr;
	switch (colorTypeIndex)
	{
	case 0:
		dataToUse = gridSettings.color.data();
		break;
	case 1:
		dataToUse = gridSettings.subGridColor.data();
		break;
	case 2:
		dataToUse = _settings.trailSettings.trailColor.data();
		break;
	}
	ImGui::Spacing();

	changed |= ImGui::ColorPicker4("##ColorPicker", dataToUse, colorFlags);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Trail Settings");
	ImGui::Spacing();
	ImGui::PopItemWidth();
	changed |= ImGui::InputFloat("Point Size", &_settings.trailSettings.trailPointSize, 0.5f, 2.0f, "%.2f");
	changed |= ImGui::InputFloat("Spawn interval", &_settings.trailSettings.pointSpawnInterval, 0.05f, 0.5f, "%.2f");
	changed |= ImGui::InputInt("Max length", &_settings.trailSettings.trailMaxLength);
	ImGui::PushItemWidth(-FLT_MIN);
	changed |= ImGui::Checkbox("Show Trail", &_settings.showTrail);

	_settings.trailSettings.trailPointSize = std::max(_settings.trailSettings.trailPointSize, 0.0f);
	_settings.trailSettings.pointSpawnInterval = std::max(_settings.trailSettings.pointSpawnInterval, 0.0f);
	_settings.trailSettings.trailMaxLength = std::clamp(_settings.trailSettings.trailMaxLength, 0, 10000);
	if (ImGui::Button("Reset Trail", ImVec2(-1, 30))) {
		_clearRobotTrail();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Robot Vector Settings");
	ImGui::Spacing();

	changed |= ImGui::Checkbox("Show Forward Vectors", &_settings.showForwardVectors);
	changed |= ImGui::Checkbox("Show Roller Vectors", &_settings.showRollerVectors);
	changed |= ImGui::Checkbox("Show Wheel Direction Vectors", &_settings.showWheelDirectionVectors);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Frame Rate Limit");
	changed |= ImGui::SliderInt("##FrameRateLimit", &_settings.frameRateLimit, 0, 480, "%d", sliderFlags);
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	// Reset button
	if (ImGui::Button("Reset to Defaults", ImVec2(-1, 30))) {
		_settings = RenderSettings();
		changed = true;
	}

	if (changed && _onSettingsChanged) {
		_onSettingsChanged(_settings);
	}

	ImGui::PopItemWidth();
	ImGui::End();
}