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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AppSettings, inputType)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TrailSettings, trailPointSize, trailColor, trailMaxLenght)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GridSettings, spacing, autoSpacing, color, subdivisionsCount, subGridColor, backgroundColor, textBackgroundOpacity)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WindowConfig, position, size, resizable, open, wasOpenedBefore)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RenderSettings, scaleFactor, showGrid, showTrail, lockViewOnRobot, frameRateLimit, gridSettings, trailSettings)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AppConfig, appName, configVersion, fontPath, mainWindow, renderSettingsWindow, appSettingsWindow, renderSettings, appSettings)

#endif // !CONFIG_JSON_SERIALIZATION_HPP