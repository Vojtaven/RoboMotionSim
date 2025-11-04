#ifndef ROBO_CONFIG_H
#define ROBO_CONFIG_H

#include <vector>
namespace RoboParts {
	struct Motor {
		/**
		 * @brief Stores the absolute maximum speed the hardware can theoretically handle (in steps/sec).
		 * This acts as an upper safety limit, potentially used by derived classes
		 * to cap the speed set by setMaxSpeed().
		 */
		float _hardwareMaxSpeedLimit;

		// @brief Stores the amount of steps the motors need to take to do full rotation of wheel including gearbox's ratio etc
		float _stepsPerRotation;

		// @brief Stores the absolute maximum acceleration the motor can handle (in steps/second^2)
		float _maxAcceleration;
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

	struct DriveAxle_t { Wheel wheel; Motor motor; };
}
class RoboConfig {
public:
	const std::vector<RoboParts::DriveAxle_t>& GetRobotDriveAxels() const { return _axels; }
	void AddAxel(RoboParts::DriveAxle_t& axel) { _axels.push_back(axel); }
	void AddAxel(RoboParts::DriveAxle_t axel) { _axels.push_back(axel); }
private:
	std::vector<RoboParts::DriveAxle_t> _axels;

	// Things used in simulator only
	// TODO
	// 1) Add chassis
};


#endif //ROBO_CONFIG_H