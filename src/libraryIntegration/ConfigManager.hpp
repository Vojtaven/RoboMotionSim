#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include "RoboConfig.hpp"
class ConfigManager {
public:
	void LoadConfigFromFile(const std::string& path);
	void SaveConfigToFile(const std::string& nameOfFile);

	const RoboConfig* GetReadOnlyConfig() const { return &_config; }
	RoboConfig* GetConfig() { return &_config; }
private:
	RoboConfig _config;
};

#endif //CONFIG_PARSER_HPP
