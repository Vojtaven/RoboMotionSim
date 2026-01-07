#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <string>
#include <asio.hpp>
#include "VisualizationEngine.hpp"
#include "RobotConfig.hpp"
#include "ConfigManager.hpp"
#include <RobotShape.hpp>
#include <numbers> 
#include "PhysicsEngine.hpp"
#include <chrono>
#include "MathUtils.hpp"
constexpr size_t VIEW_WIDTH = 800;
constexpr size_t VIEW_HEIGHT = 800;
using Clock = std::chrono::steady_clock;


int main() {
	ConfigManager configManager;
	std::cout << std::filesystem::current_path() << "\n";
	//configManager.loadConfigFromFile("../src/libraryIntegration/DifferentialConfig.ini");
	//configManager.loadConfigFromFile("../src/libraryIntegration/MecanumConfig.ini");
	configManager.loadConfigFromFile("../src/libraryIntegration/OmniConfig.ini");
	configManager.saveConfigToFile("../src/libraryIntegration/outTestConfig.ini");
	auto config = configManager.getReadOnlyConfig();

	sf::View view(sf::FloatRect({ 0.f, 0.f }, { VIEW_WIDTH	, VIEW_HEIGHT }));
	sf::RenderWindow window(sf::VideoMode({ VIEW_WIDTH, VIEW_HEIGHT }), "RoboMotionSim Test");
	window.setView(view);

	VisualizationEngine vizEngine(window, configManager.getReadOnlyConfig());
	PhysicsEngine physicsEngine;
	RobotState state(config.getWheelCount());
	vizEngine.setScaleFactor(1);
	vizEngine.setGridSpacing({ 50,50 });
	window.setFramerateLimit(60);

	state.position = vizEngine.getWindowCenter();
	const float moveStep = 200; // 200mm/s
	const float rotationStep = DegreesToRadians(30); // 30 degrees/s
	auto last = Clock::now();
	while (window.isOpen()) {
		Vec2 speed{ 0,0 };
		float rotationSpeed = 0;
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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
			speed.y -= moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
			speed.y += moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
			speed.x -= moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
			speed.x += moveStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q))
			rotationSpeed -= rotationStep;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E))
			rotationSpeed += rotationStep;

		state.localVelocity = speed;
		state.angularVelocity = rotationSpeed;
		auto now = Clock::now();
		std::chrono::duration<float> delta = now - last;
		physicsEngine.update(delta.count(), state, config);
		last = now;
		vizEngine.update(state);
		window.clear();
		vizEngine.draw(window);
		window.display();
	}
}


