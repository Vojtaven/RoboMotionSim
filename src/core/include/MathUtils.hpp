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
    Vec2 operator*(T scalar) const { return { x * scalar, y * scalar }; }
    Vec2 operator/(T scalar) const { return { x / scalar, y / scalar }; }
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
    T length() const { return std::hypot(x, y); }
    T* data() { return &x; }

    // 2D rotation (forward: counterclockwise by angle)
    Vec2 rotated(T angle) const {
        T c = std::cos(angle), s = std::sin(angle);
        return { x * c - y * s, x * s + y * c };
    }
    // Inverse 2D rotation (clockwise by angle, i.e. transpose of rotation matrix)
    Vec2 rotatedInverse(T angle) const {
        T c = std::cos(angle), s = std::sin(angle);
        return { x * c + y * s, -x * s + y * c };
    }
};

using Vec2i = Vec2<int>;
using Vec2f =  Vec2<float>;
using Vec2d = Vec2<double>;

static inline double DegreesToRadians(double degrees) {
    return degrees * (std::numbers::pi / 180.0);
}

static inline double RadiansToDegrees(double radians) {
    return radians / (std::numbers::pi / 180.0);
}

#endif // !MATHUTILS_HPP