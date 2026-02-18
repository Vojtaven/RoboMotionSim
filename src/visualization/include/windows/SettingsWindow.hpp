#ifndef SETTINGS_WINDOW_HPP
#define SETTINGS_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>

struct VisualizationSettings {
	float scaleFactor = 1.0f;
	float gridSpacing[2] = { 50.0f, 50.0f };
	float gridColor[3] = { 80.0f / 255.0f, 80.0f / 255.0f, 80.0f / 255.0f };
};

class SettingsWindow {
public:
	using OnSettingsChanged = std::function<void(const VisualizationSettings&)>;

	SettingsWindow();
	~SettingsWindow();

	void open(const VisualizationSettings& currentSettings, sf::RenderWindow& callerWindow);
	void close();
	void update();
	bool isOpen() const;

	void setOnSettingsChanged(OnSettingsChanged callback) { _onSettingsChanged = std::move(callback); }

private:
	void renderContent();

	std::unique_ptr<sf::RenderWindow> _window;
	sf::RenderWindow* _callerWindow = nullptr;
	sf::Clock _deltaClock;
	VisualizationSettings _settings;
	OnSettingsChanged _onSettingsChanged;
	bool _isOpen = false;
	bool _pendingClose = false;
};

#endif // !SETTINGS_WINDOW_HPP