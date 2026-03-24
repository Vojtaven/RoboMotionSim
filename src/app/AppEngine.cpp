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
	_physicsEngine = std::make_unique<PhysicsEngine>();

	_configManager = std::make_unique<ConfigManager>(Vec2i{ screenWidth,screenHeight });
	auto& robotConfig = _configManager->getConstRobotConfig();
	auto& appConfig = _configManager->getAppConfig();
	_vizEngine = std::make_unique<VisualizationEngine>(appConfig, robotConfig);

	_robotState = std::make_unique<RobotState>(robotConfig.getWheelCount());
	_robotState->position = { 0,0 };
	_robotState->frontAngle = DegreesToRadians(-90);

	DirectionVector centerPoint;
	centerPoint.position = Vec2f{ 0.0f, 0.0f };
	_robotState->directionVectors.push_back(centerPoint);
	_inputManager = std::make_unique<InputManager>(appConfig.inputSettings);
	_physicsEngine->setLimitMotorSpeed(appConfig.inputSettings.limitMotorSpeed);

	_vizEngine->setOnInputSettingsChanged([this]() {
		this->_inputManager->updateAfterSettingsChange();
		const auto& settings = this->_vizEngine->getSavedAppConfig().inputSettings;
		this->_physicsEngine->setLimitMotorSpeed(settings.limitMotorSpeed);
		});

	_vizEngine->setOnRobotConfigChanged([this](const RobotConfig& newConfig) {
		_configManager->getRobotConfig() = newConfig;
		_robotState->wheelCount = newConfig.getWheelCount();
		_robotState->wheels.resize(newConfig.getWheelCount());
		if (newConfig.getRobotDriveType() == RobotDriveType::DIFFERENTIAL) {
			_robotState->chassisAngle += _robotState->frontAngle;
			_robotState->frontAngle = 0;
		}

		_vizEngine->setRobotConfig(newConfig, *_robotState);
		});

}
void AppEngine::run() {

	auto lastFrame = Clock::now();
	auto lastPhysicsTick = lastFrame;

	while (_vizEngine->isWindowOpen()) {
		auto frameStart = Clock::now();
		const std::chrono::duration<float> frameDelta = frameStart - lastFrame;
		lastFrame = frameStart;
		_wallTime += std::chrono::duration_cast<std::chrono::system_clock::duration>(frameDelta);

		const auto& appConfig = _vizEngine->getSavedAppConfig();
		const float targetFrameTime = appConfig.renderSettings.frameRateLimit > 0
			? 1.0f / appConfig.renderSettings.frameRateLimit
			: 0.0f;
		float physicsTimeSpent = 0.0f;

		do {
			auto now = Clock::now();
			const std::chrono::duration<float> physicsDelta = now - lastPhysicsTick;
			lastPhysicsTick = now;

			auto inputResult = _inputManager->update(*_robotState, _vizEngine->hasFocus());
			if (inputResult.has_value()) {
				_vizEngine->showErrorMessage(inputResult.value());
			}
			_physicsEngine->update(physicsDelta.count(), *_robotState, _configManager->getConstRobotConfig());
			_inputManager->checkForInputCompletion(*_robotState, physicsDelta.count());
			physicsTimeSpent += physicsDelta.count();
		} while (physicsTimeSpent < targetFrameTime);

		_vizEngine->update(frameDelta.count(), *_robotState, _wallTime);
		_vizEngine->draw();
	}
	auto& appConfig = _vizEngine->getSavedAppConfig();
	_configManager->setAppConfig(appConfig);
	_configManager->saveDefaultAppConfig();

}
