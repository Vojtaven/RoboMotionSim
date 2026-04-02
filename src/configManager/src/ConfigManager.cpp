#include "ConfigManager.hpp"
#include "MathUtils.hpp"
#include <filesystem>
#include <string>
#include "RobotDefaultConfig.h"
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

ConfigManager::ConfigManager(const Vec2i screenSize) : _systemConfigDir(getUserConfigDir() / APP_NAME) {
    loadDefaultConfigs(screenSize); 	
}


void ConfigManager::loadDefaultConfigs(const Vec2i screenSize) {
	if (!std::filesystem::exists(_systemConfigDir)) {
        std::filesystem::create_directories(_systemConfigDir);
    }

	const std::filesystem::path robotConfigPath = _systemConfigDir / "robot_config.ini";
	const std::filesystem::path appConfigPath = _systemConfigDir / "app_config.json";
    try {
		loadAppConfig(appConfigPath.string());
        if (_appConfig.configVersion != CONFIG_VERSION) {
            throw std::runtime_error("App version mismatch");
        }
    }
    catch (const std::exception&) { // config missing or outdated — reset to defaults
		_appConfig.configVersion = CONFIG_VERSION;
		_appConfig.appName = APP_NAME;
		_appConfig.mainWindow.position = { screenSize.x / 4, screenSize.y / 4 };
		_appConfig.mainWindow.size = { screenSize.x / 2, screenSize.y / 2 };
		_appConfig.mainWindow.open = true;
		_appConfig.mainWindow.wasOpenedBefore = true;
		saveDefaultAppConfig();
    }

    try {
		loadRobotConfig((_systemConfigDir / "robot_config.ini").string());
    }
    catch (const std::exception&) {
        createDefaultConfigs(robotConfigPath, (const char*)DEFAULT_ROBOT_CONFIG_DATA, DEFAULT_ROBOT_CONFIG_DATA_SIZE);
        loadRobotConfig(robotConfigPath.string());
	}

}


void ConfigManager::createDefaultConfigs(std::filesystem::path path, const char* data, const int length) {
    std::filesystem::create_directories(path.parent_path());

    // Write embedded default config to file
    std::ofstream file(path, std::ios::binary);
    if (file) {
        file.write(data, length);
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
#elif defined(__linux__)
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