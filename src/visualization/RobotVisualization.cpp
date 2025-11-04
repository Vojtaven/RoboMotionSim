#include "RobotVisualization.hpp"
#include "CompositeShape.hpp"

void Visualization::DrawRobotFromConfig(sf::RenderTarget& target, const RoboConfig& config, const float scale, const sf::Angle rotation) {
}

std::unique_ptr<CompositeShape> Visualization::GenerateGridLines(const sf::Vector2f size, const sf::Vector2f spacing, const sf::Vector2f origin, const sf::Color color, const float thickness) {
	auto grid = std::make_unique<CompositeShape>();
	int numberOfVerticalLines = size.x / spacing.x + 1;
	int numberOfHorizontalLines = size.y / spacing.y +1;

	std::unique_ptr<sf::RectangleShape> line;
	for (int i = 0; i < numberOfVerticalLines; i++) {
		line = std::make_unique<sf::RectangleShape>(sf::Vector2f( size.y, thickness));
		line->rotate(sf::degrees(90));
		line->setPosition({ spacing.x * i,0 });
		line->setFillColor(color);
		line->setOutlineColor(color);
		grid->add(std::move(line));
	}

	for (int i = 0; i < numberOfHorizontalLines; i++) {
		line = std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x, thickness));
		line->setPosition({0, spacing.y * i });
		line->setFillColor(color);
		line->setOutlineColor(color);
		grid->add(std::move(line));
	}

	return std::move(grid);
}

