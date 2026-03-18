#ifndef ROBOT_DESIGNER_WINDOW_HPP
#define ROBOT_DESIGNER_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "AppConfig.hpp"
#include "RobotConfig.hpp"

class RobotDesignerWindow {
public:
	using OnRobotConfigApplied = std::function<void(const RobotConfig&)>;

	RobotDesignerWindow(const AppConfig& config,const RobotConfig& robotConfig);
	~RobotDesignerWindow();

	void open(const RobotConfig& robotConfig);
	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt);
	void draw();
	bool isOpen() const;
	const WindowConfig& getSavedConfig() const { return _windowConfig; }
	void setOnRobotConfigApplied(OnRobotConfigApplied callback) { _onRobotConfigApplied = std::move(callback); }
private:
	void firstTimeSetup();
	void saveConfig();
	void renderContent();
	void loadFromRobotConfig(const RobotConfig& config);
	RobotConfig buildRobotConfig() const;
	void renderAxleEditor(int index);

	std::unique_ptr<sf::RenderWindow> _window;
	WindowConfig _windowConfig;
	OnRobotConfigApplied _onRobotConfigApplied;
	bool _pendingClose = false;
	bool _isOpen = false;

	int _driveTypeIndex = 0;
	std::vector<RobotParts::DriveAxle_t> _axles;
	char _filePath[512] = {};
	std::string _statusMessage;
};

#endif // !ROBOT_DESIGNER_WINDOW_HPP
