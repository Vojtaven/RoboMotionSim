// Author: Vojtech Venzara
// Date: 2026-04-04
// Description: Abstract base class providing common functionality for all settings windows

#include "windows/SettingsWindowBase.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-SFML.h>
#include <cstdint>
#include "SFMLHelper.hpp"
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"
#include "ColorConstants.hpp"

SettingsWindowBase::~SettingsWindowBase() {
	closeWindow();
}

bool SettingsWindowBase::isOpen() const {
	return _isOpen;
}

void SettingsWindowBase::openWindow(const char* title) {
	if (isOpen()) {
		_window->requestFocus();
		return;
	}

	_pendingClose = false;
	sanitizeWindowConfig();

	_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode({ (uint32_t)_windowConfig.size.x, (uint32_t)_windowConfig.size.y }),
		title,
		sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
	_window->setPosition({ _windowConfig.position.x, _windowConfig.position.y });
	if (_icon.getSize().x > 0 && _icon.getSize().y > 0)
		_window->setIcon(_icon);
	if (!ImGui::SFML::Init(*_window)) {
		_window.reset();
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr;
	ImGui::GetCurrentContext()->ConfigNavWindowingKeyNext = 0;
	ImGui::GetCurrentContext()->ConfigNavWindowingKeyPrev = 0;
	WindowHelper::SetScaledFont(io, DEFAULT_FONT_DATA, DEFAULT_FONT_DATA_SIZE, _windowConfig.defaultFontSize);
	_windowConfig.open = true;
	_isOpen = true;
}

// Close the settings window.
// If closeFromRoot is true, it indicates that the close action was initiated from the main window,
// so we should not set open to false in the config to allow reopening the settings window without issues.
void SettingsWindowBase::closeWindow(bool closeFromRoot) {
	if (!isOpen()) return;

	saveWindowConfig();
	_windowConfig.open = closeFromRoot;
	auto defaultCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
	if (defaultCursor) _window->setMouseCursor(*defaultCursor);
	ImGui::SFML::SetCurrentWindow(*_window);
	ImGui::SFML::Shutdown(*_window);
	_window->close();
	_window.reset();
	_pendingClose = false;
	_isOpen = false;
}

void SettingsWindowBase::draw() {
	if (!isOpen()) return;

	ImGui::SFML::SetCurrentWindow(*_window);
	_window->clear(Colors::WindowClearColor);
	ImGui::SFML::Render(*_window);
	_window->display();
}

void SettingsWindowBase::saveWindowConfig() {
	_windowConfig.position = static_cast<Vec2i>(_window->getPosition());
	_windowConfig.size = static_cast<Vec2i>(_window->getSize());
	_windowConfig.resizable = true;
}

void SettingsWindowBase::firstTimeSetup(Vec2i defaultSize) {
	auto screenSize = sf::VideoMode::getDesktopMode().size;

	_windowConfig.size = defaultSize;
	_windowConfig.position = { ((int)screenSize.x - _windowConfig.size.x) / 2, ((int)screenSize.y - _windowConfig.size.y) / 2 };
	_windowConfig.resizable = true;
	_windowConfig.open = false;
	_windowConfig.wasOpenedBefore = true;
}

void SettingsWindowBase::sanitizeWindowConfig() {
	WindowHelper::sanitizeWindowConfig(_windowConfig, 200, 150);
}

bool SettingsWindowBase::processEvents() {
	ImGui::SFML::SetCurrentWindow(*_window);

	while (const std::optional event = _window->pollEvent()) {
		ImGui::SFML::ProcessEvent(*_window, *event);

		if (event->is<sf::Event::Closed>()) {
			_pendingClose = true;
		}
	}

	if (_pendingClose) {
		closeWindow();
		return false;
	}
	return true;
}
