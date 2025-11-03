#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <asio.hpp>
#include <iostream>
#include <string>
#include <RobotVisualization.hpp>
#include <RoboConfig.hpp>
int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "RoboMotionSim Test");
    while (true) {
        window.clear();
        Visualization::DrawRobotFromConfig(window, RoboConfig());

        window.display();
    }
}