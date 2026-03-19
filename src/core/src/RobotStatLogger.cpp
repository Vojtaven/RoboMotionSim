#include "RobotStatLogger.hpp"
#include <fstream>

void RobotStatLogger::startLogging(const std::string& filename, int wheelCount, bool overwrite) {
    if (overwrite) {
        _logFile = std::ofstream(filename, std::ios::out | std::ios::trunc);
		writeHeader(wheelCount);
    }
    else
        _logFile = std::ofstream(filename, std::ios::out | std::ios::app);
}

void RobotStatLogger::stopLogging() {
    _logFile.close();
}
RobotStatLogger::~RobotStatLogger() {
    stopLogging();
}
void RobotStatLogger::writeHeader(int wheelCount)
{
    _logFile << "time,pos_x,pos_y,chassis_angle,vel_x,vel_y,angular_vel,distance_x,distance_y";

    for (int i = 0; i < wheelCount; i++)
    {
        _logFile << ",w" << i << "_speed"
            << ",w" << i << "_roller_speed"
            << ",w" << i << "_distance";
    }

    _logFile << "\n";
}

void RobotStatLogger::logStats(
    std::chrono::system_clock::time_point const& timeStamp,
    const RobotState& robotState)
{
    if(!_logFile.is_open())
        return;

    using namespace std::chrono;

    // format wall clock time
    auto sec = floor<std::chrono::seconds>(timeStamp);
    auto ms = duration_cast<milliseconds>(timeStamp - sec).count();

    std::string timeStr = std::format("{:%H:%M:%S}", sec);

    _logFile << timeStr << "." << std::format("{:03}", ms);

    // robot state
    _logFile << ","
        << robotState.position.x << ","
        << robotState.position.y << ","
        << robotState.globalVelocity.x << ","
        << robotState.globalVelocity.y << ","
        << robotState.chassisAngle << ","
        << robotState.angularVelocity << ","
        << robotState.distanceTraveled.x << ","
		<< robotState.distanceTraveled.y;

    // wheels
    for (const auto& w : robotState.wheels)
    {
        _logFile << ","
            << w.speed << ","
            << w.rollerSpeed << ","
            << w.distanceTraveled << ","
            << w.lastDistanceDisplacement;
    }

    _logFile << "\n";
}
