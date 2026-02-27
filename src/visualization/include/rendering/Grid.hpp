#ifndef GRID_HPP
#define GRID_HPP

#include <SFML/Graphics.hpp>
#include "rendering/Text.hpp"
#include "AppConfig.hpp"
class Grid
{
public:
	Grid(const GridSettings& settings, const float& scale, const sf::Font& font, const sf::View& worldView);
	void updateAfterSettingsChange();
	void regenerate();
	void draw(sf::RenderTarget& target) const;
	void drawText(sf::RenderTarget& target);
	void remapNeeded() { _remapText = true; }
	void setFont(const sf::Font& font) { _font = &font; }
private: 	
	void mapText(const sf::RenderTarget& target);
	void addGridText(unsigned int textSize);
	void addGridLines(const sf::Color& color, const float spacing);
	void addGridLines() {
		addGridLines(_gridColor, _spacing);
	}
	void addSubGridLines() {
		addGridLines(_subGridColor, _subdivisionSpacing);
	}
	int snapGridToNiceValues() const;
	void updateGridBounds();
	sf::VertexArray _lines;
	std::vector<TextLabel> _text;
	bool _remapText = true;
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
	const sf::View& _worldView;
};

#endif // !GRID_HPP
