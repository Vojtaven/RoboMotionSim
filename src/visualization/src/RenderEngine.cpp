#include "RenderEngine.hpp"
#include <cmath>
#include <iostream>
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
	float scale = 2.f * _settings.scaleFactor;
	return Vec2f{ size.x / scale , size.y / scale };
}

void RenderEngine::updateAfterResize() {
	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_view->setSize(windowSize / _settings.scaleFactor);
	_view->setCenter(windowSize / 2.f);
	_window.setView(*_view);
	regenerateGridLines();
}

void RenderEngine::updateRobotShape(const RobotConfig& config, bool holdPosition) {
	sf::Vector2f lastPosition = holdPosition ? _robotShape->getPosition() :
		(sf::Vector2f)_window.getSize() / (2.0f * _settings.scaleFactor);
	_robotShape = std::make_unique<RobotShape>(config);
	resetRobotPosition(lastPosition);
}

void RenderEngine::setRenderSettings(const RenderSettings& settings) {
	_gridColor = sf::Color(
		(uint8_t)(settings.gridColor[0] * 255),
		(uint8_t)(settings.gridColor[1] * 255),
		(uint8_t)(settings.gridColor[2] * 255));
	_subGridColor = sf::Color(
		(uint8_t)(settings.subGridColor[0] * 255),
		(uint8_t)(settings.subGridColor[1] * 255),
		(uint8_t)(settings.subGridColor[2] * 255));

	_window.setFramerateLimit(settings.frameRateLimit);
	bool scaleChanged = (std::abs(settings.scaleFactor - _settings.scaleFactor) > 0.001f);

	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_view->setSize(windowSize / settings.scaleFactor);

	if (settings.lockViewOnRobot && scaleChanged)
		_view->setCenter(_robotShape->getPosition());
	_view->setViewport(sf::FloatRect({ 0, 0 }, { 1, 1 }));
	_window.setView(*_view);
	_settings = settings;
	regenerateGridLines();
}

void RenderEngine::draw() {
	if (_settings.lockViewOnRobot) {
		_view->setCenter(_robotShape->getPosition());
		_window.setView(*_view);
		regenerateGridLines();
	}

	if (_settings.showGrid)
		_window.draw(_gridLines);
	_window.draw(*_robotShape);
}


void AddGridLines(sf::VertexArray& gridLines, const sf::Color gridColor, float startX, float startY, float spacing, float left, float right, float top, float bottom) {
	for (float x = startX; x <= right; x += spacing)
	{
		gridLines.append({ { x, top },    gridColor });
		gridLines.append({ { x, bottom }, gridColor });
	}

	for (float y = startY; y <= bottom; y += spacing)
	{
		gridLines.append({ { left,  y }, gridColor });
		gridLines.append({ { right, y }, gridColor });
	}
}

void RenderEngine::regenerateGridLines() {
	_gridLines = sf::VertexArray(sf::PrimitiveType::Lines);

	sf::Vector2f viewSize = _view->getSize();
	sf::Vector2f viewCenter = _view->getCenter();

	//So we dont draw outside the view
	float left = viewCenter.x - viewSize.x / 2.f;
	float right = viewCenter.x + viewSize.x / 2.f;
	float top = viewCenter.y - viewSize.y / 2.f;
	float bottom = viewCenter.y + viewSize.y / 2.f;
	int spacing = _settings.gridSpacing;


	if (_settings.autoGridSpacing)
		spacing = SnapGridToNiceValues();

	float subdivisionSpacing = spacing / (float)(_settings.gridSubdivisions + 1);

	float startX = std::floor(left / spacing) * spacing;
	float startY = std::floor(top / spacing) * spacing;

	AddGridLines(_gridLines, _subGridColor, startX, startY, subdivisionSpacing, left, right, top, bottom);
	AddGridLines(_gridLines, _gridColor, startX, startY, spacing, left, right, top, bottom);
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


int RenderEngine::SnapGridToNiceValues()
{
	double raw = _DefaultGridSpacing / _settings.scaleFactor;
	if (raw <= 0.0)
		return _DefaultGridSpacing;

	int exponent = static_cast<int>(std::floor(std::log10(raw)));
	double base = std::pow(10.0, exponent);
	double normalized = raw / base;

	double candidates[] = { 1.0, 2, 5.0, 10.0 };

	double closest = candidates[0];
	double minDiff = std::abs(normalized - closest);

	for (int i = 1; i < 4; ++i)
	{
		double diff = std::abs(normalized - candidates[i]);
		if (diff < minDiff)
		{
			minDiff = diff;
			closest = candidates[i];
		}
	}

	if (closest == 10.0)
		return 1.0 * std::pow(10.0, exponent + 1);

	return closest * base;
}