#ifndef WHEEL_VECTORS_HPP
#define WHEEL_VECTORS_HPP
#include "PointVector.hpp"
#include "CompositeShape.hpp"
#include "RobotConfig.hpp"
#include "RobotState.hpp"
#include <SFML/Graphics.hpp>
class WheelVectors : public CompositeShape {
private:
	PointVector* _forwardVector;
	PointVector* _rollerVector;
	float _forwardBaseLength = 0;
	float _rollerBaseLength = 0;
	bool _rollerVectorVisible = true;
	bool _forwardVectorVisible = true;
public:
	WheelVectors(const RobotParts::Wheel& wheel, float forwardBaseLength, float rollerBaseLength,
		sf::Color color = sf::Color::White, float thickness = 8.f, sf::Vector2f headSize = { 25,25 });
	void setForwardLength(float length);
	void setRollerLength(float length);
	void update(const WheelState& state);

	void RollerVectorVisible(bool visible);
	void ForwardVectorVisible(bool visible);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};


#endif // !WHEEL_VECTORS_HPP
