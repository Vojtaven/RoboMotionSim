#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "rendering/RenderEngine.hpp"
#include <SFML/Graphics.hpp>
#include "RenderSettingsWindow.hpp"
#include "InputSettingsWindow.hpp"
#include "RobotStatWindow.hpp"
#include "RobotDesignerWindow.hpp"
#include "ConfigSection.hpp"
class MainWindow
{
public:
	MainWindow(AppConfig& config, ConfigSection<RobotConfig>& robotConfigSection);
	void open(const RobotConfig& robotConfig);
	void setRobotConfig(const RobotConfig& config, const RobotState& state) { _renderEngine->updateRobotShape(config, state); }
	void close();
	void update(const float dt, const RobotState& robotState, const std::chrono::system_clock::time_point& timeStamp);
	AppConfig& getSavedAppConfig() { return _appConfig; }
	void saveConfig();
	void draw();
	sf::Window& getWindow() { return *_window; }
	bool isOpen() const { return _isOpen; }
	Vec2f getRenderWindowCenter() const { return _renderEngine->getWindowCenter(); }
	void showErrorMessage(const std::string& message);
private:
	void saveWindowConfig(WindowConfig& config) const;
	void sanitizeWindowConfig();
	void renderImGuiMenu();
	void renderErrorMessages();
	void initImGui();
	bool _showMenu = false;
	bool _showFps = false;
	bool _showTimeStamp = false;
	bool _isOpen = true;
	bool _showInputError = false;
	std::chrono::system_clock::time_point _timeStamp;
	WindowConfig& _windowConfig;
	AppConfig& _appConfig;
	ConfigSection<RobotConfig>& _robotConfigSection;
	std::unique_ptr<RenderEngine> _renderEngine;
	std::unique_ptr<sf::RenderWindow> _window;
	std::unique_ptr<RenderSettingsWindow> _renderSettingsWindow;
	std::unique_ptr<InputSettingsWindow> _inputSettingsWindow;
	std::unique_ptr<RobotStatWindow> _robotStatWindow;
	std::unique_ptr<RobotDesignerWindow> _robotDesignerWindow;
	std::vector<std::string> _errorMessages;
	sf::Image _icon;
	// Other windows management
	void initializeOtherWindows();
	void closeOtherWindows();
	void updateAllOtherWindows(sf::Time dt, const RobotState& robotState);
	void drawAllOtherWindows();
};

#endif // !MAIN_WINDOW_HPP
