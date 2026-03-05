#include "windows/RobotStatWindow.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <algorithm>
#include <cstdint>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"

namespace {
    constexpr float MaxVelocity = 3000.0f;
    constexpr float LabelColumnX = 150.0f;
    constexpr float RowIndent = 10.0f;
    constexpr float SectionHeaderIndent = 8.0f;
    constexpr float AccentBarWidth = 3.0f;
    constexpr float VelocityBarHeight = 13.0f;
    constexpr float WheelBarHeight = 14.0f;
    constexpr float VelocityBarOffset = 160.0f;
    constexpr float HighVelocityThreshold = 0.5f;
    constexpr float ValueEpsilon = 0.001f;
    constexpr float WheelColumnGap = 6.0f;
    constexpr float WheelColumnIndent = 18.0f;
    constexpr float MinColWidthForFullLabel = 60.0f;
    constexpr int MaxWheelColumns = 3;

    const ImVec4 ActiveValueColor(0.4f, 1.0f, 0.6f, 1.0f);
    const ImVec4 InactiveValueColor(0.55f, 0.55f, 0.55f, 1.0f);
    const ImVec4 BarBackgroundColor(0.15f, 0.18f, 0.22f, 1.0f);
    const ImVec4 WheelTitleColor(1.0f, 0.72f, 0.3f, 1.0f);
    const ImU32 HeaderLineColor = IM_COL32(0, 180, 255, 60);
    const ImU32 SeparatorColor = IM_COL32(255, 255, 255, 18);

    constexpr ImGuiWindowFlags FullscreenWindowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    void DrawAccentBar(ImVec2 start, ImVec2 end, ImVec4 color) {
        ImGui::GetWindowDrawList()->AddRectFilled(
            start,
            ImVec2(start.x + AccentBarWidth, end.y),
            ImGui::ColorConvertFloat4ToU32(color)
        );
    }

    void DrawSectionSeparator(float panelW) {
        ImGui::Spacing();
        float y = ImGui::GetCursorScreenPos().y;
        float x = ImGui::GetWindowPos().x;
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(x + SectionHeaderIndent, y),
            ImVec2(x + panelW, y),
            SeparatorColor, 1.0f
        );
        ImGui::Spacing();
    }

    void StatRow(const char* label, float value, const char* unit) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
        ImGui::TextDisabled("%s", label);
        ImGui::SameLine(LabelColumnX);
        ImVec4 valColor = (std::abs(value) > ValueEpsilon)
            ? ActiveValueColor
            : InactiveValueColor;
        ImGui::TextColored(valColor, "%.2f", value);
        ImGui::SameLine();
        ImGui::TextDisabled("%s", unit);
    }

    void VelocityBar(const char* label, float value, float panelW) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
        ImGui::TextDisabled("%s", label);
        ImGui::SameLine(LabelColumnX);
        float fraction = std::min(std::abs(value) / MaxVelocity, 1.0f);
        ImVec4 barColor = fraction > HighVelocityThreshold
            ? ImVec4(1.0f, 0.5f, 0.2f, 1.0f)
            : ImVec4(0.2f, 0.8f, 0.45f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, BarBackgroundColor);
        char overlay[32];
        snprintf(overlay, sizeof(overlay), "%.1f mm/s", value);
        ImGui::ProgressBar(fraction, ImVec2(panelW - VelocityBarOffset, VelocityBarHeight), overlay);
        ImGui::PopStyleColor(2);
    }

    void WheelProgressBar(float value, float colW, ImVec4 barColor, const char* fullLabel, const char* shortLabel) {
        float absVal = std::abs(value);
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f", value);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, BarBackgroundColor);
        ImGui::ProgressBar(std::min(absVal / MaxVelocity, 1.f), ImVec2(colW, WheelBarHeight), buf);
        ImGui::PopStyleColor(2);
        ImGui::TextDisabled(colW > MinColWidthForFullLabel ? fullLabel : shortLabel);
    }

    int GetWheelColumnCount(int wheelCount) {
        if (wheelCount <= 3) return wheelCount;
        if (wheelCount <= 4) return 2;
        return MaxWheelColumns;
    }
} // anonymous namespace

RobotStatWindow::RobotStatWindow(const AppConfig& config)
{
	_windowConfig = config.robotStatWindow;
	if (_windowConfig.open) {
		open();
	}
}

RobotStatWindow::~RobotStatWindow() {
	close();
}

void RobotStatWindow::open() {
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
		"Robot Stats",
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

void RobotStatWindow::firstTimeSetup() {
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
void RobotStatWindow::close(bool closeFromRoot) {
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

bool RobotStatWindow::isOpen() const {
	return _isOpen;
}

void RobotStatWindow::update(sf::Time dt, const RobotState& robotState) {
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

	renderContent(robotState);
}

void RobotStatWindow::draw() {
	if (!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	_window->clear(sf::Color(30, 30, 30));
	ImGui::SFML::Render(*_window);
	_window->display();
}

void RobotStatWindow::saveConfig() {
	_windowConfig.position = FromSFMLVector(_window->getPosition());
	_windowConfig.size = FromSFMLVector(_window->getSize());
	_windowConfig.resizable = true;
}

void RobotStatWindow::renderContent(const RobotState& robotState) {
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(windowSize);
	ImGui::Begin("##RobotStats", nullptr, FullscreenWindowFlags);

	ImGui::PushItemWidth(-FLT_MIN);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 5));

	float panelW = windowSize.x - ImGui::GetStyle().WindowPadding.x * 2;

	renderHeader(windowSize.x);
	renderMotionSection(robotState, panelW);
	DrawSectionSeparator(panelW);
	renderPoseSection(robotState);
	DrawSectionSeparator(panelW);
	renderWheelsSection(robotState, panelW);

	ImGui::PopStyleVar();
	ImGui::PopItemWidth();
	ImGui::End();
}

void RobotStatWindow::renderHeader(float windowWidth) {
    ImGui::TextColored(ImVec4(0.3f, 0.85f, 1.0f, 1.0f), "  ROBOT STATS");
    ImVec2 headerEnd = ImGui::GetCursorScreenPos();
    float x = ImGui::GetWindowPos().x;
    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(x, headerEnd.y),
        ImVec2(x + windowWidth, headerEnd.y),
        HeaderLineColor, 1.0f
    );
    ImGui::Spacing();
    ImGui::Spacing();
}

void RobotStatWindow::renderMotionSection(const RobotState& robotState, float panelW) {
    ImVec2 cardStart = ImGui::GetCursorScreenPos();
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.14f, 0.18f, 1.0f));

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() +SectionHeaderIndent);
    ImGui::TextColored(ImVec4(0.35f, 0.75f, 1.0f, 1.0f), "  Motion");
    ImGui::Spacing();

    StatRow("Angular Vel", robotState.angularVelocity, "rad/s");
    StatRow("Front Angular Vel", robotState.frontAngularVelocity, "rad/s");
    ImGui::Spacing();
    VelocityBar("Local Vel X", robotState.localVelocity.x, panelW);
    VelocityBar("Local Vel Y", robotState.localVelocity.y, panelW);
    VelocityBar("Global Vel X", robotState.globalvelocity.x, panelW);
    VelocityBar("Global Vel Y", robotState.globalvelocity.y, panelW);
    ImGui::Spacing();

    ImVec2 cardEnd = ImGui::GetCursorScreenPos();
    DrawAccentBar(cardStart, cardEnd, ImVec4(0.2f, 0.7f, 1.0f, 1.0f));
    ImGui::PopStyleColor();
}

void RobotStatWindow::renderPoseSection(const RobotState& robotState) {
    ImVec2 cardStart = ImGui::GetCursorScreenPos();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SectionHeaderIndent);
    ImGui::TextColored(ImVec4(0.85f, 0.55f, 1.0f, 1.0f), "  Pose");
    ImGui::Spacing();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
    ImGui::TextDisabled("Position");
    ImGui::SameLine(LabelColumnX);
    ImGui::TextColored(ActiveValueColor, "(%.1f,", robotState.position.x);
    ImGui::SameLine();
    ImGui::TextColored(ActiveValueColor, "%.1f) mm", robotState.position.y);

    StatRow("Chassis Angle", robotState.chassisAngle, "deg");
    StatRow("Front Angle", robotState.frontAngle, "deg");
    ImGui::Spacing();

    ImVec2 cardEnd = ImGui::GetCursorScreenPos();
    DrawAccentBar(cardStart, cardEnd, ImVec4(0.8f, 0.4f, 1.0f, 1.0f));
}

void RobotStatWindow::renderWheelsSection(const RobotState& robotState, float panelW) {
    ImVec2 cardStart = ImGui::GetCursorScreenPos();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SectionHeaderIndent);
    ImGui::TextColored(WheelTitleColor, "  Wheels");
    ImGui::Spacing();

    int columns = GetWheelColumnCount(robotState.wheelCount);
    float colW = (panelW - WheelColumnIndent - WheelColumnGap * (columns - 1)) / columns;

    for (int i = 0; i < robotState.wheelCount; i++) {
        const auto& wheel = robotState.wheels[i];
        int col = i % columns;

        if (col == 0)
            ImGui::SetCursorPosX(WheelColumnIndent);
        else
            ImGui::SameLine(0, WheelColumnGap);

        ImGui::PushID(i);
        ImGui::BeginGroup();

        ImGui::TextColored(WheelTitleColor, "W%d", i + 1);
        WheelProgressBar(wheel.speed, colW, ImVec4(1.f, 0, 0, 1.f), "speed mm/s", "spd");
        WheelProgressBar(wheel.rollerSpeed, colW, ImVec4(0, 1, 0, 1.f), "rol mm/s", "rol");

        ImGui::EndGroup();
        ImGui::PopID();

        if (col == columns - 1 || i == robotState.wheelCount - 1)
            ImGui::Spacing();
    }

    ImGui::Spacing();

    ImVec2 cardEnd = ImGui::GetCursorScreenPos();
    DrawAccentBar(cardStart, cardEnd, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
}