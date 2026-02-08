#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP
#include "MathUtils.hpp"
#include <string>
struct WindowConfig {
	Vec2i position = { 0,0 };
	Vec2i size = { 0,0 };
	int frameRateLimit = 60;
	bool resizable = false;
	bool open = false;
};


struct AppConfig {
	std::string appName = "";
	std::string appVersion = "";
	WindowConfig MainWindowConfig;
};

#endif // !APP_CONFIG_HPP
