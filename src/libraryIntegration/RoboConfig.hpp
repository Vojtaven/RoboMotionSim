#ifndef ROBO_CONFIG_H
#define ROBO_CONFIG_H

#include <vector>
#include <format>
#include "Utils.hpp"
namespace RoboParts {
	class Motor {
	public:
		/**
		 * @brief Stores the absolute maximum speed the hardware can theoretically handle (in steps/sec).
		 * This acts as an upper safety limit, potentially used by derived classes
		 * to cap the speed set by setMaxSpeed().
		 */
		const float hardwareMaxSpeedLimit;

		// @brief Stores the amount of steps the motors need to take to do full rotation of wheel including gearbox's ratio etc
		const float stepsPerRotation;

		const int pin1, pin2;

		std::string ExportToConfig() {
			return std::format("{},{},{},{}", pin1, pin2, hardwareMaxSpeedLimit, stepsPerRotation);
		}

		static Motor CreateFromConfig(std::string values) {
			auto splittedValues = Utils::split(values, ',');

			if (splittedValues.size() != 4)
				throw std::runtime_error("Wrong number of values for motor constructor");

			int pin1 = std::stoi(splittedValues[0]);
			int pin2 = std::stoi(splittedValues[1]);
			float maxSpeed = std::stof(splittedValues[2]);
			float steps = std::stof(splittedValues[3]);

			return Motor{ maxSpeed,steps,pin1,pin2 };
		}
	};

	class Wheel {
	public:
		const float diameter;
		// Position of wheel center relative to robot center in mm
		const float x_position;
		// Position of wheel center relative to robot center in mm
		const float y_position;
		// Angle of the wheel's driving direction relative to robot frame in radians
		const float wheel_angle;
		// Angle of the rollers on the omni wheel in radians
		const float roller_angle;

		std::string ExportToConfig() {
			return std::format("{},{},{},{},{}", diameter, x_position, y_position,Utils::RadiansToDegree(  wheel_angle),Utils::RadiansToDegree( roller_angle));
		}

		static Wheel CreateFromConfig(std::string values) {
			auto splittedValues = Utils::split(values, ',');

			if (splittedValues.size() != 5)
				throw std::runtime_error("Wrong number of values for motor constructor");

			float diameter = std::stof(splittedValues[0]);
			float x = std::stof(splittedValues[1]);
			float y = std::stof(splittedValues[2]);
			float angle = Utils::DegreesToRadians(std::stof(splittedValues[3]));
			float roller = Utils::DegreesToRadians(std::stof(splittedValues[4]));

			return Wheel{ diameter,x,y,angle,roller };
		}
	};

	struct DriveAxle_t { Wheel wheel; Motor motor; };
}
class RoboConfig {
public:
	const std::vector<RoboParts::DriveAxle_t>& GetRobotDriveAxels() const { return _axels; }
	const std::string& GetDriveType() { return _driveType; }
	void AddAxel(RoboParts::DriveAxle_t& axel) { _axels.push_back(axel); }
	void AddAxel(RoboParts::DriveAxle_t axel) { _axels.push_back(axel); }
	void ChangeDriveType(const std::string& type) { _driveType = type; }
private:
	std::vector<RoboParts::DriveAxle_t> _axels;
	std::string _driveType = "";
	// Things used in simulator only
	// TODO
	// 1) Add chassis
};


#endif //ROBO_CONFIG_H