#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP
#include <SFML/Graphics.hpp>
#include "shapes/CompositeShape.hpp"
#include "shapes/RobotShape.hpp"
#include "SFMLHelper.hpp"
#include "MathUtils.hpp"
#include "RobotState.hpp"
#include "AppConfig.hpp"
#include "Grid.hpp"
#include "RobotTrail.hpp"
class RenderEngine
{
public:
	RenderEngine(sf::RenderWindow& window,const RenderSettings& settings);
	void update(const RobotState& state, const float dt);
	void updateAfterResize();
	void draw();
	void regenerateGridLines();
	void resetRobotPosition(sf::Vector2f pos);
	void updateAfterSettingsChange();
	void updateRobotShape(const RobotConfig& config, const RobotState& state);
	Vec2f getWindowCenter() const;
	const RenderSettings& getCurrentRenderSettings() const;
	void clearRobotTrail() { _trail->clearTrail(); }
private:
	const RenderSettings& _settings;
	sf::Font _font;
	sf::RenderWindow& _window;
	std::unique_ptr<sf::View> _worldView;
	std::unique_ptr<sf::View> _uiView;
	std::unique_ptr<RobotShape> _robotShape;
	std::unique_ptr<RobotTrail> _trail;
	std::unique_ptr<Grid> _grid;
};

#endif // !RENDER_ENGINE_HPP