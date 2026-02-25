#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP
#include "MathUtils.hpp"
#include <string>
#include <array>

// Here to discard configs with old version
#define CONFIG_VERSION "0.6.4"
enum class InputType {
	Keyboard,
	Controller
};

struct WindowConfig {
	Vec2i position = { 0,0 };
	Vec2i size = { 0,0 };
	bool resizable = true;
	bool open = false;
	bool wasOpenedBefore = false;
};

struct RenderSettings {
	float scaleFactor = 1.0f;
	int gridSpacing = 50;
	bool lockGridSpacingRatio = true;
	bool showGrid = true;
	bool autoGridSpacing = true;
	bool lockViewOnRobot = false;
	std::array<float, 3> gridColor = { 80.0f / 255.0f, 80.0f / 255.0f, 80.0f / 255.0f };
	int gridSubdivisions = 0;
	std::array<float, 3> subGridColor = { 40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f };
	int frameRateLimit = 60;
};

struct AppSettings {
	InputType inputType;
};

struct AppConfig {
	std::string appName = "";
	std::string configVersion = "";
	std::string fontPath = "D:/Vojta/Skola/RobotMotionSim/src/assets/font/ProggyClean.ttf";
	WindowConfig mainWindow;
	WindowConfig renderSettingsWindow;
	WindowConfig appSettingsWindow;
	RenderSettings renderSettings;
	AppSettings appSettings;
};

#endif // !APP_CONFIG_HPP
