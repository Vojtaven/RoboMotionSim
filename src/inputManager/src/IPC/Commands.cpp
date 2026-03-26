#include "IPC/Commands.hpp"
#include "IPC/CommandParameters.hpp"
#include <unordered_map>
#include <functional>
#include <memory>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string>
//Priority commads
// Stop
// Stop motor

static const std::unordered_map<CommandType, std::function<std::unique_ptr<Command>(uint32_t id, const uint8_t*, size_t)>> parsers = {
	{ CommandType::MOVE_BY_DISTANCE_RAW, MoveByDistanceRaw::create},
	{ CommandType::MOVE_BY_TIME_RAW, MoveByTimeRaw::create },
	{ CommandType::MOVE_AT_SPEED_RAW, MoveAtSpeedRaw::create },
	{ CommandType::MOVE_BY_ANGLE_RAW, MoveByAngleRaw::create},
	{ CommandType::RUN_MOTOR_FOR_TIME, RunMotorForTime::create },
	{ CommandType::RUN_MOTOR_FOR_DISTANCE, RunMotorForDistance::create},
	{ CommandType::MOVE_AT_SPEED_MOTORS, MultipleMotorCommand::create},
	{ CommandType::START_MOTOR, StartMotorCommand::create },
	{ CommandType::MOVE_BY_TIME, CommandFactory::createMoveByTime },
	{ CommandType::MOVE_BY_DISTANCE, CommandFactory::createMoveByDistance },
	{ CommandType::MOVE_AT_SPEED, CommandFactory::createMoveAtSpeed},
	{ CommandType::MOVE_BY_ANGLE, CommandFactory::createTurnRelative}
};


CommandType Command::getCommandType(const uint8_t* data, size_t size)
{
	if (size < sizeof(CommandType))
		throw std::exception("Invalid command payload: too small for CommandType");
	CommandType type;
	std::memcpy(&type, data, sizeof(CommandType));
	data += sizeof(CommandType);
	size -= sizeof(CommandType);
	return type;
}

std::unique_ptr<Command> Command::create(uint32_t id, CommandType type, const uint8_t* data, size_t size) {
	auto it = parsers.find(type);
	if (it == parsers.end())
		throw std::exception("Invalid command payload: unknown CommandType");

	return it->second(id, data, size);
}

// ================================================
// RAW COMMANDS
// ================================================

RawMoveCommand::RawMoveCommand(Vec2f speed, float rotationSpeed, float frontRotationSpeed) :
	_speed(speed),
	_rotationSpeed(rotationSpeed),
	_frontRotationSpeed(frontRotationSpeed) {
}
void RawMoveCommand::execute(RobotState& state) {
	state.localVelocity = { static_cast<double>(_speed.x), static_cast<double>(_speed.y) };
	state.angularVelocity = _rotationSpeed;
	state.frontAngularVelocity = _frontRotationSpeed;
}

static void validateMotorId(uint16_t motor_id, int wheelCount) {
	if (motor_id >= wheelCount)
		throw std::runtime_error("Motor ID " + std::to_string(motor_id) + " out of range (wheel count " + std::to_string(wheelCount) + ")");
}

RawMotorCommand::RawMotorCommand(uint16_t motor_id, float speed) :
	motor_id(motor_id),
	speed(speed) {
}
void RawMotorCommand::execute(RobotState& state) {
	state.wheels[motor_id].speed = speed;
	state.wheels[motor_id].powered = true;
}

// ================================================
// TIME-BASED COMMANDS
// ================================================
bool TimeCommand::updateAndCheckCompletion(const RobotState& state, const double dt) {
	_timeRemaining -= dt;
	return isMoveCompleted();
}

std::unique_ptr<Command> MoveByTimeRaw::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<MoveByTimeRaw>(id, CommandParameters::parseParams<MoveByTimeRawParams>(data, size));
}

std::unique_ptr<Command> RunMotorForTime::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<RunMotorForTime>(id, CommandParameters::parseParams<RunMotorForTimeParams>(data, size));
}
void RunMotorForTime::validate(int wheelCount) const { validateMotorId(motor_id, wheelCount); }

// ================================================
// DISTANCE-BASED COMMANDS
// ================================================
bool DistanceCommand::updateAndCheckCompletion(const RobotState& state, const double dt) {
	_distanceRemaining -= state.lastDistanceDisplacement.length();
	return isMoveCompleted();
}


std::unique_ptr<Command> MoveByDistanceRaw::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<MoveByDistanceRaw>(id, CommandParameters::parseParams<MoveByDistanceRawParams>(data, size));
}

std::unique_ptr<Command> RunMotorForDistance::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<RunMotorForDistance>(id, CommandParameters::parseParams<RunMotorForDistanceParams>(data, size));
}

void RunMotorForDistance::validate(int wheelCount) const { validateMotorId(motor_id, wheelCount); }
bool RunMotorForDistance::updateAndCheckCompletion(const RobotState& state, const double dt) {
	_distanceRemaining -= state.wheels[motor_id].lastDistanceDisplacement;
	return isMoveCompleted();
}


// ================================================
// SPEED-BASED COMMANDS
// ================================================
std::unique_ptr<Command> MoveAtSpeedRaw::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<MoveAtSpeedRaw>(id, CommandParameters::parseParams<MoveAtSpeedRawParams>(data, size));
}

std::unique_ptr<Command> StartMotorCommand::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<StartMotorCommand>(id, CommandParameters::parseParams<StartMotorParams>(data, size));
}
void StartMotorCommand::validate(int wheelCount) const { validateMotorId(motor_id, wheelCount); }

void MultipleMotorCommand::execute(RobotState& state) {
	for (size_t i = 0; i < _motorSpeeds.size(); i++) {
		state.wheels[i].speed = _motorSpeeds[i];
		state.wheels[i].powered = true;
	}
	state.fromWheelSpeeds = true;
}

std::unique_ptr<Command> MultipleMotorCommand::create(uint32_t id, const uint8_t* data, size_t size) {
	if (size < sizeof(uint16_t))
		throw std::exception("Invalid command payload: too small for motor count");
	uint16_t motorCount;
	std::memcpy(&motorCount, data, sizeof(uint16_t));
	data += sizeof(uint16_t);
	size -= sizeof(uint16_t);
	if (size != motorCount * sizeof(float))
		throw std::exception("Invalid command payload: size does not match motor count for MoveAtSpeedMotorsParams");
	std::vector<float> speeds(motorCount);
	std::memcpy(speeds.data(), data, motorCount * sizeof(float));

	return std::make_unique<MultipleMotorCommand>(id, speeds);
}
void MultipleMotorCommand::validate(int wheelCount) const {
	if (static_cast<int>(_motorSpeeds.size()) > wheelCount)
		throw std::runtime_error("Motor count " + std::to_string(_motorSpeeds.size()) + " exceeds wheel count " + std::to_string(wheelCount));
}


// ================================================
// ANGLE-BASED COMMANDS
// ===============================================

std::unique_ptr<Command>  MoveByAngleRaw::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<MoveByAngleRaw>(id, CommandParameters::parseParams<MoveByAngleRawParams>(data, size));
}

bool AngleCommand::updateAndCheckCompletion(const RobotState& state, const double dt) {
	_targetAngle -= state.lastFrontDisplacement + state.lastChassisDisplacement;
	return this->isMoveCompleted();
}

// ================================================
// MOTOR COMMANDS WRAPPER
// ================================================


bool MotorCommandWrapper::isMoveCompleted() const {
	return std::all_of(_motorCommands.begin(), _motorCommands.end(), [](const std::unique_ptr<Command>& cmd) {
		return !cmd || cmd->isMoveCompleted();
		});
}

bool MotorCommandWrapper::updateAndCheckCompletion(const RobotState& state, const double dt) {
	bool allCompleted = false;
	for (auto& cmd : _motorCommands) {
		allCompleted |= !cmd || cmd->updateAndCheckCompletion(state, dt);
	}
	return allCompleted;
}
void MotorCommandWrapper::addMotorCommand(uint16_t motor_id, std::unique_ptr<Command> cmd) {
	_motorCommands[motor_id] = std::move(cmd);
}
void MotorCommandWrapper::checkInnerCommandCompletion(std::function<void(uint32_t)> onCompletion) {
	for (auto& cmd : _motorCommands) {
		if (cmd && cmd->isMoveCompleted()) {
			onCompletion(cmd->getId());
			cmd = nullptr; // Clear completed command
		}
	}

}

void MotorCommandWrapper::execute(RobotState& state) {
	state.fromWheelSpeeds = true;
	for (int i = 0; i < state.wheelCount; i++)
	{
		if (_motorCommands[i]) {
			_motorCommands[i]->execute(state);
		}
		else {
			state.wheels[i].powered = false; 
			state.wheels[i].speed = 0;
		}
	}
}


// ================================================
// COMMAND FACTORY
// ================================================

std::unique_ptr<MoveByDistanceRaw> CommandFactory::createMoveByDistance(uint32_t id, const uint8_t* data, size_t size) {
	auto params = CommandParameters::parseParams<MoveByDistanceParams>(data, size);
	auto raw = applyPivotTransform<MoveByDistanceRawParams>(params);
	raw.distance_mm = params.distance_mm;
	return std::make_unique<MoveByDistanceRaw>(id, raw);
}
std::unique_ptr<MoveByTimeRaw> CommandFactory::createMoveByTime(uint32_t id, const uint8_t* data, size_t size) {
	auto params = CommandParameters::parseParams<MoveByTimeParams>(data, size);
	auto raw = applyPivotTransform<MoveByTimeRawParams>(params);
	raw.time_s = params.time_s;
	return std::make_unique<MoveByTimeRaw>(id, raw);
}
std::unique_ptr<MoveAtSpeedRaw> CommandFactory::createMoveAtSpeed(uint32_t id, const uint8_t* data, size_t size) {
	auto params = CommandParameters::parseParams<MoveAtSpeedParams>(data, size);
	return std::make_unique<MoveAtSpeedRaw>(id, applyPivotTransform<MoveAtSpeedRawParams>(params));
}
std::unique_ptr<MoveByAngleRaw> CommandFactory::createTurnRelative(uint32_t id, const uint8_t* data, size_t size) {
	auto params = CommandParameters::parseParams<MoveByAngleParams>(data, size);
	auto raw = applyPivotTransform<MoveByAngleRawParams>(params);
	raw.angle_rad = params.angle_rad;
	return std::make_unique<MoveByAngleRaw>(id, raw);
}
