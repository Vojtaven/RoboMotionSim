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
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WindowConfig, position, size, resizable, open, wasOpenedBefore)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RenderSettings, lockViewOnRobot, gridSubdivisions, subGridColor, frameRateLimit, scaleFactor, gridSpacing, gridColor, lockGridSpacingRatio, showGrid)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AppConfig, appName, configVersion, mainWindow, renderSettingsWindow, renderSettings, appSettingsWindow, appSettings)

#endif // !CONFIG_JSON_SERIALIZATION_HPP