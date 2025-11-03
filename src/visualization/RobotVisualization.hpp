#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include <RoboConfig.hpp>

static class Visualization {
public:
	static void DrawRobotFromConfig(sf::RenderTarget& target, const RoboConfig& config);
};

#endif //ROBOT_VISUALIZATION_HPP