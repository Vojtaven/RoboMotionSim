#ifndef ROBOTSTATE_HPP
#define ROBOTSTATE_HPP

#include <vector>
#include "MathUtils.hpp"

struct WheelState {
	float speed = 0;
	float rollerSpeed = 0;
};

struct DirectionVector {
	float angle = 0;
	float length = 0;
	Vec2f position;
};

struct RobotState {
	RobotState(int wheelCount) : wheelCount(wheelCount), wheels(wheelCount) {}

	Vec2f position;
	float chassisAngle = 0;
	float frontAngle = 0;

	Vec2f localVelocity;
	Vec2f globalvelocity;
	float angularVelocity = 0;
	float frontAngularVelocity = 0;

	int wheelCount;
	std::vector<WheelState> wheels;
	std::vector<DirectionVector> directionVectors;
};

#endif // !ROBOTSTATE_HPP
