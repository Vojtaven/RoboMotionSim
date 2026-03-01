#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP
#include "MathUtils.hpp"
#include <string>
#include <array>

// Here to discard configs with old version
#define CONFIG_VERSION "0.6.5"
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

struct TrailSettings {
	std::array<float, 4> trailColor = { 1,1,0.45f,0.5f };
	int trailMaxLenght = 100;
	float trailPointSize = 10;
	float pointSpawnInteral = 0.25f; // Time in seconds between spawning trail points
};

struct GridSettings {
	int spacing = 50;
	int WantedGridSpacing = 200; // Used for auto spacing, the grid will try to keep the spacing close to this value
	unsigned int DefaultFontSize = 40;
	bool autoSpacing = true;
	std::array<float, 4> color = { 80.0f / 255.0f, 80.0f / 255.0f, 80.0f / 255.0f, 1 };
	int subdivisionsCount = 0;
	std::array<float, 4> subGridColor = { 40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f ,1 };
	std::array<float, 4> backgroundColor = { 0.0f, 0.0f, 0.0f ,1 };
	float textBackgroundOpacity = 0.7f;
};

struct RenderSettings {
	float scaleFactor = 1.0f;
	bool showTrail = true;
	bool showGrid = true;
	bool lockViewOnRobot = false;
	int frameRateLimit = 60;
	GridSettings gridSettings;
	TrailSettings trailSettings;
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
