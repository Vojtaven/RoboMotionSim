#include "CompositeShape.hpp"


void CompositeShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	//Bounding box
	sf::RectangleShape rect;
	rect.setPosition(_bounds.position);
	rect.setSize(_bounds.size);
	rect.setFillColor({ 0,0,0 });
	rect.setOutlineColor({ 255,255,255 });
	rect.setOutlineThickness(1);
	target.draw(rect, states);


	for (auto& drawable : _drawables)
		target.draw(*drawable, states);


}

void CompositeShape::updateBounds() {
	if (_drawables.empty()) {
		_bounds = sf::FloatRect({ 0,0 }, { 0,0 });
		return;
	}

	bool first = true;
	sf::FloatRect total;

	for (const auto& d : _drawables) {
		const sf::Transform identity;
		sf::FloatRect b;
		if (auto shape = dynamic_cast<sf::Shape*>(d.get()))
			b = shape->getGlobalBounds();
		else if (auto sprite = dynamic_cast<sf::Sprite*>(d.get()))
			b = sprite->getGlobalBounds();
		else if (auto text = dynamic_cast<sf::Text*>(d.get()))
			b = text->getGlobalBounds();
		else
			throw std::runtime_error("Unknown type in drawable vector!");

		if (first) {
			total = b;
			first = false;
		}
		else {
			float minX = std::min(total.position.x, b.position.x);
			float minY = std::min(total.position.y, b.position.y);
			total.position = { minX,minY };

			float maxWidth = std::max(total.position.x + total.size.x, b.position.x + b.size.x) - total.position.x;
			float maxHeight = std::max(total.position.y + total.size.y, b.position.y + b.size.y) - total.position.y;

			total.size = { maxWidth,maxHeight };
		}
	}

	_bounds = total;
}
