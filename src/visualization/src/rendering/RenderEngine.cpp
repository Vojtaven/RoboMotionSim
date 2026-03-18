#include "rendering/RenderEngine.hpp"
#include <cmath>
#include "AppConfig.hpp"
#include "embeddedFont.h"
#include "SFML/System.hpp"
#include <iostream>

RenderEngine::RenderEngine(sf::RenderWindow& window, const RenderSettings& settings) :
	_settings(settings),
	_window(window),
	_worldView(std::make_unique<sf::View>(sf::FloatRect({ 0.f, 0.f }, (sf::Vector2f)_window.getSize()))),
	_uiView(std::make_unique<sf::View>(sf::FloatRect({ 0.f, 0.f }, (sf::Vector2f)_window.getSize()))),
	_robotShape(std::make_unique<RobotShape>(RobotConfig())),
	_trail(std::make_unique<RobotTrail>(settings.trailSettings))
{
	_robotShape->setPosition({ 0,0 });
	_robotShape->setVectorsVisibility(settings.showForwardVectors, settings.showRollerVectors, settings.showWheelDirectionVectors);
	_worldView->setCenter({ 0,0 });
	_window.setView(*_worldView);

	if (!_font.openFromMemory(DEFAULT_FONT_DATA, DEFAULT_FONT_DATA_SIZE)) {
		throw std::runtime_error("Failed to load font from memory");
	}
	_grid = std::make_unique<Grid>(settings.gridSettings, settings.scaleFactor, _font, *_worldView);
	updateAfterSettingsChange();
}

void RenderEngine::update(const RobotState& state, const float dt) {
	_robotShape->update(state);
	_trail->addTrailPoint(ToSFMLVector2f(state.position),dt);
}

Vec2f RenderEngine::getWindowCenter() const {
	sf::Vector2u size = _window.getSize();
	float scale = 2.f * _settings.scaleFactor;
	return Vec2f{ size.x / scale , size.y / scale };
}

void RenderEngine::updateAfterResize() {
	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_uiView->setSize(windowSize);
	_uiView->setCenter(windowSize / 2.f);
	_worldView->setSize(windowSize / _settings.scaleFactor);
	_worldView->setCenter({ 0,0 });
	_window.setView(*_worldView);
	regenerateGridLines();
}

void RenderEngine::updateRobotShape(const RobotConfig& config, bool holdPosition) {
	sf::Vector2f lastPosition = holdPosition ? _robotShape->getPosition() :
		(sf::Vector2f)_window.getSize() / (2.0f * _settings.scaleFactor);
	_robotShape = std::make_unique<RobotShape>(config);
	_robotShape->setVectorsVisibility(_settings.showForwardVectors, _settings.showRollerVectors, _settings.showWheelDirectionVectors);
	resetRobotPosition(lastPosition);
}

void RenderEngine::updateAfterSettingsChange() {
	_window.setFramerateLimit(_settings.frameRateLimit);
	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_worldView->setSize(windowSize / _settings.scaleFactor);
	_window.setView(*_worldView);
	_trail->updateAfterSettingsChange();
	_grid->updateAfterSettingsChange();
	_robotShape->setVectorsVisibility(_settings.showForwardVectors, _settings.showRollerVectors, _settings.showWheelDirectionVectors);
}

void RenderEngine::draw() {
	if (_settings.lockViewOnRobot) {
		_worldView->setCenter(_robotShape->getPosition());
		regenerateGridLines();
	}

	_window.setView(*_worldView);

	if (_settings.showGrid) {
		_grid->draw(_window);
		_window.setView(*_uiView);
		_grid->drawText(_window);
		_window.setView(*_worldView);
	}
	if (_settings.showTrail) {
		_trail->draw(_window);
	}

	_window.draw(*_robotShape);
}

void RenderEngine::resetRobotPosition(sf::Vector2f pos) {
	_robotShape->setPosition(pos);
	_robotShape->setRotation(sf::degrees(0));
	_worldView->setCenter(pos);
	_window.setView(*_worldView);
	regenerateGridLines();
}

void RenderEngine::regenerateGridLines() {
	_grid->regenerate();
}

const RenderSettings& RenderEngine::getCurrentRenderSettings() const {
	return _settings;
}