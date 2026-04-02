#ifndef INPUT_SETTINGS_WINDOW_HPP
#define INPUT_SETTINGS_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "AppConfig.hpp"
#include "ConfigSection.hpp"
#include "windows/SettingsWindowBase.hpp"

class InputSettingsWindow : public SettingsWindowBase {
public:
	InputSettingsWindow(ConfigSection<InputSettings>& inputSettings, const WindowConfig& windowConfig, const sf::Image& icon);
	~InputSettingsWindow() = default;

	void open(const InputSettings& settings);
	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt);
private:
	void renderContent();
	bool renderCommonSettings();
	bool renderKeyboardMapping(bool& commonChanged);
	bool renderControllerMapping(bool& commonChanged);
	bool renderIPCMapping();
	bool renderSerialMapping();
	void notifyAboutChanges(bool reset,bool commonChanged, bool subsectionChanged,bool topLevelChanged);
	ConfigSection<InputSettings>& _inputSettings;
	InputSettings _settings;
	KeyboardMapping _keyboardMapping;
	ControllerMapping _controllerMapping;
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
