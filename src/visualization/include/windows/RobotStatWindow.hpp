// Author: Vojtech Venzara
// Date: 2026-03-21
// Description: Statistics and telemetry display window showing real-time robot state data and performance metrics

#ifndef ROBOT_STAT_WINDOW_HPP
#define ROBOT_STAT_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <string>
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include "RobotStatLogger.hpp"
#include "windows/SettingsWindowBase.hpp"

class RobotStatWindow : public SettingsWindowBase {
public:
	RobotStatWindow(const AppConfig& config, const sf::Image& icon);
	~RobotStatWindow() = default;

	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt, const RobotState& robotState);
private:
	void renderContent(const RobotState& robotState);
	void renderHeader(float windowWidth);
	void renderMotionSection(const RobotState& robotState, float panelW);
	void renderPoseSection(const RobotState& robotState);
	void renderWheelsSection(const RobotState& robotState, float panelW);
	void renderLoggingSection(const RobotState& robotState);

	RobotStatLogger _logger;
	bool _isLogging = false;
	std::string _lastLogFileName = "";
	std::filesystem::path _logFolder = "";
};

#endif // !ROBOT_STAT_WINDOW_HPP
