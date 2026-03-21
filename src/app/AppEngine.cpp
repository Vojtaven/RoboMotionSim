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

	auto last = Clock::now();


	while (vizEngine->isWindowOpen()) { 
		auto now = Clock::now();
		const std::chrono::duration<float> delta = now - last;
		_wallTime += std::chrono::duration_cast<std::chrono::system_clock::duration>(delta);
		auto inputResult = inputManager->update(*robotState, vizEngine->hasFocus());
		if (inputResult.has_value()) {
			vizEngine->showErrorMessage(inputResult.value());
		}
		physicsEngine->update(delta.count(), *robotState, configManager->getConstRobotConfig());
		last = now;
		vizEngine->update(delta.count(), *robotState, _wallTime);
		inputManager->checkForInputCompletion(*robotState, delta.count());
		vizEngine->draw();
	}
	auto& appConfig = vizEngine->getSavedAppConfig();
	configManager->setAppConfig(appConfig);
	configManager->saveDefaultAppConfig();

}
