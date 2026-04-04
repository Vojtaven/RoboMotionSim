#ifndef SFMLHELPER_HPP
#define SFMLHELPER_HPP
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <array>
#include <cstdint>
#include "MathUtils.hpp"

// Use static_cast to convert between Vec2 and sf::Vector2 types, e.g.:
//   static_cast<sf::Vector2f>(myVec2f)
//   static_cast<Vec2i>(sfmlVec)

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
