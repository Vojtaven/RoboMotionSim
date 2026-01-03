#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include "RobotConfig.hpp"
#include <CompositeShape.hpp>
#include <RobotShape.hpp>
#include "RobotState.hpp"
#include "MathUtils.hpp"
#include "SFMLHelper.hpp"
class VisualizationEngine {
public:
	VisualizationEngine(sf::RenderTarget& target, const RobotConfig& config = RobotConfig());
	void setRobotConfig(const RobotConfig& config, bool holdPosition = false);
	void setRobotPosition(const sf::Vector2f position, const sf::Angle angle);
	void moveRobotBy(const sf::Vector2f offset, const sf::Angle angleOffset);
	void draw(sf::RenderTarget& target);
	void setScaleFactor(const float scale);
	void setGridSpacing(const sf::Vector2f spacing);
	void setGridColor(const sf::Color color);
	void update(const RobotState& state);
	Vec2 getWindowCenter() const;
private:
	void RegenerateGridLines();
	void updateWindowSize(const sf::Vector2f size);
	void resetRobotPosition(sf::Vector2f pos);

	std::unique_ptr<RobotShape> _robotShape;
	sf::Vector2f _lastWindowSize;
	float _scaleFactor = 1.0f; // 1 pixel = 1 mm
	sf::Vector2f _gridSpacing = { 25,25 };
	sf::Color _gridColor = { 80,80,80 };
	float _gridLineThickness = 1.0f;
	CompositeShape _gridLines;
};

#endif //ROBOT_VISUALIZATION_HPP