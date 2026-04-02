#include "shapes/WheelVectors.hpp"
#include "RobotState.hpp"

WheelVectors::WheelVectors(const RobotParts::Wheel& wheel,
	sf::Color forwardColor, sf::Color rollerColor, sf::Color directionColor,
	float thickness, sf::Vector2f headSize)
	: _rollerAngle(wheel.roller_angle), _wheelAngle(wheel.wheel_angle),
	  _forwardColor(forwardColor), _rollerColor(rollerColor), _directionColor(directionColor)
{
	auto position = sf::Vector2f{ wheel.x_position, wheel.y_position };

	auto fwdVec = std::make_unique<PointVector>(position, wheel.wheel_angle,0.0f, forwardColor, thickness, headSize);
	auto rollVec = std::make_unique<PointVector>(position,wheel.roller_angle + wheel.wheel_angle, 0.0f, rollerColor, thickness, headSize);
	auto dirVec = std::make_unique<PointVector>(position, wheel.wheel_angle, 0.0f, directionColor, thickness, headSize);
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
	_rollerLength = length;
	_rollerVector->setLength(length);
}

// X = forward length, Y = roller length
void WheelVectors::update(const WheelState& state) {
	setForwardLength(state.speed);
	setRollerLength(state.rollerSpeed);
	updateDirectionVector();

	auto applyPowered = [&](sf::Color c) -> sf::Color {
		if (!state.powered) return sf::Color(150, 150, 150, 180);
		return c;
	};
	_forwardVector->setColor(applyPowered(_forwardColor));
	_rollerVector->setColor(applyPowered(_rollerColor));
	_directionVector->setColor(applyPowered(_directionColor));
}


void WheelVectors::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (_forwardVectorVisible) 
		_forwardVector->draw(target, states);
	if (_rollerVectorVisible)
		_rollerVector->draw(target, states);
	if (_directionVectorVisible)
		_directionVector->draw(target, states);
}

// Combine forward and roller velocity vectors using the law of cosines / atan2
void WheelVectors::updateDirectionVector() {
	const float directionVectorLength = std::sqrtf(_rollerLength * _rollerLength
		+ _forwardLength * _forwardLength
		+ 2 * _forwardLength * _rollerLength * std::cos(_rollerAngle));


	_directionVector->setLength(directionVectorLength);

	const float directionVectorAngle = std::atan2f(_rollerLength * std::sinf(_rollerAngle),
		_forwardLength + _rollerLength * std::cosf(_rollerAngle));

	_directionVector->setRotation(directionVectorAngle + _wheelAngle);
}
