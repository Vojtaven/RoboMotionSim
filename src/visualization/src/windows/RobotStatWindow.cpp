#include "windows/RobotStatWindow.hpp"
#include <imgui.h>
#include "MathUtils.hpp"
#include <imgui-SFML.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"
#include "ColorConstants.hpp"
#include "nfd.hpp"

namespace {
    std::filesystem::path getExecutableDir() {
#ifdef _WIN32
        wchar_t buf[MAX_PATH];
        GetModuleFileNameW(nullptr, buf, MAX_PATH);
        return std::filesystem::path(buf).parent_path();
#elif __APPLE__
        char buf[PATH_MAX];
        uint32_t size = sizeof(buf);
        _NSGetExecutablePath(buf, &size);
        return std::filesystem::path(buf).parent_path();
#else // Linux/Unix
        char buf[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buf, PATH_MAX);
        return std::filesystem::path(std::string(buf, count)).parent_path();
#endif
    }

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
    constexpr float LoggingLabelColumnX = 60.0f;
    constexpr int MaxWheelColumns = 3;

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
            Colors::SectionSeparator, 1.0f
        );
        ImGui::Spacing();
    }

    void StatRow(const char* label, float value, const char* unit) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
        ImGui::TextDisabled("%s", label);
        ImGui::SameLine(LabelColumnX);
        ImVec4 valColor = (std::abs(value) > ValueEpsilon)
            ? Colors::ActiveValue
            : Colors::InactiveValue;
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
            ? Colors::HighVelocityBar
            : Colors::NormalVelocityBar;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::BarBackground);
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
        ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::BarBackground);
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

RobotStatWindow::RobotStatWindow(const AppConfig& config, const sf::Image& icon)
	: SettingsWindowBase(icon), _logFolder(getExecutableDir() / "logs")
{
	_windowConfig = config.robotStatWindow;
	if (_windowConfig.open) {
		open();
	}
}

void RobotStatWindow::open() {
	if (!_windowConfig.wasOpenedBefore) {
		firstTimeSetup({420, 600});
	}
	openWindow("Robot Stats");
}

void RobotStatWindow::close(bool closeFromRoot) {
	closeWindow(closeFromRoot);
}

void RobotStatWindow::update(sf::Time dt, const RobotState& robotState) {
	if (!isOpen()) return;

	if (!processEvents()) return;

	ImGui::SFML::Update(*_window, dt);

	if (_isLogging)
		_logger.logStats(std::chrono::system_clock::now(), robotState);

	renderContent(robotState);
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

	DrawSectionSeparator(panelW);
	renderLoggingSection(robotState);

	ImGui::PopStyleVar();
	ImGui::PopItemWidth();
	ImGui::End();
}

void RobotStatWindow::renderHeader(float windowWidth) {
    ImGui::TextColored(Colors::StatsTitle, "  ROBOT STATS");
    ImVec2 headerEnd = ImGui::GetCursorScreenPos();
    float x = ImGui::GetWindowPos().x;
    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(x, headerEnd.y),
        ImVec2(x + windowWidth, headerEnd.y),
        Colors::HeaderLine, 1.0f
    );
    ImGui::Spacing();
    ImGui::Spacing();
}

void RobotStatWindow::renderMotionSection(const RobotState& robotState, float panelW) {
    ImVec2 cardStart = ImGui::GetCursorScreenPos();
    ImGui::PushStyleColor(ImGuiCol_Header, Colors::SectionHeaderBg);

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() +SectionHeaderIndent);
    ImGui::TextColored(Colors::MotionSectionTitle, "  Motion");
    ImGui::Spacing();

    StatRow("Angular Vel", static_cast<float>(RadiansToDegrees(robotState.angularVelocity)), "deg/s");
    StatRow("Front Angular Vel", static_cast<float>(RadiansToDegrees(robotState.frontAngularVelocity)), "deg/s");
    ImGui::Spacing();
    VelocityBar("Local Vel X", static_cast<float>(robotState.localVelocity.x), panelW);
    VelocityBar("Local Vel Y", static_cast<float>(robotState.localVelocity.y), panelW);
    VelocityBar("Global Vel X", static_cast<float>(robotState.globalVelocity.x), panelW);
    VelocityBar("Global Vel Y", static_cast<float>(robotState.globalVelocity.y), panelW);
    ImGui::Spacing();

    ImVec2 cardEnd = ImGui::GetCursorScreenPos();
    DrawAccentBar(cardStart, cardEnd, Colors::MotionAccent);
    ImGui::PopStyleColor();
}

void RobotStatWindow::renderPoseSection(const RobotState& robotState) {
    ImVec2 cardStart = ImGui::GetCursorScreenPos();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SectionHeaderIndent);
    ImGui::TextColored(Colors::PoseSectionTitle, "  Pose");
    ImGui::Spacing();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
    ImGui::TextDisabled("Position");
    ImGui::SameLine(LabelColumnX);
    ImGui::TextColored(Colors::ActiveValue, "(%.1f,", robotState.position.x);
    ImGui::SameLine();
    ImGui::TextColored(Colors::ActiveValue, "%.1f) mm", robotState.position.y);

    StatRow("Chassis Angle", static_cast<float>(RadiansToDegrees(robotState.chassisAngle)), "deg");
    StatRow("Front Angle", static_cast<float>(RadiansToDegrees(robotState.frontAngle)), "deg");
    ImGui::Spacing();

    ImVec2 cardEnd = ImGui::GetCursorScreenPos();
    DrawAccentBar(cardStart, cardEnd, Colors::PoseAccent);
}

void RobotStatWindow::renderWheelsSection(const RobotState& robotState, float panelW) {
    ImVec2 cardStart = ImGui::GetCursorScreenPos();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SectionHeaderIndent);
    ImGui::TextColored(Colors::WheelSectionTitle, "  Wheels");
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

        ImGui::TextColored(Colors::WheelSectionTitle, "W%d", i + 1);
        WheelProgressBar(wheel.speed, colW, Colors::WheelSpeedBar, "speed mm/s", "spd");
        WheelProgressBar(wheel.rollerSpeed, colW, Colors::RollerSpeedBar, "rol mm/s", "rol");

        ImGui::EndGroup();
        ImGui::PopID();

        if (col == columns - 1 || i == robotState.wheelCount - 1)
            ImGui::Spacing();
    }

    ImGui::Spacing();

    ImVec2 cardEnd = ImGui::GetCursorScreenPos();
    DrawAccentBar(cardStart, cardEnd, Colors::WheelAccent);
}

void RobotStatWindow::renderLoggingSection(const RobotState& robotState) {
    ImVec2 cardStart = ImGui::GetCursorScreenPos();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + SectionHeaderIndent);
    ImGui::TextColored(Colors::LoggingSectionTitle, "  Logging");
    ImGui::Spacing();

    if(ImGui::Button("Find log folder", ImVec2(-FLT_MIN, 0))) {
        NFD::UniquePath outPath;

        nfdresult_t result = NFD::PickFolder(outPath);

        if (result == NFD_OKAY) {
            _logFolder = outPath.get();
        }
    }


    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
    ImGui::TextDisabled("Dir:");
    ImGui::SameLine(LoggingLabelColumnX);
    ImGui::TextDisabled("%s", _logFolder.string().c_str());

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
    ImGui::TextDisabled("Log:");
    ImGui::SameLine(LoggingLabelColumnX);
    ImGui::TextDisabled("%s", _lastLogFileName.c_str());

    ImGui::Spacing();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);

    if (!_isLogging) {
        if (ImGui::Button("Start Logging", ImVec2(-FLT_MIN, 0))) {
			_lastLogFileName = "log_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".csv";
            auto fullPath = _logFolder / _lastLogFileName;
            _logger.startLogging(fullPath.string(), robotState.wheelCount, true);
            _isLogging = true;
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button,        Colors::DangerButton);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Colors::DangerButtonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  Colors::DangerButtonActive);
        if (ImGui::Button("Stop Logging", ImVec2(-FLT_MIN, 0))) {
            _logger.stopLogging();
            _isLogging = false;
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::Spacing();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + RowIndent);
    if (_isLogging) {
        ImGui::TextColored(Colors::RecordingText, "[REC] Recording...");
    } else {
        ImGui::TextDisabled("[---] Idle");
    }
    ImGui::Spacing();

    ImVec2 cardEnd = ImGui::GetCursorScreenPos();
    DrawAccentBar(cardStart, cardEnd, Colors::LoggingAccent);
}
