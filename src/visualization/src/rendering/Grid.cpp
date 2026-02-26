#include "rendering/Grid.hpp"


Grid::Grid(const GridSettings& settings, const float& scale, const sf::View& view) : _settings(settings), _scale(scale) {
	updateAfterSettingsChange(view);
}
void Grid::updateAfterSettingsChange(const sf::View& view) {
	_gridColor = sf::Color(
		(uint8_t)(_settings.color[0] * 255),
		(uint8_t)(_settings.color[1] * 255),
		(uint8_t)(_settings.color[2] * 255));
	_subGridColor = sf::Color(
		(uint8_t)(_settings.subGridColor[0] * 255),
		(uint8_t)(_settings.subGridColor[1] * 255),
		(uint8_t)(_settings.subGridColor[2] * 255));
	regenerate(view);
}
void Grid::regenerate(const sf::View& view) {
	_lines = sf::VertexArray(sf::PrimitiveType::Lines);
	updateGridBounds(view);
	_spacing = _settings.autoSpacing ? snapGridToNiceValues() : _settings.spacing;

	float startX = std::floor(_bounds.left / _spacing) * _spacing;
	float startY = std::floor(_bounds.top / _spacing) * _spacing;


	if (_settings.subdivisionsCount > 0) {
		_subdivisionSpacing = _spacing / (float)(_settings.subdivisionsCount + 1);
		addSubGridLines(startX, startY);
	}
	addGridLines(startX, startY);

	//TODO add text
}
void Grid::draw(sf::RenderTarget& target) const {
	target.draw(_lines);
}
void Grid::addGridLines(float startX, float startY, const sf::Color& color, const float spacing) {
	for (float x = startX; x <= _bounds.right; x += spacing)
	{
		_lines.append({ { x, _bounds.top },    color });
		_lines.append({ { x, _bounds.bottom }, color });
	}

	for (float y = startY; y <= _bounds.bottom; y += spacing)
	{
		_lines.append({ { _bounds.left,  y }, color });
		_lines.append({ { _bounds.right, y }, color });
	}

}

void Grid::addGridText() {}

int Grid::snapGridToNiceValues() const {
	double raw = _settings.WantedGridSpacing / _scale;
	if (raw <= 0.0)
		return _settings.WantedGridSpacing;

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
void Grid::updateGridBounds(const sf::View& view) {
	sf::Vector2f viewSize = view.getSize();
	sf::Vector2f viewCenter = view.getCenter();

	_bounds.left = viewCenter.x - viewSize.x / 2.f;
	_bounds.right = viewCenter.x + viewSize.x / 2.f;
	_bounds.top = viewCenter.y - viewSize.y / 2.f;
	_bounds.bottom = viewCenter.y + viewSize.y / 2.f;
}
