#include "windows/MainWindow.hpp"
#include <memory>
#include "SFMLHelper.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_internal.h>
#include "embeddedFont.h"
#include "embeddedIcon.h"
#include "windows/WindowHelper.hpp"
#include "windows/RenderSettingsWindow.hpp"
#include "windows/InputSettingsWindow.hpp"
#include "windows/RobotStatWindow.hpp"
#include "windows/RobotDesignerWindow.hpp"
#include "ColorConstants.hpp"
MainWindow::MainWindow(AppConfig& config, ConfigSection<RobotConfig>& robotConfigSection)
	: _appConfig(config), _windowConfig(config.mainWindow), _robotConfigSection(robotConfigSection)
{
}

void MainWindow::open(const RobotConfig& robotConfig)
{
	sanitizeWindowConfig();

	_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode(ToSFMLVector2u(_windowConfig.size)),
		_appConfig.appName,
		_windowConfig.resizable ? sf::Style::Default : sf::Style::Titlebar | sf::Style::Close);

	_window->setPosition(ToSFMLVector2i(_windowConfig.position));

	if (_icon.loadFromMemory(APP_ICON_DATA, APP_ICON_DATA_SIZE))
		_window->setIcon(_icon);

	initImGui();
	_renderEngine = std::make_unique<RenderEngine>(*_window, _appConfig.renderSettings);
	setRobotConfig(robotConfig, RobotState(robotConfig.getWheelCount()));
	initializeOtherWindows();
	_window->requestFocus();
}


void MainWindow::close() {
	closeOtherWindows();
	saveWindowConfig(_windowConfig);
	saveConfig();
	auto defaultCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);
	if (defaultCursor) _window->setMouseCursor(*defaultCursor);
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
			_renderSettingsWindow->open(current);
			_showMenu = false;
		}
		else if (ImGui::MenuItem("Input Settings")) {
			const InputSettings& current = _appConfig.inputSettings.get();
			_inputSettingsWindow->open(current);
			_showMenu = false;
		}
		else if (ImGui::MenuItem("Robot Stats")) {
			_robotStatWindow->open();
			_showMenu = false;
		}
		else if (ImGui::MenuItem("Robot Designer")) {
			_robotDesignerWindow->open();
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
		ImGui::PushStyleColor(ImGuiCol_Text, Colors::OverlayText);
		ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::PopStyleColor();
		ImGui::End();
	}

	if (_showTimeStamp) {
		using namespace std::chrono;
		auto sec = floor<seconds>(_timeStamp);
		auto ms = duration_cast<milliseconds>(_timeStamp - sec).count();
		std::string timeStr = std::format("{:%H:%M:%S}", sec);

		ImGui::SetNextWindowSize(ImVec2(160, 30), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(padding, padding), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.6f);
		ImGui::Begin("##Timestamp", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PushStyleColor(ImGuiCol_Text, Colors::OverlayText);
		ImGui::Text("Time: %s.%03lld", timeStr.c_str(), ms);
		ImGui::PopStyleColor();
		ImGui::End();
	}

	renderErrorMessages();
	_errorMessages.clear();
}

void MainWindow::renderErrorMessages() {
	if (_errorMessages.empty()) return;

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	const float windowWidth = 400.0f;
	const float padding = 10.0f;
	ImGui::SetNextWindowPos(ImVec2((displaySize.x - windowWidth) * 0.5f, displaySize.y * 0.25f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, 0), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.92f);

	ImGui::PushStyleColor(ImGuiCol_TitleBg, Colors::ErrorTitleBg);
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Colors::ErrorTitleBgActive);

	ImGui::Begin("Errors", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::PushStyleColor(ImGuiCol_Text, Colors::ErrorText);

	for (int i = static_cast<int>(_errorMessages.size()) - 1; i >= 0; --i) {
		ImGui::TextWrapped("%s", _errorMessages[i].c_str());
		ImGui::SameLine(windowWidth - 50.0f);
		if (i > 0)
			ImGui::Separator();
	}

	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::End();
	ImGui::PopStyleColor(2);
}

void MainWindow::initImGui() {
	if (!ImGui::SFML::Init(*_window)) {
		throw std::runtime_error("Failed to initialize ImGui-SFML");
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::GetCurrentContext()->ConfigNavWindowingKeyNext = 0;                                                                  
	ImGui::GetCurrentContext()->ConfigNavWindowingKeyPrev = 0;
	io.IniFilename = nullptr; // Disable automatic .ini saving/loading
	WindowHelper::SetScaledFont(io, DEFAULT_FONT_DATA, DEFAULT_FONT_DATA_SIZE, _windowConfig.defaultFontSize);
	// Configure ImGui style
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 6.0f;
	style.FrameRounding = 4.0f;
	style.GrabRounding = 4.0f;

	// Set colors for a modern look
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_WindowBg] = Colors::StyleWindowBg;
	colors[ImGuiCol_TitleBg] = Colors::StyleTitleBg;
	colors[ImGuiCol_TitleBgActive] = Colors::StyleTitleBgActive;
	colors[ImGuiCol_Button] = Colors::StyleButton;
	colors[ImGuiCol_ButtonHovered] = Colors::StyleButtonHovered;
	colors[ImGuiCol_ButtonActive] = Colors::StyleButtonActive;
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

void MainWindow::sanitizeWindowConfig() {
	WindowHelper::sanitizeWindowConfig(_windowConfig, 400, 300);
}

void MainWindow::update(const float dt, const RobotState& robotState, const std::chrono::system_clock::time_point& timeStamp) {
	_timeStamp = timeStamp;
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
			else if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::F8) {
				_showFps = !_showFps;
			}
			else if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::F7) {
				_showTimeStamp = !_showTimeStamp;
			}
		}
	}
}


void MainWindow::showErrorMessage(const std::string& message) {
	_errorMessages.push_back(message);
}

void MainWindow::saveConfig() {
	_appConfig.mainWindow = _windowConfig;
	_appConfig.renderSettingsWindow = _renderSettingsWindow->getSavedConfig();
	_appConfig.inputSettingsWindow = _inputSettingsWindow->getSavedConfig();
	_appConfig.robotStatWindow = _robotStatWindow->getSavedConfig();
	_appConfig.robotDesignerWindow = _robotDesignerWindow->getSavedConfig();
}

// Other Windows management
void MainWindow::initializeOtherWindows() {
	// === SETTINGS WINDOW ===
	_renderSettingsWindow = std::make_unique<RenderSettingsWindow>(_appConfig.renderSettings, _appConfig.renderSettingsWindow, _icon);
	_renderSettingsWindow->setClearRobotTrail([this]() {
		_renderEngine->clearRobotTrail();
		});

	// === INPUT SETTINGS WINDOW ===
	_inputSettingsWindow = std::make_unique<InputSettingsWindow>(_appConfig.inputSettings, _appConfig.inputSettingsWindow, _icon);

	// === ROBOT STATISTICS WINDOW ===
	_robotStatWindow = std::make_unique<RobotStatWindow>(_appConfig, _icon);

	// === ROBOT DESIGNER WINDOW ===
	_robotDesignerWindow = std::make_unique<RobotDesignerWindow>(_robotConfigSection, _appConfig.robotDesignerWindow, _icon);
}

void MainWindow::closeOtherWindows() {
	_renderSettingsWindow->close(true);
	_inputSettingsWindow->close(true);
	_robotStatWindow->close(true);
	_robotDesignerWindow->close(true);
}

void MainWindow::updateAllOtherWindows(sf::Time dt, const RobotState& robotState) {
	_renderSettingsWindow->update(dt);
	_inputSettingsWindow->update(dt);
	_robotStatWindow->update(dt, robotState);
	_robotDesignerWindow->update(dt);
}

void MainWindow::drawAllOtherWindows() {
	_renderSettingsWindow->draw();
	_inputSettingsWindow->draw();
	_robotStatWindow->draw();
	_robotDesignerWindow->draw();
}