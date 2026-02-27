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
		const float hardwareMaxSpeedLimit;

		// @brief Stores the amount of steps the motors need to take to do full
		// rotation of wheel including gearbox's ratio etc
		const float stepsPerRotation;

		const int pin1, pin2;
	};

	struct Wheel {
		const float diameter;
		// Position of wheel center relative to robot center in mm
		const float x_position;
		// Position of wheel center relative to robot center in mm
		const float y_position;
		// Angle of the wheel's driving direction relative to robot frame in radians
		const float wheel_angle;
		// Angle of the rollers on the omni wheel in radians
		const float roller_angle;

		const Vec2f driveVector;
	};

	struct DriveAxle_t {
		Wheel wheel;
		Motor motor;
	};
} // namespace RobotParts

enum class RobotDriveType {
	DIFFERENTIAL,
	OMNI_WHEEL,
	MECANUM
};

class RobotConfig {
public:
	const std::vector<RobotParts::DriveAxle_t>& GetRobotDriveAxels() const { return _axels; }
	std::vector<RobotParts::DriveAxle_t>& GetRobotDriveAxels() { return _axels; }
	auto GetRobotWheels() const {
		return _axels |
			std::ranges::views::transform([](const auto& axel) -> const RobotParts::Wheel& { return axel.wheel; });
	}
	auto GetRobotWheels() {
		return _axels |
			std::ranges::views::transform([](auto& axel) -> RobotParts::Wheel& { return axel.wheel; });
	}
	const RobotDriveType GetRobotDriveType() const { return _DriveType; }
	void AddAxel(RobotParts::DriveAxle_t& axel) { _axels.push_back(axel); }
	void AddAxel(RobotParts::DriveAxle_t axel) { _axels.push_back(axel); }
	void ChangeDriveType(RobotDriveType type) { _DriveType = type; }
	int getWheelCount() const { return static_cast<int>(_axels.size()); }
private:
	std::vector<RobotParts::DriveAxle_t> _axels;
	RobotDriveType _DriveType ;
	// Things used in simulator only
	// TODO
	// 1) Add chassis
};

#endif // ROBO_CONFIG_H