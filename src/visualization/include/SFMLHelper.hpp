#ifndef SFMLHELPER_HPP
#define SFMLHELPER_HPP
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cstdint>
#include "MathUtils.hpp"

static inline sf::Vector2f ToSFMLVector2f(const Vec2f vec) { return { vec.x, vec.y }; }
static inline sf::Vector2f ToSFMLVector2f(const Vec2d vec) { return { (float)vec.x, (float)vec.y }; }
static inline sf::Vector2f ToSFMLVector2f(const Vec2i vec) { return { (float)vec.x, (float)vec.y }; }
static inline sf::Vector2i ToSFMLVector2i(const Vec2f vec) { return { (int)vec.x, (int)vec.y }; }
static inline sf::Vector2i ToSFMLVector2i(const Vec2i vec) { return { (int)vec.x, (int)vec.y }; }
static inline sf::Vector2u ToSFMLVector2u(const Vec2f vec) { return { (unsigned int)vec.x, (unsigned int)vec.y }; }
static inline sf::Vector2u ToSFMLVector2u(const Vec2i vec) { return { (unsigned int)vec.x, (unsigned int)vec.y }; }

static inline Vec2f FromSFMLVector(const sf::Vector2f vec) { return { vec.x, vec.y }; }
static inline Vec2i FromSFMLVector(const sf::Vector2i vec) { return { vec.x, vec.y }; }
static inline Vec2i FromSFMLVector(const sf::Vector2u vec) { return { (int)vec.x, (int)vec.y }; }

static inline sf::Angle ToSFMLAngle(const double angle) { return sf::radians(static_cast<float>(angle)); }

static inline float FromSFMLAngle(const sf::Angle angle) { return angle.asRadians(); }
static inline sf::Vector2f MultiplyVectors(const sf::Vector2f a, const sf::Vector2f b) { return { a.x * b.x, a.y * b.y }; }	

static inline sf::Color ToSFMLColor(const std::array<float, 3>& color) {
	return sf::Color(
		(uint8_t)(color[0] * 255),
		(uint8_t)(color[1] * 255),
		(uint8_t)(color[2] * 255));
}

static inline sf::Color ToSFMLColor(const std::array<float, 4>& color) {
	return sf::Color(
		(uint8_t)(color[0] * 255),
		(uint8_t)(color[1] * 255),
		(uint8_t)(color[2] * 255),
		(uint8_t)(color[3] * 255));
}

static inline sf::Color ToSFMLColor(const sf::Color color, const float alpha) {
	return sf::Color(
		color.r,
		color.g,
		color.b,
		(uint8_t)(alpha * 255));
}

#endif // !SFMLHELPER_HPP
