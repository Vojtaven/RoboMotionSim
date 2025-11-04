#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include <RoboConfig.hpp>
#include <CompositeShape.hpp>
static class Visualization {
public:
	void DrawRobotFromConfig(sf::RenderTarget& target, const RoboConfig& config, const float scale = 1, const sf::Angle rotation = sf::Angle::Zero);
	std::unique_ptr<CompositeShape> static GenerateGridLines(const sf::Vector2f size, const sf::Vector2f spacing, const sf::Vector2f origin, const sf::Color color, const float thickness);
private:


};

#endif //ROBOT_VISUALIZATION_HPP