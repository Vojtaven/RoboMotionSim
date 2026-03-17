#ifndef CONFIG_JSON_SERIALIZATION_HPP
#define CONFIG_JSON_SERIALIZATION_HPP
#include "AppConfig.hpp"
#include "MathUtils.hpp"
#include <nlohmann/json.hpp>

template<typename T>
void to_json(nlohmann::json& j, const Vec2<T>& v)
{
	j = { {"x", v.x}, {"y", v.y} };
}

template<typename T>
void from_json(const nlohmann::json& j, Vec2<T>& v)
{
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(JoystickControll, isAxis, invert, axisId, buttonId1, buttonId2)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(KeyboardMapping, upKey, downKey, leftKey, rightKey, chassisRotateLeftKey, chassisRotateRightKey, frontRotateLeftKey, frontRotateRightKey)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ControllerMapping, controllerId, yAxisControl,xAxisControl, chassisRotateControl, frontRotateControl, deadzone)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(IPCMapping, heartbeatTimeout, address, command_port, response_port, telemetry_port)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SerialMapping, portName, baudRate)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InputSettings, inputType, controllerMapping, keyboardMapping, ipcMapping, serialMapping, maxSpeed, maxRotationSpeed, registerInputWithoutFocus)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TrailSettings, trailPointSize, trailColor, trailMaxLenght, pointSpawnInteral)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GridSettings, spacing, WantedGridSpacing, DefaultFontSize, autoSpacing, color, subdivisionsCount, subGridColor, backgroundColor, textBackgroundOpacity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WindowConfig, position, size, defaultFontSize, resizable, open, wasOpenedBefore)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RenderSettings, scaleFactor, showGrid, showTrail, lockViewOnRobot, frameRateLimit, gridSettings, trailSettings)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AppConfig, appName, configVersion, mainWindow, inputSettingsWindow, robotStatWindow, renderSettingsWindow, appSettingsWindow, renderSettings, inputSettings)

#endif // !CONFIG_JSON_SERIALIZATION_HPP
