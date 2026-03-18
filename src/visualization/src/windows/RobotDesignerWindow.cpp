#include "windows/RobotDesignerWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <format>
#include <sstream>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"
#include "MathUtils.hpp"
#include "ExportHelper.hpp"
#include "ImportHelper.hpp"
#include "nfd.hpp"

RobotDesignerWindow::RobotDesignerWindow(const AppConfig& config, const RobotConfig& robotConfig)
{
	_windowConfig = config.robotDesignerWindow;
	// Start with one default axle
	loadFromRobotConfig(robotConfig);
	if (_windowConfig.open) {
		open();
	}
}

RobotDesignerWindow::~RobotDesignerWindow() {
	close();
}

void RobotDesignerWindow::open(const RobotConfig& robotConfig) {
	loadFromRobotConfig(robotConfig);
	open();
}

void RobotDesignerWindow::open() {
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
		"Robot Designer",
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

void RobotDesignerWindow::firstTimeSetup() {
	auto screenSize = sf::VideoMode::getDesktopMode().size;

	_windowConfig.size = { 480, 700 };
	_windowConfig.position = { ((int)screenSize.x - _windowConfig.size.x) / 2, ((int)screenSize.y - _windowConfig.size.y) / 2 };
	_windowConfig.resizable = true;
	_windowConfig.open = false;
	_windowConfig.wasOpenedBefore = true;
}

void RobotDesignerWindow::close(bool closeFromRoot) {
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

bool RobotDesignerWindow::isOpen() const {
	return _isOpen;
}

void RobotDesignerWindow::update(sf::Time dt) {
	if (!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);

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

void RobotDesignerWindow::draw() {
	if (!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	_window->clear(sf::Color(30, 30, 30));
	ImGui::SFML::Render(*_window);
	_window->display();
}

void RobotDesignerWindow::saveConfig() {
	_windowConfig.position = FromSFMLVector(_window->getPosition());
	_windowConfig.size = FromSFMLVector(_window->getSize());
	_windowConfig.resizable = true;
}

void RobotDesignerWindow::loadFromRobotConfig(const RobotConfig& config) {
	_axles.clear();

	switch (config.GetRobotDriveType()) {
	case RobotDriveType::DIFFERENTIAL: _driveTypeIndex = 0; break;
	case RobotDriveType::OMNI_WHEEL:   _driveTypeIndex = 1; break;
	case RobotDriveType::MECANUM:       _driveTypeIndex = 2; break;
	}

	for (const auto& axle : config.GetRobotDriveAxels()) {
		_axles.push_back(axle);
	}

	if (_axles.empty()) {
		_axles.push_back(RobotParts::DriveAxle_t{});
	}
}

RobotConfig RobotDesignerWindow::buildRobotConfig() const {
	RobotConfig config;

	switch (_driveTypeIndex) {
	case 0: config.ChangeDriveType(RobotDriveType::DIFFERENTIAL); break;
	case 1: config.ChangeDriveType(RobotDriveType::OMNI_WHEEL); break;
	case 2: config.ChangeDriveType(RobotDriveType::MECANUM); break;
	}

	for (const auto& axel : _axles) {
		config.AddAxel(axel);
	}

	return config;
}

void RobotDesignerWindow::renderAxleEditor(int index) {
	RobotParts::DriveAxle_t& axle = _axles[index];
	std::string id = std::to_string(index);

	float wheelAngleDeg = RadiansToDegree(axle.wheel.wheel_angle);
	float rollerAngleDeg = RadiansToDegree(axle.wheel.roller_angle);

	if (!ImGui::TreeNode(("Axle " + id).c_str())) return;

	ImGui::PushID(index);
	ImGui::PopItemWidth();
	ImGui::TextColored(ImVec4(0.6f, 0.9f, 0.6f, 1.0f), "Wheel");
	ImGui::InputFloat("Diameter (mm)", &axle.wheel.diameter, 1.0f, 10.0f, "%.1f");
	ImGui::InputFloat("X Position (mm)", &axle.wheel.x_position, 1.0f, 10.0f, "%.1f");
	ImGui::InputFloat("Y Position (mm)", &axle.wheel.y_position, 1.0f, 10.0f, "%.1f");
	ImGui::InputFloat("Wheel Angle (deg)", &wheelAngleDeg, 1.0f, 15.0f, "%.1f");
	ImGui::InputFloat("Roller Angle (deg)", &rollerAngleDeg, 1.0f, 15.0f, "%.1f");
	axle.wheel.roller_angle = DegreesToRadians(rollerAngleDeg);
	axle.wheel.wheel_angle = DegreesToRadians(wheelAngleDeg);

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.6f, 1.0f), "Motor");
	ImGui::InputFloat("Max Speed (steps/s)", &axle.motor.hardwareMaxSpeedLimit, 10.0f, 100.0f, "%.0f");
	ImGui::InputFloat("Steps/Rotation", &axle.motor.stepsPerRotation, 1.0f, 10.0f, "%.0f");
	ImGui::InputInt("Pin 1", &axle.motor.pin1);
	ImGui::InputInt("Pin 2", &axle.motor.pin2);
	ImGui::PushItemWidth(-FLT_MIN);

	ImGui::Spacing();
	if (_axles.size() > 1) {
		if (ImGui::Button("Remove Axle")) {
			_axles.erase(_axles.begin() + index);
		}
	}

	ImGui::PopID();
	ImGui::TreePop();
}

void RobotDesignerWindow::renderContent() {
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(windowSize);

	ImGui::Begin("##DesignerContent", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse);

	ImGui::PushItemWidth(-FLT_MIN);
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Robot Designer");
	ImGui::Separator();
	ImGui::Spacing();

	// Drive type
	const char* driveTypes[] = { "Differential", "Omni Wheel", "Mecanum" };
	ImGui::Text("Drive Type");
	ImGui::Combo("##DriveType", &_driveTypeIndex, driveTypes, IM_ARRAYSIZE(driveTypes));
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Axles
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Drive Axles (%d)", (int)_axles.size());
	ImGui::Spacing();

	for (int i = 0; i < (int)_axles.size(); i++) {
		renderAxleEditor(i);
		if (i < (int)_axles.size() - 1) {
			ImGui::Separator();
		}
	}

	ImGui::Spacing();
	if (ImGui::Button("+ Add Axle")) {
		_axles.push_back(RobotParts::DriveAxle_t{});
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Apply button
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Actions");
	ImGui::Spacing();

	if (ImGui::Button("Apply to Simulation", ImVec2(-FLT_MIN, 30))) {
		if (_onRobotConfigApplied) {
			_onRobotConfigApplied(buildRobotConfig());
			_statusMessage = "Config applied to simulation.";
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// File operations
	ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Save / Load");
	ImGui::Spacing();
	float buttonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
	if (ImGui::Button("Save As...", ImVec2(buttonWidth, 0))) {
		NFD::UniquePath outPath;

		nfdfilteritem_t filterItem[1] = { {"Config File", "ini"} };
		nfdresult_t result = NFD::SaveDialog(outPath, filterItem, 1, nullptr, "my_robot_config.ini");

		if (result == NFD_OKAY) {
			std::string savePath = outPath.get();
			RobotConfig config = buildRobotConfig();
			ExportHelper::saveRobotConfigTo(config, std::string(_filePath));
			_statusMessage = "Saved successfully.";
		}
		else  if (result == NFD_ERROR) {
			_statusMessage = std::string("Error: ") + NFD::GetError();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load from", ImVec2(buttonWidth, 0))) {
		NFD::UniquePath outPath;

		nfdfilteritem_t filterItem[1] = {
			{"Configs", "ini"}
		};

		nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);

		if (result == NFD_OKAY) {
			std::string selectedPath = outPath.get();
			RobotConfig loaded = ImportHelper::loadRobotConfigFrom(selectedPath);
			loadFromRobotConfig(loaded);
			_statusMessage = "Loaded successfully.";
		}
		else  if (result == NFD_ERROR) {
			_statusMessage = std::string("Error: ") + NFD::GetError();
		}
	}

	// Status message
	if (!_statusMessage.empty()) {
		ImGui::Spacing();
		ImGui::TextWrapped("%s", _statusMessage.c_str());
	}

	ImGui::PopItemWidth();
	ImGui::End();
}
