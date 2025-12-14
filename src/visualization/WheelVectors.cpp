#include "WheelVectors.hpp"

WheelVectors::WheelVectors(sf::Vector2f position,
	sf::Angle forwardAngle, float forwardBaseLength,
	sf::Angle rollerAngle, float rollerBaseLength,
	sf::Color color, float thickness, sf::Vector2f headSize)
	: forwardBaseLength(forwardBaseLength), rollerBaseLength(rollerBaseLength)
{
	auto fwdVec = std::make_unique<PointVector>(position, forwardAngle, forwardBaseLength, color, thickness, headSize);
	auto rollVec = std::make_unique<PointVector>(position, rollerAngle, rollerBaseLength, color, thickness, headSize);
	forwardVector = fwdVec.get();
	rollerVector = rollVec.get();
	add(std::move(fwdVec));
	add(std::move(rollVec));
}
void WheelVectors::setForwardLength(float length) {
	forwardVector->setLength(length + (length > 0 ? 1 : -1) * forwardBaseLength);
}
void WheelVectors::setRollerLength(float length) {
	rollerVector->setLength(length + (length > 0 ? 1 : -1) * rollerBaseLength);
}

void WheelVectors::RollerVectorVisible(bool visible) { rollerVectorVisible = visible; }
void WheelVectors::ForwardVectorVisible(bool visible) { forwardVectorVisible = visible; }
// X = forward length, Y = roller length
void WheelVectors::setBothVectors(sf::Vector2f lengths) {
	forwardVector->setLength(lengths.x + (lengths.x > 0 ? 1 : -1) * forwardBaseLength);
	rollerVector->setLength(lengths.y + (lengths.y > 0 ? 1 : -1) * rollerBaseLength);
}