#include "AppEngine.hpp"

#include <chrono>
#include <memory>

#include <SFML/Graphics.hpp>
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
	vizEngine = std::make_unique<VisualizationEngine>(appConfig, robotConfig);

	robotState = std::make_unique<RobotState>(robotConfig.getWheelCount());
	robotState->position = { 0,0 };
	robotState->frontAngle = DegreesToRadians(-90);

	DirectionVector centerPoint;
	centerPoint.position = Vec2f{ 0.0f, 0.0f };
	robotState->directionVectors.push_back(centerPoint);
	inputManager = std::make_unique<InputManager>(appConfig.inputSettings);
	physicsEngine->setLimitMotorSpeed(appConfig.inputSettings.limitMotorSpeed);

	vizEngine->SetOnInputSettingsChanged([this]() {
		this->inputManager->updateAfterSettingsChange();
		const auto& settings = this->vizEngine->getSavedAppConfig().inputSettings;
		this->physicsEngine->setLimitMotorSpeed(settings.limitMotorSpeed);
		});

	vizEngine->SetOnRobotConfigChanged([this](const RobotConfig& newConfig) {
		configManager->getRobotConfig() = newConfig;
		robotState->wheelCount = newConfig.getWheelCount();
		robotState->wheels.resize(newConfig.getWheelCount());
		if (newConfig.GetRobotDriveType() == RobotDriveType::DIFFERENTIAL) {
			robotState->chassisAngle += robotState->frontAngle;
			robotState->frontAngle = 0;
		}

		vizEngine->setRobotConfig(newConfig, *robotState);
		});

}
void AppEngine::run() {

	auto lastFrame = Clock::now();
	auto lastPhysicsTick = lastFrame;

	while (vizEngine->isWindowOpen()) {
		auto frameStart = Clock::now();
		const std::chrono::duration<float> frameDelta = frameStart - lastFrame;
		lastFrame = frameStart;
		_wallTime += std::chrono::duration_cast<std::chrono::system_clock::duration>(frameDelta);

		const auto& appConfig = vizEngine->getSavedAppConfig();
		const float targetFrameTime = appConfig.renderSettings.frameRateLimit > 0
			? 1.0f / appConfig.renderSettings.frameRateLimit
			: 0.0f;
		float physicsTimeSpent = 0.0f;

		do {
			auto now = Clock::now();
			const std::chrono::duration<float> physicsDelta = now - lastPhysicsTick;
			lastPhysicsTick = now;

			auto inputResult = inputManager->update(*robotState, vizEngine->hasFocus());
			if (inputResult.has_value()) {
				vizEngine->showErrorMessage(inputResult.value());
			}
			physicsEngine->update(physicsDelta.count(), *robotState, configManager->getConstRobotConfig());
			inputManager->checkForInputCompletion(*robotState, physicsDelta.count());
			physicsTimeSpent += physicsDelta.count();
		} while (physicsTimeSpent < targetFrameTime);

		vizEngine->update(frameDelta.count(), *robotState, _wallTime);
		vizEngine->draw();
	}
	auto& appConfig = vizEngine->getSavedAppConfig();
	configManager->setAppConfig(appConfig);
	configManager->saveDefaultAppConfig();

}
