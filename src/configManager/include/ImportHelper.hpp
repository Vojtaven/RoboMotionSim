// Author: Vojtech Venzara
// Date: 2026-03-18
// Description: Utility module for importing and validating robot configuration from external files
//              with error handling

#ifndef IMPORT_HELPER_HPP
#define IMPORT_HELPER_HPP
#include <string>
#include "RobotConfig.hpp"
#include "AppConfig.hpp"	
#include "Utils.hpp"
class ImportHelper
{
public:
	static RobotConfig loadRobotConfigFrom(const std::string& path);
	static AppConfig loadAppConfigFrom(const std::string& path);
private:
	static RobotParts::Motor createFromConfigMotor(const std::string& values, ExprEvaluator<float>& evaluator);
	static RobotParts::Wheel createFromConfigWheel(const std::string& values, ExprEvaluator<float>& evaluator);
	static RobotDriveType createFromConfigDriveType(const std::string& str);
};
#endif // !IMPORT_HELPER_HPP
