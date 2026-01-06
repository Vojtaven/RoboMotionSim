#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include "RobotConfig.hpp"
#include "silent_exprtk.hpp"
#include "Utils.hpp"
class ConfigManager {
public:
	void loadConfigFromFile(const std::string& path);
	void saveConfigToFile(const std::string& nameOfFile);

	const RobotConfig& getReadOnlyConfig() const { return _config; }
	RobotConfig& getConfig() { return _config; }
private:
	static std::string exportToConfig(const RobotParts::Motor& motor);
	static std::string exportToConfig(const RobotParts::Wheel& motor);
	static RobotParts::Motor createFromConfigMotor(const std::string& values, ExprEvaluator<float>& evaluator);
	static RobotParts::Wheel createFromConfigWheel(const std::string& values,ExprEvaluator<float>& evaluator);
	RobotDriveType stringToRobotDriveType(const std::string& str);
	std::string robotDriveTypeToString(RobotDriveType type);
	RobotConfig _config;
};

#endif //CONFIG_PARSER_HPP
