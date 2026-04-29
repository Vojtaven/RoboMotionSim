// Author: Vojtech Venzara
// Date: 2026-04-29
// Description: Curved arrow shape used to visualize rotational speed as an arc whose sweep
//              encodes magnitude and whose orientation encodes direction (CW/CCW)

#include "shapes/CurvedArrow.hpp"
#include <algorithm>
#include <cmath>

CurvedArrow::CurvedArrow(float radius, sf::Color color, float thickness, sf::Vector2f headSize)
	: _radius(radius), _thickness(thickness), _headSize(headSize), _color(color)
{
	_head.setFillColor(color);
	rebuild(0.f);
}

void CurvedArrow::setColor(sf::Color color) {
	_color = color;
	_head.setFillColor(color);
	for (size_t i = 0; i < _arc.getVertexCount(); ++i)
		_arc[i].color = color;
}

void CurvedArrow::setSweep(float radians) {
	if (std::abs(radians - _currentSweep) < 1e-4f) return;
	_currentSweep = radians;
	rebuild(radians);
}

void CurvedArrow::rebuild(float sweep) {
	_arc.clear();
	_head.setPointCount(0);
	if (std::abs(sweep) < 1e-3f) return;

	const int segments = std::max(6, static_cast<int>(std::abs(sweep) * 24.f));
	const float step = sweep / static_cast<float>(segments);
	const float halfT = _thickness * 0.5f;

	for (int i = 0; i <= segments; ++i) {
		const float a = step * static_cast<float>(i);
		const float c = std::cos(a);
		const float s = std::sin(a);
		_arc.append(sf::Vertex{ { (_radius - halfT) * c, (_radius - halfT) * s }, _color });
		_arc.append(sf::Vertex{ { (_radius + halfT) * c, (_radius + halfT) * s }, _color });
	}

	const float endAngle = sweep;
	const float c = std::cos(endAngle);
	const float s = std::sin(endAngle);
	const sf::Vector2f tipBase{ _radius * c, _radius * s };
	const float dirSign = sweep >= 0.f ? 1.f : -1.f;
	const sf::Vector2f tangent{ -s * dirSign, c * dirSign };
	const sf::Vector2f normal{ c, s };

	_head.setPointCount(3);
	_head.setFillColor(_color);
	_head.setPoint(0, tipBase + tangent * _headSize.x);
	_head.setPoint(1, tipBase - normal * (_headSize.y * 0.5f));
	_head.setPoint(2, tipBase + normal * (_headSize.y * 0.5f));
}

void CurvedArrow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_arc.getVertexCount() == 0) return;
	states.transform *= getTransform();
	target.draw(_arc, states);
	target.draw(_head, states);
}
