#ifndef APP_ENGINE_HPP
#define APP_ENGINE_HPP

#include "VisualizationEngine.hpp"
#include "ConfigManager.hpp"
#include "PhysicsEngine.hpp"
#include "RobotState.hpp"
#include "InputManager.hpp"
#include <memory>
#include <chrono>
class AppEngine {
public:
	AppEngine();
	void run();

private:
	std::chrono::system_clock::time_point _wallTime;
	std::unique_ptr<ConfigManager> configManager;
	std::unique_ptr < VisualizationEngine> vizEngine;
	std::unique_ptr < PhysicsEngine> physicsEngine;
	std::unique_ptr<RobotState> robotState;
	std::unique_ptr<InputManager> inputManager;
};

#endif // !APP_ENGINE_HPP
