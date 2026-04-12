// Author: Vojtech Venzara
// Date: 2026-03-29
// Description: Text rendering system providing font management and dynamic text display in the visualization

#include "rendering/Text.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

TextLabel::TextLabel(const sf::Font& font,
	const std::string& text,
	unsigned int letterHeightPx,
	sf::Color letterColor,
	sf::Color backgroundColor,
	const AnchorPoint anchor)
	: _text(font, text, letterHeightPx), _anchor(anchor)
{
	_text.setFillColor(letterColor);
	// Calculate background size based on text bounds
	auto size = _text.getLocalBounds().size;
	_background.setSize({ size.x + 2 * _padding, size.y + 2 * _padding });
	_background.setFillColor(backgroundColor);
	_background.setPosition({ _text.getPosition().x - _padding, _text.getPosition().y - _padding });
}

TextLabel::TextLabel(const sf::Font& font,
	const float content,
	unsigned int letterHeightPx,
	sf::Color letterColor,
	sf::Color backgroundColor,
	const AnchorPoint anchor)
	: _text(font, "", letterHeightPx), _anchor(anchor)
{
	_text.setFillColor(letterColor);
	setNumber(content);
	// Calculate background size based on text bounds
	auto size = _text.getLocalBounds().size;
	_background.setSize({ size.x + 2 * _padding, size.y + 2 * _padding });
	_background.setFillColor(backgroundColor);
	_background.setPosition({ _text.getPosition().x - _padding, _text.getPosition().y - _padding });
}

void TextLabel::setNumber(float number, int precision )
{
	std::ostringstream oss;
	if (std::floor(number) == number)
		oss << (int)(number);
	else
		oss << std::fixed << std::setprecision(precision) << number;

	setText(oss.str());
}

void TextLabel::setText(const std::string& text)
{
	_text.setString(text);
	auto size = _text.getLocalBounds().size;
	_background.setSize({ size.x + 2 * _padding, size.y + 2 * _padding });
}

void TextLabel::mapToPixels(const sf::RenderTarget& target, const sf::View& view)
{
	auto pixelPos = target.mapCoordsToPixel(_background.getPosition(), view);
	setPosition((float)pixelPos.x, (float)pixelPos.y, _anchor);
}

void TextLabel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(_background, states);
	target.draw(_text, states);
}


void TextLabel::setPosition(float x, float y, AnchorPoint anchor)
{
	const sf::Vector2f size = _background.getSize();
	sf::Vector2f offset(0.f, 0.f);

	switch (anchor) {
	case AnchorPoint::TopLeft:
		offset = { 0.f, 0.f };
		break;
	case AnchorPoint::TopCenter:
		offset = { size.x / 2.f, 0.f };
		break;
	case AnchorPoint::TopRight:
		offset = { size.x, 0.f };
		break;
	case AnchorPoint::CenterLeft:
		offset = { 0.f, size.y / 2.f };
		break;
	case AnchorPoint::Center:
		offset = { size.x / 2.f, size.y / 2.f };
		break;
	case AnchorPoint::CenterRight:
		offset = { size.x, size.y / 2.f };
		break;
	case AnchorPoint::BottomLeft:
		offset = { 0.f, size.y };
		break;
	case AnchorPoint::BottomCenter:
		offset = { size.x / 2.f, size.y };
		break;
	case AnchorPoint::BottomRight:
		offset = { size.x, size.y };
		break;
	}

	_background.setPosition({ x - offset.x, y - offset.y });
	const float textHeight = _text.getLocalBounds().size.y;
	sf::Vector2f textOffset(
		_background.getPosition().x + _padding,
		_background.getPosition().y - textHeight / 2.f - _padding
	);
	_text.setPosition(textOffset);
}

