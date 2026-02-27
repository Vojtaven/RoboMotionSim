#ifndef TEXT_HPP
#define TEXT_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <iomanip>

enum class AnchorPoint{
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
              sf::Color backgroundColor)
		: _text(font, text, letterHeightPx)
    {
        _text.setFillColor(letterColor);
        // Calculate background size based on text bounds
        auto size = _text.getLocalBounds().size;
        _background.setSize({size.x + 2 * _padding, size.y + 2 * _padding});
        _background.setFillColor(backgroundColor);
        _background.setPosition({_text.getPosition().x - _padding, _text.getPosition().y - _padding});
    }

    TextLabel(const sf::Font& font,
        const float content,
        unsigned int letterHeightPx,
        sf::Color letterColor,
        sf::Color backgroundColor)
        : _text(font, "", letterHeightPx)
    {
        _text.setFillColor(letterColor);
		setNumber(content);
        // Calculate background size based on text bounds
        auto size = _text.getLocalBounds().size;
        _background.setSize({ size.x + 2 * _padding, size.y + 2 * _padding });
        _background.setFillColor(backgroundColor);
        _background.setPosition({ _text.getPosition().x - _padding, _text.getPosition().y - _padding });
    }

    sf::Vector2f getPosition() const
    {
		return _background.getPosition();
	}   

	void setPosition(sf::Vector2i position)
    {
        setPosition(static_cast<float>(position.x), static_cast<float>(position.y), AnchorPoint::TopLeft);
    }

    void setPosition(float x, float y, AnchorPoint anchor)
    {
        const sf::Vector2f size = _background.getLocalBounds().size;
        sf::Vector2f offset(0.f, 0.f);

        switch (anchor) {
            case AnchorPoint::TopLeft:
                offset = {0.f, 0.f};
                break;
            case AnchorPoint::TopCenter:
                offset = {size.x / 2.f, 0.f};
                break;
            case AnchorPoint::TopRight:
                offset = {size.x, 0.f};
                break;
            case AnchorPoint::CenterLeft:
                offset = {0.f, size.y / 2.f};
                break;
            case AnchorPoint::Center:
                offset = {size.x / 2.f, size.y / 2.f};
                break;
            case AnchorPoint::CenterRight:
                offset = {size.x, size.y / 2.f};
                break;
            case AnchorPoint::BottomLeft:
                offset = {0.f, size.y};
                break;
            case AnchorPoint::BottomCenter:
                offset = {size.x / 2.f, size.y};
                break;
            case AnchorPoint::BottomRight:
                offset = {size.x, size.y};
                break;
        }

        _text.setPosition({x - offset.x + _padding, y - offset.y + _padding});
        _background.setPosition({x - offset.x, y - offset.y});
    }

    void setNumber(float number, int precision = 2)
    {
        std::ostringstream oss;
        if (std::floor(number) == number) 
            oss << static_cast<int>(number);
         else 
            oss << std::fixed << std::setprecision(precision) << number;
        
        setText(oss.str());
    }

    void setText(const std::string& text)
    {
        _text.setString(text);
        auto size = _text.getLocalBounds().size;
        _background.setSize({ size.x + 2 * _padding, size.y + 2 * _padding });
    }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(_background, states);
        target.draw(_text, states);
    }

private:
    sf::Text _text;
    sf::RectangleShape _background;
    const float _padding = 4.f;
};

#endif // !TEXT_HPP
