#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include "RobotConfig.hpp"
#include "silent_exprtk.hpp"
#include "Utils.hpp"
class ConfigManager {
public:
	void LoadConfigFromFile(const std::string& path);
	void SaveConfigToFile(const std::string& nameOfFile);

	const RobotConfig& GetReadOnlyConfig() const { return _config; }
	RobotConfig& GetConfig() { return _config; }
private:
	static std::string ExportToConfig(const RobotParts::Motor& motor);
	static std::string ExportToConfig(const RobotParts::Wheel& motor);
	static RobotParts::Motor CreateFromConfigMotor(const std::string& values, ExprEvaluator<float>& evaluator);
	static RobotParts::Wheel CreateFromConfigWheel(const std::string& values,ExprEvaluator<float>& evaluator);
	RobotConfig _config;
};

#endif //CONFIG_PARSER_HPP
