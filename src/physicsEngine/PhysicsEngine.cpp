#include "PhysicsEngine.hpp"


void PhysicsEngine::Update(float dt, RoboConfig* config) 
{
	config->GetPhysicsHandle()->updatePosition(dt);
	this->ToWheelSpeed(config);
}

void PhysicsEngine::ToWheelSpeed(RoboConfig* config) {
	auto handle = config->GetPhysicsHandle();
	auto wheels = config->GetRobotWheels();
	auto robotSpeed = handle->speed;

	const float speedFirstPart = robotSpeed.x - robotSpeed.y * robotSpeed.angular_velocity;
	const float speedSecondPart = robotSpeed.x + robotSpeed.y * robotSpeed.angular_velocity;
	for (RoboParts::Wheel& wheel : wheels) {
		wheel.linear_speed = (speedFirstPart * cos(wheel.wheel_angle + wheel.roller_angle) 
			+ speedSecondPart * sin(wheel.wheel_angle + wheel.roller_angle))
			/ cos(wheel.roller_angle);
	}
}
