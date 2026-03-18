#ifndef ROBOT_STAT_WINDOW_HPP
#define ROBOT_STAT_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <string>
#include "AppConfig.hpp"
#include "RobotState.hpp"
#include "RobotStatLogger.hpp"

class RobotStatWindow {
public:
	RobotStatWindow(const AppConfig& config);
	~RobotStatWindow();

	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt, const RobotState& robotState);
	void draw();
	bool isOpen() const;
	const WindowConfig& getSavedConfig() const { return _windowConfig; }
private:
	void firstTimeSetup();
	void saveConfig();
	void renderContent(const RobotState& robotState);
	void renderHeader(float windowWidth);
	void renderMotionSection(const RobotState& robotState, float panelW);
	void renderPoseSection(const RobotState& robotState);
	void renderWheelsSection(const RobotState& robotState, float panelW);
	void renderLoggingSection(const RobotState& robotState);

	std::unique_ptr<sf::RenderWindow> _window;
	RobotStatLogger _logger;
	WindowConfig _windowConfig;
	bool _pendingClose = false;
	bool _isOpen = false;
	bool _isLogging = false;
	std::string _lastLogFileName = "";
	std::filesystem::path _logFolder = "";
};

#endif // !ROBOT_STAT_WINDOW_HPP		
