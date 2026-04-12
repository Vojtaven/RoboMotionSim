// Author: Vojtech Venzara
// Date: 2026-04-06
// Description: Definition of IPC command types and structures used for inter-process communication

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <cstdint>
#include <cmath>
#include "CommandParameters.hpp"
#include "Protocol.hpp"
#include "RobotState.hpp"
#include <memory>
#include <functional>

class Command
{
protected:
	const uint32_t _id = 0;
	Command(uint32_t id) : _id(id) {}
public:
	virtual ~Command() = default;

	// Advances internal progress (time/distance remaining). Returns true if any sub-command completed.
	virtual bool updateAndCheckCompletion(const RobotState& state, const double dt) = 0;
	// Writes this command's target velocities/speeds into state
	virtual void execute(RobotState& state) = 0;
	virtual bool isMoveCompleted() const = 0;
	// Validates command parameters against robot configuration. Throws on failure.
	virtual void validate(int /*wheelCount*/) const {}

	static CommandType getCommandType(const uint8_t* data, size_t size);
	static std::unique_ptr<Command> create(uint32_t id, CommandType type, const uint8_t* data, size_t size);

	uint32_t getId() const { return _id; }
};


class RawMoveCommand
{
protected:
	Vec2f _speed; // mm/s
	float _rotationSpeed; // rad/s
	float _frontRotationSpeed; // rad/s
	bool _chassisFrameVelocity = false; // when true, _speed is in chassis frame and gets converted to front frame each tick
public:
	RawMoveCommand(Vec2f speed, float rotationSpeed, float frontRotationSpeed, bool chassisFrameVelocity = false);
	void execute(RobotState& state);
	virtual ~RawMoveCommand() = default;
};

class RawMotorCommand
{
public:
	const uint16_t motor_id;
	const float speed;
	RawMotorCommand(uint16_t motor_id, float speed);
	void execute(RobotState& state);
	virtual ~RawMotorCommand() = default;
};


// ================================================
// TIME-BASED COMMANDS
// ================================================

// Command that moves for a specified duration, then stops
class TimeCommand : public Command
{
protected:
	double _timeRemaining; // seconds
	TimeCommand(uint32_t id, float time) : Command(id), _timeRemaining(time) {}
public:

	bool isMoveCompleted() const override { return _timeRemaining <= 0.0; }
	bool updateAndCheckCompletion(const RobotState& state, const double dt) override;
	virtual ~TimeCommand() = default;
};

class MoveByTimeRaw : public TimeCommand, public RawMoveCommand
{
public:
	MoveByTimeRaw(uint32_t id, MoveByTimeRawParams params, bool chassisFrameVelocity = false) :
		TimeCommand(id, params.time_s),
		RawMoveCommand({ params.x_speed, params.y_speed }, params.rotation_speed, params.front_rotation_speed, chassisFrameVelocity) {
	}
	void execute(RobotState& state) override { RawMoveCommand::execute(state); }
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~MoveByTimeRaw() = default;
};


class RunMotorForTime : public TimeCommand, public RawMotorCommand
{
public:
	RunMotorForTime(uint32_t id, RunMotorForTimeParams params) : TimeCommand(id, params.time_s), RawMotorCommand(params.motor_id, params.speed) {}
	void execute(RobotState& state) override { RawMotorCommand::execute(state); }
	void validate(int wheelCount) const override;
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~RunMotorForTime() = default;
};

// ================================================
// DISTANCE-BASED COMMANDS
// ================================================

// Command that moves until a certain distance is traveled, then stops
class DistanceCommand : public Command
{
protected:
	double _distanceRemaining; // mm
	DistanceCommand(uint32_t id, float distance) : Command(id), _distanceRemaining(distance) {}
public:
	bool isMoveCompleted() const override { return _distanceRemaining <= 0.0; }
	bool updateAndCheckCompletion(const RobotState& state, const double dt) override;
	virtual ~DistanceCommand() = default;
};


class MoveByDistanceRaw : public DistanceCommand, public RawMoveCommand
{
public:
	MoveByDistanceRaw(uint32_t id, MoveByDistanceRawParams params, bool chassisFrameVelocity = false) :
		DistanceCommand(id, params.distance_mm),
		RawMoveCommand({ params.x_speed, params.y_speed }, params.rotation_speed, params.front_rotation_speed, chassisFrameVelocity) {
	}
	void execute(RobotState& state) override { RawMoveCommand::execute(state); }
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~MoveByDistanceRaw() = default;
};


// Command that moves until a certain distance is traveled, then stops, but only counts distance traveled by the one motor 
class RunMotorForDistance : public DistanceCommand, public RawMotorCommand
{
public:
	RunMotorForDistance(uint32_t id, RunMotorForDistanceParams params) : DistanceCommand(id, params.distance_mm), RawMotorCommand(params.motor_id, params.speed) {}
	void execute(RobotState& state) override { RawMotorCommand::execute(state); }
	void validate(int wheelCount) const override;
	bool updateAndCheckCompletion(const RobotState& state, const double dt) override;
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~RunMotorForDistance() = default;
};

// ================================================
// SPEED-BASED COMMANDS
// ================================================

// Command that moves at a specified speed until stopped by another command
class SpeedCommand : public Command
{
protected:
	SpeedCommand(uint32_t id) : Command(id) {}
public:
	// Moving at speed until stopeed by another command 
	bool isMoveCompleted() const override { return false; }
	bool updateAndCheckCompletion(const RobotState&, const double) override { return false; }
	virtual ~SpeedCommand() = default;
};

class MoveAtSpeedRaw : public SpeedCommand, public RawMoveCommand
{
public:
	MoveAtSpeedRaw(uint32_t id, MoveAtSpeedRawParams params, bool chassisFrameVelocity = false) :
		SpeedCommand(id),
		RawMoveCommand({ params.x_speed, params.y_speed }, params.rotation_speed, params.front_rotation_speed, chassisFrameVelocity) {
	}
	void execute(RobotState& state) override { RawMoveCommand::execute(state); }
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~MoveAtSpeedRaw() = default;
};

// Command that moves until a certain distance is traveled, then stops, but only counts distance traveled by the one motor 
class StartMotorCommand : public SpeedCommand, public RawMotorCommand
{
public:
	StartMotorCommand(uint32_t id, StartMotorParams params) : SpeedCommand(id), RawMotorCommand(params.motor_id, params.speed) {}
	void execute(RobotState& state) override { RawMotorCommand::execute(state); }
	void validate(int wheelCount) const override;
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~StartMotorCommand() = default;
};


class MultipleMotorCommand : public SpeedCommand {
private:
	std::vector<float> _motorSpeeds;
public:
	MultipleMotorCommand(uint32_t, std::vector<float> motorSpeeds) : SpeedCommand(0), _motorSpeeds(motorSpeeds) {}
	void execute(RobotState& state) override;
	void validate(int wheelCount) const override;
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~MultipleMotorCommand() = default;
};

// ================================================
// ANGLE-BASED COMMANDS
// ===============================================
class AngleCommand : public Command
{
private:
	double _targetAngle; // In radians
protected:
	AngleCommand(uint32_t id, float targetAngle) : Command(id), _targetAngle(targetAngle) {}
public:
	bool isMoveCompleted() const override { return _targetAngle < 0.001; }
	bool updateAndCheckCompletion(const RobotState& state, const double dt) override;
	virtual ~AngleCommand() = default;
};

class MoveByAngleRaw : public AngleCommand, public RawMoveCommand
{
public:
	MoveByAngleRaw(uint32_t id, MoveByAngleRawParams params, bool chassisFrameVelocity = false) :
		AngleCommand(id, std::abs(params.angle_rad)),
		RawMoveCommand({ params.x_speed, params.y_speed },
			params.angle_rad < 0 ? -std::abs(params.rotation_speed) : std::abs(params.rotation_speed),
			params.front_rotation_speed, chassisFrameVelocity) {
	}
public:
	void execute(RobotState& state) override { RawMoveCommand::execute(state); }
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~MoveByAngleRaw() = default;
};

// ================================================
// COMMAND FACTORY
// ================================================

class CommandFactory
{
	// Converts high-level move (with pivot point offset) to raw velocities.
	// Linear speed is adjusted for rotation around the off-center pivot.
	template<typename TRaw, typename TParams>
	static TRaw applyPivotTransform(const TParams& params) {
		TRaw raw{};
		raw.x_speed = params.x_speed - params.rotation_speed * params.center_y_mm;
		raw.y_speed = params.y_speed + params.rotation_speed * params.center_x_mm;
		raw.rotation_speed = params.rotation_speed;
		// if only the base rotates, counter-rotate front to keep it pointing the same way
		raw.front_rotation_speed = params.rotate_chassis ? 0.0f : -params.rotation_speed;
		return raw;
	}
public:
	static std::unique_ptr<MoveByDistanceRaw> createMoveByDistance(uint32_t id, const uint8_t* data, size_t size);
	static std::unique_ptr<MoveByTimeRaw> createMoveByTime(uint32_t id, const uint8_t* data, size_t size);
	static std::unique_ptr<MoveAtSpeedRaw> createMoveAtSpeed(uint32_t id, const uint8_t* data, size_t size);
	static std::unique_ptr<MoveByAngleRaw> createTurnRelative(uint32_t id, const uint8_t* data, size_t size);
};

// ================================================
// MOTOR COMMANDS WRAPPER
// ================================================

class MotorCommandWrapper : public Command
{
private:
	std::vector<std::unique_ptr<Command>> _motorCommands;
public:
	MotorCommandWrapper(int motorCount) : Command(0), _motorCommands(motorCount) {}
	bool isMoveCompleted() const override;
	bool updateAndCheckCompletion(const RobotState& state, const double dt) override;
	void addMotorCommand(uint16_t motor_id, std::unique_ptr<Command> cmd);
	void removeCommand(uint16_t motor_id) { if (motor_id < _motorCommands.size()) _motorCommands[motor_id] = nullptr; }
	void execute(RobotState& state) override;
	void checkInnerCommandCompletion(std::function<void(uint32_t)> onCompletion);
	~MotorCommandWrapper() = default;
};



#endif // !COMMANDS_HPP
