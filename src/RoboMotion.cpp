#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <asio.hpp>
#include <iostream>
#include <string>
#include <RobotVisualization.hpp>
#include <RoboConfig.hpp>
int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "RoboMotionSim Test");
    sf::Angle angle = sf::Angle::Zero;
    //while (true) {
    //    window.clear();
    //    Visualization::DrawRobotFromConfig(window, RoboConfig(),1,angle);

    //    window.display();
    //    angle += sf::degrees(1);
    //}

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->scancode == sf::Keyboard::Scancode::N) {
                    //window.clear();
                    Visualization::DrawRobotFromConfig(window, RoboConfig(), 1, angle);


                    window.display();
                    angle += sf::degrees(1);
                }


        }
    }
}