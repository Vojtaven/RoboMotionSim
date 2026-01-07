#ifndef POINT_VECTOR_HPP
#define POINT_VECTOR_HPP
#include "Arrow.hpp"
#include <SFML/Graphics.hpp>
class PointVector : public Arrow {
public:
	PointVector(sf::Vector2f position, sf::Angle angle, float length, sf::Color color = sf::Color::White, float thickness = 3.f, sf::Vector2f headSize = { 10,10 })
		: Arrow(length, color, thickness, headSize) {
		setPosition(position);
		setRotation(angle);
	}
	PointVector(sf::Vector2f position, float angle, float length, sf::Color color = sf::Color::White, float thickness = 3.f, sf::Vector2f headSize = { 10,10 })
		: PointVector(position, sf::radians(angle), length, color, thickness, headSize) {
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (std::abs( _shaft->getSize().x) > 0.1f)
			Arrow::draw(target, states);
	}
};

#endif // !POINT_VECTOR_HPP