#ifndef ROBOT_TRAIL_HPP
#define ROBOT_TRAIL_HPP
#include <SFML/Graphics.hpp>
#include <deque>
#include "AppConfig.hpp"
#include "SFMLHelper.hpp"
class RobotTrail
{
public:
	RobotTrail(const TrailSettings& settings);
	void addTrailPoint(const sf::Vector2f& position, const float dt);
	void updateAfterSettingsChange();
	void clearTrail();
	void draw(sf::RenderTarget& target) const;
private:
	sf::Color _color;
	std::deque<sf::CircleShape> _trail = {};
	const TrailSettings& _settings;
	float interval = 0;
};
#endif // !ROBOT_TRAIL_HPP
