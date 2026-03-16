#include "PhysicsEngine.hpp"
#include "RobotState.hpp"
#include "RobotConfig.hpp"
#include <iostream>
#include <algorithm> // For std::abs
void PhysicsEngine::update(const float dt, RobotState& state, const RobotConfig& config)
{
	limitMovement(state, config);

	if (state.fromWheelSpeeds)
		calculateLocalVelocityFromWheelSpeed(state, config);

	toGlobalFrame(state);

	if (!state.fromWheelSpeeds)
		toWheelSpeed(state, config, dt);

	updatePosition(dt, state);
	updateDirectionVectors(state);
}

void PhysicsEngine::updatePosition(const float dt, RobotState& state) {
	state.distanceTraveled += state.localVelocity * dt;
	state.position += state.globalvelocity * dt;
	state.chassisAngle += state.angularVelocity * dt;
	state.frontAngle += state.frontAngularVelocity * dt;

	state.lastChassisDisplacement = state.angularVelocity * dt;
	state.lastFrontDisplacement = state.frontAngularVelocity  * dt;
	state.lastDistanceDisplacement = state.localVelocity * dt;
	for (size_t i = 0; i < state.wheels.size(); i++) {
		state.wheels[i].lastDistanceDisplacement = state.wheels[i].speed * dt;
	}
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

void PhysicsEngine::calculateLocalVelocityFromWheelSpeed(RobotState& state, const RobotConfig& config) {
	const auto wheels = config.GetRobotWheels();

	// Least-squares: (J^T J) * x = J^T * b, where x = [chassis_vx, chassis_vy, omega]
	float A[3][3] = {};
	float rhs[3] = {};

	int i = 0;
	for (const auto& wheel : wheels) {
		const float cos_w = std::cos(wheel.wheel_angle);
		const float sin_w = std::sin(wheel.wheel_angle);
		const float cos_roller = std::cos(wheel.roller_angle);

		float jvx, jvy;
		if (std::abs(cos_roller) < 0.001f) {
			jvx = cos_w;
			jvy = sin_w;
		}
		else {
			const float tan_r = std::tan(wheel.roller_angle);
			jvx = cos_w - sin_w * tan_r;
			jvy = sin_w + cos_w * tan_r;
		}
		const float jw = -wheel.y_position * jvx + wheel.x_position * jvy;
		const float j[3] = { jvx, jvy, jw };

		const float speed = state.wheels[i].speed;
		for (int r = 0; r < 3; r++) {
			rhs[r] += j[r] * speed;
			for (int c = 0; c < 3; c++)
				A[r][c] += j[r] * j[c];
		}
		i++;
	}

	// Gauss-Jordan elimination with partial pivoting
	float aug[3][4] = {};
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) aug[r][c] = A[r][c];
		aug[r][3] = rhs[r];
	}

	for (int col = 0; col < 3; col++) {
		int pivot = col;
		for (int row = col + 1; row < 3; row++)
			if (std::abs(aug[row][col]) > std::abs(aug[pivot][col]))
				pivot = row;
		for (int k = 0; k < 4; k++)
			std::swap(aug[col][k], aug[pivot][k]);

		if (std::abs(aug[col][col]) < 1e-9f) continue;

		for (int row = 0; row < 3; row++) {
			if (row == col) continue;
			const float factor = aug[row][col] / aug[col][col];
			for (int k = col; k < 4; k++)
				aug[row][k] -= factor * aug[col][k];
		}
	}

	const float chassis_vx = (std::abs(aug[0][0]) > 1e-9f) ? aug[0][3] / aug[0][0] : 0.0f;
	const float chassis_vy = (std::abs(aug[1][1]) > 1e-9f) ? aug[1][3] / aug[1][1] : 0.0f;
	const float omega = (std::abs(aug[2][2]) > 1e-9f) ? aug[2][3] / aug[2][2] : 0.0f;

	// Rotate chassis-frame velocity back to local (front-relative) frame
	const float cos_front = std::cos(state.frontAngle);
	const float sin_front = std::sin(state.frontAngle);
	state.localVelocity.x = chassis_vx * cos_front + chassis_vy * sin_front;
	state.localVelocity.y = -chassis_vx * sin_front + chassis_vy * cos_front;
	state.angularVelocity = omega;
}
