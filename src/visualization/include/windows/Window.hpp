#ifndef WINDOW_HPP
#define WINDOW_HPP
#include "AppConfig.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
class Window
{
public:
	virtual void open(const AppConfig& config) = 0;
    virtual void close() = 0;
    virtual void update() = 0;
	virtual void saveConfig(AppConfig& config) = 0;	
    virtual bool isOpen() const = 0;
    virtual ~Window() = default;
protected:
	WindowConfig _config;
	std::unique_ptr<sf::RenderWindow> _window;
};

#endif // !WINDOW_HPP
