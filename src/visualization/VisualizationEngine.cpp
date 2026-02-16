#include "VisualizationEngine.hpp"
#include "CompositeShape.hpp"
#include "RobotShape.hpp"
#include "RobotState.hpp"
#include "SFMLHelper.hpp"
#include "MathUtils.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdint>

void  VisualizationEngine::CreateMainWindow(const AppConfig& appConfig) {
	auto& mainConfig = appConfig.MainWindowConfig;
	_mainView = std::make_unique<sf::View>(sf::FloatRect({ 0.f, 0.f }, ToSFMLVector2f( mainConfig.size)));
	_mainWindow = std::make_unique<sf::RenderWindow>(
		sf::VideoMode(ToSFMLVector2u(mainConfig.size)),
		appConfig.appName,
		mainConfig.resizable ? sf::Style::Default : sf::Style::Titlebar | sf::Style::Close);
	_mainWindow->setPosition(ToSFMLVector2i(mainConfig.position));
	_mainWindow->setView(*_mainView);
	_mainWindow->setFramerateLimit(mainConfig.frameRateLimit);
}

VisualizationEngine::VisualizationEngine(const AppConfig& appConfig, const RobotConfig& robotConfig)
	:_appConfig(appConfig), _robotShape(std::make_unique<RobotShape>(robotConfig)) {
	CreateMainWindow(appConfig);
	InitImGui();

	// Wire up the settings window callback
	_settingsWindow.setOnSettingsChanged([this](const VisualizationSettings& s) {
		setScaleFactor(s.scaleFactor);
		setGridSpacing({ s.gridSpacing[0], s.gridSpacing[1] });
		setGridColor(sf::Color(
			(uint8_t)(s.gridColor[0] * 255),
			(uint8_t)(s.gridColor[1] * 255),
			(uint8_t)(s.gridColor[2] * 255)
		));
	});

	setScaleFactor(1);
	setGridSpacing({ 50,50 });
	resetRobotPosition((sf::Vector2f)_mainWindow->getSize() / 2.0f);
}

VisualizationEngine::~VisualizationEngine() {
	_settingsWindow.close();
	ImGui::SFML::SetCurrentWindow(*_mainWindow);
	ImGui::SFML::Shutdown();
}

void VisualizationEngine::InitImGui() {
	if (!ImGui::SFML::Init(*_mainWindow)) {
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

void VisualizationEngine::RenderImGuiMenu() {
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
			VisualizationSettings current;
			current.scaleFactor = _scaleFactor;
			current.gridSpacing[0] = _gridSpacing.x;
			current.gridSpacing[1] = _gridSpacing.y;
			current.gridColor[0] = _gridColor.r / 255.0f;
			current.gridColor[1] = _gridColor.g / 255.0f;
			current.gridColor[2] = _gridColor.b / 255.0f;
			_settingsWindow.open(current, *_mainWindow);
			_showMenu = false;
		}
		
		ImGui::Separator();
		
		if (ImGui::MenuItem("Exit", "ESC")) {
			_mainWindow->close();
		}
		ImGui::ShowDemoWindow();
		ImGui::End();
	}
}

void VisualizationEngine::setRobotConfig(const RobotConfig& config, bool holdPosition) {
	sf::Vector2f lastPosition = holdPosition ? _robotShape->getPosition() : (sf::Vector2f)_mainWindow->getSize() / 2.0f;
	_robotShape = std::make_unique<RobotShape>(config);
	resetRobotPosition(lastPosition);
}

void VisualizationEngine::setRobotPosition(const sf::Vector2f position, const sf::Angle angle) {
	_robotShape->setPosition(position);
	_robotShape->setRotation(angle);
}

void VisualizationEngine::moveRobotBy(const sf::Vector2f offset, const sf::Angle angleOffset) {
	_robotShape->move(offset);
	_robotShape->rotate(angleOffset);
}

void VisualizationEngine::draw() {
	// Restore main window ImGui context before rendering
	ImGui::SFML::SetCurrentWindow(*_mainWindow);
	
	_mainWindow->clear();
	_mainWindow->draw(_gridLines);
	_mainWindow->draw(*_robotShape);
	ImGui::SFML::Render(*_mainWindow);
	_mainWindow->display();
}

void VisualizationEngine::setScaleFactor(const float scale) {
	_scaleFactor = scale;
	_robotShape->setScale({ scale, scale });
	RegenerateGridLines();
}

void VisualizationEngine::setGridSpacing(const sf::Vector2f spacing) {
	_gridSpacing = spacing;
	RegenerateGridLines();
}

void VisualizationEngine::setGridColor(const sf::Color color) {
	_gridColor = color;
	RegenerateGridLines();
}

void VisualizationEngine::update(const RobotState& state) { 
	_robotShape->Update(state); 
	
	// 1. Update the separate settings window FIRST (its own context)
	_settingsWindow.update();
	
	// 2. Switch to main window ImGui context and start frame
	ImGui::SFML::SetCurrentWindow(*_mainWindow);
	ImGui::SFML::Update(*_mainWindow, _deltaClock.restart());
	
	// 3. Process main window events
	while (const std::optional event = _mainWindow->pollEvent()) {
		ImGui::SFML::ProcessEvent(*_mainWindow, *event);
		
		if (event->is<sf::Event::Closed>()) {
			saveAppConfigBeforeClose();
			_mainWindow->close();
		}

		if (auto resize = event->getIf<sf::Event::Resized>())
		{
			float newWidth = static_cast<float>(resize->size.x);
			float newHeight = static_cast<float>(resize->size.y);
			_mainView->setSize({ newWidth, newHeight });
			_mainView->setCenter({ newWidth / 2.f, newHeight / 2.f });
			_mainWindow->setView(*_mainView);
			updateAfterResize();
		}
		
		// Close menu with ESC key
		if (event->is<sf::Event::KeyPressed>()) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				_showMenu = false;
			}
		}
	}
	
	// 4. Render ImGui menu on main window
	RenderImGuiMenu();
}

void VisualizationEngine::saveAppConfigBeforeClose() {
	auto& mainConfig = _appConfig.MainWindowConfig;

	mainConfig.position = FromSFMLVector(_mainWindow->getPosition());
	mainConfig.size = FromSFMLVector(_mainWindow->getSize());
}

void VisualizationEngine::RegenerateGridLines() {
	CompositeShape grid;
	sf::Vector2f pixelSpacing = _gridSpacing * _scaleFactor;

	sf::Vector2f windowSize = (sf::Vector2f)_mainWindow->getSize();

	int numberOfVerticalLines = windowSize.x / pixelSpacing.x + 1;
	int numberOfHorizontalLines = windowSize.y / pixelSpacing.y + 1;

	std::unique_ptr<sf::RectangleShape> line;
	for (int i = 0; i < numberOfVerticalLines; i++) {
		line = std::make_unique<sf::RectangleShape>(sf::Vector2f(_gridLineThickness, windowSize.y));
		line->setPosition({ pixelSpacing.x * i,0 });
		line->setFillColor(_gridColor);
		line->setOutlineColor(_gridColor);
		grid.add(std::move(line));
	}

	for (int i = 0; i < numberOfHorizontalLines; i++) {
		line = std::make_unique<sf::RectangleShape>(sf::Vector2f(windowSize.x, _gridLineThickness));
		line->setPosition({ 0, pixelSpacing.y * i });
		line->setFillColor(_gridColor);
		line->setOutlineColor(_gridColor);
		grid.add(std::move(line));
	}

	_gridLines = std::move(grid);
}

void VisualizationEngine::updateAfterResize() {
	RegenerateGridLines();
}

void VisualizationEngine::resetRobotPosition(sf::Vector2f pos) {
	_robotShape->setPosition(pos);
	_robotShape->setRotation(sf::degrees(0));
	_robotShape->setScale({ _scaleFactor, _scaleFactor });
}

Vec2f VisualizationEngine::getWindowCenter() const {
	return FromSFMLVector((sf::Vector2f)_mainWindow->getSize() / 2.f);
}

void AddDirectionVectorToRobot(Vec2f position, RobotState& state) {
	DirectionVector dirVec;
	dirVec.position = position;
	dirVec.angle = std::atan2(position.y, position.x);
	dirVec.length = std::hypot(position.x, position.y);
	state.directionVectors.push_back(dirVec);
}
