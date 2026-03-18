#ifndef ROBOTSHAPE_HPP
#define ROBOTSHAPE_HPP
#include "CompositeShape.hpp"
#include "RobotConfig.hpp"
#include "RobotShape.hpp"
#include "WheelVectors.hpp"
#include "PointVector.hpp"
#include <vector>

class RobotShape : public CompositeShape {
public:
	RobotShape(const RobotConfig& config, bool drawCenter = false, bool showSpeed = false);
	RobotShape(const RobotConfig* config, bool drawCenter = false, bool showSpeed = false) : RobotShape(*config, drawCenter, showSpeed) {}
	void showSpeedEnable() { _showSpeed = true; }
	void showSpeedDisable() { _showSpeed = false; }
	void update(const RobotState& state);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setVectorsVisibility(bool forwardVisible, bool rollerVisible, bool directionVisible);
private:
	void addWheel(const RobotParts::Wheel& wheel);
	void addWheelVector(const RobotParts::Wheel& wheel);
	void updateDirectionVectors(const RobotState& state);
	bool _showSpeed;
	bool _drawCenter;
	int _numberOfWheels = 0;
	PointVector* _frontVector = nullptr;
	std::vector<WheelVectors*> _speedOfWheels;
	std::vector<sf::Vector2f> _wheelMountingPoints;
	std::vector<PointVector*> _directionVectors;
};
#endif // !ROBOTSHAPE_HPP
