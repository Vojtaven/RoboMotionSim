#include "VisualizationEngine.hpp"
#include "shapes/CompositeShape.hpp"
#include "shapes/RobotShape.hpp"
#include "RobotState.hpp"
#include "SFMLHelper.hpp"
#include "MathUtils.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdint>

void  VisualizationEngine::createMainWindow(AppConfig& appConfig) {
	_mainWindow = std::make_unique<MainWindow>(appConfig);
}

VisualizationEngine::VisualizationEngine(AppConfig& appConfig, const RobotConfig& robotConfig)
{
	_mainWindow = std::make_unique<MainWindow>(appConfig);
	_mainWindow->open(robotConfig);
}
