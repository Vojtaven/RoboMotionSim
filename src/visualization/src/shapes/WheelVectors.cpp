#include "shapes/WheelVectors.hpp"
#include "RobotState.hpp"

WheelVectors::WheelVectors(const RobotParts::Wheel& wheel,
	sf::Color forwardColor, sf::Color rollerColor, sf::Color directionColor,
	float thickness, sf::Vector2f headSize)
	: _rollerAngle(wheel.roller_angle), _wheelAngle(wheel.wheel_angle)
{
	auto position = sf::Vector2f{ wheel.x_position, wheel.y_position };

	auto fwdVec = std::make_unique<PointVector>(position, wheel.wheel_angle,0, forwardColor, thickness, headSize);
	auto rollVec = std::make_unique<PointVector>(position,wheel.roller_angle + wheel.wheel_angle, 0, rollerColor, thickness, headSize);
	auto dirVec = std::make_unique<PointVector>(position, wheel.wheel_angle, 0, directionColor, thickness, headSize);
	_forwardVector = fwdVec.get();
	_rollerVector = rollVec.get();
	_directionVector = dirVec.get();
	add(std::move(fwdVec));
	add(std::move(rollVec));
	add(std::move(dirVec));
}
void WheelVectors::setForwardLength(float length) {
	_forwardLength = length;
	_forwardVector->setLength(length);
}
void WheelVectors::setRollerLength(float length) {
	_rollerLenght = length;
	_rollerVector->setLength(length);
}

void WheelVectors::RollerVectorVisible(bool visible) { _rollerVectorVisible = visible; }
void WheelVectors::ForwardVectorVisible(bool visible) { _forwardVectorVisible = visible; }
// X = forward length, Y = roller length
void WheelVectors::update(const WheelState& state) {
	setForwardLength(state.speed);
	setRollerLength(state.rollerSpeed);
	UpdateDirectionVector();
}


void WheelVectors::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_forwardVectorVisible) 
		_forwardVector->draw(target, states);
	if (_rollerVectorVisible)
		_rollerVector->draw(target, states);
	if (_directionVectorVisible)
		_directionVector->draw(target, states);
}

void WheelVectors::UpdateDirectionVector() {
	const float directionVectorLength = std::sqrtf(_rollerLenght * _rollerLenght
		+ _forwardLength * _forwardLength
		+ 2 * _forwardLength * _rollerLenght * std::cos(_rollerAngle));


	_directionVector->setLength(directionVectorLength);

	const float directionVectorAngle = std::atan2f(_rollerLenght * std::sinf(_rollerAngle), 
		_forwardLength + _rollerLenght * std::cosf(_rollerAngle));

	_directionVector->setRotation(directionVectorAngle + _wheelAngle);
}
