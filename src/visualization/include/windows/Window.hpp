#ifndef WINDOW_HPP
#define WINDOW_HPP
#include "AppConfig.hpp"
class Window
{
public:
    virtual void open() = 0;  
    virtual void close() = 0;
    virtual void update() = 0;
    virtual bool isOpen() const = 0;
    virtual ~Window() = default;
};

#endif // !WINDOW_HPP
