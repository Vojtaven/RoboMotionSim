#include "rendering/Grid.hpp"
#include "SFMLHelper.hpp"
#include "windows/WindowHelper.hpp"
#include "ColorConstants.hpp"
Grid::Grid(const GridSettings& settings, const float& scale, const sf::Font& font, const sf::View& view)
	: _settings(settings),
	_scale(scale),
	_font(&font),
	_worldView(view),
	_backgroundColor(ToSFMLColor(_settings.backgroundColor)),
	_textBackgroundColor(ToSFMLColor(_backgroundColor, _settings.textBackgroundOpacity)),
	_DPIScale(WindowHelper::getDpiScale())
{
	updateAfterSettingsChange();
}

void Grid::updateAfterSettingsChange() {
	_gridColor = ToSFMLColor(_settings.color);
	_subGridColor = ToSFMLColor(_settings.subGridColor);
	regenerate();
}

void Grid::regenerate() {
	_lines = sf::VertexArray(sf::PrimitiveType::Lines);
	updateGridBounds();
	_spacing = _settings.autoSpacing ? snapGridToNiceValues() : _settings.spacing;

	_startPos.x = std::floor(_bounds.left / _spacing) * _spacing;
	_startPos.y = std::floor(_bounds.top / _spacing) * _spacing;


	if (_settings.subdivisionsCount > 0) {
		_subdivisionSpacing = _spacing / (float)(_settings.subdivisionsCount + 1);
		addSubGridLines();
	}
	addGridLines();
	addGridText((unsigned int)(_settings.defaultGridFontSize * _DPIScale));
}

void Grid::mapText(const sf::RenderTarget& target) {
	for (auto& label : _text) {
		label.mapToPixels(target, _worldView);
	}
}

void Grid::draw(sf::RenderTarget& target) const {
	target.draw(_lines);
}

void Grid::drawText(sf::RenderTarget& target) {
	if (_remapText) {
		mapText(target);
		_remapText = false;
	}
	for (const auto& label : _text) {
		target.draw(label);
	}
}

void Grid::addGridLines(const sf::Color& color, const float spacing) {
	for (float x = _startPos.x; x <= _bounds.right; x += spacing)
	{
		_lines.append({ { x, _bounds.top },    color });
		_lines.append({ { x, _bounds.bottom }, color });
	}

	for (float y = _startPos.y; y <= _bounds.bottom; y += spacing)
	{
		_lines.append({ { _bounds.left,  y }, color });
		_lines.append({ { _bounds.right, y }, color });
	}

}

void Grid::addGridText(unsigned int textSize) {
	_text.clear();
	TextLabelFactory factory(*_font, textSize, _gridColor, Colors::GridTextBackground);
	//sf::Color(0, 0, 0, 100)
	for (float x = _startPos.x; x <= _bounds.right; x += _spacing)
	{
		_text.push_back(factory.createNumberLabel(x, AnchorPoint::TopCenter));
		_text.back().setPosition(x, _bounds.top);
	}

	for (float y = _startPos.y; y <= _bounds.bottom; y += _spacing)
	{
		_text.push_back(factory.createNumberLabel(y, AnchorPoint::CenterLeft));
		_text.back().setPosition(_bounds.left, y);
	}
	_remapText = true;
}

// Snap to nearest "nice" value in the 1-2-5 sequence (standard for graph/ruler spacing)
int Grid::snapGridToNiceValues() const {
	double raw = _settings.wantedGridSpacing / _scale;
	if (raw <= 0.0)
		return _settings.wantedGridSpacing;

	int exponent = static_cast<int>(std::floor(std::log10(raw)));
	double base = std::pow(10.0, exponent);
	double normalized = raw / base;

	double candidates[] = { 1.0, 2, 5.0, 10.0 };

	double closest = candidates[0];
	double minDiff = std::abs(normalized - closest);

	for (int i = 1; i < 4; ++i)
	{
		double diff = std::abs(normalized - candidates[i]);
		if (diff < minDiff)
		{
			minDiff = diff;
			closest = candidates[i];
		}
	}

	if (closest == 10.0)
		return 1.0 * std::pow(10.0, exponent + 1);

	return closest * base;
}

void Grid::updateGridBounds() {
	sf::Vector2f viewSize = _worldView.getSize();
	sf::Vector2f viewCenter = _worldView.getCenter();

	_bounds.left = viewCenter.x - viewSize.x / 2.f;
	_bounds.right = viewCenter.x + viewSize.x / 2.f;
	_bounds.top = viewCenter.y - viewSize.y / 2.f;
	_bounds.bottom = viewCenter.y + viewSize.y / 2.f;
}
