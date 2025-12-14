#include "RobotShape.hpp"
#include <format>
#include "Arrow.hpp"
sf::ConvexShape makeRobotBase(const std::vector<sf::Vector2f>& points) {
    if (points.size() < 3) {
        // Too few points for a polygon
        sf::ConvexShape shape;
        shape.setPointCount(points.size());
        for (size_t i = 0; i < points.size(); ++i)
            shape.setPoint(i, points[i]);
        return shape;
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
    shape.setFillColor({50,50,50});
    shape.setOutlineColor(sf::Color::Cyan);
    shape.setOutlineThickness(4.f);

    return shape;
}



RobotShape::RobotShape(const RoboConfig& config, bool drawCenter, bool showSpeed) :_showSpeed(showSpeed), _drawCenter(drawCenter) {
	auto axels = config.GetRobotDriveAxels();
	if (_drawCenter) {}

	for (const auto& [wheel, motor] : axels)
		AddWheel(wheel);

    _numberOfWheels = _wheelMountingPoints.size();
    auto shape = makeRobotBase(_wheelMountingPoints);
    add(std::make_unique<sf::ConvexShape>(shape));
    add(std::make_unique<Arrow>(100,sf::Color::Red,10,25,25));
}
void RobotShape::UpdateSpeed(std::vector<float> speedOfWheels) {
}

void RobotShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	CompositeShape::draw(target, states);
	states.transform *= getTransform();
	//if (_showSpeed) {
	//	for (auto& text : _speedOfWheels)
	//		target.draw(text, states);
	//}
}

void RobotShape::AddWheel(const RoboParts::Wheel& wheel) {
	auto wheelShape = std::make_unique<sf::RectangleShape>();
	wheelShape->setSize({ wheel.diameter,wheel.diameter / 2 });
	wheelShape->setOrigin({ wheel.diameter / 2,wheel.diameter / 4 });
	wheelShape->setPosition({ wheel.x_position,wheel.y_position });
	wheelShape->setRotation(sf::radians(wheel.wheel_angle));
	add(std::move(wheelShape));
	_wheelMountingPoints.push_back({ wheel.x_position,wheel.y_position });
}

