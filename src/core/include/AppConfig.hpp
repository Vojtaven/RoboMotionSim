#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP
#include "MathUtils.hpp"
#include <string>
#include <array>

// Here to discard configs with old version
#define CONFIG_VERSION "0.6.3"
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
	Vec2i gridSpacing = { 50, 50 };
	bool lockGridSpacingRatio = true;
	bool showGrid = true;
	bool lockViewOnRobot = false;
	std::array<float, 3> gridColor = { 80.0f / 255.0f, 80.0f / 255.0f, 80.0f / 255.0f };
	int frameRateLimit = 60;
};

struct AppSettings {
	InputType inputType;
};

struct AppConfig {
	std::string appName = "";
	std::string configVersion = "";
	WindowConfig mainWindow;
	WindowConfig renderSettingsWindow;
	WindowConfig appSettingsWindow;
	RenderSettings renderSettings;
	AppSettings appSettings;
};

#endif // !APP_CONFIG_HPP
