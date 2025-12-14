#include "Arrow.hpp"

Arrow::Arrow(float length, sf::Color color, float thickness,sf::Vector2f headSize) {
	auto shaft = std::make_unique<sf::RectangleShape>();
	auto head = std::make_unique<sf::ConvexShape>();
	shaft->setFillColor(color);
	shaft->setSize({ length, thickness });
	shaft->setOrigin({ 0, thickness / 2 });

	head->setFillColor(color);
	head->setPointCount(3);
	head->setPoint(0, { headSize.y, 0 });
	head->setPoint(1, { 0, headSize.x /2 });
	head->setPoint(2, { 0, -headSize.x /2 });
	head->setOrigin({ 0, 0 });

	head->setPosition({ length, 0 });

	this->shaft = shaft.get();
	this->head = head.get();
	add(std::move(shaft));
	add(std::move(head));
}


void Arrow::setLength(float length) {
	float absLength = std::abs(length);
	shaft->setSize({ absLength, shaft->getSize().y });
	head->setPosition({ absLength, 0 });

	if (length < 0)
		setScale({ -1.f, 1.f });
	else
		setScale({ 1.f, 1.f });
}
