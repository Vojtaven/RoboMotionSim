#include "ImportHelper.hpp"
#include "RobotConfig.hpp"
#include "ConfigJsonSerialization.hpp"
std::vector<std::pair<std::string, std::string>> readIni(const std::string& path);
static inline std::string trim(std::string s);
std::vector<std::pair<std::string, std::string>> getByPrefix(
	const std::vector<std::pair<std::string, std::string>>& vec, const std::string& prefix);

RobotConfig ImportHelper::loadRobotConfigFrom(const std::string& path) {
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

	RobotConfig config;
	config.ChangeDriveType(createFromConfigDriveType(type));
	ExprEvaluator<float> evaluator;

	for (size_t i = 0; i < wheelCount; i++)
	{
		config.AddAxle(RobotParts::DriveAxle_t{
		createFromConfigWheel(wheels[i].second,evaluator),
		createFromConfigMotor(motors[i].second,evaluator)
			});
	}


	return config;
}



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



RobotParts::Wheel ImportHelper::createFromConfigWheel(const std::string& values,
	ExprEvaluator<float>& evaluator) {
	auto splittedValues = Utils::split(values, ',');

	if (splittedValues.size() != 5)
		throw std::runtime_error("Wrong number of values for motor constructor");

	float diameter = evaluator.EvaluateExpressions(splittedValues[0]);
	float x = evaluator.EvaluateExpressions(splittedValues[1]);
	float y = evaluator.EvaluateExpressions(splittedValues[2]);
	float angle =
		DegreesToRadians(evaluator.EvaluateExpressions(splittedValues[3]));
	float roller =
		DegreesToRadians(evaluator.EvaluateExpressions(splittedValues[4]));

	return RobotParts::Wheel{ diameter, x, y, angle, roller };
}


RobotDriveType ImportHelper::createFromConfigDriveType(const std::string& str) {
	std::string lower = str;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

	if (lower == "differential") return RobotDriveType::DIFFERENTIAL;
	if (lower == "omni_wheel" || lower == "omniwheel") return RobotDriveType::OMNI_WHEEL;
	if (lower == "mecanum") return RobotDriveType::MECANUM;

	throw std::invalid_argument("Invalid RobotDriveType: " + str);
}


RobotParts::Motor ImportHelper::createFromConfigMotor(const std::string& values, ExprEvaluator<float>& evaluator) {
	auto splittedValues = Utils::split(values, ',');

	if (splittedValues.size() != 4)
		throw std::runtime_error("Wrong number of values for motor constructor");

	int pin1 = (int)evaluator.EvaluateExpressions(splittedValues[0]);
	int pin2 = (int)evaluator.EvaluateExpressions(splittedValues[1]);
	float maxSpeed = evaluator.EvaluateExpressions(splittedValues[2]);
	float steps = evaluator.EvaluateExpressions(splittedValues[3]);

	return RobotParts::Motor{ maxSpeed, steps, pin1, pin2 };
}



// App config


AppConfig ImportHelper::loadAppConfigFrom(const std::string& path) {
	using json = nlohmann::json;

	AppConfig config; 

	if (!std::filesystem::exists(path))
		throw std::runtime_error("Failed to open config file");

	std::ifstream file(path);

	if (!file.is_open())
		throw std::runtime_error("Failed to open config file");

	json j;
	file >> j;
	config = j.get<AppConfig>();
	return config;
}
