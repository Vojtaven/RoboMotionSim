#include "ConfigManager.hpp"
#include "MathUtils.hpp"
#include <filesystem>
#include <string>
#include "AppDefaultConfig.h"
#include "RobotDefaultConfig.h"
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

ConfigManager::ConfigManager(const Vec2i screenSize) : _systemConfigDir(getUserConfigDir() / APP_NAME) {
    loadDefaultConfigs(screenSize); 	
}


void ConfigManager::loadDefaultConfigs(const Vec2i screenSize) {
	const std::filesystem::path robotConfigPath = _systemConfigDir / "robot_config.ini";
	const std::filesystem::path appConfigPath = _systemConfigDir / "app_config.json";
    try {
		loadAppConfig(appConfigPath.string());
    }
    catch(...){
		createDefaultConfigs(appConfigPath, (const char*)DEFAULT_APP_CONFIG, DEFAULT_APP_CONFIG_SIZE);
        loadAppConfig(appConfigPath.string());
		_appConfig.appVersion = APP_VERSION;
		_appConfig.appName = APP_NAME;
		_appConfig.MainWindowConfig.position = { screenSize.x / 4, screenSize.y / 4 };
		_appConfig.MainWindowConfig.size = { screenSize.x / 2, screenSize.y / 2 };
		saveDefaultAppConfig();
    }

    try {
		loadRobotConfig((_systemConfigDir / "robot_config.ini").string());
    }
    catch (...){
        createDefaultConfigs(robotConfigPath, (const char*)DEFAULT_ROBOT_CONFIG, DEFAULT_ROBOT_CONFIG_SIZE);
        loadRobotConfig(robotConfigPath.string());
	}

}


void ConfigManager::createDefaultConfigs(std::filesystem::path path, const char* data, const int lenght) {
    std::filesystem::create_directories(path.parent_path());

    // Write embedded default config to file
    std::ofstream file(path, std::ios::binary);
    if (file) {
        file.write(data,lenght);
    }
    else {
		throw std::runtime_error("Failed to create default config at: " + path.string());
    }

	file.close();
}

std::filesystem::path ConfigManager::getUserConfigDir() {
#ifdef _WIN32
    // Windows: %APPDATA%
    size_t len = 0;
    char* appData = nullptr;
    if (_dupenv_s(&appData, &len, "APPDATA") == 0 && appData) {
        std::filesystem::path path(appData);
        free(appData);
        return path;
    }
    return std::filesystem::temp_directory_path();
#elif __LINUX__
    // Linux: ~/.config
    const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfig) {
        return std::filesystem::path(xdgConfig) ;
    }
    const char* home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".config";
    }
    return std::filesystem::temp_directory_path();

#else 
	throw std::runtime_error("Unsupported platform for config directory retrieval");
#endif
}