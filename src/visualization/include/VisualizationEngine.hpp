#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include "RobotConfig.hpp"
#include "RobotState.hpp"
#include "AppConfig.hpp"
#include "ConfigSection.hpp"
#include "windows/MainWindow.hpp"
#include <chrono>
class VisualizationEngine {
public:
	VisualizationEngine(AppConfig& appConfig, ConfigSection<RobotConfig>& robotConfigSection, const RobotConfig& robotConfig = RobotConfig());
	void setRobotConfig(const RobotConfig& config, const RobotState& state) { _mainWindow->setRobotConfig(config, state); }
	void draw() { _mainWindow->draw(); }
	void update(float dt, const RobotState& state, const std::chrono::system_clock::time_point& timeStamp) { _mainWindow->update(dt, state, timeStamp); }
	sf::Window& getMainWindow()	{ return _mainWindow->getWindow(); }
	AppConfig& getSavedAppConfig() { return _mainWindow->getSavedAppConfig(); }
	bool isWindowOpen() const { return _mainWindow->isOpen(); }
	bool hasFocus() const { return _mainWindow->getWindow().hasFocus(); }
	// Returns in normal units
	Vec2f getWindowCenter() const { return _mainWindow->getRenderWindowCenter(); }
	void showErrorMessage(const std::string& message) { _mainWindow->showErrorMessage(message); }
private:
	std::unique_ptr<MainWindow> _mainWindow;
};

#endif //ROBOT_VISUALIZATION_HPP