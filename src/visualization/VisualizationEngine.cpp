#include "VisualizationEngine.hpp"
#include "CompositeShape.hpp"
#include "RobotShape.hpp"
#include "RobotState.hpp"
#include "SFMLHelper.hpp"
#include "MathUtils.hpp"
VisualizationEngine::VisualizationEngine(sf::RenderTarget& target, const RobotConfig& config)
	: _robotShape(std::make_unique<RobotShape>(config)), _lastWindowSize((sf::Vector2f)target.getSize()) {
	RegenerateGridLines();
	resetRobotPosition(_lastWindowSize / 2.0f);
}

void VisualizationEngine::setRobotConfig(const RobotConfig& config, bool holdPosition) {
	sf::Vector2f lastPosition = holdPosition ? _robotShape->getPosition() : _lastWindowSize / 2.0f;
	_robotShape = std::make_unique<RobotShape>(config);
	resetRobotPosition(lastPosition);
}

void VisualizationEngine::setRobotPosition(const sf::Vector2f position, const sf::Angle angle) {
	_robotShape->setPosition(position);
	_robotShape->setRotation(angle);
}

void VisualizationEngine::moveRobotBy(const sf::Vector2f offset, const sf::Angle angleOffset) {
	_robotShape->move(offset);
	_robotShape->rotate(angleOffset);
}

void VisualizationEngine::draw(sf::RenderTarget& target) {
	if (_lastWindowSize != (sf::Vector2f)target.getSize())
		updateWindowSize((sf::Vector2f)target.getSize());

	target.draw(_gridLines);
	target.draw(*_robotShape);
}

void VisualizationEngine::setScaleFactor(const float scale) {
	_scaleFactor = scale;
	_robotShape->setScale({ scale, scale });
	RegenerateGridLines();
}

void VisualizationEngine::setGridSpacing(const sf::Vector2f spacing) {
	_gridSpacing = spacing;
	RegenerateGridLines();
}

void VisualizationEngine::setGridColor(const sf::Color color) {
	_gridColor = color;
	RegenerateGridLines();
}
void VisualizationEngine::update(const RobotState& state) { _robotShape->Update(state); }

void VisualizationEngine::RegenerateGridLines() {
	CompositeShape grid;
	sf::Vector2f pixelSpacing = _gridSpacing * _scaleFactor;

	int numberOfVerticalLines = _lastWindowSize.x / pixelSpacing.x + 1;
	int numberOfHorizontalLines = _lastWindowSize.y / pixelSpacing.y + 1;

	std::unique_ptr<sf::RectangleShape> line;
	for (int i = 0; i < numberOfVerticalLines; i++) {
		line = std::make_unique<sf::RectangleShape>(sf::Vector2f(_gridLineThickness, _lastWindowSize.y));
		line->setPosition({ pixelSpacing.x * i,0 });
		line->setFillColor(_gridColor);
		line->setOutlineColor(_gridColor);
		grid.add(std::move(line));
	}

	for (int i = 0; i < numberOfHorizontalLines; i++) {
		line = std::make_unique<sf::RectangleShape>(sf::Vector2f(_lastWindowSize.x, _gridLineThickness));
		line->setPosition({ 0, pixelSpacing.y * i });
		line->setFillColor(_gridColor);
		line->setOutlineColor(_gridColor);
		grid.add(std::move(line));
	}

	_gridLines = std::move(grid);
}

void VisualizationEngine::updateWindowSize(const sf::Vector2f size) {
	_lastWindowSize = size;
	RegenerateGridLines();
}

void VisualizationEngine::resetRobotPosition(sf::Vector2f pos) {
	_robotShape->setPosition(pos);
	_robotShape->setRotation(sf::degrees(0));
	_robotShape->setScale({ _scaleFactor, _scaleFactor });
}

Vec2 VisualizationEngine::getWindowCenter() const {
	return FromSFMLVector(_lastWindowSize / 2.f);
}

