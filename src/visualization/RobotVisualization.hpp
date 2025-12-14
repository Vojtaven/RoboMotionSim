#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include <RoboConfig.hpp>
#include <CompositeShape.hpp>
class Visualization {
public:
	static void DrawRobotFromConfig(sf::RenderTarget& target, const RoboConfig& config, const float scale = 1, const sf::Angle rotation = sf::Angle::Zero);
	static std::unique_ptr<CompositeShape> GenerateGridLines(const sf::Vector2f size, const sf::Vector2f spacing, const sf::Vector2f origin, const sf::Color color, const float thickness);
private:


};

#endif //ROBOT_VISUALIZATION_HPP