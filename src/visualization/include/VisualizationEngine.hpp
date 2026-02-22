#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include "RobotConfig.hpp"
#include "RobotState.hpp"
#include "AppConfig.hpp"
#include "windows/MainWindow.hpp"

class VisualizationEngine {
public:
	VisualizationEngine(const AppConfig& appConfig, const RobotConfig& robotConfig = RobotConfig());
	void setRobotConfig(const RobotConfig& config, bool holdPosition = false) { _mainWindow->setRobotConfig(config, holdPosition); }
	void draw() { _mainWindow->draw(); }
	void update(float dt, const RobotState& state) { _mainWindow->update(dt, state); }
	sf::Window& getMainWindow()	{ return _mainWindow->getWindow(); }
	AppConfig& getSavedAppConfig() { return _mainWindow->getSavedAppConfig(); }
	bool isWindowOpen() const { return _mainWindow->isOpen(); }
	// Returns in normal units
	Vec2f getWindowCenter() const { return _mainWindow->getRenderWindowCenter(); }
private:
	void CreateMainWindow(AppConfig& appConfig);
	std::unique_ptr<MainWindow> _mainWindow;
};

#endif //ROBOT_VISUALIZATION_HPP