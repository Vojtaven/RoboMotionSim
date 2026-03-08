#include "IPC/Commands.hpp"
#include <unordered_map>
#include <functional>

template<typename T>
CommandParams ParseParams(const uint8_t* data, size_t size) {
    if (size != sizeof(T))
        throw std::exception("Invalid command payload: incorrect size for expected parameters");
    T params;
    std::memcpy(&params, data, sizeof(T));
    return params;
}

static const std::unordered_map<CommandType, std::function<CommandParams(const uint8_t*, size_t)>> parsers = {
    { CommandType::MOVE_BY_DISTANCE_RAW, ParseParams<MoveByDistanceRawParams> },
    { CommandType::MOVE_BY_TIME_RAW, ParseParams<MoveByTimeRawParams> },
    { CommandType::MOVE_AT_SPEED_RAW, ParseParams<MoveAtSpeedRawParams> },
    { CommandType::STOP, ParseParams<StopParams> },
    { CommandType::RUN_MOTOR_FOR_TIME, ParseParams<RunMotorForTimeParams> },
    { CommandType::RUN_MOTOR_FOR_DISTANCE, ParseParams<RunMotorForDistanceParams> },
    { CommandType::STOP_MOTOR, ParseParams<StopMotorParams> },
    { CommandType::START_MOTOR, ParseParams<StartMotorParams> },
    { CommandType::MOVE_BY_TIME, ParseParams<MoveByTimeParams> },
    { CommandType::MOVE_BY_DISTANCE, ParseParams<MoveByDistanceParams> },
    { CommandType::MOVE_AT_SPEED, ParseParams<MoveAtSpeedParams> }
};

Command Command::Create(const MsgHeader& header, const uint8_t* data, size_t size) {
    Command command;
    command.id = header.id;
    if (size < sizeof(CommandType))
        throw std::exception("Invalid command payload: too small for CommandType");

    CommandType type;
    std::memcpy(&type, data, sizeof(CommandType));
    data += sizeof(CommandType);
    size -= sizeof(CommandType);

    if (type == CommandType::MOVE_AT_SPEED_MOTORS) {
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
        command.params = MoveAtSpeedMotorsParams{ speeds };
    } else {
        auto it = parsers.find(type);
        if (it == parsers.end())
            throw std::exception("Invalid command payload: unknown CommandType");
        command.params = it->second(data, size);
    }
    return command;
}
