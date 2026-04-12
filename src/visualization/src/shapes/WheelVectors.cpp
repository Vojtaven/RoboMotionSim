#include "shapes/WheelVectors.hpp"
#include "RobotState.hpp"
#include "MathUtils.hpp"

WheelVectors::WheelVectors(const RobotParts::Wheel& wheel,
	sf::Color forwardColor, sf::Color rollerColor, sf::Color directionColor,
	float thickness, sf::Vector2f headSize)
	: _wheelAngle(wheel.wheel_angle),
	  _forwardColor(forwardColor), _rollerColor(rollerColor), _directionColor(directionColor)
{
	auto position = sf::Vector2f{ wheel.x_position, wheel.y_position };

	auto fwdVec = std::make_unique<PointVector>(position, wheel.wheel_angle,0.0f, forwardColor, thickness, headSize);
	auto rollVec = std::make_unique<PointVector>(position, HALF_PI_F - wheel.roller_angle + wheel.wheel_angle, 0.0f, rollerColor, thickness, headSize);
	auto dirVec = std::make_unique<PointVector>(position, wheel.wheel_angle, 0.0f, directionColor, thickness, headSize);
	_forwardVector = fwdVec.get();
	_rollerVector = rollVec.get();
	_directionVector = dirVec.get();
	add(std::move(fwdVec));
	add(std::move(rollVec));
	add(std::move(dirVec));
}
void WheelVectors::update(const WheelState& state) {
	_forwardVector->setLength(state.speed);
	_rollerVector->setLength(state.rollerSpeed);
	_directionVector->setLength(state.directionSpeed);
	_directionVector->setRotation(state.directionAngle + _wheelAngle);

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

