#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP
#include "MathUtils.hpp"
#include <string>
#include <array>

#define CONFIG_VERSION "0.6.0"

struct WindowConfig {
	Vec2i position = { 0,0 };
	Vec2i size = { 0,0 };
	int frameRateLimit = 60;
	bool resizable = true;
	bool open = false;
	bool wasOpenedBefore = false;
};

struct RenderSettings {
	float scaleFactor = 1.0f;
	bool lockOnRobotCenterWhileScaling = true;
	Vec2f gridSpacing = { 50.0f, 50.0f };
	bool lockGridSpacingRatio = true;
	bool showGrid = true;
	std::array<float, 3> gridColor = { 80.0f / 255.0f, 80.0f / 255.0f, 80.0f / 255.0f };
	float gridLineThickness = 1.0f;
};

struct AppConfig {
	std::string appName = "";
	std::string configVersion = "";
	WindowConfig mainWindowConfig;
	WindowConfig settingsWindowConfig;
	RenderSettings renderSettings;
};

#endif // !APP_CONFIG_HPP
