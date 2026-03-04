#ifndef INPUT_SETTINGS_WINDOW_HPP
#define INPUT_SETTINGS_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "AppConfig.hpp"

class InputSettingsWindow {
public:
	using OnSettingsChanged = std::function<void(const InputSettings&)>;

	InputSettingsWindow(const AppConfig& config);
	~InputSettingsWindow();

	void open(const InputSettings& settings);
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
	void renderContent();

	//Return true if settings were changed and callback will called
	bool renderKeyboardMapping();
	bool renderControllerMapping();

	std::unique_ptr<sf::RenderWindow> _window;
	WindowConfig _windowConfig;
	InputSettings _settings;
	KeyboardMapping _keyboardMapping;
	ControllerMapping _controllerMapping;
	OnSettingsChanged _onSettingsChanged;
	bool _pendingClose = false;
	bool _isOpen = false;
	int _inputTypeIndex = 0; // 0 for keyboard, 1 for controller

	int* _waitingForKey = nullptr;
	int* _waitingForButton = nullptr;
	int* _waitingForAxis = nullptr;
	bool _bindingChanged = false;

	const InputSettings _defaultSettings;
};

#endif // !INPUT_SETTINGS_WINDOW_HPP