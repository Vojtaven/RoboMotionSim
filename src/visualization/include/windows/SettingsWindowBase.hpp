// Author: Vojtech Venzara
// Date: 2026-04-04
// Description: Abstract base class providing common functionality for all settings windows

#ifndef SETTINGS_WINDOW_BASE_HPP
#define SETTINGS_WINDOW_BASE_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include "AppConfig.hpp"

class SettingsWindowBase {
public:
	SettingsWindowBase(const sf::Image& icon) : _icon(icon) {}
	virtual ~SettingsWindowBase();

	bool isOpen() const;
	const WindowConfig& getSavedConfig() const { return _windowConfig; }
	void draw();

protected:
	void openWindow(const char* title);
	void closeWindow(bool closeFromRoot = false);
	void saveWindowConfig();
	void firstTimeSetup(Vec2i defaultSize);
	void sanitizeWindowConfig();

	// Common event processing: polls events, processes ImGui events, detects close.
	// Returns false if the window was closed (caller should return early).
	bool processEvents();

	std::unique_ptr<sf::RenderWindow> _window;
	const sf::Image& _icon;
	WindowConfig _windowConfig;
	bool _pendingClose = false;
	bool _isOpen = false;
};

#endif // !SETTINGS_WINDOW_BASE_HPP
