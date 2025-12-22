#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP
#include "PhysicsObjects.hpp"
#include "RoboConfig.hpp"
class PhysicsEngine {
public:
	void Update(float dt, RoboConfig* config);
private:
	void ToWheelSpeed(RoboConfig* config);
};
#endif // PHYSICS_ENGINE_HPP