#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP
#include "RobotConfig.hpp"
#include "RobotState.hpp"
class PhysicsEngine {
public:
	void Update(const float dt, RobotState& state,const RobotConfig& config);
private:
	void UpdatePosition(const float dt, RobotState& state);
	void ToWheelSpeed(RobotState& state, const RobotConfig& config);
	void ToGlobalFrame(RobotState& state);
};
#endif // PHYSICS_ENGINE_HPP