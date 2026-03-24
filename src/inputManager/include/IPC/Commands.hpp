#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <cstdint>
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

	virtual bool updateAndCheckCompletion(const RobotState& state, const float dt) = 0;
	virtual void execute(RobotState& state) = 0;
	virtual bool isMoveCompleted() const = 0;
	// Returns the ID of the command that completed, or 0 if not completed yet

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
public:
	RawMoveCommand(Vec2f speed, float rotationSpeed, float frontRotationSpeed);
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
	float _timeRemaining; // seconds
	TimeCommand(uint32_t id, float time) : Command(id), _timeRemaining(time) {}
public:

	bool isMoveCompleted() const override { return _timeRemaining <= 0.0f; }
	bool updateAndCheckCompletion(const RobotState& state, const float dt) override;
	virtual ~TimeCommand() = default;
};

class MoveByTimeRaw : public TimeCommand, public RawMoveCommand
{
public:
	MoveByTimeRaw(uint32_t id, MoveByTimeRawParams params) :
		TimeCommand(id, params.time_s),
		RawMoveCommand({ params.x_speed, params.y_speed }, params.rotation_speed, params.front_rotation_speed) {
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
	float _distanceRemaining; // mm
	DistanceCommand(uint32_t id, float distance) : Command(id), _distanceRemaining(distance) {}
public:
	bool isMoveCompleted() const override { return _distanceRemaining <= 0.0f; }
	bool updateAndCheckCompletion(const RobotState& state, const float dt) override;
	virtual ~DistanceCommand() = default;
};


class MoveByDistanceRaw : public DistanceCommand, public RawMoveCommand
{
public:
	MoveByDistanceRaw(uint32_t id, MoveByDistanceRawParams params) :
		DistanceCommand(id, params.distance_mm),
		RawMoveCommand({ params.x_speed, params.y_speed }, params.rotation_speed, params.front_rotation_speed) {
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
	bool updateAndCheckCompletion(const RobotState& state, const float dt) override;
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
	bool updateAndCheckCompletion(const RobotState&, const float) override { return false; }
	virtual ~SpeedCommand() = default;
};

class MoveAtSpeedRaw : public SpeedCommand, public RawMoveCommand
{
public:
	MoveAtSpeedRaw(uint32_t id, MoveAtSpeedRawParams params) :
		SpeedCommand(id),
		RawMoveCommand({ params.x_speed, params.y_speed }, params.rotation_speed, params.front_rotation_speed) {
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
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~StartMotorCommand() = default;
};


class MultipleMotorCommand : public SpeedCommand {
private:
	std::vector<float> _motorSpeeds;
public:
	MultipleMotorCommand(uint32_t, std::vector<float> motorSpeeds) : SpeedCommand(0), _motorSpeeds(motorSpeeds) {}
	void execute(RobotState& state) override;
	static std::unique_ptr<Command> create(uint32_t id, const uint8_t* data, size_t size);
	~MultipleMotorCommand() = default;
};

// ================================================
// ANGLE-BASED COMMANDS
// ===============================================
class AngleCommand : public Command
{
private:
	float _targetAngle; // In radians
protected:
	AngleCommand(uint32_t id, float targetAngle) : Command(id), _targetAngle(targetAngle) {}
public:
	bool isMoveCompleted() const override { return _targetAngle < 0.001f; }
	bool updateAndCheckCompletion(const RobotState& state, const float dt) override;
	virtual ~AngleCommand() = default;
};

class MoveByAngleRaw : public AngleCommand, public RawMoveCommand
{
public:
	MoveByAngleRaw(uint32_t id, MoveByAngleRawParams params) :
		AngleCommand(id, params.angle_rad),
		RawMoveCommand({ params.x_speed, params.y_speed }, params.rotation_speed, params.front_rotation_speed) {
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
	// Applies pivot-point transformation common to all high-level move commands
	template<typename TRaw, typename TParams>
	static TRaw applyPivotTransform(const TParams& params) {
		TRaw raw{};
		raw.x_speed = params.x_speed - params.rotation_speed * params.center_y_mm;
		raw.y_speed = params.y_speed + params.rotation_speed * params.center_x_mm;
		raw.rotation_speed = params.rotation_speed;
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
	bool updateAndCheckCompletion(const RobotState& state, const float dt) override;
	void addMotorCommand(uint16_t motor_id, std::unique_ptr<Command> cmd);
	void removeCommand(uint16_t motor_id) { if (motor_id < _motorCommands.size()) _motorCommands[motor_id] = nullptr; }
	void execute(RobotState& state) override;
	void checkInnerCommandCompletion(std::function<void(uint32_t)> onCompletion);
	~MotorCommandWrapper() = default;
};



#endif // !COMMANDS_HPP
