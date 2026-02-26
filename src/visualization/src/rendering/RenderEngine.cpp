#include "rendering/RenderEngine.hpp"
#include <cmath>
#include "AppConfig.hpp"
#include "SFML/System.hpp"
#include <iostream>
RenderEngine::RenderEngine(sf::RenderWindow& window,const RenderSettings& settings, const std::string& fontPath) :
	_settings(settings),
	_window(window),
	_view(std::make_unique<sf::View>(sf::FloatRect({ 0.f, 0.f }, (sf::Vector2f)_window.getSize()))),
	_robotShape(std::make_unique<RobotShape>(RobotConfig())),
	_grid(std::make_unique<Grid>(settings.gridSettings, settings.scaleFactor, *_view))
{
	_robotShape->setPosition({ 0,0 });
	_view->setCenter({ 0,0 });
	_window.setView(*_view);

	if(!_font.openFromFile(fontPath)) {
		throw std::runtime_error("Failed to load font at: " + fontPath);
	}
	_grid->setFont(_font);
	updateAfterSettingsChange();
}

void RenderEngine::update(const RobotState& state) {
	_robotShape->update(state);
}

Vec2f RenderEngine::getWindowCenter() const {
	sf::Vector2u size = _window.getSize();
	float scale = 2.f * _settings.scaleFactor;
	return Vec2f{ size.x / scale , size.y / scale };
}

void RenderEngine::updateAfterResize() {
	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_view->setSize(windowSize / _settings.scaleFactor);
	_view->setCenter({0,0});
	_window.setView(*_view);
	regenerateGridLines();
}

void RenderEngine::updateRobotShape(const RobotConfig& config, bool holdPosition) {
	sf::Vector2f lastPosition = holdPosition ? _robotShape->getPosition() :
		(sf::Vector2f)_window.getSize() / (2.0f * _settings.scaleFactor);
	_robotShape = std::make_unique<RobotShape>(config);
	resetRobotPosition(lastPosition);
}

void RenderEngine::updateAfterSettingsChange() {
	_window.setFramerateLimit(_settings.frameRateLimit);
	bool scaleChanged = (std::abs(_settings.scaleFactor - _settings.scaleFactor) > 0.001f);

	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_view->setSize(windowSize / _settings.scaleFactor);
	if (_settings.lockViewOnRobot && scaleChanged)
		_view->setCenter(_robotShape->getPosition());
	_window.setView(*_view);
	_grid->updateAfterSettingsChange(*_view);
}

void RenderEngine::draw() {
	if (_settings.lockViewOnRobot) {
		_view->setCenter(_robotShape->getPosition());
		_window.setView(*_view);
		regenerateGridLines();
	}

	if (_settings.showGrid) {
		_grid->draw(_window);
	}
	_window.draw(*_robotShape);
}

void RenderEngine::resetRobotPosition(sf::Vector2f pos) {
	_robotShape->setPosition(pos);
	_robotShape->setRotation(sf::degrees(0));
	_view->setCenter(pos);
	_window.setView(*_view);
	regenerateGridLines();
}

const RenderSettings& RenderEngine::getCurrentRenderSettings() const {
	return _settings;
}