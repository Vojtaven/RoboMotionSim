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
	RenderEngine(sf::RenderWindow& window, const RenderSettings& settings, const std::string& fontPath);
	void update(const RobotState& state);
	void updateAfterResize();
	void draw();
	void regenerateGridLines();
	void resetRobotPosition(sf::Vector2f pos);
	void setRenderSettings(const RenderSettings& settings);
	void updateRobotShape(const RobotConfig& config, bool holdPosition = true);
	Vec2f getWindowCenter() const;
	const RenderSettings& getCurrentRenderSettings() const;
private:
	int SnapGridToNiceValues();
	sf::RenderWindow& _window;
	std::unique_ptr<RobotShape> _robotShape;
	std::vector <sf::Text> _gridText;
	sf::VertexArray _gridLines{ sf::PrimitiveType::Lines };
	std::unique_ptr<sf::View> _view;
	RenderSettings _settings;
	const int _DefaultGridSpacing = 200; // 200 mm at 1x scale 
	sf::Color _gridColor = { 80,80,80 };
	sf::Color _subGridColor = { 40,40,40 };
	sf::Font _font;
};

#endif // !RENDER_ENGINE_HPP