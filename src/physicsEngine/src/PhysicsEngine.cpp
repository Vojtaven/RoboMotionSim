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
	toWheelSpeed(state, config,dt);
	updateDirectionVectors(state);
}

void PhysicsEngine::updatePosition(const float dt, RobotState& state) {
	state.distanceTraveled += state.localVelocity * dt;
	state.position += state.globalvelocity * dt;
	state.chassisAngle += state.angularVelocity * dt;
	state.frontAngle += state.frontAngularVelocity * dt;
}

void PhysicsEngine::toGlobalFrame(RobotState& state) {
	Vec2f& global = state.globalvelocity;
	Vec2f& local = state.localVelocity;
	const float angle = state.frontAngle + state.chassisAngle;
	global.x = local.x * std::cos(angle) - local.y * std::sin(angle);
	global.y = local.x * std::sin(angle) + local.y * std::cos(angle);
}

void PhysicsEngine::toWheelSpeed(RobotState& state, const RobotConfig& config, const float dt) {
	const auto wheels = config.GetRobotWheels();
	const RobotDriveType driveType = config.GetRobotDriveType();
	const float chassisCos = std::cos(state.chassisAngle);
	const float chassisSin = std::sin(state.chassisAngle);
	// Convert global velocity to chassis-front-relative velocity
	const float chassis_vx = state.globalvelocity.x * chassisCos + state.globalvelocity.y * chassisSin;
	const float chassis_vy = -state.globalvelocity.x * chassisSin + state.globalvelocity.y * chassisCos;

	const auto omega = state.angularVelocity;
	int i = 0;
	for (const auto& wheel : wheels) {
		float finalWheelSpeed = 0.0f;

		float local_vx = chassis_vx - (omega * wheel.y_position);
		float local_vy = chassis_vy + (omega * wheel.x_position);

		float cos_w = std::cos(wheel.wheel_angle);
		float sin_w = std::sin(wheel.wheel_angle);

		float v_long = local_vx * cos_w + local_vy * sin_w;
		float v_tran = -local_vx * sin_w + local_vy * cos_w;

		float cos_roller = cos(wheel.roller_angle);

		if (std::abs(cos_roller) < 0.001f) {
			state.wheels[i].speed = v_long;
			state.wheels[i].rollerSpeed = v_tran;
		}
		else {
			state.wheels[i].speed = v_long + v_tran * std::tan(wheel.roller_angle);
			state.wheels[i].rollerSpeed = (driveType == RobotDriveType::DIFFERENTIAL) ? 0 : v_tran / cos_roller;
		}

		state.wheels[i].distanceTraveled += state.wheels[i].speed * dt;
		i++;
	}
}

void PhysicsEngine::limitMovement(RobotState& state, const RobotConfig& config) {
	if (config.GetRobotDriveType() == RobotDriveType::DIFFERENTIAL) {
		state.localVelocity.y = 0;
		state.frontAngle = 0;
		state.frontAngularVelocity = 0;
	}

}

void PhysicsEngine::updateDirectionVectors(RobotState& state) {
	for (auto& dirVec : state.directionVectors) {
		const Vec2f& localPos = dirVec.position;

		Vec2f rotationalVelocity;
		rotationalVelocity.x = -state.angularVelocity * localPos.y;
		rotationalVelocity.y = state.angularVelocity * localPos.x;

		const float cos_front = std::cos(state.frontAngle);
		const float sin_front = std::sin(state.frontAngle);

		Vec2f totalLocalVel;
		totalLocalVel.x = state.localVelocity.x * cos_front - state.localVelocity.y * sin_front;
		totalLocalVel.y = state.localVelocity.x * sin_front + state.localVelocity.y * cos_front;

		Vec2f pointVelocity;
		pointVelocity.x = totalLocalVel.x + rotationalVelocity.x;
		pointVelocity.y = totalLocalVel.y + rotationalVelocity.y;

		dirVec.length = std::hypot(pointVelocity.x, pointVelocity.y);
		dirVec.angle = std::atan2(pointVelocity.y, pointVelocity.x);
	}
}
