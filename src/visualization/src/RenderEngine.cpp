#include "RenderEngine.hpp"
#include <cmath>

RenderEngine::RenderEngine(sf::RenderWindow& window, const RenderSettings& settings) :
	_window(window),
	_robotShape(std::make_unique<RobotShape>(RobotConfig()))
{
	_view = std::make_unique<sf::View>(sf::FloatRect({ 0.f, 0.f }, (sf::Vector2f)_window.getSize()));
	_window.setView(*_view);
	setRenderSettings(settings);
}

void RenderEngine::update(const RobotState& state) {
	_robotShape->update(state);
}

Vec2f RenderEngine::getWindowCenter() const {
	sf::Vector2u size = _window.getSize();
	float scale = 2.f * _scaleFactor;
	return Vec2f{ size.x / scale , size.y / scale };
}



void RenderEngine::updateAfterResize() {
	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_view->setSize(windowSize / _scaleFactor);
	_window.setView(*_view);
	regenerateGridLines();
}

void RenderEngine::updateRobotShape(const RobotConfig& config, bool holdPosition) {
	sf::Vector2f lastPosition = holdPosition ? _robotShape->getPosition() :
		(sf::Vector2f)_window.getSize() / (2.0f * _scaleFactor);
	_robotShape = std::make_unique<RobotShape>(config);
	resetRobotPosition(lastPosition);
}

void RenderEngine::setRenderSettings(const RenderSettings& settings) {
	_gridSpacing = ToSFMLVector2f(settings.gridSpacing);
	_gridColor = sf::Color(
		(uint8_t)(settings.gridColor[0] * 255),
		(uint8_t)(settings.gridColor[1] * 255),
		(uint8_t)(settings.gridColor[2] * 255));

	bool scaleChanged = (std::abs(settings.scaleFactor - _scaleFactor) > 0.001f);
	_scaleFactor = settings.scaleFactor;
	_showGridLines = settings.showGrid;

	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_view->setSize(windowSize / _scaleFactor);

	if (settings.lockOnRobotCenterWhileScaling && scaleChanged)
		_view->setCenter(_robotShape->getPosition());
	_view->setViewport(sf::FloatRect({ 0, 0 }, { 1, 1 }));
	_window.setView(*_view);
	regenerateGridLines();
}

void RenderEngine::draw() {
	if (_showGridLines)
		_window.draw(_gridLines);
	_window.draw(*_robotShape);
}

void RenderEngine::regenerateGridLines() {
	_gridLines = sf::VertexArray(sf::PrimitiveType::Lines);

	sf::Vector2f viewSize = _view->getSize();
	sf::Vector2f viewCenter = _view->getCenter();
	float left = viewCenter.x - (viewSize.x / 2.0f);
	float top = viewCenter.y - (viewSize.y / 2.0f);
	float right = left + viewSize.x;
	float bottom = top + viewSize.y;

	sf::Vector2f robotPos = _robotShape->getPosition();
	sf::Vector2f anchor = {
		std::round(robotPos.x / _gridDefaultSpacing.x) * _gridDefaultSpacing.x,
		std::round(robotPos.y / _gridDefaultSpacing.y) * _gridDefaultSpacing.y
	};

	float startX = anchor.x - std::ceil((anchor.x - left) / _gridSpacing.x) * _gridSpacing.x;
	float startY = anchor.y - std::ceil((anchor.y - top) / _gridSpacing.y) * _gridSpacing.y;

	for (float x = startX; x <= right; x += _gridSpacing.x) {
		_gridLines.append({ { x, top },    _gridColor });
		_gridLines.append({ { x, bottom }, _gridColor });
	}

	for (float y = startY; y <= bottom; y += _gridSpacing.y) {
		_gridLines.append({ { left,  y }, _gridColor });
		_gridLines.append({ { right, y }, _gridColor });
	}
}

void RenderEngine::resetRobotPosition(sf::Vector2f pos) {
	_robotShape->setPosition(pos);
	_robotShape->setRotation(sf::degrees(0));
	_view->setCenter(pos);
	_window.setView(*_view);
	regenerateGridLines();
}


void RenderEngine::saveRenderSettings(RenderSettings& settings) const {
	settings.gridSpacing = FromSFMLVector(_gridSpacing);
	settings.gridColor[0] = _gridColor.r / 255.0f;
	settings.gridColor[1] = _gridColor.g / 255.0f;
	settings.gridColor[2] = _gridColor.b / 255.0f;
	settings.scaleFactor = _scaleFactor;
}

RenderSettings RenderEngine::getCurrentRenderSettings() const {
	RenderSettings settings;
	saveRenderSettings(settings);
	return settings;
}