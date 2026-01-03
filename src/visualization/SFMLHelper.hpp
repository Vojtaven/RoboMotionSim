#ifndef SFMLHELPER_HPP
#define SFMLHELPER_HPP
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include "MathUtils.hpp"

static inline sf::Vector2f ToSFMLVector(const Vec2 vec) { return { vec.x, vec.y }; }
static inline sf::Angle ToSFMLAngle(const float angle) { return sf::radians(angle); }


static inline Vec2 FromSFMLVector(const sf::Vector2f vec) { return { vec.x, vec.y }; }
static inline float FromSFMLAngle(const sf::Angle angle) { return angle.asRadians(); }
#endif // !SFMLHELPER_HPP
