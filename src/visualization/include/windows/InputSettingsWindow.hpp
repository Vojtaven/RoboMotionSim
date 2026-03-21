#ifndef INPUT_SETTINGS_WINDOW_HPP
#define INPUT_SETTINGS_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "AppConfig.hpp"
#include "windows/SettingsWindowBase.hpp"

class InputSettingsWindow : public SettingsWindowBase {
public:
	using OnSettingsChanged = std::function<void(const InputSettings&)>;

	InputSettingsWindow(const AppConfig& config, const sf::Image& icon);
	~InputSettingsWindow() = default;

	void open(const InputSettings& settings);
	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt);
	void setOnSettingsChanged(OnSettingsChanged callback) { _onSettingsChanged = std::move(callback); }
private:
	void renderContent();
	//Return true if settings were changed and callback will called
	bool renderCommonSettings();
	bool renderKeyboardMapping();
	bool renderControllerMapping();
	bool renderIPCMapping();
	bool renderSerialMapping();

	InputSettings _settings;
	KeyboardMapping _keyboardMapping;
	ControllerMapping _controllerMapping;
	OnSettingsChanged _onSettingsChanged;
	int _inputTypeIndex = 0; // 0 for keyboard, 1 for controller, 2 for IPC

	int* _waitingForKey = nullptr;
	int* _waitingForButton = nullptr;
	int* _waitingForAxis = nullptr;
	bool _bindingChanged = false;

	std::vector<std::string> _controllerNames;
	sf::Time _controllerRefreshAccumulator;
	const InputSettings _defaultSettings;
};

#endif // !INPUT_SETTINGS_WINDOW_HPP
