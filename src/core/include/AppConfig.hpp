#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP
#include "MathUtils.hpp"
#include <string>
#include <array>

// Here to discard configs with old version
#define CONFIG_VERSION "0.6.8"
enum class InputType {
	Keyboard = 0,
	Controller = 1,
	IPC = 2
};

struct WindowConfig {
	Vec2i position = { 0,0 };
	Vec2i size = { 0,0 };
	int defaultFontSize = 13;
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
	unsigned int DefaultFontSize = 30;
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

struct KeyboardMapping {
	int upKey = 22;
	int downKey = 18;
	int leftKey = 0;
	int rightKey = 3;
	int chassisRotateLeftKey = 16;
	int chassisRotateRightKey = 4;
	int frontRotateLeftKey = 9;
	int frontRotateRightKey = 10;
};

struct ControllerMapping {
	int controllerId = 0;
	int moveXAxis = 1;
	int moveYAxis = 0;
	int chassisRotateAxis = 4;
	int frontRotateLeftButton = 2;
	int frontRotateRightButton = 1;
	int deadzone = 5; // Deadzone for joystick axes, in percentage (0-100)
};


struct IPCMapping {
	std::string address = "tcp://127.0.0.1";
	uint16_t command_port = 5555;
	uint16_t response_port = 5556;
	uint16_t telemetry_port = 5557;
	float heartbeatTimeout = 5.0f; // Time in seconds to wait without receiving a heartbeat before considering the connection lost
};

struct InputSettings {
	InputType inputType = InputType::Keyboard;
	ControllerMapping controllerMapping;
	KeyboardMapping keyboardMapping;
	IPCMapping ipcMapping;
	float maxSpeed = 200; // Max speed in mm/s, used to scale input values to real speeds
	float maxRotationSpeed = 60; // Max rotation speed in degrees/s, used to scale input values to real speeds
	bool registerInputWithoutFocus = true;
};

struct AppConfig {
	std::string appName = "";
	std::string configVersion = "";
	WindowConfig mainWindow;
	WindowConfig renderSettingsWindow;
	WindowConfig appSettingsWindow;
	WindowConfig inputSettingsWindow;
	WindowConfig robotStatWindow;
	RenderSettings renderSettings;
	InputSettings inputSettings;
};

#endif // !APP_CONFIG_HPP
