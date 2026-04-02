#ifndef RENDER_SETTINGS_WINDOW_HPP
#define RENDER_SETTINGS_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include "AppConfig.hpp"
#include "ConfigSection.hpp"
#include "windows/SettingsWindowBase.hpp"

class RenderSettingsWindow : public SettingsWindowBase {
public:
	RenderSettingsWindow(ConfigSection<RenderSettings>& renderSettings, const WindowConfig& windowConfig, const sf::Image& icon);
	~RenderSettingsWindow() = default;

	void open(const RenderSettings& settings);
	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt);
	void setClearRobotTrail(std::function<void()> clearTrailCallback) { _clearRobotTrail = std::move(clearTrailCallback); }
private:
	void renderContent();
	ConfigSection<RenderSettings>& _renderSettings;
	RenderSettings _settings;
	std::function<void()> _clearRobotTrail;
	int _colorTypeIndex = 0; // 0 for grid color, 1 for subgrid color, 2 for trail color
};

#endif // !RENDER_SETTINGS_WINDOW_HPP
