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

struct GridSettings {
	int spacing = 50;
	static const int WantedGridSpacing = 200; // Used for auto spacing, the grid will try to keep the spacing close to this value
	bool autoSpacing = true;
	std::array<float, 3> color = { 80.0f / 255.0f, 80.0f / 255.0f, 80.0f / 255.0f };
	int subdivisionsCount = 0;
	std::array<float, 3> subGridColor = { 40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f };
};

struct RenderSettings {
	float scaleFactor = 1.0f;
	bool showGrid = true;
	bool lockViewOnRobot = false;
	int frameRateLimit = 60;
	GridSettings gridSettings;
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
