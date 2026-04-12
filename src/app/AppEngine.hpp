// Author: Vojtech Venzara
// Date: 2026-04-02
// Description: Main application orchestrator that coordinates initialization and execution of all
//              simulation subsystems including physics, visualization, input, and configuration management

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
	std::unique_ptr<ConfigManager> _configManager;
	std::unique_ptr<VisualizationEngine> _vizEngine;
	std::unique_ptr<PhysicsEngine> _physicsEngine;
	std::unique_ptr<RobotState> _robotState;
	std::unique_ptr<InputManager> _inputManager;
	ScopedSubscription<RobotConfig> _robotConfigSubscription;
};

#endif // !APP_ENGINE_HPP
