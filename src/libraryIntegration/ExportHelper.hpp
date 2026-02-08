#ifndef EXPORT_HELPER_HPP
#define EXPORT_HELPER_HPP
#include <string>
#include "RobotConfig.hpp"
#include "AppConfig.hpp"

class ExportHelper
{
public:
	static void saveRobotConfigTo(const RobotConfig& config, const std::string& nameOfFile);
	static void saveAppConfigTo(const AppConfig& config, const std::string& nameOfFile);
private:
	static std::string exportToConfig(const RobotParts::Motor& motor);
	static std::string exportToConfig(const RobotParts::Wheel& motor);
	static std::string exportToConfig(const RobotDriveType type);
};
#endif // !EXPORT_HELPER_HPP
