#ifndef SFMLHELPER_HPP
#define SFMLHELPER_HPP
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include "MathUtils.hpp"

static inline sf::Vector2f ToSFMLVector2f(const Vec2f vec) { return { vec.x, vec.y }; }
static inline sf::Vector2f ToSFMLVector2f(const Vec2i vec) { return { (float)vec.x, (float)vec.y }; }
static inline sf::Vector2i ToSFMLVector2i(const Vec2f vec) { return { (int)vec.x, (int)vec.y }; }
static inline sf::Vector2i ToSFMLVector2i(const Vec2i vec) { return { (int)vec.x, (int)vec.y }; }
static inline sf::Vector2u ToSFMLVector2u(const Vec2f vec) { return { (unsigned int)vec.x, (unsigned int)vec.y }; }
static inline sf::Vector2u ToSFMLVector2u(const Vec2i vec) { return { (unsigned int)vec.x, (unsigned int)vec.y }; }

static inline Vec2f FromSFMLVector(const sf::Vector2f vec) { return { vec.x, vec.y }; }
static inline Vec2i FromSFMLVector(const sf::Vector2i vec) { return { vec.x, vec.y }; }
static inline Vec2i FromSFMLVector(const sf::Vector2u vec) { return { (int)vec.x, (int)vec.y }; }

static inline sf::Angle ToSFMLAngle(const float angle) { return sf::radians(angle); }

static inline float FromSFMLAngle(const sf::Angle angle) { return angle.asRadians(); }
#endif // !SFMLHELPER_HPP
