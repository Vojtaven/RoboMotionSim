#ifndef ROBOTSHAPE_HPP
#define ROBOTSHAPE_HPP
#include "CompositeShape.hpp"
#include "RobotConfig.hpp"
#include "RobotShape.hpp"
#include "WheelVectors.hpp"
#include <vector>

class RobotShape : public CompositeShape {
public:
	RobotShape(const RobotConfig& config, bool drawCenter = false, bool showSpeed = false);
	RobotShape(const RobotConfig* config, bool drawCenter = false, bool showSpeed = false) : RobotShape(*config, drawCenter, showSpeed) {}
	void ShowSpeedEnable() { _showSpeed = true; }
	void ShowSpeedDisable() { _showSpeed = false; }
	void Update(const RobotState& state);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
	void AddWheel(const RobotParts::Wheel& wheel);
	void AddWheelVector(const RobotParts::Wheel& wheel);
	bool _showSpeed;
	bool _drawCenter;
	int _numberOfWheels = 0;
	std::vector<WheelVectors*> _speedOfWheels;
	std::vector<sf::Vector2f> _wheelMountingPoints;
};
#endif // !ROBOTSHAPE_HPP
