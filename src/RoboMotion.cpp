#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <asio.hpp>
#include <iostream>
#include <string>
#include <RobotVisualization.hpp>
#include <RoboConfig.hpp>
#include <RobotShape.hpp>
#include <numbers> 

constexpr double PI = std::numbers::pi;
int main() {
	sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "RoboMotionSim Test");
    auto grid = Visualization::GenerateGridLines({ 800,800 }, { 25,25 }, { 0,0 }, { 80,80,80 }, 1);
	window.setFramerateLimit(60);
	RoboConfig config;
	config.AddAxel({ {47,0,165,0,0},{0,0,0} });
	config.AddAxel({ {47,(float)(165 * sin(2 * PI / 3)),-82.5,240 * std::numbers::pi / 180,0},{0,0,0} });
	config.AddAxel({ {47,(float)(165 * sin(4 * PI / 3)),-82.5,120 * std::numbers::pi / 180,0},{0,0,0} });

	RobotShape roboShape(config);
	roboShape.setPosition({ 400,400 });
	roboShape.scale({0.2f,0.2f});

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