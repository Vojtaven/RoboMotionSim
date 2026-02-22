#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "Window.hpp"
#include "RenderEngine.hpp"
#include <SFML/Graphics.hpp>
#include "SettingsWindow.hpp"
class MainWindow
{
public:
	MainWindow(const AppConfig& config);
	void open(const RobotConfig& robotConfig);
	void setRobotConfig(const RobotConfig& config, bool holdPosition = false) { _renderEngine->updateRobotShape(config, holdPosition); }
	void close();
	void update(float dt, const RobotState& robotState);
	AppConfig& getSavedAppConfig() { return _appConfig; }
	void saveConfig();
	void draw();
	sf::Window& getWindow() { return *_window; }
	bool isOpen() const { return _isOpen; }
	Vec2f getRenderWindowCenter() const { return _renderEngine->getWindowCenter(); }
private:
	void saveWindowConfig(WindowConfig& config) const;
	void renderImGuiMenu();
	void initImGui();
	bool _showMenu = false;
	bool _showFps = false;
	bool _isOpen = true;
	WindowConfig& _windowConfig;
	AppConfig _appConfig;
	std::unique_ptr<RenderEngine> _renderEngine;
	std::unique_ptr<sf::View> _mainView;
	std::unique_ptr<sf::RenderWindow> _window;
	std::unique_ptr<SettingsWindow> _settingsWindow;

	// Other windows management
	void initializeOtherWindows();
	void closeOtherWindows();
	void updateAllOtherWindows(sf::Time dt);
	void drawAllOtherWindows();
};

#endif // !MAIN_WINDOW_HPP
