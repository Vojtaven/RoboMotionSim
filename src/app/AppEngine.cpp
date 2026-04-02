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
	_vizEngine = std::make_unique<VisualizationEngine>(appConfig, _configManager->getRobotConfig(), robotConfig);

	_robotState = std::make_unique<RobotState>(robotConfig.getWheelCount());
	_robotState->position = { 0,0 };
	_robotState->frontAngle = DegreesToRadians(-90);

	DirectionVector centerPoint;
	centerPoint.position = Vec2f{ 0.0f, 0.0f };
	_robotState->directionVectors.push_back(centerPoint);
	_inputManager = std::make_unique<InputManager>(appConfig.inputSettings);
	_physicsEngine->subscribeToSettings(appConfig.inputSettings);

	// react to robot config changes (e.g. user adds/removes wheels in designer)
	_robotConfigSubscription = _configManager->getRobotConfig().scopedSubscribe([this](const RobotConfig& newConfig) {
		_robotState->wheelCount = newConfig.getWheelCount();
		_robotState->wheels.resize(newConfig.getWheelCount());
		if (newConfig.getRobotDriveType() == RobotDriveType::DIFFERENTIAL) {
			// differential has no independent front angle — merge it into chassis
			_robotState->chassisAngle += _robotState->frontAngle;
			_robotState->frontAngle = 0;
		}
		_vizEngine->setRobotConfig(newConfig, *_robotState);
		});

}
void AppEngine::run() {

	auto lastFrame = Clock::now();
	auto lastPhysicsTick = lastFrame;
	std::optional<std::string> inputErrorMessage;
	while (_vizEngine->isWindowOpen()) {
		auto frameStart = Clock::now();
		const std::chrono::duration<double> frameDelta = frameStart - lastFrame;
		lastFrame = frameStart;
		_wallTime += std::chrono::duration_cast<std::chrono::system_clock::duration>(frameDelta);

		const auto& appConfig = _vizEngine->getSavedAppConfig();
		const float targetFrameTime = appConfig.renderSettings.get().frameRateLimit > 0
			? 1.0f / appConfig.renderSettings.get().frameRateLimit
			: 0.0f;
		double physicsTimeSpent = 0.0;

		// run physics in a tight loop until we've consumed enough time for one frame
		do {
			auto now = Clock::now();
			const std::chrono::duration<double> physicsDelta = now - lastPhysicsTick;
			lastPhysicsTick = now;

			inputErrorMessage = _inputManager->update(*_robotState, _vizEngine->hasFocus());
			_physicsEngine->update(physicsDelta.count(), *_robotState, _configManager->getConstRobotConfig());
			_inputManager->checkForInputCompletion(*_robotState, physicsDelta.count());
			physicsTimeSpent += physicsDelta.count();
		} while (physicsTimeSpent < targetFrameTime);


		if (inputErrorMessage.has_value()) {
			_vizEngine->showErrorMessage(inputErrorMessage.value());
		}

		_vizEngine->update(static_cast<float>(frameDelta.count()), *_robotState, _wallTime);
		_vizEngine->draw();
	}
	_configManager->saveDefaultAppConfig();
}
