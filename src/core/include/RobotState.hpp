#ifndef ROBOTSTATE_HPP
#define ROBOTSTATE_HPP

#include <vector>
#include "MathUtils.hpp"

struct WheelState {
	float speed = 0; // mm/s
	float rollerSpeed = 0; // mm/s
	double distanceTraveled = 0; // mm
	float lastDistanceDisplacement = 0; // mm
};

struct DirectionVector {
	float angle = 0; // Rad
	float length = 0; // mm
	Vec2f position; // mm
};

struct RobotState {
	RobotState(int wheelCount) : wheelCount(wheelCount), wheels(wheelCount) {}
	Vec2d position; //mm
	Vec2d distanceTraveled = { 0,0 }; //mm
	Vec2d lastDistanceDisplacement = { 0,0 }; //mm
	Vec2d localVelocity; // mm/s
	Vec2d globalVelocity; // mm/s
	double chassisAngle = 0; //Rad
	double lastChassisDisplacement = 0; //Rad
	double frontAngle = 0; //Rad
	double lastFrontDisplacement = 0; //Rad
	double angularVelocity = 0; //  Rad/s
	double frontAngularVelocity = 0; //  Rad/s
	
	int wheelCount;
	std::vector<WheelState> wheels;
	std::vector<DirectionVector> directionVectors;
	
	bool fromWheelSpeeds = false; // Determines if we calculate robot local speed from wheel speeds
};

#endif // !ROBOTSTATE_HPP
