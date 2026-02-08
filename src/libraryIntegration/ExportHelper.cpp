#include "ExportHelper.hpp"
#include <format>
#include <fstream>
#include "ConfigJsonSerialization.hpp"
#include "Utils.hpp"
void writeConfigLine(const std::string& identifier, const std::string& value, std::ofstream& file) {
	file << std::format("{} = {}\n", identifier, value);
}

void ExportHelper::saveRobotConfigTo(const RobotConfig& config, const std::string& nameOfFile){
	std::ofstream file(nameOfFile);
	if (!file.is_open())
		throw std::runtime_error("Output file cannot be opened.");

	auto axels = config.GetRobotDriveAxels();
	int wheelCount = axels.size();
	RobotDriveType type = config.GetRobotDriveType();
	writeConfigLine("DRIVE_TYPE", exportToConfig(type), file);
	writeConfigLine("WHEELS", std::to_string(wheelCount), file);

	for (int i = 0; i < wheelCount; i++)
		writeConfigLine(std::format("WHEEL_{}", i), exportToConfig(axels[i].wheel), file);

	for (int i = 0; i < wheelCount; i++)
		writeConfigLine(std::format("MOTOR_{}", i), exportToConfig(axels[i].motor), file);

	file.close();
}


std::string ExportHelper::exportToConfig(const RobotParts::Motor& motor) {
	return std::format("{},{},{},{}", motor.pin1, motor.pin2, motor.hardwareMaxSpeedLimit,
		motor.stepsPerRotation);
}



std::string ExportHelper::exportToConfig(const RobotParts::Wheel& wheel) {
	return std::format("{},{},{},{},{}", wheel.diameter, wheel.x_position, wheel.y_position,
		Utils::RadiansToDegree(wheel.wheel_angle),
		Utils::RadiansToDegree(wheel.roller_angle));
}


// Convert enum to string
std::string ExportHelper::exportToConfig(RobotDriveType type) {
	switch (type) {
	case RobotDriveType::DIFFERENTIAL: return "DIFFERENTIAL";
	case RobotDriveType::OMNI_WHEEL: return "OMNI_WHEEL";
	case RobotDriveType::MECANUM: return "MECANUM";
	default: throw std::invalid_argument("Unknown RobotDriveType");
	}
}


// App config

void ExportHelper::saveAppConfigTo(const AppConfig& config, const std::string& nameOfFile) {
	using json = nlohmann::json;

	json j = config;

	std::ofstream file(nameOfFile);

	if (!file.is_open())
		throw std::runtime_error("Failed to write config file");

	file << j.dump(4); // pretty print
}
