#ifndef GRID_HPP
#define GRID_HPP

#include <SFML/Graphics.hpp>
#include "rendering/Text.hpp"
#include "AppConfig.hpp"
class Grid
{
public:
	Grid(const GridSettings& settings, const float& scale, const sf::Font& font, const sf::View& view);
	void updateAfterSettingsChange(const sf::View& view);
	void regenerate(const sf::View& view);
	void draw(sf::RenderTarget& target) const;
	void mapText(const sf::RenderTarget& target, const sf::View& view);
	void drawText(sf::RenderTarget& target) const;
	void setFont(const sf::Font& font) { _font = &font; }
private: 	
	void addGridText(unsigned int textSize);
	void addGridLines(const sf::Color& color, const float spacing);
	void addGridLines() {
		addGridLines(_gridColor, _spacing);
	}
	void addSubGridLines() {
		addGridLines(_subGridColor, _subdivisionSpacing);
	}
	int snapGridToNiceValues() const;
	void updateGridBounds(const sf::View& view);
	sf::VertexArray _lines;
	std::vector<TextLabel> _text;
	sf::Color _gridColor;
	sf::Color _subGridColor;
	const GridSettings& _settings;
	const float& _scale;
	float _spacing = 0, _subdivisionSpacing = 0;
	struct GridBounds {
		float left;
		float right;
		float top;
		float bottom;
	};
	Vec2f startPos;
	const sf::Font* _font;
	GridBounds _bounds;
};

#endif // !GRID_HPP
