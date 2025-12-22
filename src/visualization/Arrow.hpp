#ifndef ARROW_HPP
#define ARROW_HPP
#include <SFML/Graphics.hpp>
#include "CompositeShape.hpp"

class Arrow : public CompositeShape {
public:
    Arrow(float length, sf::Color color = sf::Color::White, float thickness = 3.f, sf::Vector2f headSize = {10,10});
    void setLength(float length);

protected:
    sf::RectangleShape* _shaft;
    sf::ConvexShape* _head;
};

#endif // !ARROW_HPP
