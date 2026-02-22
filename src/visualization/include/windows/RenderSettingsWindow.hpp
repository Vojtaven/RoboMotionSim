#ifndef SETTINGS_WINDOW_HPP
#define SETTINGS_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "AppConfig.hpp"
#include "Window.hpp"

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

private:
	void firstTimeSetup();
	void saveConfig();
	std::unique_ptr<sf::RenderWindow> _window;
	void renderContent();
	WindowConfig _windowConfig;
	RenderSettings _settings;
	OnSettingsChanged _onSettingsChanged;
	bool _pendingClose = false;
	bool _isOpen = false;
	float _gridSpacingRatio = 1.0f;
};

#endif // !SETTINGS_WINDOW_HPP