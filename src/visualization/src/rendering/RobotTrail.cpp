#include "rendering/RobotTrail.hpp"


RobotTrail::RobotTrail(const TrailSettings& settings) : _color(ToSFMLColor(settings.trailColor)), _settings(settings) {}

void RobotTrail::addTrailPoint(const sf::Vector2f& position, const float dt) {
	if ((interval += dt) < _settings.pointSpawnInteral)
		return;

	interval = 0;
	if (_trail.size() >= _settings.trailMaxLenght)
		_trail.pop_front();

	_trail.emplace_back(sf::CircleShape(_settings.trailPointSize));
	auto& circle = _trail.back();
	// Center the circle at the given position
	circle.setOrigin({ _settings.trailPointSize, _settings.trailPointSize });
	circle.setPosition(position);
	circle.setFillColor(_color);
}

void RobotTrail::updateAfterSettingsChange() {
	_color = ToSFMLColor(_settings.trailColor);
	for (auto& point : _trail) {
		point.setOrigin({ _settings.trailPointSize, _settings.trailPointSize });
		point.setRadius(_settings.trailPointSize);
		point.setFillColor(_color);
	}
}

void RobotTrail::clearTrail() { _trail = {}; }

void RobotTrail::draw(sf::RenderTarget& target) const {
	for (const auto& point : _trail)
		target.draw(point);
}