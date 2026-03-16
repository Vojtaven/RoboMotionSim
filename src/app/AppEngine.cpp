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
	_wallTime = std::chrono::system_clock::now();
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
		_wallTime += std::chrono::duration_cast<std::chrono::system_clock::duration>(delta);
		inputManager->update(*robotState, vizEngine->hasFocus());
		physicsEngine->update(delta.count(), *robotState, robotConfig);
		last = now;
		vizEngine->update(delta.count(), *robotState, _wallTime);
		inputManager->checkForInputCompletion(*robotState, delta.count());
		vizEngine->draw();
	}
	auto& appConfig = vizEngine->getSavedAppConfig();
	configManager->setAppConfig(appConfig);
	configManager->saveDefaultAppConfig();

}
