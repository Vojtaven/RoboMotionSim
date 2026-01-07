#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP
#include "RobotConfig.hpp"
#include "RobotState.hpp"
class PhysicsEngine {
public:
	void update(const float dt, RobotState& state,const RobotConfig& config);
private:
	void updatePosition(const float dt, RobotState& state);
	void toWheelSpeed(RobotState& state, const RobotConfig& config);
	void toGlobalFrame(RobotState& state);
	void limitMovement(RobotState& state, const RobotConfig& config);
};
#endif // PHYSICS_ENGINE_HPP