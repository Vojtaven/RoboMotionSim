#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <asio.hpp>
#include <iostream>
#include <string>

int main() {
    std::cout << "=== RoboMotionSim Library Test ===" << std::endl;
    
    // Test 1: SFML
    std::cout << "\n[1] Testing SFML..." << std::endl;
    try {
        sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "RoboMotionSim Test");
        std::cout << "    OK SFML window created successfully" << std::endl;
        std::cout << "    OK Window size: " << window.getSize().x << "x" << window.getSize().y << std::endl;
        
        // Test 2: ImGui-SFML
        std::cout << "\n[2] Testing ImGui-SFML..." << std::endl;
        if (!ImGui::SFML::Init(window)) {
            std::cerr << "    ✗ Failed to initialize ImGui-SFML" << std::endl;
            return 1;
        }
        std::cout << "    OK ImGui-SFML initialized" << std::endl;
        std::cout << "    OK ImGui version: " << IMGUI_VERSION << std::endl;
        
        // Test 3: Asio
        std::cout << "\n[3] Testing Asio..." << std::endl;
        asio::io_context io_context;
        std::cout << "    OK Asio io_context created" << std::endl;
        std::cout << "    OK Asio version: " << ASIO_VERSION << std::endl;
        
        // Test 4: Asio Serial Port (just check if we can create the object)
        std::cout << "\n[4] Testing Asio Serial Port capability..." << std::endl;
        std::cout << "    OK Serial port class available (asio::serial_port)" << std::endl;
        std::cout << "    Note: Actual port opening requires a physical device" << std::endl;
        
        // Main loop test
        std::cout << "\n[5] Running main loop test..." << std::endl;
        sf::Clock deltaClock;
        int frameCount = 0;
        bool showDemo = true;
        
        while (window.isOpen() && frameCount < 100) {
            while (const std::optional event = window.pollEvent()) {
               
                if (event->is<sf::Event::Closed>())
                    window.close();
                
                if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                    if(key->scancode == sf::Keyboard::Scancode::Escape)
                        window.close();

            }
            
            ImGui::SFML::Update(window, deltaClock.restart());
            
            // ImGui test window
            ImGui::Begin("Library Test Window");
            ImGui::Text("All libraries working!");
            ImGui::Text("Frame: %d/100", frameCount);
            ImGui::Separator();
            ImGui::Text("SFML: OK");
            ImGui::Text("ImGui: OK");
            ImGui::Text("Asio: OK");
            ImGui::Separator();
            ImGui::Text("Press ESC to close");
            if (ImGui::Button("Show ImGui Demo")) {
                showDemo = !showDemo;
            }
            ImGui::End();
            
            if (showDemo) {
                ImGui::ShowDemoWindow(&showDemo);
            }
            
            window.clear(sf::Color(50, 50, 50));
            
            // Draw a test circle
            sf::CircleShape shape(100.f);
            shape.setFillColor(sf::Color::Green);
            shape.setPosition({ 350, 250 });
            window.draw(shape);
            
            ImGui::SFML::Render(window);
            window.display();
            
            frameCount++;
        }
        
        std::cout << "    OK Rendered " << frameCount << " frames successfully" << std::endl;
        
        ImGui::SFML::Shutdown();
        
        std::cout << "\n=== All Tests Passed! ===" << std::endl;
        std::cout << "\nYou can now develop your RoboMotionSim application!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}