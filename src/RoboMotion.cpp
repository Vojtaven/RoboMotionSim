#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <asio.hpp>
#include <iostream>
#include <string>
#include <RobotVisualization.hpp>
#include <RoboConfig.hpp>
#include <ConfigManager.hpp>
#include <RobotShape.hpp>
#include <numbers> 

constexpr double PI = std::numbers::pi;
int main() {
    ConfigManager configManager;
    std::cout << std::filesystem::current_path() << "\n";
    configManager.LoadConfigFromFile("../src/libraryIntegration/testConfig.ini");
    configManager.SaveConfigToFile("../src/libraryIntegration/outTestConfig.ini");
	sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "RoboMotionSim Test");
    auto grid = Visualization::GenerateGridLines({ 800,800 }, { 25,25 }, { 0,0 }, { 80,80,80 }, 1);
	window.setFramerateLimit(244);
	auto config = configManager.GetReadOnlyConfig();
	RobotShape roboShape(config);
	roboShape.setPosition({ 400,400 });
	roboShape.scale({0.5f,0.5f});

	const float moveStep = 2;
	const sf::Angle rotationStep = sf::degrees(2);
    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
            roboShape.move({ 0, -moveStep });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
            roboShape.move({ 0,  moveStep });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
            roboShape.move({ -moveStep, 0 });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
            roboShape.move({ moveStep, 0 });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q))
            roboShape.rotate(-rotationStep);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E))
            roboShape.rotate(rotationStep);

        window.clear();
        window.draw(*grid);
        window.draw(roboShape);
        window.display();
    }
}