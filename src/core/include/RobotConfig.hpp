// Author: Vojtech Venzara
// Date: 2026-04-12
// Description: Data structure containing all robot-specific configuration parameters including wheel
//              layout, dynamics, and constraints

#ifndef ROBO_CONFIG_H
#define ROBO_CONFIG_H

#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include "MathUtils.hpp"
namespace RobotParts {
	struct Motor {
		/**
		 * @brief Stores the absolute maximum speed the hardware can theoretically
		 * handle (in steps/sec). This acts as an upper safety limit, potentially used
		 * by derived classes to cap the speed set by setMaxSpeed().
		 */
		float hardwareMaxSpeedLimit;

		// @brief Stores the amount of steps the motors need to take to do full
		// rotation of wheel including gearbox's ratio etc
		float stepsPerRotation;

		int pin1, pin2;
	};

	struct Wheel {
		float diameter;
		// Position of wheel center relative to robot center in mm
		float x_position;
		// Position of wheel center relative to robot center in mm
		float y_position;
		// Angle of the wheel's driving direction relative to robot frame in radians
		float wheel_angle;
		// Roller angle: deviation from pure omni in radians (0 = omni, ±45° = mecanum, 90° = standard)
		float roller_angle;
	};

	struct DriveAxle_t {
		Wheel wheel;
		Motor motor;
	};
} // namespace RobotParts

enum class RobotDriveType {
	DIFFERENTIAL = 0,
	OMNI_WHEEL = 1,
	MECANUM = 2,
	CUSTOM = 3
};

class RobotConfig {
public:
	const std::vector<RobotParts::DriveAxle_t>& getRobotDriveAxles() const { return _axles; }
	std::vector<RobotParts::DriveAxle_t>& getRobotDriveAxles() { return _axles; }
	auto getRobotWheels() const {
		return _axles |
			std::ranges::views::transform([](const auto& axle) -> const RobotParts::Wheel& { return axle.wheel; });
	}
	auto getRobotWheels() {
		return _axles |
			std::ranges::views::transform([](auto& axle) -> RobotParts::Wheel& { return axle.wheel; });
	}
	RobotDriveType getRobotDriveType() const { return _driveType; }
	void addAxle(RobotParts::DriveAxle_t axle) {_axles.push_back(axle); }
	void changeDriveType(RobotDriveType type) { _driveType = type; }
	int getWheelCount() const { return static_cast<int>(_axles.size()); }

	// Returns error message if config is invalid, empty optional if valid.
	// CUSTOM drive type skips all validation.
	std::optional<std::string> validateConfig() const;

private:
	std::vector<RobotParts::DriveAxle_t> _axles;
	RobotDriveType _driveType = RobotDriveType::DIFFERENTIAL;
};

#endif // ROBO_CONFIG_H
