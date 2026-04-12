// Author: Vojtech Venzara
// Date: 2026-04-02
// Description: High-level rendering orchestrator managing windows, scenes, input display, and
//              integration with the SFML rendering pipeline

#include "VisualizationEngine.hpp"
#include "shapes/CompositeShape.hpp"
#include "shapes/RobotShape.hpp"
#include "RobotState.hpp"
#include "SFMLHelper.hpp"
#include "MathUtils.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdint>

VisualizationEngine::VisualizationEngine(AppConfig& appConfig, ConfigSection<RobotConfig>& robotConfigSection, const RobotConfig& robotConfig)
{
	_mainWindow = std::make_unique<MainWindow>(appConfig, robotConfigSection);
	_mainWindow->open(robotConfig);
}
