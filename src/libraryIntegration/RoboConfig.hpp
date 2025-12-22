#ifndef ROBO_CONFIG_H
#define ROBO_CONFIG_H

#include <vector>
#include <format>
#include <algorithm>
#include "Utils.hpp"
#include "PhysicsObjects.hpp"
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

		static Motor CreateFromConfig(const std::string& values, ExprEvaluator<float>& evaluator) {
			auto splittedValues = Utils::split(values, ',');

			if (splittedValues.size() != 4)
				throw std::runtime_error("Wrong number of values for motor constructor");

			int pin1 = (int)evaluator.EvaluateExpressions(splittedValues[0]);
			int pin2 = (int)evaluator.EvaluateExpressions(splittedValues[1]);
			float maxSpeed = evaluator.EvaluateExpressions(splittedValues[2]);
			float steps = evaluator.EvaluateExpressions(splittedValues[3]);

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

		float angular_speed = 0;
		float linear_speed = 0;
		std::string ExportToConfig() {
			return std::format("{},{},{},{},{}", diameter, x_position, y_position, Utils::RadiansToDegree(wheel_angle), Utils::RadiansToDegree(roller_angle));
		}

		static Wheel CreateFromConfig(const std::string& values, ExprEvaluator<float>& evaluator) {
			auto splittedValues = Utils::split(values, ',');

			if (splittedValues.size() != 5)
				throw std::runtime_error("Wrong number of values for motor constructor");

			float diameter = evaluator.EvaluateExpressions(splittedValues[0]);
			float x = evaluator.EvaluateExpressions(splittedValues[1]);
			float y = evaluator.EvaluateExpressions(splittedValues[2]);
			float angle = Utils::DegreesToRadians(evaluator.EvaluateExpressions(splittedValues[3]));
			float roller = Utils::DegreesToRadians(evaluator.EvaluateExpressions(splittedValues[4]));

			return Wheel{ diameter,x,y,angle,roller };
		}
	};

	struct DriveAxle_t { Wheel wheel; Motor motor; };
}
class RoboConfig {
public:
	const std::vector<RoboParts::DriveAxle_t>& GetRobotDriveAxels() const { return _axels; }
	std::vector<RoboParts::DriveAxle_t>& GetRobotDriveAxels() { return _axels; }
	auto GetRobotWheels() { return _axels | std::ranges::views::transform([](auto& axel) -> RoboParts::Wheel& { return axel.wheel; }); }
	const std::string& GetDriveType() { return _driveType; }
	void AddAxel(RoboParts::DriveAxle_t& axel) { _axels.push_back(axel); }
	void AddAxel(RoboParts::DriveAxle_t axel) { _axels.push_back(axel); }
	void ChangeDriveType(const std::string& type) { _driveType = type; }
	PhysicsHandle* GetPhysicsHandle() { return &_handle; }
private:
	std::vector<RoboParts::DriveAxle_t> _axels;
	std::string _driveType = "";
	PhysicsHandle _handle;
	// Things used in simulator only
	// TODO
	// 1) Add chassis
};


#endif //ROBO_CONFIG_H