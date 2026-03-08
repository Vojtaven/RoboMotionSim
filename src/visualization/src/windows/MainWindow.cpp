#include "windows/MainWindow.hpp"
#include <memory>
#include "SFMLHelper.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include "embeddedFont.h"
#include "windows/WindowHelper.hpp"
#include "windows/RenderSettingsWindow.hpp"
#include "windows/InputSettingsWindow.hpp"
#include "windows/RobotStatWindow.hpp"
MainWindow::MainWindow(AppConfig& config)
	: _appConfig(config), _windowConfig(config.mainWindow)
{
}

void MainWindow::open(const RobotConfig& robotConfig)
{
	_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode(ToSFMLVector2u(_windowConfig.size)),
		_appConfig.appName,
		_windowConfig.resizable ? sf::Style::Default : sf::Style::Titlebar | sf::Style::Close);

	_window->setPosition(ToSFMLVector2i(_windowConfig.position));
	initImGui();
	_renderEngine = std::make_unique<RenderEngine>(*_window, _appConfig.renderSettings);
	setRobotConfig(robotConfig);
	initializeOtherWindows();
	_window->requestFocus();
}


void MainWindow::close() {
	closeOtherWindows();
	saveWindowConfig(_windowConfig);
	saveConfig();
	ImGui::SFML::SetCurrentWindow(*_window);
	ImGui::SFML::Shutdown(*_window);
	_window->close();
	_window.reset();
	_isOpen = false;
}

void MainWindow::renderImGuiMenu() {
	ImGui::SFML::SetCurrentWindow(*_window);
	// Get window size for positioning
	ImVec2 windowSize = ImGui::GetIO().DisplaySize;

	// Menu button in top-right corner
	const float menuButtonSize = 40.0f;
	const float padding = 10.0f;
	ImGui::SetNextWindowPos(ImVec2(windowSize.x - menuButtonSize - padding, padding));
	ImGui::SetNextWindowSize(ImVec2(menuButtonSize, menuButtonSize));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
	ImGui::Begin("##MenuButton", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBackground);

	if (ImGui::Button("=", ImVec2(menuButtonSize - 8, menuButtonSize - 8))) {
		_showMenu = !_showMenu;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Menu");
	}
	ImGui::End();
	ImGui::PopStyleVar();

	// Show dropdown menu if button was clicked
	if (_showMenu) {
		ImGui::SetNextWindowPos(ImVec2(windowSize.x - 200.0f - padding, menuButtonSize + padding * 2));
		ImGui::SetNextWindowSize(ImVec2(200, 0));

		ImGui::Begin("Menu", &_showMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (ImGui::MenuItem("Render Settings")) {
			const RenderSettings& current = _renderEngine->getCurrentRenderSettings();
			_settingsWindow->open(current);
			_showMenu = false;
		}
		else if (ImGui::MenuItem("Input Settings")) {
			const InputSettings& current = _appConfig.inputSettings;
			_inputSettingsWindow->open(current);
			_showMenu = false;
		}
		else if (ImGui::MenuItem("Robot Stats")) {
			_robotStatWindow->open();
			_showMenu = false;
		}

		ImGui::SFML::SetCurrentWindow(*_window);
		ImGui::Separator();

		if (ImGui::MenuItem("Exit", "ESC")) {
			close();
		}
		ImGui::End();
	}

	if (_showFps) {
		// Calculate position for bottom-right corner
		ImVec2 windowSize = ImGui::GetIO().DisplaySize;
		const float fpsWindowPadding = 10.0f;
		ImVec2 fpsWindowSize(0, 0); // Will be set after first frame
		// Set a dummy size to avoid flicker on first frame
		ImGui::SetNextWindowSize(ImVec2(100, 30), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(windowSize.x - 110.0f, windowSize.y - 40.0f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.6f);
		ImGui::Begin("##FPS", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1.0f));
		ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::PopStyleColor();
		ImGui::End();
	}
}

void MainWindow::initImGui() {
	if (!ImGui::SFML::Init(*_window)) {
		// Handle initialization failure if needed
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr; // Disable automatic .ini saving/loading
	WindowHelper::SetScaledFont(io, DEFAULT_FONT_DATA, DEFAULT_FONT_DATA_SIZE, _windowConfig.defaultFontSize);
	// Configure ImGui style
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 6.0f;
	style.FrameRounding = 4.0f;
	style.GrabRounding = 4.0f;

	// Set colors for a modern look
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.95f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
}

void MainWindow::draw() {
	if (!isOpen()) return;
	drawAllOtherWindows();
	ImGui::SFML::SetCurrentWindow(*_window);
	_window->clear();
	_renderEngine->draw();
	renderImGuiMenu();
	ImGui::SFML::Render(*_window);
	_window->display();
}

void MainWindow::saveWindowConfig(WindowConfig& config) const {
	config.position = FromSFMLVector(_window->getPosition());
	config.size = FromSFMLVector(_window->getSize());
	config.open = true;
	config.resizable = true;
}

void MainWindow::update(const float dt, const RobotState& robotState) {
	sf::Time deltaTime = sf::seconds(dt);
	_renderEngine->update(robotState, dt);
	updateAllOtherWindows(deltaTime, robotState);

	ImGui::SFML::SetCurrentWindow(*_window);
	ImGui::SFML::Update(*_window, deltaTime);

	// 3. Process main window events
	while (const std::optional event = _window->pollEvent()) {
		ImGui::SFML::ProcessEvent(*_window, *event);

		if (event->is<sf::Event::Closed>()) {
			close();
			return;
		}

		if (auto resize = event->getIf<sf::Event::Resized>())
		{
			_renderEngine->updateAfterResize();
		}

		if (event->is<sf::Event::KeyPressed>()) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				_showMenu = false;
			}
			if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::F8) {
				_showFps = !_showFps;
			}
		}
	}
}

void MainWindow::SetOnInputSettingsChanged(std::function<void()> callback) {
	_onInputSettingsChanged = std::move(callback);
	_inputSettingsWindow->setOnSettingsChanged([this](const InputSettings& newSettings) {
		this->_appConfig.inputSettings = newSettings;
		_onInputSettingsChanged();
		});
}

void MainWindow::saveConfig() {
	_appConfig.mainWindow = _windowConfig;
	_appConfig.renderSettingsWindow = _settingsWindow->getSavedConfig();
	_appConfig.renderSettings = _renderEngine->getCurrentRenderSettings();
	_appConfig.inputSettingsWindow = _inputSettingsWindow->getSavedConfig();
	_appConfig.robotStatWindow = _robotStatWindow->getSavedConfig();
}

// Other Windows management
void MainWindow::initializeOtherWindows() {
	// === SETTINGS WINDOW ===
	_settingsWindow = std::make_unique<RenderSettingsWindow>(_appConfig);
	_settingsWindow->setOnSettingsChanged([this](const RenderSettings& newSettings) {
		this->_appConfig.renderSettings = newSettings;
		_renderEngine->updateAfterSettingsChange();
		});
	_settingsWindow->setClearRobotTrail([this]() {
		_renderEngine->clearRobotTrail();
		});

	// === INPUT SETTINGS WINDOW ===
	_inputSettingsWindow = std::make_unique<InputSettingsWindow>(_appConfig);
	_inputSettingsWindow->setOnSettingsChanged([this](const InputSettings& newSettings) {
		this->_appConfig.inputSettings = newSettings;
		_onInputSettingsChanged();
		});

	// === ROBOT STATISTICS WINDOW ===
	_robotStatWindow = std::make_unique<RobotStatWindow>(_appConfig);
}

void MainWindow::closeOtherWindows() {
	_settingsWindow->close(true);
	_inputSettingsWindow->close(true);
	_robotStatWindow->close(true);
}

void MainWindow::updateAllOtherWindows(sf::Time dt, const RobotState& robotState) {
	_settingsWindow->update(dt);
	_inputSettingsWindow->update(dt);
	_robotStatWindow->update(dt, robotState);
}

void MainWindow::drawAllOtherWindows() {
	_settingsWindow->draw();
	_inputSettingsWindow->draw();
	_robotStatWindow->draw();
}