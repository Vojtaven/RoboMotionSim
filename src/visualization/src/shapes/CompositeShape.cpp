#include "shapes/CompositeShape.hpp"


void CompositeShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
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
		else if (auto compositeShape = dynamic_cast<CompositeShape*>(d.get()))
			b = compositeShape->getLocalBounds();
		else
			throw std::runtime_error("Unknown type in drawable vector!");

		if (first) {
			total = b;
			first = false;
		}
		else {
			float minX = std::min(total.position.x, b.position.x);
			float minY = std::min(total.position.y, b.position.y);
			float maxX = std::max(total.position.x + total.size.x, b.position.x + b.size.x);
			float maxY = std::max(total.position.y + total.size.y, b.position.y + b.size.y);

			total.position = { minX, minY };
			total.size = { maxX - minX, maxY - minY };
		}
	}

	_bounds = total;
}
