#include "RobotVisualization.hpp"
#include "CompositeShape.hpp"

void Visualization::DrawRobotFromConfig(sf::RenderTarget& target, const RoboConfig& config, const float scale, const sf::Angle rotation) {
	CompositeShape compositeShape;

	compositeShape.setPosition({ 400,400 });
	compositeShape.setOrigin({ 0,0 });

	std::unique_ptr<sf::CircleShape> circle;

	circle = std::make_unique<sf::CircleShape>(5);
	circle->setPosition({ 50,50 });
	circle->setFillColor({ 255,255,255 });
	circle->setOrigin({ 5,5 });
	compositeShape.add(std::move(circle));

	circle = std::make_unique<sf::CircleShape>(5);
	circle->setPosition({ -50,-50 });
	circle->setFillColor({ 0,0,255 });
	circle->setOrigin({ 5,5 });
	compositeShape.add(std::move(circle));

	circle = std::make_unique<sf::CircleShape>(5);
	circle->setPosition({ 50,-50 });
	circle->setFillColor({ 255,0,0 });
	circle->setOrigin({ 5,5 });
	compositeShape.add(std::move(circle));

	circle = std::make_unique<sf::CircleShape>(5);
	circle->setPosition({ -50,50 });
	circle->setFillColor({ 0,255,0 });
	circle->setOrigin({ 5,5 });
	compositeShape.add(std::move(circle));

	circle = std::make_unique<sf::CircleShape>(1);
	circle->setPosition({ 0,0 });
	circle->setFillColor({ 50,50,50 });
	circle->setOrigin({ 1,1 });
	compositeShape.add(std::move(circle));

	compositeShape.setRotation(rotation);
	target.draw(compositeShape);
}

