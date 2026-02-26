#ifndef GRID_HPP
#define GRID_HPP

#include <SFML/Graphics.hpp>
#include "rendering/Text.hpp"
#include "AppConfig.hpp"
class Grid
{
public:
	Grid(const GridSettings& settings, const float& scale, const sf::View& view);
	void updateAfterSettingsChange(const sf::View& view);
	void regenerate(const sf::View& view);
	void draw(sf::RenderTarget& target) const;
	void setFont(const sf::Font& font) { _font = &font; }
private: 	
	void addGridLines(float startX, float startY, const sf::Color& color, const float spacing);
	void addGridLines(float startX, float startY) {
		addGridLines(startX, startY, _gridColor, _spacing);
	}
	void addSubGridLines(float startX, float startY) {
		addGridLines(startX, startY, _subGridColor, _subdivisionSpacing);
	}
	void addGridText();
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
	const sf::Font* _font;
	GridBounds _bounds;
};

#endif // !GRID_HPP
