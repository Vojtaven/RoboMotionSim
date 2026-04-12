// Author: Vojtech Venzara
// Date: 2026-03-19
// Description: Logging system for recording robot state samples over time for analysis and debugging

#ifndef ROBOT_STAT_LOGGER_HPP
#define ROBOT_STAT_LOGGER_HPP
#include <RobotState.hpp>
#include <chrono>
#include <string>
#include <fstream>
class RobotStatLogger
{
public:
	RobotStatLogger() {}
	void startLogging(const std::string& filename, int wheelCount, bool overwrite = false);
	void stopLogging();
	~RobotStatLogger();
	void logStats(std::chrono::system_clock::time_point const& timeStamp, const RobotState& robotState);
private:
	void writeHeader(int wheelCount);
	std::ofstream _logFile;
};

#endif // !ROBOT_STAT_LOGGER_HPP
