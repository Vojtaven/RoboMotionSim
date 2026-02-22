#include "windows/MainWindow.hpp"
#include <memory>
#include "SFMLHelper.hpp"
#include <imgui.h>
#include <imgui-SFML.h>

MainWindow::MainWindow(const AppConfig& config)
    : _appConfig(config), _windowConfig(_appConfig.mainWindow)
{}

void MainWindow::open(const RobotConfig& robotConfig)
{
	_mainView = std::make_unique<sf::View>(sf::FloatRect({ 0.f, 0.f }, ToSFMLVector2f(_windowConfig.size)));
	_window = std::make_unique<sf::RenderWindow>(
		sf::VideoMode(ToSFMLVector2u(_windowConfig.size)),
		_appConfig.appName,
		_windowConfig.resizable ? sf::Style::Default : sf::Style::Titlebar | sf::Style::Close);

	_window->setPosition(ToSFMLVector2i(_windowConfig.position));
	_window->setView(*_mainView);
	initImGui();
	_renderEngine = std::make_unique<RenderEngine>(*_window, _appConfig.renderSettings);
	setRobotConfig(robotConfig);
	initializeOtherWindows();
}

void MainWindow::close() {
	closeOtherWindows();
	ImGui::SFML::SetCurrentWindow(*_window);
	ImGui::SFML::Shutdown(*_window);
	saveWindowConfig(_windowConfig);
	saveConfig();
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

		if (ImGui::MenuItem("Settings")) {
			RenderSettings current = _renderEngine->getCurrentRenderSettings();
			_settingsWindow->open(current);
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
		const float fpsWindowPadding = 10.0f;
		ImGui::SetNextWindowPos(ImVec2(fpsWindowPadding, fpsWindowPadding));
		ImGui::SetNextWindowBgAlpha(0.6f);
		ImGui::Begin("##FPS", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

void MainWindow::initImGui() {
	if (!ImGui::SFML::Init(*_window)) {
		// Handle initialization failure if needed
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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

void MainWindow::update(float dt,const RobotState& robotState ) {
	sf::Time deltaTime = sf::seconds(dt);
	_renderEngine->update(robotState);
	updateAllOtherWindows(deltaTime);

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
			float newWidth = static_cast<float>(resize->size.x);
			float newHeight = static_cast<float>(resize->size.y);
			_mainView->setSize({ newWidth, newHeight });
			_mainView->setCenter({ newWidth / 2.f, newHeight / 2.f });
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


void MainWindow::saveConfig() {
	_appConfig.mainWindow = _windowConfig;
	_appConfig.renderSettingsWindow = _settingsWindow->getSavedConfig();
	_appConfig.renderSettings = _renderEngine->getCurrentRenderSettings();
}

// Other Windows management
void MainWindow::initializeOtherWindows() {
	_settingsWindow = std::make_unique<RenderSettingsWindow>(_appConfig);
	_settingsWindow->setOnSettingsChanged([this](const RenderSettings& newSettings) {
		_renderEngine->setRenderSettings(newSettings);
		});
}

void MainWindow::closeOtherWindows() {
	_settingsWindow->close(true);
}

void MainWindow::updateAllOtherWindows(sf::Time dt) {
	_settingsWindow->update(dt);
}

void MainWindow::drawAllOtherWindows() {
	_settingsWindow->draw();
}