#include "windows/RobotDesignerWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdint>
#include <sstream>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"
#include "ColorConstants.hpp"
#include "MathUtils.hpp"
#include "ExportHelper.hpp"
#include "ImportHelper.hpp"
#include "nfd.hpp"
#include "shapes/RobotShape.hpp"

RobotDesignerWindow::RobotDesignerWindow(const AppConfig& config, const RobotConfig& robotConfig, const sf::Image& icon)
	: _icon(icon)
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
	_window->clear(Colors::WindowClearColor);
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

	_driveTypeIndex = (int)config.GetRobotDriveType();

	for (const auto& axle : config.GetRobotDriveAxles()) {
		_axles.push_back(axle);
	}

	if (_axles.empty()) {
		_axles.push_back(RobotParts::DriveAxle_t{});
	}
}

RobotConfig RobotDesignerWindow::buildRobotConfig() const {
	RobotConfig config;

	config.ChangeDriveType((RobotDriveType)_driveTypeIndex);
	float rollerAngle;
	switch ((RobotDriveType)_driveTypeIndex) {
	case RobotDriveType::DIFFERENTIAL:	rollerAngle = 0.0f;						break;
	case RobotDriveType::OMNI_WHEEL:	rollerAngle = DegreesToRadians(90.0f);	break;
	case RobotDriveType::MECANUM:		rollerAngle = DegreesToRadians(45.0f);	break;
	default:							rollerAngle = 0.0f;						break;
	}

	for (auto& axel : _axles) {
		RobotParts::DriveAxle_t copy = axel;
		if ((RobotDriveType)_driveTypeIndex != RobotDriveType::CUSTOM)
			copy.wheel.roller_angle = rollerAngle;
		config.AddAxle(copy);
	}

	return config;
}

void RobotDesignerWindow::renderAxleEditor(int index, bool renderRollerAngle) {
	RobotParts::DriveAxle_t& axle = _axles[index];
	std::string id = std::to_string(index);

	float wheelAngleDeg = RadiansToDegrees(axle.wheel.wheel_angle);
	float rollerAngleDeg = RadiansToDegrees(axle.wheel.roller_angle);

	if (!ImGui::TreeNode(("Axle " + id).c_str())) return;

	ImGui::PushID(index);
	ImGui::PopItemWidth();
	ImGui::TextColored(Colors::WheelLabel, "Wheel");
	ImGui::InputFloat("Diameter (mm)", &axle.wheel.diameter, 1.0f, 10.0f, "%.1f");
	ImGui::InputFloat("X Position (mm)", &axle.wheel.x_position, 1.0f, 10.0f, "%.1f");
	ImGui::InputFloat("Y Position (mm)", &axle.wheel.y_position, 1.0f, 10.0f, "%.1f");
	ImGui::InputFloat("Wheel Angle (deg)", &wheelAngleDeg, 1.0f, 15.0f, "%.1f");
	if (renderRollerAngle)
		ImGui::InputFloat("Roller Angle (deg)", &rollerAngleDeg, 1.0f, 15.0f, "%.1f");
	axle.wheel.roller_angle = DegreesToRadians(rollerAngleDeg);
	axle.wheel.wheel_angle = DegreesToRadians(wheelAngleDeg);

	ImGui::Spacing();
	ImGui::TextColored(Colors::MotorLabel, "Motor");
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

void RobotDesignerWindow::renderPreview() {
	float availWidth = ImGui::GetWindowWidth()
		- ImGui::GetStyle().WindowPadding.x * 2.0f
		- ImGui::GetStyle().ScrollbarSize;
	float availHeight = availWidth * 0.75f;
	if (availWidth < 1.0f || availHeight < 1.0f) return;

	auto texW = static_cast<unsigned int>(availWidth);
	auto texH = static_cast<unsigned int>(availHeight);

	if (!_previewTexture || _previewTexture->getSize() != sf::Vector2u{ texW, texH }) {
		_previewTexture = std::make_unique<sf::RenderTexture>(sf::Vector2u{ texW, texH });
		_previewTexture->setSmooth(true);
	}

	RobotConfig config = buildRobotConfig();
	RobotShape shape(config);
	sf::FloatRect bounds = shape.getLocalBounds();

	float padding = 1.2f;
	float viewW = bounds.size.x * padding;
	float viewH = bounds.size.y * padding;

	if (viewW < 1.0f) viewW = 100.0f;
	if (viewH < 1.0f) viewH = 100.0f;

	float aspect = availWidth / availHeight;
	float boundsAspect = viewW / viewH;
	if (boundsAspect > aspect) {
		viewH = viewW / aspect;
	}
	else {
		viewW = viewH * aspect;
	}

	sf::Vector2f center = bounds.position + bounds.size / 2.0f;
	sf::View view(center, { viewW, viewH });
	_previewTexture->setView(view);

	_previewTexture->clear(Colors::PreviewBackground);
	_previewTexture->draw(shape);
	_previewTexture->display();

	ImGui::Image(*_previewTexture, sf::Vector2f{ availWidth, availHeight });
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
	ImGui::TextColored(Colors::WindowHeader, "Robot Designer");
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
	ImGui::TextColored(Colors::WindowHeader, "Drive Axles (%d)", (int)_axles.size());
	ImGui::Spacing();

	for (int i = 0; i < (int)_axles.size(); i++) {
		renderAxleEditor(i, _driveTypeIndex == (int)RobotDriveType::CUSTOM);
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

	// Robot preview
	ImGui::Checkbox("Show Preview", &_showPreview);
	if (_showPreview) {
		ImGui::Spacing();
		renderPreview();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Apply button
	ImGui::TextColored(Colors::WindowHeader, "Actions");
	ImGui::Spacing();

	if (ImGui::Button("Apply to Simulation", ImVec2(-FLT_MIN, 30))) {
		if (_onRobotConfigApplied) {
			RobotConfig config = buildRobotConfig();
			auto validationError = config.validateConfig();
			if(validationError == std::nullopt) {
				_onRobotConfigApplied(config);
				_statusMessage = StatusMessage::Info("Config applied to simulation.");
			}
			else {
				_statusMessage = StatusMessage::Error("Config validation error: " + *validationError);
			}
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// File operations
	ImGui::TextColored(Colors::WindowHeader, "Save / Load");
	ImGui::Spacing();
	float buttonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
	if (ImGui::Button("Save As...", ImVec2(buttonWidth, 0))) {
		NFD::UniquePath outPath;

		nfdfilteritem_t filterItem[1] = { {"Config File", "ini"} };
		nfdresult_t result = NFD::SaveDialog(outPath, filterItem, 1, nullptr, "my_robot_config.ini");

		if (result == NFD_OKAY) {
			std::string savePath = outPath.get();
			RobotConfig config = buildRobotConfig();
			ExportHelper::saveRobotConfigTo(config, savePath);
			_statusMessage = StatusMessage::Info("Saved successfully.");
		}
		else  if (result == NFD_ERROR) {
			_statusMessage = StatusMessage::Error(std::string("Error: ") + NFD::GetError());
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
			_statusMessage = StatusMessage::Info("Loaded successfully.");
		}
		else  if (result == NFD_ERROR) {
			_statusMessage = StatusMessage::Error(std::string("Error: ") + NFD::GetError());
		}
	}

	// Status message
	if (!_statusMessage.message.empty()) {
		ImGui::OpenPopup("Status Message");
	}

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(displaySize.x * 0.5f, displaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Status Message", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::PushTextWrapPos(400.0f); // Set wrap width as needed
		ImGui::TextColored(
			_statusMessage.isError ? Colors::ErrorStatusText : Colors::SuccessStatusText,
			"%s", _statusMessage.message.c_str()
		);
		float windowWidth = ImGui::GetWindowSize().x;
		float okButtonWidth = 80.0f;
		ImGui::SetCursorPosX((windowWidth - okButtonWidth) * 0.5f);
		if (ImGui::Button("OK", ImVec2(okButtonWidth, 0))) {
			_statusMessage.message.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}


	ImGui::PopItemWidth();
	ImGui::End();
}
