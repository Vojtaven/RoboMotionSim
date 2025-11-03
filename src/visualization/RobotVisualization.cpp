#include "RobotVisualization.hpp"


void Visualization::DrawRobotFromConfig(sf::RenderTarget& target, const RoboConfig& config) {
	sf::CircleShape circle;
	circle.setPosition(sf::Vector2f(0, 0));
	circle.setRadius(20);
	target.draw(circle);
}

