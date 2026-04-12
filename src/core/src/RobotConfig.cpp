// Author: Vojtech Venzara
// Date: 2026-04-02
// Description: Data structure containing all robot-specific configuration parameters including wheel
//              layout, dynamics, and constraints

#include "RobotConfig.hpp"

#include <cmath>
#include <sstream>

std::optional<std::string> RobotConfig::validateConfig() const {
	if (_driveType == RobotDriveType::CUSTOM)
		return std::nullopt;

	const int wheelCount = getWheelCount();

	switch (_driveType) {
		case RobotDriveType::DIFFERENTIAL:
			if (wheelCount < 2)
				return "Differential drive requires at least 2 wheels";
			break;
		case RobotDriveType::OMNI_WHEEL:
			if (wheelCount < 3)
				return "Omni wheel drive requires at least 3 wheels";
			break;
		case RobotDriveType::MECANUM:
			if (wheelCount != 4)
				return "Mecanum drive requires exactly 4 wheels";
			break;
		default:
			break;
	}

	for (int i = 0; i < wheelCount; i++) {
		const auto& axle = _axles[i];

		if (axle.wheel.diameter <= 0.0f)
			return "Wheel " + std::to_string(i + 1) + " has non-positive diameter";

		if (axle.motor.stepsPerRotation <= 0.0f)
			return "Motor " + std::to_string(i + 1) + " has non-positive steps per rotation";

		if (axle.motor.hardwareMaxSpeedLimit <= 0.0f)
			return "Motor " + std::to_string(i + 1) + " has non-positive hardware max speed limit";
	}

	constexpr float positionEpsilon = 0.01f; // mm
	for (int i = 0; i < wheelCount; i++) {
		for (int j = i + 1; j < wheelCount; j++) {
			float dx = _axles[i].wheel.x_position - _axles[j].wheel.x_position;
			float dy = _axles[i].wheel.y_position - _axles[j].wheel.y_position;
			if (std::hypot(dx, dy) < positionEpsilon)
				return "Wheels " + std::to_string(i + 1) + " and " + std::to_string(j + 1) + " are at the same position";
		}
	}

	if (wheelCount >= 3) {
		bool allCollinear = true;
		float x0 = _axles[0].wheel.x_position;
		float y0 = _axles[0].wheel.y_position;
		float x1 = _axles[1].wheel.x_position;
		float y1 = _axles[1].wheel.y_position;
		for (int i = 2; i < wheelCount; i++) {
			// cross product of (p1-p0) x (pi-p0) — zero means all points lie on the same line
			float cross = (x1 - x0) * (_axles[i].wheel.y_position - y0)
			            - (y1 - y0) * (_axles[i].wheel.x_position - x0);
			if (std::fabs(cross) > 0.1f) {
				allCollinear = false;
				break;
			}
		}
		if (allCollinear && _driveType != RobotDriveType::DIFFERENTIAL)
			return "All wheels are collinear - robot cannot perform omnidirectional movement";
	}

	return std::nullopt;
}
