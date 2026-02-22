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
	_gridLineThickness = settings.gridLineThickness;
	_showGridLines = settings.showGrid;

	sf::Vector2f windowSize = (sf::Vector2f)_window.getSize();
	_view->setSize(windowSize / _scaleFactor);

	if(settings.lockOnRobotCenterWhileScaling && scaleChanged)
		_view->setCenter(_robotShape->getPosition());
	_window.setView(*_view);
	regenerateGridLines();
}

void RenderEngine:: draw() {
	if(	_showGridLines)
		_window.draw(*_gridLines);
	_window.draw(*_robotShape);
}

void RenderEngine::regenerateGridLines() {

	// TODO PREKOPAT TOHLE
	_gridLines = std::make_unique<CompositeShape>();

	sf::Vector2f viewSize = _view->getSize();
	float worldLineThickness = _gridLineThickness / _scaleFactor;

	sf::Vector2f robotPos = _robotShape->getPosition();
	_gridLines->setPosition({
		std::round(robotPos.x / _gridSpacing.x) * _gridSpacing.x,
		std::round(robotPos.y / _gridSpacing.y) * _gridSpacing.y
	});

	int halfX = static_cast<int>(viewSize.x / (2.f * _gridSpacing.x)) + 2;
	int halfY = static_cast<int>(viewSize.y / (2.f * _gridSpacing.y)) + 2;
	float totalHeight = (2 * halfY) * _gridSpacing.y;
	float totalWidth  = (2 * halfX) * _gridSpacing.x;

	for (int i = -halfX; i <= halfX; ++i) {
		auto line = std::make_unique<sf::RectangleShape>(
			sf::Vector2f(worldLineThickness, totalHeight));
		line->setPosition({ _gridSpacing.x * i, -totalHeight / 2.f });
		line->setFillColor(_gridColor);
		line->setOutlineColor(_gridColor);
		_gridLines->add(std::move(line));
	}

	for (int i = -halfY; i <= halfY; ++i) {
		auto line = std::make_unique<sf::RectangleShape>(
			sf::Vector2f(totalWidth, worldLineThickness));
		line->setPosition({ -totalWidth / 2.f, _gridSpacing.y * i });
		line->setFillColor(_gridColor);
		line->setOutlineColor(_gridColor);
		_gridLines->add(std::move(line));
	}
}


void RenderEngine::resetRobotPosition(sf::Vector2f pos) {
	_robotShape->setPosition(pos);
	_robotShape->setRotation(sf::degrees(0));
	_view->setCenter(pos);
	_window.setView(*_view);
	regenerateGridLines();
}


void RenderEngine::saveRenderSettings(RenderSettings& settings) const{
	settings.gridSpacing = FromSFMLVector(_gridSpacing);
	settings.gridColor[0] = _gridColor.r / 255.0f;
	settings.gridColor[1] = _gridColor.g / 255.0f;
	settings.gridColor[2] = _gridColor.b / 255.0f;
	settings.scaleFactor = _scaleFactor;
	settings.gridLineThickness = _gridLineThickness;
}

RenderSettings RenderEngine::getCurrentRenderSettings() const {
	RenderSettings settings;
	saveRenderSettings(settings);
	return settings;
}