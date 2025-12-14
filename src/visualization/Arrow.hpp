#ifndef ARROW_HPP
#define ARROW_HPP
#include <SFML/Graphics.hpp>
#include "CompositeShape.hpp"

class Arrow : public CompositeShape {
public:
    Arrow(float length, sf::Color color = sf::Color::White, float thickness = 3.f, float headWidth = 10, float headHeigh = 10);
    void setLength(float length);

private:
    sf::RectangleShape* shaft;
    sf::ConvexShape* head;
};

#endif // !ARROW_HPP
