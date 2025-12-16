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

int main() {
    ConfigManager configManager;
    std::cout << std::filesystem::current_path() << "\n";
    configManager.LoadConfigFromFile("../src/libraryIntegration/testConfig.ini");
    configManager.SaveConfigToFile("../src/libraryIntegration/outTestConfig.ini");
	sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "RoboMotionSim Test");

	VisualizationEngine vizEngine(window, configManager.GetReadOnlyConfig());
	vizEngine.setScaleFactor(0.25f);
	vizEngine.setGridSpacing({ 50,50 });
    //uto grid = Visualization::GenerateGridLines({ 800,800 }, { 25,25 }, { 0,0 }, { 80,80,80 }, 1);
	window.setFramerateLimit(244);
	/*auto config = configManager.GetReadOnlyConfig();
	RobotShape roboShape(config);
	roboShape.setPosition({ 400,400 });
	roboShape.scale({0.5f,0.5f});*/

	const float moveStep = 2;
	const sf::Angle rotationStep = sf::degrees(2);
    while (window.isOpen()) {
		sf::Vector2f moveVec{ 0,0 };
		sf::Angle rotateAngle = sf::degrees(0);
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
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