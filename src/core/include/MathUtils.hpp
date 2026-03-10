#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP
#include <cmath>
#include <numbers>

template<typename T>
struct Vec2 {
    T x = 0;
    T y = 0;

    // Basic vector math (Operator Overloading)
    Vec2 operator+(const Vec2& other) const { return { x + other.x, y + other.y }; }
    Vec2 operator-(const Vec2& other) const { return { x - other.x, y - other.y }; }
    Vec2 operator*(float scalar) const { return { x * (T)scalar, y * (T)scalar }; }
    Vec2 operator*(int scalar) const { return { x * (T)scalar, y * (T)scalar }; }
    Vec2 operator/(float scalar) const { return { x / (T)scalar, y / (T)scalar }; }
    Vec2 operator/(int scalar) const { return { x / (T)scalar, y / (T)scalar }; }
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

	bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return !(*this == other); }
    // Helper for distances/physics
    float length() const { return std::sqrt(x * x + y * y); }
    float* data() { return &x; }
};

struct Vec2i : public Vec2<int> {};
struct Vec2f : public Vec2<float> {};

static inline float DegreesToRadians(float degrees) {
    return (float)(degrees * (std::numbers::pi / 180));
}

static inline float RadiansToDegree(float radians) {
    return (float)(radians / (std::numbers::pi / 180));
}

#endif // !MATHUTILS_HPP