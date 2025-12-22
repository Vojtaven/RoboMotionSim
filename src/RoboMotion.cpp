#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <string>
#include <asio.hpp>
#include "RobotVisualization.hpp"
#include "RoboConfig.hpp"
#include "ConfigManager.hpp"
#include <RobotShape.hpp>
#include <numbers> 
#include "PhysicsEngine.hpp"
constexpr size_t VIEW_WIDTH = 800;
constexpr size_t VIEW_HEIGHT = 800;

int main() {
	ConfigManager configManager;
	std::cout << std::filesystem::current_path() << "\n";
	configManager.LoadConfigFromFile("../src/libraryIntegration/testConfig.ini");
	configManager.SaveConfigToFile("../src/libraryIntegration/outTestConfig.ini");
	sf::View view(sf::FloatRect({ 0.f, 0.f }, { VIEW_WIDTH	, VIEW_HEIGHT  }));
	sf::RenderWindow window(sf::VideoMode({ VIEW_WIDTH, VIEW_HEIGHT }), "RoboMotionSim Test");
	window.setView(view);

	VisualizationEngine vizEngine(window, configManager.GetReadOnlyConfig());
	PhysicsEngine physicsEngine;
	vizEngine.setScaleFactor(1);
	vizEngine.setGridSpacing({ 50,50 });
	window.setFramerateLimit(244);
	auto config = configManager.GetConfig();
	config->GetPhysicsHandle()->speed = { 10,10,0 };
	const float moveStep = 2;
	const sf::Angle rotationStep = sf::degrees(2);
	while (window.isOpen()) {
		sf::Vector2f moveVec{ 0,0 };
		sf::Angle rotateAngle = sf::degrees(0);
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();

			if (auto resize = event->getIf<sf::Event::Resized>())
			{
				float newWidth = static_cast<float>(resize->size.x);
				float newHeight = static_cast<float>(resize->size.y);
				view.setSize({ newWidth, newHeight });
				view.setCenter({ newWidth / 2.f, newHeight / 2.f });

				window.setView(view);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
			moveVec.y -= moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
			moveVec.y += moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
			moveVec.x -= moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
			moveVec.x += moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q))
			rotateAngle -= rotationStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E))
			rotateAngle += rotationStep;

		vizEngine.moveRobotBy(moveVec, rotateAngle);
		window.clear();
		vizEngine.draw(window);
		window.display();
	}
}


