// Author: Vojtech Venzara
// Date: 2026-02-18
// Description: Point and vector rendering utility for displaying position vectors and directional information

#ifndef POINT_VECTOR_HPP
#define POINT_VECTOR_HPP
#include "Arrow.hpp"
#include <SFML/Graphics.hpp>
class PointVector : public Arrow {
public:
	PointVector(sf::Vector2f position, float angle, float length, sf::Color color = sf::Color::White, float thickness = 8.f, sf::Vector2f headSize = { 25,25 })
		: Arrow(length, color, thickness, headSize) {
		setPosition(position);
		setRotation(angle);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (std::abs( _shaft->getSize().x) > 0.1f)
			Arrow::draw(target, states);
	}

	void setRotation(const float angle) {
		Arrow::setRotation(sf::radians(angle));
	}
};

#endif // !POINT_VECTOR_HPP
