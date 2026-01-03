#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP
#include <cmath>
#include <numbers>

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    // Basic vector math (Operator Overloading)
    Vec2 operator+(const Vec2& other) const { return { x + other.x, y + other.y }; }
    Vec2 operator-(const Vec2& other) const { return { x - other.x, y - other.y }; }
    Vec2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    // Helper for distances/physics
    float length() const { return std::sqrt(x * x + y * y); }
};

static inline float DegreesToRadians(float degrees) {
    return (float)(degrees * (std::numbers::pi / 180));
}

static inline float RadiansToDegree(float radians) {
    return (float)(radians / (std::numbers::pi / 180));
}

#endif // !MATHUTILS_HPP