#include "IPC/Commands.hpp"
#include "IPC/CommandParameters.hpp"
#include <unordered_map>
#include <functional>
#include <memory>
#include <algorithm>
#include <exception>

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

std::unique_ptr<Command> Command::Create(uint32_t id, CommandType type, const uint8_t* data, size_t size) {
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
	state.localVelocity = _speed;
	state.angularVelocity = _rotationSpeed;
	state.frontAngularVelocity = _frontRotationSpeed;
}

RawMotorCommand::RawMotorCommand(uint16_t motor_id, float speed) :
	motor_id(motor_id),
	speed(speed) {
}
void RawMotorCommand::execute(RobotState& state) {
	state.wheels[motor_id].speed = speed;
}

// ================================================
// TIME-BASED COMMANDS
// ================================================
bool TimeCommand::updateAndCheckCompletion(const RobotState& state, const float dt) {
	timeRemaining -= dt;
	return isMoveCompleted();
}

std::unique_ptr<Command> MoveByTimeRaw::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<MoveByTimeRaw>(id, CommandParameters::ParseParams<MoveByTimeRawParams>(data, size));
}

std::unique_ptr<Command> RunMotorForTime::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<RunMotorForTime>(id, CommandParameters::ParseParams<RunMotorForTimeParams>(data, size));
}

// ================================================
// DISTANCE-BASED COMMANDS
// ================================================
bool DistanceCommand::updateAndCheckCompletion(const RobotState& state, const float dt) {
	distanceRemaining -= state.lastDistanceDisplacement.length();
	return isMoveCompleted();
}


std::unique_ptr<Command> MoveByDistanceRaw::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<MoveByDistanceRaw>(id, CommandParameters::ParseParams<MoveByDistanceRawParams>(data, size));
}

std::unique_ptr<Command> RunMotorForDistance::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<RunMotorForDistance>(id, CommandParameters::ParseParams<RunMotorForDistanceParams>(data, size));
}

bool RunMotorForDistance::updateAndCheckCompletion(const RobotState& state, const float dt) {
	distanceRemaining -= state.wheels[motor_id].lastDistanceDisplacement;
	return isMoveCompleted();
}


// ================================================
// SPEED-BASED COMMANDS
// ================================================
std::unique_ptr<Command> MoveAtSpeedRaw::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<MoveAtSpeedRaw>(id, CommandParameters::ParseParams<MoveAtSpeedRawParams>(data, size));
}

std::unique_ptr<Command> StartMotorCommand::create(uint32_t id, const uint8_t* data, size_t size) {
	return std::make_unique<StartMotorCommand>(id, CommandParameters::ParseParams<StartMotorParams>(data, size));
}

void MultipleMotorCommand::execute(RobotState& state) {
	for (size_t i = 0; i < _motorSpeeds.size(); i++) {
		state.wheels[i].speed = _motorSpeeds[i];
	}
}

std::unique_ptr<Command> MultipleMotorCommand::create(uint32_t id, const uint8_t* data, size_t size) {
	if (size < sizeof(uint16_t))
		throw std::exception("Invalid command payload: too small for motor count");
	uint16_t motor_count;
	std::memcpy(&motor_count, data, sizeof(uint16_t));
	data += sizeof(uint16_t);
	size -= sizeof(uint16_t);
	if (size != motor_count * sizeof(float))
		throw std::exception("Invalid command payload: size does not match motor count for MoveAtSpeedMotorsParams");
	std::vector<float> speeds(motor_count);
	std::memcpy(speeds.data(), data, motor_count * sizeof(float));

	return std::make_unique<MultipleMotorCommand>(id, speeds);
}


// ================================================
// ANGLE-BASED COMMANDS
// ===============================================

bool AngleCommand::updateAndCheckCompletion(const RobotState& state, const float dt) {
	targetAngle -= state.lastFrontDisplacement;
	return isMoveCompleted();
}

// ================================================
// MOTOR COMMANDS WRAPPER
// ================================================


bool MotorCommandWrapper::isMoveCompleted() const {
	return std::all_of(motorCommands.begin(), motorCommands.end(), [](const std::unique_ptr<Command>& cmd) {
		return !cmd || cmd->isMoveCompleted();
		});

}
bool MotorCommandWrapper::updateAndCheckCompletion(const RobotState& state, const float dt) {
	bool allCompleted = true;
	for (auto& cmd : motorCommands) {
		if (cmd && !cmd->updateAndCheckCompletion(state, dt)) {
			allCompleted = false;
		}
	}
	return allCompleted;
}
void MotorCommandWrapper::addMotorCommand(uint16_t motor_id, std::unique_ptr<Command> cmd) {
	motorCommands[motor_id] = std::move(cmd);
}
uint32_t MotorCommandWrapper::getCompletedId() const {
	for (const auto& cmd : motorCommands) {
		if (cmd && cmd->isMoveCompleted()) {
			return cmd->getId();
		}
	}
	return 0;
}

void MotorCommandWrapper::execute(RobotState& state) {
	state.fromWheelSpeeds = true;
	for (const auto& cmd : motorCommands) {
		if (cmd) {
			cmd->execute(state);
		}
	}
}


