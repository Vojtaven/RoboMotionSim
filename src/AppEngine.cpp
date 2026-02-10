#include "AppEngine.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <string>
#include <asio.hpp>
#include "VisualizationEngine.hpp"
#include "RobotConfig.hpp"
#include "ConfigManager.hpp"
#include <RobotShape.hpp>
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
	auto& appConfig = configManager->getConstAppConfig();
	vizEngine = std::make_unique<VisualizationEngine>(appConfig,robotConfig);

	robotState = std::make_unique<RobotState>(robotConfig.getWheelCount());
	robotState->position = vizEngine->getWindowCenter();
}
void AppEngine::run() {
	const float moveStep = 200; // 200mm/s
	const float rotationStep = DegreesToRadians(30); // 30 degrees/s
	auto& robotConfig = configManager->getConstRobotConfig();
	auto last = Clock::now();

	while (vizEngine->isWindowOpen()) {
		Vec2f speed{ 0,0 };
		float rotationSpeed = 0;
		float frontRotationSpeed = 0;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
			speed.y -= moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
			speed.y += moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
			speed.x -= moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
			speed.x += moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q))
			rotationSpeed -= rotationStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E))
			rotationSpeed += rotationStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::J))
			frontRotationSpeed -= rotationStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::K))
			frontRotationSpeed += rotationStep;

		robotState->localVelocity = speed;
		robotState->angularVelocity = rotationSpeed;
		robotState->frontAngularVelocity = frontRotationSpeed;
		auto now = Clock::now();
		std::chrono::duration<float> delta = now - last;
		physicsEngine->update(delta.count(), *robotState, robotConfig);
		last = now;
		vizEngine->update(*robotState);
		vizEngine->draw();
	}
	auto& appConfig = vizEngine->getSavedAppConfig();
	configManager->setAppConfig(appConfig);
	configManager->saveDefaultAppConfig();

}
