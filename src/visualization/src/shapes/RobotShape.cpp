// Author: Vojtech Venzara
// Date: 2026-04-29
// Description: Visual representation of the robot body, wheels, and orientation indicators

#include "shapes/RobotShape.hpp"
#include <format>
#include "shapes/PointVector.hpp"
#include <memory>
#include "shapes/WheelVectors.hpp"
#include "shapes/CurvedArrow.hpp"
#include "SFMLHelper.hpp"
#include "ColorConstants.hpp"

std::unique_ptr<sf::ConvexShape> makeRobotBase(const std::vector<sf::Vector2f>& points) {
	if (points.size() < 2) {
		// Zero or one point: nothing meaningful to draw
		sf::ConvexShape shape;
		shape.setPointCount(points.size());
		for (size_t i = 0; i < points.size(); ++i)
			shape.setPoint(i, points[i]);
		return std::make_unique<sf::ConvexShape>(shape);
	}

	if (points.size() == 2) {
		// Two wheels: extrude a rectangle around the wheel axis
		const sf::Vector2f& a = points[0];
		const sf::Vector2f& b = points[1];
		sf::Vector2f dir = b - a;
		float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		sf::Vector2f perp = (len > 0.f)
			? sf::Vector2f{ -dir.y / len, dir.x / len }
			: sf::Vector2f{ 0.f, 1.f };
		float depth = len * 0.25f;

		sf::ConvexShape shape;
		shape.setPointCount(4);
		shape.setPoint(0, a + perp * depth);
		shape.setPoint(1, b + perp * depth);
		shape.setPoint(2, b - perp * depth);
		shape.setPoint(3, a - perp * depth);
		shape.setFillColor(Colors::RobotBaseFill);
		shape.setOutlineColor(Colors::RobotOutline);
		shape.setOutlineThickness(4.f);
		return std::make_unique<sf::ConvexShape>(shape);
	}

	// Convex hull via Andrew's monotone chain algorithm
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
	shape.setFillColor(Colors::RobotBaseFill);
	shape.setOutlineColor(Colors::RobotOutline);
	shape.setOutlineThickness(4.f);

	return std::make_unique<sf::ConvexShape>(shape);
}



RobotShape::RobotShape(const RobotConfig& config, bool drawCenter, bool showSpeed) :
	_showSpeed(showSpeed),
	_drawCenter(drawCenter)
{
	auto axles = config.getRobotDriveAxles();

	std::transform(axles.begin(), axles.end(), std::back_inserter(_wheelMountingPoints),
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

	auto frontVec = std::make_unique<PointVector>(sf::Vector2f{0,0}, 0.0f, distanceFromCenter, Colors::FrontVector);
	_frontVector = frontVec.get();

	for (const auto& wheel : config.getRobotWheels())
		addWheel(wheel);

	add(std::move(frontVec));

	sf::Vector2f centroid{ 0.f, 0.f };
	for (const auto& pt : _wheelMountingPoints)
		centroid += pt;

	if(!_wheelMountingPoints.empty())
		centroid /= static_cast<float>(_wheelMountingPoints.size());
	setOrigin(centroid);
}

void RobotShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	CompositeShape::draw(target, states);
	states.transform *= getTransform();
}

void RobotShape::update(const RobotState& state) {
	setPosition(static_cast<sf::Vector2f>(state.position));
	setRotation(ToSFMLAngle(state.chassisAngle));
	_frontVector->setRotation(static_cast<float>(state.frontAngle));

	for (int i = 0; i < state.wheelCount;i++) {
		_speedOfWheels[i]->update(state.wheels[i]);
	}
	
	updateDirectionVectors(state);
}

void RobotShape::setVectorsVisibility(bool forwardVisible, bool rollerVisible, bool directionVisible) {
	for (auto& wheelVectors : _speedOfWheels) {
		wheelVectors->setVectorsVisible(forwardVisible, rollerVisible, directionVisible);
	}
}

void RobotShape::updateDirectionVectors(const RobotState& state) {
	// If the number of direction vectors changed, rebuild them
	if (_directionVectors.size() != state.directionVectors.size()) {
		_directionVectors.clear();
		_rotationIndicators.clear();
		_rotationBaseAngles.clear();

		// Add new direction vectors
		for (const auto& dirVec : state.directionVectors) {
			auto pointVec = std::make_unique<PointVector>(
				static_cast<sf::Vector2f>(dirVec.position),
				dirVec.angle,
				dirVec.length,
				Colors::DirectionVector,
				6.f,
				sf::Vector2f{20, 20}
			);
			_directionVectors.push_back(pointVec.get());
			add(std::move(pointVec));

			auto rotInd = std::make_unique<CurvedArrow>(30.f, Colors::RotationIndicator, 4.f, sf::Vector2f{ 15, 15 });
			rotInd->setPosition(static_cast<sf::Vector2f>(dirVec.position));
			rotInd->setRotation(sf::radians(dirVec.angle));
			_rotationIndicators.push_back(rotInd.get());
			_rotationBaseAngles.push_back(dirVec.angle);
			add(std::move(rotInd));
		}
	}
	else {
		// Update existing direction vectors
		for (size_t i = 0; i < state.directionVectors.size(); ++i) {
			const auto& dirVec = state.directionVectors[i];
			_directionVectors[i]->setPosition(static_cast<sf::Vector2f>(dirVec.position));
			_directionVectors[i]->setRotation(dirVec.angle);
			_directionVectors[i]->setLength(dirVec.length);

			// Only refresh the base angle when linear motion is strong enough to define one;
			// below the threshold we hold the last angle so the arc doesn't jitter on noise.
			constexpr float kBaseAngleUpdateThreshold = 5.f; // mm/s
			if (dirVec.length > kBaseAngleUpdateThreshold)
				_rotationBaseAngles[i] = dirVec.angle;

			_rotationIndicators[i]->setPosition(static_cast<sf::Vector2f>(dirVec.position));
			_rotationIndicators[i]->setRotation(sf::radians(_rotationBaseAngles[i]));
			_rotationIndicators[i]->setSweep(static_cast<float>(state.angularVelocity));
		}
	}
}

void RobotShape::addWheel(const RobotParts::Wheel& wheel) {
	auto wheelShape = std::make_unique<sf::RectangleShape>();
	wheelShape->setSize({ wheel.diameter,wheel.diameter / 2.f });
	wheelShape->setOrigin({ wheel.diameter / 2.f,wheel.diameter / 4.f });
	wheelShape->setPosition({ wheel.x_position,wheel.y_position });
	wheelShape->setRotation(sf::radians(wheel.wheel_angle));
	add(std::move(wheelShape));
	_wheelMountingPoints.push_back({ wheel.x_position,wheel.y_position });

	addWheelVector(wheel);
}

void RobotShape::addWheelVector(const RobotParts::Wheel& wheel) {
	//Vector of speed for this wheel
	auto wheelVectors = std::make_unique<WheelVectors>(
		wheel,
		Colors::WheelForwardVector,
		Colors::WheelRollerVector,
		Colors::WheelDirVector,
		4.f, sf::Vector2f{ 25, 25 });

	_speedOfWheels.push_back(wheelVectors.get());
	add(std::move(wheelVectors));
}

