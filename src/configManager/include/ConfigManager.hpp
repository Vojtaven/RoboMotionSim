#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <filesystem>
#include "RobotConfig.hpp"
#include "AppConfig.hpp"
#include "MathUtils.hpp"
#include "ExportHelper.hpp"
#include "ImportHelper.hpp"
class ConfigManager {
private:
	RobotConfig _robotConfig;
	AppConfig _appConfig;
	const std::filesystem::path _systemConfigDir;

public:
	ConfigManager(const Vec2i screenSize);

	void loadRobotConfig(const std::string& path) { _robotConfig = ImportHelper::loadRobotConfigFrom(path); }
	void saveRobotConfig(const std::string& nameOfFile) { ExportHelper::saveRobotConfigTo(_robotConfig, nameOfFile); }
	void loadAppConfig(const std::string& path) { _appConfig = ImportHelper::loadAppConfigFrom(path); }
	void saveAppConfig(const std::string& nameOfFile) { ExportHelper::saveAppConfigTo(_appConfig, nameOfFile); }
	void saveDefaultAppConfig() { saveAppConfig((_systemConfigDir / "app_config.json").string()); }
	void setAppConfig(const AppConfig& config) { _appConfig = config; }
	const AppConfig& getConstAppConfig() const { return _appConfig; }
	AppConfig& getAppConfig() { return _appConfig; }
	const RobotConfig& getConstRobotConfig() const { return _robotConfig; }
	RobotConfig& getRobotConfig() { return _robotConfig; }
private:
	void createDefaultConfigs(std::filesystem::path path, const char* data, const int lenght);
	void loadDefaultConfigs(const Vec2i screenSize);
	std::filesystem::path getUserConfigDir();
};

#endif //CONFIG_PARSER_HPP
