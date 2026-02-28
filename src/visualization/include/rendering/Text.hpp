#ifndef TEXT_HPP
#define TEXT_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

enum class AnchorPoint {
	TopLeft,
	TopCenter,
	TopRight,
	CenterLeft,
	Center,
	CenterRight,
	BottomLeft,
	BottomCenter,
	BottomRight
};

class TextLabel : public sf::Drawable
{
public:
	TextLabel(const sf::Font& font,
		const std::string& text,
		unsigned int letterHeightPx,
		sf::Color letterColor,
		sf::Color backgroundColor,
		const AnchorPoint anchor);

	TextLabel(const sf::Font& font,
		const float content,
		unsigned int letterHeightPx,
		sf::Color letterColor,
		sf::Color backgroundColor,
		const AnchorPoint anchor);

	sf::Vector2f getPosition() const { return _background.getPosition(); }
	void inline setPosition(float x, float y) { setPosition({ x, y }); }
	void setPosition(sf::Vector2f pos) { _background.setPosition(pos); }
	void setNumber(float number, int precision = 2);
	void setText(const std::string& text);
	void mapToPixels(const sf::RenderTarget& target, const sf::View& view);
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
	void setPosition(float x, float y, AnchorPoint anchor);
	sf::Text _text;
	sf::RectangleShape _background;
	const AnchorPoint _anchor = AnchorPoint::TopLeft;
	const float _padding = 4.f;
};


class TextLabelFactory {
public:
	TextLabelFactory(const sf::Font& font, unsigned int letterHeightPx, sf::Color letterColor, sf::Color backgroundColor)
		: _font(font), _letterHeightPx(letterHeightPx), _letterColor(letterColor), _backgroundColor(backgroundColor) {
	}

	TextLabel createTextLabel(const std::string& text, AnchorPoint anchor) const {
		return TextLabel(_font, text, _letterHeightPx, _letterColor, _backgroundColor, anchor);
	}

	TextLabel createNumberLabel(float number, AnchorPoint anchor) const {
		return TextLabel(_font, number, _letterHeightPx, _letterColor, _backgroundColor, anchor);
	}
private:
	const sf::Font& _font;
	const unsigned int _letterHeightPx;
	const sf::Color _letterColor;
	const sf::Color _backgroundColor;
};

#endif // !TEXT_HPP
