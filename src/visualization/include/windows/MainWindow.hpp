#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "rendering/RenderEngine.hpp"
#include <SFML/Graphics.hpp>
#include "RenderSettingsWindow.hpp"
#include "InputSettingsWindow.hpp"
class MainWindow
{
public:
	MainWindow(AppConfig& config);
	void open(const RobotConfig& robotConfig);
	void setRobotConfig(const RobotConfig& config, bool holdPosition = true) { _renderEngine->updateRobotShape(config, holdPosition); }
	void close();
	void update(const float dt, const RobotState& robotState);
	AppConfig& getSavedAppConfig() { return _appConfig; }
	void saveConfig();
	void draw();
	sf::Window& getWindow() { return *_window; }
	bool isOpen() const { return _isOpen; }
	Vec2f getRenderWindowCenter() const { return _renderEngine->getWindowCenter(); }
	void SetOnInputSettingsChanged(std::function<void()> callback);
private:
	void saveWindowConfig(WindowConfig& config) const;
	void renderImGuiMenu();
	void initImGui();
	bool _showMenu = false;
	bool _showFps = false;
	bool _isOpen = true;
	bool _showInputError = false;
	WindowConfig& _windowConfig;
	AppConfig& _appConfig;
	std::unique_ptr<RenderEngine> _renderEngine;
	std::unique_ptr<sf::RenderWindow> _window;
	std::unique_ptr<RenderSettingsWindow> _settingsWindow;
	std::unique_ptr<InputSettingsWindow> _inputSettingsWindow;

	std::function<void()> _onInputSettingsChanged;
	// Other windows management
	void initializeOtherWindows();
	void closeOtherWindows();
	void updateAllOtherWindows(sf::Time dt);
	void drawAllOtherWindows();
};

#endif // !MAIN_WINDOW_HPP
