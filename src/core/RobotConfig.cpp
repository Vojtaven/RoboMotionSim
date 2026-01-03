#include "RobotConfig.hpp"

const std::vector<RobotParts::DriveAxle_t>& RobotConfig::GetRobotDriveAxels() const {
	return _axels;
}
std::vector<RobotParts::DriveAxle_t>& RobotConfig::GetRobotDriveAxels() {
	return _axels;
}

const std::string& RobotConfig::GetRobotDriveType() {
	return _driveType;
}
void RobotConfig::AddAxel(RobotParts::DriveAxle_t& axel) {
	_axels.push_back(axel);
}
void RobotConfig::AddAxel(RobotParts::DriveAxle_t axel) {
	_axels.push_back(axel);
}
void RobotConfig::ChangeDriveType(const std::string& type) {
	_driveType = type;
}

int RobotConfig::getWheelCount()const  {
	return _axels.size();
}
