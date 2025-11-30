#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <numbers>
static class Utils {
public:
    static std::vector<std::string> split(const std::string& s, char delimiter);
    static inline float DegreesToRadians(float degrees) {
        return degrees * (std::numbers::pi / 180);
    }

    static inline float RadiansToDegree(float radians) {
        return radians / (std::numbers::pi / 180);
    }
};
#endif // !UTILS_HPP
