#ifndef COMPOSITESHAPE_HPP
#define COMPOSITESHAPE_HPP
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

template <typename T>
concept SFMLDrawableTransformable = std::derived_from<T, sf::Drawable>&&
std::derived_from<T, sf::Transformable>;

class CompositeShape : public sf::Drawable, public sf::Transformable {
public:

	template <SFMLDrawableTransformable T>
	void add(std::unique_ptr<T> object) {
		_drawables.push_back(std::move(object));
		updateBounds();
	}

	sf::FloatRect getLocalBounds() const { return _bounds; }

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
protected: 
	void drawBoundingBox(sf::RenderTarget& target, sf::RenderStates states) const;
private:
	std::vector<std::unique_ptr<sf::Drawable>> _drawables;
	sf::FloatRect _bounds = sf::FloatRect({ 0,0 }, { 0,0 });

	void updateBounds();
};

#endif // !COMPOSITESHAPE_HPP
