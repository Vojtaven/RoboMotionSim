#ifndef ROBOT_VISUALIZATION_HPP
#define ROBOT_VISUALIZATION_HPP

#include <SFML/Graphics.hpp>
#include "RobotConfig.hpp"
#include <shapes/CompositeShape.hpp>
#include <shapes/RobotShape.hpp>
#include "RobotState.hpp"
#include "MathUtils.hpp"
#include "AppConfig.hpp"
#include "SFMLHelper.hpp"
#include "windows/SettingsWindow.hpp"

class VisualizationEngine {
public:
	VisualizationEngine(const AppConfig& appConfig, const RobotConfig& robotConfig = RobotConfig());
	~VisualizationEngine();
	void setRobotConfig(const RobotConfig& config, bool holdPosition = false);
	void setRobotPosition(const sf::Vector2f position, const sf::Angle angle);
	void moveRobotBy(const sf::Vector2f offset, const sf::Angle angleOffset);
	void draw();
	void setScaleFactor(const float scale);
	void setGridSpacing(const sf::Vector2f spacing);
	void setGridColor(const sf::Color color);
	void update(const RobotState& state);
	bool isWindowOpen() const { return _mainWindow->isOpen(); }
	Vec2f getWindowCenter() const;
	sf::Window& getMainWindow() { return *_mainWindow; }
	AppConfig& getSavedAppConfig() { return _appConfig; }
private:
	void RegenerateGridLines();
	void updateAfterResize();
	void saveAppConfigBeforeClose();
	void resetRobotPosition(sf::Vector2f pos);
	void CreateMainWindow(const AppConfig& appConfig);
	void InitImGui();
	void RenderImGuiMenu();

	std::unique_ptr<RobotShape> _robotShape;
	std::unique_ptr<sf::View> _mainView;
	std::unique_ptr<sf::RenderWindow> _mainWindow;
	float _scaleFactor = 1.0f; // 1 pixel = 1 mm
	sf::Vector2f _gridSpacing = { 25,25 };
	sf::Color _gridColor = { 80,80,80 };
	float _gridLineThickness = 1.0f;
	CompositeShape _gridLines;
	AppConfig _appConfig;

	// ImGui state
	bool _showMenu = false;
	sf::Clock _deltaClock;

	// Separate settings window
	SettingsWindow _settingsWindow;
};

#endif //ROBOT_VISUALIZATION_HPP