// Author: Vojtech Venzara
// Date: 2026-03-01
// Description: General utility functions and helper methods used across the configuration system for common operations

#include "Utils.hpp"

std::vector<std::string> Utils::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        tokens.push_back(item);
    } 
    return tokens;
}

