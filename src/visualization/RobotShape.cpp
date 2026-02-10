#include "RobotShape.hpp"
#include <format>
#include "PointVector.hpp"
#include <memory>
#include "WheelVectors.hpp"
#include "SFMLHelper.hpp"
std::unique_ptr<sf::ConvexShape> makeRobotBase(const std::vector<sf::Vector2f>& points) {
	if (points.size() < 3) {
		// Too few points for a polygon
		sf::ConvexShape shape;
		shape.setPointCount(points.size());
		for (size_t i = 0; i < points.size(); ++i)
			shape.setPoint(i, points[i]);
		return std::make_unique<sf::ConvexShape>(shape);
	}

	// Copy points and sort them
	std::vector<sf::Vector2f> pts = points;
	std::sort(pts.begin(), pts.end(), [](const sf::Vector2f& a, const sf::Vector2f& b) {
		return (a.x < b.x) || (a.x == b.x && a.y < b.y);
		});

	auto cross = [](const sf::Vector2f& O, const sf::Vector2f& A, const sf::Vector2f& B) {
		return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
		};

	std::vector<sf::Vector2f> hull;

	// Build lower hull
	for (const auto& p : pts) {
		while (hull.size() >= 2 && cross(hull[hull.size() - 2], hull.back(), p) <= 0)
			hull.pop_back();
		hull.push_back(p);
	}

	// Build upper hull
	size_t lower_size = hull.size();
	for (int i = (int)pts.size() - 2; i >= 0; --i) {
		const auto& p = pts[i];
		while (hull.size() > lower_size && cross(hull[hull.size() - 2], hull.back(), p) <= 0)
			hull.pop_back();
		hull.push_back(p);
	}

	hull.pop_back(); // last point repeats the first one

	// Create sf::ConvexShape
	sf::ConvexShape shape;
	shape.setPointCount(hull.size());
	for (size_t i = 0; i < hull.size(); ++i)
		shape.setPoint(i, hull[i]);

	// Optional styling
	shape.setFillColor({ 50,50,50 });
	shape.setOutlineColor(sf::Color::Cyan);
	shape.setOutlineThickness(4.f);

	return std::make_unique<sf::ConvexShape>(shape);
}



RobotShape::RobotShape(const RobotConfig& config, bool drawCenter, bool showSpeed) :
	_showSpeed(showSpeed),
	_drawCenter(drawCenter)
{
	auto axels = config.GetRobotDriveAxels();
	if (_drawCenter) {}

	std::transform(axels.begin(), axels.end(), std::back_inserter(_wheelMountingPoints),
		[](const RobotParts::DriveAxle_t& axle) {
			return sf::Vector2f{ axle.wheel.x_position, axle.wheel.y_position };
		});
	_numberOfWheels = _wheelMountingPoints.size();
	auto shape = makeRobotBase(_wheelMountingPoints);
	add(std::move(shape));
	float distanceFromCenter = 0;
	for(const auto& x : _wheelMountingPoints)
		distanceFromCenter += std::sqrtf(x.x * x.x + x.y * x.y);

	distanceFromCenter /= _numberOfWheels * 3;

	auto frontVec = std::make_unique<PointVector>(sf::Vector2f{0,0}, 0.0f, distanceFromCenter, sf::Color::White);
	_frontVector = frontVec.get();

	for (const auto& wheel : config.GetRobotWheels())
		AddWheel(wheel);

	add(std::move(frontVec));
}

void RobotShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	CompositeShape::draw(target, states);
	states.transform *= getTransform();
}

void RobotShape::Update(const RobotState& state) {
	setPosition(ToSFMLVector2f(state.position));
	setRotation(ToSFMLAngle(state.chassisAngle));
	_frontVector->setRotation(state.frontAngle);

	for (int i = 0; i < state.wheelCount;i++) {
		_speedOfWheels[i]->update(state.wheels[i]);
	}
}

void RobotShape::AddWheel(const RobotParts::Wheel& wheel) {
	auto wheelShape = std::make_unique<sf::RectangleShape>();
	wheelShape->setSize({ wheel.diameter,wheel.diameter / 2.f });
	wheelShape->setOrigin({ wheel.diameter / 2.f,wheel.diameter / 4.f });
	wheelShape->setPosition({ wheel.x_position,wheel.y_position });
	wheelShape->setRotation(sf::radians(wheel.wheel_angle));
	add(std::move(wheelShape));
	_wheelMountingPoints.push_back({ wheel.x_position,wheel.y_position });

	AddWheelVector(wheel);
}

void RobotShape::AddWheelVector(const RobotParts::Wheel& wheel) {
	//Vector of speed for this wheel
	auto wheelVectors = std::make_unique<WheelVectors>(
		wheel,
		sf::Color::Red,
		sf::Color::Green,
		sf::Color::Yellow,
		4.f, sf::Vector2f{ 25, 25 });

	_speedOfWheels.push_back(wheelVectors.get());
	add(std::move(wheelVectors));
}

