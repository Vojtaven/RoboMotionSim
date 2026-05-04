// Author: Vojtech Venzara
// Date: 2026-05-04
// Description: JSON serialization and deserialization module for converting configuration objects
//              to/from JSON format for persistent storage

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

template<typename T>
void to_json(nlohmann::json& j, const ConfigSection<T>& section)
{
	j = section.get();
}

template<typename T>
void from_json(const nlohmann::json& j, ConfigSection<T>& section)
{
	// Uses getMutable() to skip notification — deserialization happens before subscribers exist
	section.getMutable() = j.get<T>();
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(JoystickControl, isAxis, invert, axisId, buttonId1, buttonId2)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(KeyboardMapping, upKey, downKey, leftKey, rightKey, chassisRotateLeftKey, chassisRotateRightKey, frontRotateLeftKey, frontRotateRightKey)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ControllerMapping, controllerId, yAxisControl,xAxisControl, chassisRotateControl, frontRotateControl, deadzone)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(IPCMapping, heartbeatTimeout, address, command_port, telemetry_port)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SerialMapping, portName, baudRate)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(InputSettings, inputType, controllerMapping, keyboardMapping, ipcMapping, serialMapping, maxSpeed, maxRotationSpeed, registerInputWithoutFocus, limitMotorSpeed, limitAcceleration, maxAcceleration, proportionalAccelerationLimit)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TrailSettings, trailPointSize, trailColor, trailMaxLength, pointSpawnInterval)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GridSettings, spacing, wantedGridSpacing, defaultGridFontSize, autoSpacing, color, subdivisionsCount, subGridColor, backgroundColor, textBackgroundOpacity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WindowConfig, position, size, defaultFontSize, resizable, open, wasOpenedBefore)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RenderSettings, scaleFactor, showGrid, showTrail, lockViewOnRobot, showForwardVectors, showRollerVectors, showWheelDirectionVectors, frameRateLimit, gridSettings, trailSettings)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AppConfig, appName, configVersion, mainWindow, inputSettingsWindow, robotStatWindow, robotDesignerWindow, renderSettingsWindow, renderSettings, inputSettings)

#endif // !CONFIG_JSON_SERIALIZATION_HPP
