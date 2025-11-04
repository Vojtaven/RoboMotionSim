#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include <RoboConfig.hpp>

static class Visualization {
public:
	static void DrawRobotFromConfig(sf::RenderTarget& target, const RoboConfig& config, const float scale = 1, const sf::Angle rotation = sf::Angle::Zero);
};

#endif //ROBOT_VISUALIZATION_HPP