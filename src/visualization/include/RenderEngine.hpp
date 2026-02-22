#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP
#include <SFML/Graphics.hpp>
#include "shapes/CompositeShape.hpp"
#include "shapes/RobotShape.hpp"
#include "SFMLHelper.hpp"
#include "MathUtils.hpp"
#include "RobotState.hpp"
#include "AppConfig.hpp"
class RenderEngine
{
public:
	RenderEngine(sf::RenderWindow& window, const RenderSettings& settings);
	void update(const RobotState& state);
	void updateAfterResize();
	void draw();
	void regenerateGridLines();
	void resetRobotPosition(sf::Vector2f pos);
	void setRenderSettings(const RenderSettings& settings);
	void saveRenderSettings(RenderSettings& settings) const;
	void updateRobotShape(const RobotConfig& config, bool holdPosition = false);
	Vec2f getWindowCenter() const;
	RenderSettings getCurrentRenderSettings() const;
private:
	sf::RenderWindow& _window;
	std::unique_ptr<RobotShape> _robotShape;
	std::unique_ptr<CompositeShape> _gridLines;
	std::unique_ptr<sf::View> _view;

	float _scaleFactor = 1.0f;
	sf::Vector2f _gridSpacing = { 50.0f, 50.0f };
	sf::Color _gridColor = { 80,80,80 };
	float _gridLineThickness = 1.0f;
	bool _showGridLines = true;
};

#endif // !RENDER_ENGINE_HPP
