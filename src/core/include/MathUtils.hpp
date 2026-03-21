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
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

	bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return !(*this == other); }
    // Helper for distances/physics
    float length() const { return std::hypot(x, y); }
    float* data() { return &x; }

    // 2D rotation (forward: counterclockwise by angle)
    Vec2 rotated(float angle) const {
        T c = (T)std::cos(angle), s = (T)std::sin(angle);
        return { x * c - y * s, x * s + y * c };
    }
    // Inverse 2D rotation (clockwise by angle, i.e. transpose of rotation matrix)
    Vec2 rotatedInverse(float angle) const {
        T c = (T)std::cos(angle), s = (T)std::sin(angle);
        return { x * c + y * s, -x * s + y * c };
    }
};

using Vec2i = Vec2<int>;
using Vec2f =  Vec2<float>;

static inline float DegreesToRadians(float degrees) {
    return (float)(degrees * (std::numbers::pi / 180));
}

static inline float RadiansToDegrees(float radians) {
    return (float)(radians / (std::numbers::pi / 180));
}

#endif // !MATHUTILS_HPP