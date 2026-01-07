#include "RobotConfig.hpp"

const std::vector<RobotParts::DriveAxle_t>& RobotConfig::GetRobotDriveAxels() const {
	return _axels;
}
std::vector<RobotParts::DriveAxle_t>& RobotConfig::GetRobotDriveAxels() {
	return _axels;
}

const RobotDriveType RobotConfig::GetRobotDriveType() const  {
	return _DriveType;
}
void RobotConfig::AddAxel(RobotParts::DriveAxle_t& axel) {
	_axels.push_back(axel);
}
void RobotConfig::AddAxel(RobotParts::DriveAxle_t axel) {
	_axels.push_back(axel);
}
void RobotConfig::ChangeDriveType(RobotDriveType type) {
	_DriveType = type;
}

int RobotConfig::getWheelCount()const  {
	return _axels.size();
}
