#ifndef APP_ENGINE_HPP
#define APP_ENGINE_HPP

#include "VisualizationEngine.hpp"
#include "ConfigManager.hpp"
#include "PhysicsEngine.hpp"
#include "RobotState.hpp"
#include <memory>
class AppEngine {
public:
	AppEngine();
	void run();

private:
	std::unique_ptr<ConfigManager> configManager;
	std::unique_ptr < VisualizationEngine> vizEngine;
	std::unique_ptr < PhysicsEngine> physicsEngine;
	std::unique_ptr<RobotState> robotState;
};

#endif // !APP_ENGINE_HPP
