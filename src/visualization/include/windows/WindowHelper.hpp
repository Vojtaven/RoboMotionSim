#ifndef WINDOW_HELPER_HPP
#define WINDOW_HELPER_HPP

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <algorithm>
#include "AppConfig.hpp"
#include "SFMLHelper.hpp"
#ifdef _WIN32
#include <windows.h>

#elif defined(__linux__)
#include <X11/Xlib.h>
#endif

class WindowHelper
{
public:
    struct ScreenBounds {
        int x, y, w, h;
    };

    static ScreenBounds getVirtualScreenBounds() {
#ifdef _WIN32
        return {
            GetSystemMetrics(SM_XVIRTUALSCREEN),
            GetSystemMetrics(SM_YVIRTUALSCREEN),
            GetSystemMetrics(SM_CXVIRTUALSCREEN),
            GetSystemMetrics(SM_CYVIRTUALSCREEN)
        };
#elif defined(__linux__)
        Display* display = XOpenDisplay(nullptr);
        if (display) {
            int screenCount = ScreenCount(display);
            int minX = 0, minY = 0, maxX = 0, maxY = 0;
            for (int i = 0; i < screenCount; i++) {
                Screen* screen = ScreenOfDisplay(display, i);
                maxX = std::max(maxX, screen->width);
                maxY = std::max(maxY, screen->height);
            }
            XCloseDisplay(display);
            return { minX, minY, maxX, maxY };
        }
        auto sz = sf::VideoMode::getDesktopMode().size;
        return { 0, 0, static_cast<int>(sz.x), static_cast<int>(sz.y) };
#else
        auto sz = sf::VideoMode::getDesktopMode().size;
        return { 0, 0, static_cast<int>(sz.x), static_cast<int>(sz.y) };
#endif
    }

    static void sanitizeWindowConfig(WindowConfig& config, int minWidth, int minHeight) {
		// There is still possibility of window being off-screen in "dead zone" areas on some multi-monitor setups.
        // But this covers most common coruption of config.
        auto bounds = getVirtualScreenBounds();

        config.size.x = std::max(config.size.x, minWidth);
        config.size.y = std::max(config.size.y, minHeight);

        if (config.size.x > bounds.w)
            config.size.x = bounds.w;
        if (config.size.y > bounds.h)
            config.size.y = bounds.h;

        bool offScreen = config.position.x + config.size.x <= bounds.x
            || config.position.x >= bounds.x + bounds.w
            || config.position.y + config.size.y <= bounds.y
            || config.position.y >= bounds.y + bounds.h;

        if (offScreen) {
            // We will put of screen windows in center of main screen
			Vec2i screenSize = static_cast<Vec2i>(sf::VideoMode::getDesktopMode().size);

            config.position = {
                (screenSize.x - config.size.x) / 2,
                (screenSize.y - config.size.y) / 2
            };
        }                                   
    }

    static float getDpiScale() {
#ifdef _WIN32
        HDC hdc = GetDC(nullptr);
        float dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(nullptr, hdc);
        return dpi / 96.0f; // 96 DPI = 100% scaling on Windows

#elif defined(__linux__)
        Display* display = XOpenDisplay(nullptr);
        if (display) {
            float dpi = (DisplayWidth(display, 0) * 25.4f) / DisplayWidthMM(display, 0);
            XCloseDisplay(display);
            return dpi / 96.0f;
        }
        return 1.0f;

#else
        return 1.0f;
#endif
    }

    static void SetScaledFont(ImGuiIO& io,
        const void* fontData,
        const int fontDataSize,
        const float fontSize)
    {
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        config.PixelSnapH = true;
        ImFont* font = io.Fonts->AddFontFromMemoryTTF(
            (void*)fontData,
            fontDataSize,
            (float)fontSize * getDpiScale(),  // no manual DPI scaling
            &config
        );

        if (!font || !ImGui::SFML::UpdateFontTexture())
            throw std::runtime_error("Failed to load font");
        io.FontDefault = font;
    }

};


#endif 
