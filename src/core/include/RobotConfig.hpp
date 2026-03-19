#ifndef ROBO_CONFIG_H
#define ROBO_CONFIG_H

#include <ranges>
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
		// Angle of the rollers on the omni wheel in radians
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
	const std::vector<RobotParts::DriveAxle_t>& GetRobotDriveAxles() const { return _axles; }
	std::vector<RobotParts::DriveAxle_t>& GetRobotDriveAxles() { return _axles; }
	auto GetRobotWheels() const {
		return _axles |
			std::ranges::views::transform([](const auto& axle) -> const RobotParts::Wheel& { return axle.wheel; });
	}
	auto GetRobotWheels() {
		return _axles |
			std::ranges::views::transform([](auto& axle) -> RobotParts::Wheel& { return axle.wheel; });
	}
	RobotDriveType GetRobotDriveType() const { return _driveType; }
	void AddAxle(RobotParts::DriveAxle_t axle) {_axles.push_back(axle); }
	void ChangeDriveType(RobotDriveType type) { _driveType = type; }
	int getWheelCount() const { return static_cast<int>(_axles.size()); }
private:
	std::vector<RobotParts::DriveAxle_t> _axles;
	RobotDriveType _driveType = RobotDriveType::DIFFERENTIAL;
};

#endif // ROBO_CONFIG_H
