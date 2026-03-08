#include "AppEngine.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <asio.hpp>
#include "VisualizationEngine.hpp"
#include "RobotConfig.hpp"
#include "ConfigManager.hpp"
#include <numbers> 
#include "PhysicsEngine.hpp"
#include <chrono>
#include "MathUtils.hpp"
#include <memory>
using Clock = std::chrono::steady_clock;

AppEngine::AppEngine() 
{
	int screenWidth = sf::VideoMode::getDesktopMode().size.x;
	int screenHeight = sf::VideoMode::getDesktopMode().size.y;
	physicsEngine = std::make_unique<PhysicsEngine>();

	configManager = std::make_unique<ConfigManager>(Vec2i{ screenWidth,screenHeight });
	auto& robotConfig = configManager->getConstRobotConfig();
	auto& appConfig = configManager->getAppConfig();
	vizEngine = std::make_unique<VisualizationEngine>(appConfig,robotConfig);

	robotState = std::make_unique<RobotState>(robotConfig.getWheelCount());
	robotState->position = { 0,0 };
	robotState->frontAngle = DegreesToRadians(-90);

	DirectionVector centerPoint;
	centerPoint.position = Vec2f{ 0.0f, 0.0f };
	robotState->directionVectors.push_back(centerPoint);
	inputManager = std::make_unique<InputManager>(appConfig.inputSettings);

	vizEngine->SetOnInputSettingsChanged([this]() {
		this->inputManager->updateAfterSettingsChange();
		});

}
void AppEngine::run() {
	auto& robotConfig = configManager->getConstRobotConfig();
	auto last = Clock::now();
	

	while (vizEngine->isWindowOpen()) {
		auto now = Clock::now();
		const std::chrono::duration<float> delta = now - last;
		inputManager->update(*robotState, vizEngine->hasFocus(), delta.count());
		physicsEngine->update(delta.count(), *robotState, robotConfig);
		last = now;
		vizEngine->update(delta.count(), *robotState);
		vizEngine->draw();
	}
	auto& appConfig = vizEngine->getSavedAppConfig();
	configManager->setAppConfig(appConfig);
	configManager->saveDefaultAppConfig();

}
