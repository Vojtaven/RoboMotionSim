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
class RenderEngine
{
public:
	RenderEngine(sf::RenderWindow& window,const RenderSettings& settings, const std::string& fontPath);
	void update(const RobotState& state);
	void updateAfterResize();
	void draw();
	void regenerateGridLines() {_grid->regenerate(*_view);}
	void resetRobotPosition(sf::Vector2f pos);
	void updateAfterSettingsChange();
	void updateRobotShape(const RobotConfig& config, bool holdPosition = true);
	Vec2f getWindowCenter() const;
	const RenderSettings& getCurrentRenderSettings() const;
private:
	const RenderSettings& _settings;
	sf::Font _font;
	sf::RenderWindow& _window;
	std::unique_ptr<sf::View> _view;
	std::unique_ptr<RobotShape> _robotShape;
	std::unique_ptr<Grid> _grid;
};

#endif // !RENDER_ENGINE_HPP