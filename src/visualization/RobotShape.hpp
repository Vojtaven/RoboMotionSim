#ifndef ROBOTSHAPE_HPP
#define ROBOTSHAPE_HPP
#include "CompositeShape.hpp"
#include "RoboConfig.hpp"
#include <vector>
class RobotShape : public CompositeShape {
public: 
	RobotShape(const RoboConfig& config,bool drawCenter = false, bool showSpeed = false);
	void ShowSpeedEnable() { _showSpeed = true; }
	void ShowSpeedDisable() { _showSpeed = false; }
	void UpdateSpeed(std::vector<float> speedOfWheels);
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
	void AddWheel(const RoboParts::Wheel& wheel);
	bool _showSpeed;
	bool _drawCenter;
	int _numberOfWheels = 0;
	std::vector<sf::Text> _speedOfWheels;
	std::vector<sf::Vector2f> _wheelMountingPoints;
};
#endif // !ROBOTSHAPE_HPP
