#pragma once

#include <imgui.h>
#include <SFML/Graphics/Color.hpp>

namespace Colors {

// ── Shared Base Palette ─────────────────────────────────────────────
constexpr ImVec4 PrimaryBlue(0.4f, 0.8f, 1.0f, 1.0f);
constexpr ImVec4 AccentBlue(0.2f, 0.7f, 1.0f, 1.0f);
constexpr ImVec4 AccentPurple(0.85f, 0.55f, 1.0f, 1.0f);
constexpr ImVec4 AccentOrange(1.0f, 0.72f, 0.3f, 1.0f);
constexpr ImVec4 AccentGreen(0.4f, 1.0f, 0.7f, 1.0f);

constexpr ImVec4 DangerRed(0.65f, 0.1f, 0.1f, 1.0f);
constexpr ImVec4 DangerRedHover(0.8f, 0.2f, 0.2f, 1.0f);
constexpr ImVec4 DangerRedActive(0.5f, 0.05f, 0.05f, 1.0f);

constexpr ImVec4 NeutralGray20(0.2f, 0.2f, 0.2f, 1.0f);
constexpr ImVec4 NeutralGray30(0.3f, 0.3f, 0.3f, 1.0f);
constexpr ImVec4 NeutralGray35(0.35f, 0.35f, 0.35f, 1.0f);
constexpr ImVec4 NeutralGray45(0.45f, 0.45f, 0.45f, 1.0f);

// ── Window & Section Headers ────────────────────────────────────────
// Primary header color used across all windows (light blue)
constexpr ImVec4 WindowHeader = PrimaryBlue;
// Robot Stats main title (slightly brighter blue)
constexpr ImVec4 StatsTitle = PrimaryBlue;

// ── Section Title Colors (RobotStatWindow) ──────────────────────────
constexpr ImVec4 MotionSectionTitle = PrimaryBlue;
constexpr ImVec4 PoseSectionTitle = AccentPurple;
constexpr ImVec4 WheelSectionTitle = AccentOrange;
constexpr ImVec4 LoggingSectionTitle = AccentGreen;

// ── Section Accent Bar Colors ───────────────────────────────────────
constexpr ImVec4 MotionAccent = AccentBlue;
constexpr ImVec4 PoseAccent = AccentPurple;
constexpr ImVec4 WheelAccent = AccentOrange;
constexpr ImVec4 LoggingAccent = AccentGreen;

// ── Section Header Background ───────────────────────────────────────
constexpr ImVec4 SectionHeaderBg(0.1f, 0.14f, 0.18f, 1.0f);

// ── Value Display Colors ────────────────────────────────────────────
constexpr ImVec4 ActiveValue(0.4f, 1.0f, 0.6f, 1.0f);
constexpr ImVec4 InactiveValue(0.55f, 0.55f, 0.55f, 1.0f);

// ── Progress Bar Colors ─────────────────────────────────────────────
constexpr ImVec4 BarBackground(0.15f, 0.18f, 0.22f, 1.0f);
constexpr ImVec4 HighVelocityBar(1.0f, 0.5f, 0.2f, 1.0f);
constexpr ImVec4 NormalVelocityBar(0.2f, 0.8f, 0.45f, 1.0f);
constexpr ImVec4 WheelSpeedBar(1.0f, 0.0f, 0.0f, 1.0f);
constexpr ImVec4 RollerSpeedBar(0.0f, 1.0f, 0.0f, 1.0f);

// ── Line / Separator Colors (ImU32) ────────────────────────────────
constexpr ImU32 HeaderLine = IM_COL32(0, 180, 255, 60);
constexpr ImU32 SectionSeparator = IM_COL32(255, 255, 255, 18);

// ── Error / Status Colors ───────────────────────────────────────────
constexpr ImVec4 ErrorTitleBg = DangerRed;
constexpr ImVec4 ErrorTitleBgActive(0.7f, 0.1f, 0.1f, 1.0f);
constexpr ImVec4 ErrorText(1.0f, 0.4f, 0.4f, 1.0f);
constexpr ImVec4 ErrorStatusText(1.0f, 0.6f, 0.6f, 1.0f);
constexpr ImVec4 SuccessStatusText(0.6f, 0.9f, 0.6f, 1.0f);
constexpr ImVec4 RecordingText(0.3f, 1.0f, 0.4f, 1.0f);

// ── Danger Button Colors (Stop / Delete) ────────────────────────────
constexpr ImVec4 DangerButton = DangerRed;
constexpr ImVec4 DangerButtonHovered = DangerRedHover;
constexpr ImVec4 DangerButtonActive = DangerRedActive;

// ── Overlay Text (FPS, Timestamp) ───────────────────────────────────
constexpr ImVec4 OverlayText(1.0f, 0.0f, 0.0f, 1.0f);

// ── Global ImGui Style Colors ───────────────────────────────────────
constexpr ImVec4 StyleWindowBg(0.1f, 0.1f, 0.1f, 0.95f);
constexpr ImVec4 StyleTitleBg = NeutralGray20;
constexpr ImVec4 StyleTitleBgActive = NeutralGray30;
constexpr ImVec4 StyleButton(0.25f, 0.25f, 0.25f, 1.0f);
constexpr ImVec4 StyleButtonHovered = NeutralGray35;
constexpr ImVec4 StyleButtonActive = NeutralGray45;

// ── Designer Labels ─────────────────────────────────────────────────
constexpr ImVec4 WheelLabel(0.6f, 0.9f, 0.6f, 1.0f);
constexpr ImVec4 MotorLabel(0.9f, 0.6f, 0.6f, 1.0f);

// ── Controller Mapping ──────────────────────────────────────────────
constexpr ImVec4 JoystickControlLabel(0.8f, 0.8f, 0.2f, 1.0f);

// ── SFML Rendering Colors ───────────────────────────────────────────
inline const sf::Color WindowClearColor(30, 30, 30);
inline const sf::Color PreviewBackground(20, 20, 20);
inline const sf::Color RobotBaseFill(50, 50, 50);
inline const sf::Color GridTextBackground(0, 0, 0, 150);

// Robot shape vector colors
inline const sf::Color RobotOutline       = sf::Color::Cyan;
inline const sf::Color FrontVector        = sf::Color::White;
inline const sf::Color DirectionVector    = sf::Color::Magenta;
inline const sf::Color WheelForwardVector = sf::Color::Red;
inline const sf::Color WheelRollerVector  = sf::Color::Green;
inline const sf::Color WheelDirVector     = sf::Color::Yellow;

} // namespace Colors
