#include "ConfigManager.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include "exprtk.hpp"


static inline std::string trim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		[](unsigned char ch) { return !std::isspace(ch); }));
	s.erase(std::find_if(s.rbegin(), s.rend(),
		[](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
	return s;
}

static std::vector<std::pair<std::string, std::string>> readIni(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("Config cannot be opened.");
	std::vector<std::pair<std::string, std::string>> result;

	std::string line;
	while (std::getline(file, line)) {
		// strip leading/trailing whitespace
		line = trim(line);

		// skip empty or comment
		if (line.empty() || line[0] == '#')
			continue;

		auto pos = line.find('=');
		if (pos == std::string::npos)
			continue;

		std::string key = trim(line.substr(0, pos));
		std::string value = trim(line.substr(pos + 1));

		result.push_back(std::make_pair(key, value));
	}

	file.close();
	return result;
}


std::vector<std::pair<std::string, std::string>> getByPrefix(
	const std::vector<std::pair<std::string, std::string>>& vec, const std::string& prefix)
{
	std::vector<std::pair<std::string, std::string>> result;

	std::copy_if(vec.begin(), vec.end(), std::back_inserter(result),
		[&](const auto& p) {
			return p.first.rfind(prefix, 0) == 0;
		});

	return result;
}



void ConfigManager::LoadConfigFromFile(const std::string& path) {
	auto parsedConfig = readIni(path);
	int wheelCount;
	std::string type;
	if (auto numberOfWheels = std::find_if(parsedConfig.begin(), parsedConfig.end(),
		[](std::pair<std::string, std::string> i) { return i.first == "WHEELS"; }); numberOfWheels == parsedConfig.end())
		throw std::runtime_error("Config doesn't contain number of wheels");
	else
		wheelCount = std::stoi(numberOfWheels->second);

	if (auto driveType = std::find_if(parsedConfig.begin(), parsedConfig.end(),
		[](std::pair<std::string, std::string> i) { return i.first == "DRIVE_TYPE"; }); driveType == parsedConfig.end())
		throw std::runtime_error("Config doesn't contain drive type");
	else
		type = driveType->second;

	auto wheels = getByPrefix(parsedConfig, "WHEEL_");
	auto motors = getByPrefix(parsedConfig, "MOTOR_");

	if (wheels.size() != wheelCount)
		throw std::runtime_error("Number of wheels and number of wheels in list doesn't match");

	if (motors.size() != wheelCount)
		throw std::runtime_error("Number of wheels and number of motors in list doesn't match");

	_config = RoboConfig();
	_config.ChangeDriveType(type);
	ExprEvaluator<float> evaluator;

	for (size_t i = 0; i < wheelCount; i++)
		_config.AddAxel({ RoboParts::Wheel::CreateFromConfig(wheels[i].second,evaluator),RoboParts::Motor::CreateFromConfig(motors[i].second,evaluator) });
}

void WriteConfigLine(const std::string& identifier, const std::string& value, std::ofstream& file) {
	file << std::format("{} = {}\n", identifier, value);
}

void ConfigManager::SaveConfigToFile(const std::string& nameOfFile) {
	std::ofstream file(nameOfFile);
	if (!file.is_open())
		throw std::runtime_error("Output file cannot be opened.");

	auto axels = _config.GetRobotDriveAxels();
	int wheelCount = axels.size();
	WriteConfigLine("DRIVE_TYPE", _config.GetDriveType(), file);
	WriteConfigLine("WHEELS", std::to_string(wheelCount), file);

	for (int i = 0; i < wheelCount; i++)
		WriteConfigLine(std::format("WHEEL_{}", i), axels[i].wheel.ExportToConfig(),file);

	for (int i = 0; i < wheelCount; i++)
		WriteConfigLine(std::format("MOTOR_{}", i), axels[i].motor.ExportToConfig(), file);

	file.close();
}
