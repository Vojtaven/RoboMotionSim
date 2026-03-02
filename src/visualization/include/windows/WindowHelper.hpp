#ifndef WINDOW_HELPER_HPP
#define WINDOW_HELPER_HPP

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#ifdef _WIN32
#include <windows.h> 

#elif defined(__linux__)
#include <X11/Xlib.h>  
#endif

class WindowHelper
{
public:
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

    static void SetScaledFont(ImGuiIO& io, const void* fontData, const int fontDataSize, const int fontSize) {
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        ImFont* font = io.Fonts->AddFontFromMemoryTTF((void*)fontData, fontDataSize, fontSize * getDpiScale(), &config);
        if (!font || !ImGui::SFML::UpdateFontTexture()) {
            throw std::runtime_error("Failed to load font from memory");
        }
    }

};


#endif 
