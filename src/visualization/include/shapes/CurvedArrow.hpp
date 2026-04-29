// Author: Vojtech Venzara
// Date: 2026-04-29
// Description: Curved arrow shape used to visualize rotational speed as an arc whose sweep
//              encodes magnitude and whose orientation encodes direction (CW/CCW)

#ifndef CURVED_ARROW_HPP
#define CURVED_ARROW_HPP
#include <SFML/Graphics.hpp>
#include "CompositeShape.hpp"

class CurvedArrow : public CompositeShape {
public:
	CurvedArrow(float radius, sf::Color color = sf::Color::White, float thickness = 4.f, sf::Vector2f headSize = { 15, 15 });
	void setSweep(float radians); // sign = direction; magnitude = arc length in radians
	void setColor(sf::Color color);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	void rebuild(float sweep);
	float _radius;
	float _thickness;
	sf::Vector2f _headSize;
	sf::Color _color;
	sf::VertexArray _arc{ sf::PrimitiveType::TriangleStrip };
	sf::ConvexShape _head;
	float _currentSweep = 0.f;
};

#endif // !CURVED_ARROW_HPP
