#include "PhysicsEngine.hpp"
#include "RobotState.hpp"
#include "RobotConfig.hpp"
#include <iostream>
#include <algorithm> // For std::abs
void PhysicsEngine::update(const float dt, RobotState& state, const RobotConfig& config)
{
	limitMovement(state, config);
	toGlobalFrame(state);
	updatePosition(dt, state);
	this->toWheelSpeed(state, config);
}

void PhysicsEngine::updatePosition(const float dt, RobotState& state) {
	//state.position += state.globalvelocity * dt;
	state.chassisAngle += state.angularVelocity * dt;
	state.frontAngle += state.frontAngularVelocity * dt;
}

void PhysicsEngine::toGlobalFrame(RobotState& state) {
	Vec2f& global = state.globalvelocity;
	Vec2f& local = state.localVelocity;
	const float angle = state.frontAngle + state.chassisAngle;
	global.x = local.x * cos(angle) - local.y * sin(angle);
	global.y = local.x * sin(angle) + local.y * cos(angle);
}

void PhysicsEngine::toWheelSpeed(RobotState& state, const RobotConfig& config) {
	const auto wheels = config.GetRobotWheels();
	const RobotDriveType driveType = config.GetRobotDriveType();
	const float chassisCos = cos(state.chassisAngle);
	const float chassisSin = sin(state.chassisAngle);
	// Convert global velocity to chassis-front-relative velocity
	const float chassis_vx = state.globalvelocity.x * chassisCos + state.globalvelocity.y * chassisSin;
	const float chassis_vy = -state.globalvelocity.x * chassisSin + state.globalvelocity.y * chassisCos;

	const auto omega = state.angularVelocity;
	int i = 0;
	for (const auto& wheel : wheels) {
		float finalWheelSpeed = 0.0f;

		float local_vx = chassis_vx - (omega * wheel.y_position);
		float local_vy = chassis_vy + (omega * wheel.x_position);

		float cos_w = cos(wheel.wheel_angle);
		float sin_w = sin(wheel.wheel_angle);

		float v_long = local_vx * cos_w + local_vy * sin_w;
		float v_tran = -local_vx * sin_w + local_vy * cos_w;

		float cos_roller = cos(wheel.roller_angle);

		if (std::abs(cos_roller) < 0.001f) {
			state.wheels[i].speed = v_long;
			state.wheels[i].rollerSpeed = v_tran;
		}
		else {
			state.wheels[i].speed = v_long + v_tran * tan(wheel.roller_angle);
			state.wheels[i].rollerSpeed = (driveType == RobotDriveType::DIFFERENTIAL) ? 0 : v_tran / cos_roller;
		}
		i++;
	}
}

void PhysicsEngine::limitMovement(RobotState& state, const RobotConfig& config) {
	if (config.GetRobotDriveType() == RobotDriveType::DIFFERENTIAL) {
		state.localVelocity.y = 0;
		state.frontAngle = state.chassisAngle;
	}

}
