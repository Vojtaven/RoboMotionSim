#ifndef ROBOTSTATE_HPP
#define ROBOTSTATE_HPP

#include <vector>
#include "MathUtils.hpp"

struct WheelState {
	float speed = 0; // mm/s
	float rollerSpeed = 0; // mm/s
	float distanceTraveled = 0; // mm
};

struct DirectionVector {
	float angle = 0; // Rad
	float length = 0; // mm
	Vec2f position; // mm
};

struct RobotState {
	RobotState(int wheelCount) : wheelCount(wheelCount), wheels(wheelCount) {}
	Vec2f position; //mm
	float chassisAngle = 0; //Rad
	float frontAngle = 0; //Rad
	Vec2f distanceTraveled = { 0,0 }; //mm
	Vec2f localVelocity; // mm/s
	Vec2f globalvelocity; // mm/s
	float angularVelocity = 0; //  Rad/s
	float frontAngularVelocity = 0; //  Rad/s

	int wheelCount;
	std::vector<WheelState> wheels;
	std::vector<DirectionVector> directionVectors;
};

#endif // !ROBOTSTATE_HPP
