#include "WheelVectors.hpp"

WheelVectors::WheelVectors(sf::Vector2f position,
	sf::Angle forwardAngle, float forwardBaseLength,
	sf::Angle rollerAngle, float rollerBaseLength,
	sf::Color color, float thickness, sf::Vector2f headSize)
	: _forwardBaseLength(forwardBaseLength), _rollerBaseLength(rollerBaseLength)
{
	auto fwdVec = std::make_unique<PointVector>(position, forwardAngle, _forwardBaseLength, color, thickness, headSize);
	auto rollVec = std::make_unique<PointVector>(position, rollerAngle, _rollerBaseLength, color, thickness, headSize);
	_forwardVector = fwdVec.get();
	_rollerVector = rollVec.get();
	add(std::move(fwdVec));
	add(std::move(rollVec));
}
void WheelVectors::setForwardLength(float length) {
	_forwardVector->setLength(length + (length > 0 ? 1 : -1) * _forwardBaseLength);
}
void WheelVectors::setRollerLength(float length) {
	_rollerVector->setLength(length + (length > 0 ? 1 : -1) * _rollerBaseLength);
}

void WheelVectors::RollerVectorVisible(bool visible) { _rollerVectorVisible = visible; }
void WheelVectors::ForwardVectorVisible(bool visible) { _forwardVectorVisible = visible; }
// X = forward length, Y = roller length
void WheelVectors::setBothVectors(sf::Vector2f lengths) {
	_forwardVector->setLength(lengths.x + (lengths.x > 0 ? 1 : -1) * _forwardBaseLength);
	_rollerVector->setLength(lengths.y + (lengths.y > 0 ? 1 : -1) * _rollerBaseLength);
}


void WheelVectors::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_forwardVectorVisible)
		_forwardVector->draw(target, states);
	if (_rollerVectorVisible)
		_rollerVector->draw(target, states);
}
