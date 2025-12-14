#ifndef WHEEL_VECTORS_HPP
#define WHEEL_VECTORS_HPP
#include "PointVector.hpp"
#include "CompositeShape.hpp"
#include <SFML/Graphics.hpp>
class WheelVectors : public CompositeShape {
private:
	PointVector* forwardVector;
	PointVector* rollerVector;
	float forwardBaseLength = 0;
	float rollerBaseLength = 0;
	bool rollerVectorVisible = true;
	bool forwardVectorVisible = true;
public:
	WheelVectors(sf::Vector2f position,
		sf::Angle forwardAngle, float forwardBaseLength,
		sf::Angle rollerAngle, float rollerBaseLength,
		sf::Color color = sf::Color::White, float thickness = 8.f, sf::Vector2f headSize = { 25,25 });
	void setForwardLength(float length);
	void setRollerLength(float length);
	void setBothVectors(sf::Vector2f lengths);

	void RollerVectorVisible(bool visible);
	void ForwardVectorVisible(bool visible);
};


#endif // !WHEEL_VECTORS_HPP
