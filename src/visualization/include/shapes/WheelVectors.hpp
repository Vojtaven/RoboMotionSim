#ifndef WHEEL_VECTORS_HPP
#define WHEEL_VECTORS_HPP
#include "PointVector.hpp"
#include "CompositeShape.hpp"
#include "RobotConfig.hpp"
#include "RobotState.hpp"
#include <SFML/Graphics.hpp>
class WheelVectors : public CompositeShape {
private:
	PointVector* _forwardVector = nullptr;
	PointVector* _rollerVector = nullptr;
	PointVector* _directionVector = nullptr;
	bool _directionVectorVisible = true;
	bool _rollerVectorVisible = true;
	bool _forwardVectorVisible = true;
	float _forwardLength = 0, _rollerLenght = 0;
	const float _rollerAngle = 0, _wheelAngle = 0;	
public:
	WheelVectors(const RobotParts::Wheel& wheel,
		sf::Color forwardColor = sf::Color::White,
		sf::Color rollerColor = sf::Color::White,
		sf::Color directionColor = sf::Color::White,
		float thickness = 8.f,
		sf::Vector2f headSize = { 25,25 });

	void setForwardLength(float length);
	void setRollerLength(float length);
	void update(const WheelState& state);

	void RollerVectorVisible(bool visible);
	void ForwardVectorVisible(bool visible);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void UpdateDirectionVector();
};


#endif // !WHEEL_VECTORS_HPP
