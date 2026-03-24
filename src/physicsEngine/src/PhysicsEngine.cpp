#include "PhysicsEngine.hpp"
#include "RobotState.hpp"
#include "RobotConfig.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
void PhysicsEngine::update(const float dt, RobotState& state, const RobotConfig& config)
{
	limitMovement(state, config);

	if (state.fromWheelSpeeds) {
		if (_limitMotorSpeed)
			limitMotorSpeeds(state, config);
		calculateLocalVelocityFromWheelSpeed(state, config);
	}
	else {
		toWheelSpeed(state, config, dt);
		if (_limitMotorSpeed) {
			limitMotorSpeeds(state, config);
			calculateLocalVelocityFromWheelSpeed(state, config);
		}
	}
	toGlobalFrame(state);

	updatePosition(dt, state);
	updateDirectionVectors(state);
}

void PhysicsEngine::updatePosition(const float dt, RobotState& state) {
	state.distanceTraveled += state.localVelocity * dt;
	state.position += state.globalVelocity * dt;
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
	state.globalVelocity = state.localVelocity.rotated(state.frontAngle + state.chassisAngle);
}

void PhysicsEngine::toWheelSpeed(RobotState& state, const RobotConfig& config, const float dt) {
	const auto wheels = config.getRobotWheels();
	const RobotDriveType driveType = config.getRobotDriveType();
	// Convert global velocity to chassis-front-relative velocity
	const Vec2f chassisVel = state.localVelocity.rotated(state.frontAngle);
	const float chassisVx = chassisVel.x;
	const float chassisVy = chassisVel.y;

	const auto omega = state.angularVelocity;
	int i = 0;
	for (const auto& wheel : wheels) {
		float finalWheelSpeed = 0.0f;

		float localVx = chassisVx - (omega * wheel.y_position);
		float localVy = chassisVy + (omega * wheel.x_position);

		float cos_w = std::cos(wheel.wheel_angle);
		float sin_w = std::sin(wheel.wheel_angle);

		float v_long = localVx * cos_w + localVy * sin_w;
		float v_tran = -localVx * sin_w + localVy * cos_w;

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
	if (config.getRobotDriveType() == RobotDriveType::DIFFERENTIAL) {
		state.localVelocity.y = 0;
		state.frontAngle = 0;
		state.frontAngularVelocity = 0;
	}

}

void PhysicsEngine::limitMotorSpeeds(RobotState& state, const RobotConfig& config) {
	const auto& axles = config.getRobotDriveAxles();
	float maxRatio = 1.0f;

	for (int i = 0; i < (int)state.wheels.size() && i < (int)axles.size(); i++) {
		const auto& wheel = axles[i].wheel;
		const auto& motor = axles[i].motor;
		const float circumference = std::numbers::pi_v<float> * wheel.diameter;
		if (circumference <= 0.0f || motor.stepsPerRotation <= 0.0f) continue;

		const float speedInSteps = std::fabs(state.wheels[i].speed) * motor.stepsPerRotation / circumference;
		if (speedInSteps > motor.hardwareMaxSpeedLimit)
			maxRatio = std::max(maxRatio, speedInSteps / motor.hardwareMaxSpeedLimit);
	}

	if (maxRatio > 1.0f) {
		for (auto& wheel : state.wheels) {
			wheel.speed /= maxRatio;
			wheel.rollerSpeed /= maxRatio;
		}
	}
}

void PhysicsEngine::updateDirectionVectors(RobotState& state) {
	for (auto& dirVec : state.directionVectors) {
		const Vec2f& localPos = dirVec.position;

		Vec2f rotationalVelocity;
		rotationalVelocity.x = -state.angularVelocity * localPos.y;
		rotationalVelocity.y = state.angularVelocity * localPos.x;

		Vec2f totalLocalVel = state.localVelocity.rotated(state.frontAngle);

		Vec2f pointVelocity;
		pointVelocity.x = totalLocalVel.x + rotationalVelocity.x;
		pointVelocity.y = totalLocalVel.y + rotationalVelocity.y;

		dirVec.length = std::hypot(pointVelocity.x, pointVelocity.y);
		dirVec.angle = std::atan2(pointVelocity.y, pointVelocity.x);
	}
}

void PhysicsEngine::calculateLocalVelocityFromWheelSpeed(RobotState& state, const RobotConfig& config) {
	const auto wheels = config.getRobotWheels();

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

	const float chassisVx = (std::abs(aug[0][0]) > 1e-9f) ? aug[0][3] / aug[0][0] : 0.0f;
	const float chassisVy = (std::abs(aug[1][1]) > 1e-9f) ? aug[1][3] / aug[1][1] : 0.0f;
	const float omega = (std::abs(aug[2][2]) > 1e-9f) ? aug[2][3] / aug[2][2] : 0.0f;

	state.localVelocity = Vec2f{chassisVx, chassisVy}.rotatedInverse(state.frontAngle);
	state.angularVelocity = omega;
}
