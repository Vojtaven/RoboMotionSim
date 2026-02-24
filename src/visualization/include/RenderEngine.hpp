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
	void updateRobotShape(const RobotConfig& config, bool holdPosition = false);
	Vec2f getWindowCenter() const;
	const RenderSettings& getCurrentRenderSettings() const;
private:
	sf::RenderWindow& _window;
	std::unique_ptr<RobotShape> _robotShape;
	sf::VertexArray _gridLines{ sf::PrimitiveType::Lines };
	std::unique_ptr<sf::View> _view;
	RenderSettings _settings;
	sf::Vector2i _gridDefaultSpacing = { 200,200 };
	sf::Color _gridColor = { 80,80,80 };
	sf::Vector2i _gridSpacing = { 50, 50 };
	//float _scaleFactor = 1.0f;
	//bool _showGridLines = true;
	//bool lockOnRobot = false;
};

#endif // !RENDER_ENGINE_HPP