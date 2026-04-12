// Author: Vojtech Venzara
// Date: 2026-04-02
// Description: UI window for creating and modifying robot designs with interactive parameter adjustment

#ifndef ROBOT_DESIGNER_WINDOW_HPP
#define ROBOT_DESIGNER_WINDOW_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include "AppConfig.hpp"
#include "RobotConfig.hpp"
#include "ConfigSection.hpp"
#include "windows/SettingsWindowBase.hpp"

class RobotDesignerWindow : public SettingsWindowBase {
public:
	RobotDesignerWindow(ConfigSection<RobotConfig>& robotConfig, const WindowConfig& windowConfig, const sf::Image& icon);
	~RobotDesignerWindow() = default;

	void open(const RobotConfig& robotConfig);
	void open();
	void close(bool closeFromRoot = false);
	void update(sf::Time dt);
private:
	void renderContent();
	void renderPreview();
	void loadFromRobotConfig(const RobotConfig& config);
	RobotConfig buildRobotConfig() const;
	void renderAxleEditor(int index, bool renderRollerAngle);

	ConfigSection<RobotConfig>& _robotConfig;

	int _driveTypeIndex = 0;
	std::vector<RobotParts::DriveAxle_t> _axles;



	struct StatusMessage {
		std::string message;
		bool isError;
		static StatusMessage Info(const std::string& msg) { return StatusMessage{ msg, false }; }
		static StatusMessage Error(const std::string& msg) { return StatusMessage{ msg, true }; }
		static StatusMessage Empty() { return StatusMessage{ "", false }; }
	};

	StatusMessage _statusMessage;
	bool _showPreview = true;
	std::unique_ptr<sf::RenderTexture> _previewTexture;
};

#endif // !ROBOT_DESIGNER_WINDOW_HPP
