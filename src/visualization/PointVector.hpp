#ifndef POINT_VECTOR_HPP
#define POINT_VECTOR_HPP
#include "Arrow.hpp"
#include <SFML/Graphics.hpp>
class PointVector : public Arrow {
public:
	PointVector(sf::Vector2f position,sf::Angle angle, float length, sf::Color color = sf::Color::White, float thickness = 3.f, sf::Vector2f headSize = {10,10})
		: Arrow(length, color, thickness, headSize) {
		setPosition(position);
		setRotation(angle);
	}
};

#endif // !POINT_VECTOR_HPP
