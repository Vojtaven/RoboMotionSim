#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include <RoboConfig.hpp>
#include <CompositeShape.hpp>
#include <RobotShape.hpp>


class VisualizationEngine {
public:
	VisualizationEngine(sf::RenderTarget& target, const RoboConfig& config = RoboConfig())
		: _robotShape(config), _lastWindowSize((sf::Vector2f)target.getSize()) {
		RegenerateGridLines();
		resetRobotPosition(_lastWindowSize / 2.0f);
	}

	void setRobotConfig(const RoboConfig& config, bool holdPosition = false) {
		sf::Vector2f lastPosition = holdPosition ? _robotShape.getPosition() : _lastWindowSize / 2.0f;
		_robotShape = RobotShape(config);
		resetRobotPosition(lastPosition);
	}

	void setRobotPosition(const sf::Vector2f position, const sf::Angle angle) {
		_robotShape.setPosition(position);
		_robotShape.setRotation(angle);
	}

	void moveRobotBy(const sf::Vector2f offset, const sf::Angle angleOffset) {
		_robotShape.move(offset);
		_robotShape.rotate(angleOffset);
	}

	void draw(sf::RenderTarget& target) {
		if (_lastWindowSize != (sf::Vector2f)target.getSize()) 
			updateWindowSize((sf::Vector2f)target.getSize());

		target.draw(_gridLines);
		target.draw(_robotShape);
	}

	void setScaleFactor(const float scale) { 
		_scaleFactor = scale; 
		_robotShape.setScale({ scale, scale });
		RegenerateGridLines();
	}

	void setGridSpacing(const sf::Vector2f spacing) {
		_gridSpacing = spacing;
		RegenerateGridLines();
	}

	void setGridColor(const sf::Color color) {
		_gridColor = color;
		RegenerateGridLines();
	}
private:
	void RegenerateGridLines(){
		CompositeShape grid;
		sf::Vector2f pixelSpacing = _gridSpacing * _scaleFactor;

		int numberOfVerticalLines = _lastWindowSize.x / pixelSpacing.x + 1;
		int numberOfHorizontalLines = _lastWindowSize.y / pixelSpacing.y + 1;

		std::unique_ptr<sf::RectangleShape> line;
		for (int i = 0; i < numberOfVerticalLines; i++) {
			line = std::make_unique<sf::RectangleShape>(sf::Vector2f(_lastWindowSize.y, _gridLineThickness));
			line->rotate(sf::degrees(90));
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

	void updateWindowSize(const sf::Vector2f size) {
		_lastWindowSize = size;
		RegenerateGridLines();
	}

	void resetRobotPosition(sf::Vector2f pos) {
		_robotShape.setPosition(pos);
		_robotShape.setRotation(sf::degrees(0));
		_robotShape.setScale({ _scaleFactor, _scaleFactor });
	}

	RobotShape _robotShape;
	sf::Vector2f _lastWindowSize;
	float _scaleFactor = 1.0f; // 1 pixel = 1 mm
	sf::Vector2f _gridSpacing = { 25,25 };
	sf::Color _gridColor = { 80,80,80 };
	float _gridLineThickness = 1.0f;
	CompositeShape _gridLines;
};

#endif //ROBOT_VISUALIZATION_HPP