#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP
#include "RobotConfig.hpp"
#include "RobotState.hpp"
#include "AppConfig.hpp"
#include "ConfigSection.hpp"
class PhysicsEngine {
public:
	void update(const double dt, RobotState& state,const RobotConfig& config);
	void subscribeToSettings(ConfigSection<InputSettings>& inputSettings);
private:
	bool _limitMotorSpeed = false;
	ScopedSubscription<InputSettings> _settingsSubscription;
	void calculateLocalVelocityFromWheelSpeed(RobotState& state, const RobotConfig& config);
	void updatePosition(const double dt, RobotState& state);
	void toWheelSpeed(RobotState& state, const RobotConfig& config);
	void toGlobalFrame(RobotState& state);
	void limitMovement(RobotState& state, const RobotConfig& config);
	void limitMotorSpeeds(RobotState& state, const RobotConfig& config);
	void updateDirectionVectors(RobotState& state);
};
#endif // PHYSICS_ENGINE_HPP