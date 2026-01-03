#include "PhysicsEngine.hpp"
#include "RobotState.hpp"
#include "RobotConfig.hpp"

#include <iostream>
void PhysicsEngine::Update(const float dt, RobotState& state, const RobotConfig& config)
{
	ToGlobalFrame(state);
	UpdatePosition(dt, state);
	this->ToWheelSpeed(state,config);
}

void PhysicsEngine::UpdatePosition(const float dt, RobotState& state) {
	state.position += state.globalvelocity * dt;
	state.chassisAngle += state.angularVelocity * dt;
	state.frontAngle = state.chassisAngle;
}

void PhysicsEngine::ToGlobalFrame(RobotState& state) {
	Vec2& global = state.globalvelocity;
	Vec2& local = state.localVelocity;
	const float angle = state.frontAngle;
	global.x = local.x * cos(angle) - local.y * sin(angle);
	global.y = local.x * sin(angle) + local.y * cos(angle);

}

void PhysicsEngine::ToWheelSpeed(RobotState& state, const RobotConfig& config){
    auto wheels = config.GetRobotWheels(); // This is our transform_view
    auto robotSpeed = state.localVelocity;
    auto omega = state.angularVelocity;
    int i = 0;
    for (const auto& wheel : wheels) {
        // 1. Calculate the local velocity of the point where the wheel is attached
        // Velocity = Linear Velocity + (Angular Velocity * Radius)
        float local_vx = robotSpeed.x - (omega * wheel.y_position);
        float local_vy = robotSpeed.y + (omega * wheel.x_position);

        if(omega != 0)
        std::cout << "Wheel " << i << " at [" << wheel.x_position << "," << wheel.y_position << "] "
            << "Omega: " << omega << " -> LocalVX: " << local_vx;

        // 2. Project that velocity onto the wheel's direction
        // This is the generalized formula for Omni/Mecanum wheels
        float angleSum = wheel.wheel_angle + wheel.roller_angle;

        state.wheels[i].speed = (local_vx * cos(angleSum) + local_vy * sin(angleSum))
            / cos(wheel.roller_angle);
        if (omega != 0)
        std::cout << " Speed: " << state.wheels[i].speed << std::endl;
        i++;
    }
}
