#ifndef RENDER_SETTINGS_WINDOW_HPP
#define RENDER_SETTINGS_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "AppConfig.hpp"

class RenderSettingsWindow{
public:
	using OnSettingsChanged = std::function<void(const RenderSettings&)>;

	RenderSettingsWindow(const AppConfig& config);
	~RenderSettingsWindow();

	void open(const RenderSettings& settings);
	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt);
	void draw();	
	bool isOpen() const;
	const WindowConfig& getSavedConfig() const { return _windowConfig; }
	void setOnSettingsChanged(OnSettingsChanged callback) { _onSettingsChanged = std::move(callback); }
	void setClearRobotTrail(std::function<void()> clearTrailCallback) { _clearRobotTrail = std::move(clearTrailCallback); }
private:
	void firstTimeSetup();
	void saveConfig();
	std::unique_ptr<sf::RenderWindow> _window;
	void renderContent();
	WindowConfig _windowConfig;
	RenderSettings _settings;
	std::function<void()> _clearRobotTrail;
	OnSettingsChanged _onSettingsChanged;
	bool _pendingClose = false;
	bool _isOpen = false;
	int colorTypeIndex = 0; // 0 for grid color, 1 for subgrid color, 2 for trail color
};

#endif // !RENDER_SETTINGS_WINDOW_HPP