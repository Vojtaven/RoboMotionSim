#include "PhysicsEngine.hpp"
#include "RobotState.hpp"
#include "RobotConfig.hpp"
#include <algorithm> // For std::abs
void PhysicsEngine::update(const float dt, RobotState& state, const RobotConfig& config)
{
	limitMovement(state, config);
	toGlobalFrame(state);
	updatePosition(dt, state);
	this->toWheelSpeed(state, config);
}

void PhysicsEngine::updatePosition(const float dt, RobotState& state) {
	state.position += state.globalvelocity * dt;
	state.chassisAngle += state.angularVelocity * dt;
	state.frontAngle = state.chassisAngle;
}

void PhysicsEngine::toGlobalFrame(RobotState& state) {
	Vec2f& global = state.globalvelocity;
	Vec2f& local = state.localVelocity;
	const float angle = state.frontAngle;
	global.x = local.x * cos(angle) - local.y * sin(angle);
	global.y = local.x * sin(angle) + local.y * cos(angle);

}

void PhysicsEngine::toWheelSpeed(RobotState& state, const RobotConfig& config) {
	const auto wheels = config.GetRobotWheels();
	const auto robotSpeed = state.localVelocity;
	const auto omega = state.angularVelocity;
	const RobotDriveType driveType = config.GetRobotDriveType();

	int i = 0;
	for (const auto& wheel : wheels) {
		float finalWheelSpeed = 0.0f;

		float local_vx = robotSpeed.x - (omega * wheel.y_position);
		float local_vy = robotSpeed.y + (omega * wheel.x_position);

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
