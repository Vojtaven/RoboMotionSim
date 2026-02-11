#include "VisualizationEngine.hpp"
#include "CompositeShape.hpp"
#include "RobotShape.hpp"
#include "RobotState.hpp"
#include "SFMLHelper.hpp"
#include "MathUtils.hpp"

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

	setScaleFactor(1);
	setGridSpacing({ 50,50 });
	resetRobotPosition((sf::Vector2f)_mainWindow->getSize() / 2.0f);
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
	_mainWindow->clear();
	_mainWindow->draw(_gridLines);
	_mainWindow->draw(*_robotShape);
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
	while (const std::optional event = _mainWindow->pollEvent()) {
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
	}

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
