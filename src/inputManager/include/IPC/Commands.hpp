#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <cstdint>
#include <vector>
#include <variant>
#include "Protocol.hpp"

struct MoveByDistanceRawParams
{
	float distance_mm;
	float x_speed;
	float y_speed;
	float rotation_speed;
	float front_rotation_speed;
};

struct MoveByTimeRawParams
{
	float time;
	float x_speed;
	float y_speed;
	float rotation_speed;
	float front_rotation_speed;
};

struct MoveAtSpeedRawParams
{
	float x_speed;
	float y_speed;
	float rotation_speed;
	float front_rotation_speed;
};

struct StopParams
{};

struct MoveAtSpeedMotorsParams
{
	std::vector<float> speeds;
};

struct RunMotorForTimeParams
{
	uint16_t motor_id;
	float speed;
	float time_s;
};

struct RunMotorForDistanceParams
{
	uint16_t motor_id;
	float speed;
	float distance_mm;
};

struct StopMotorParams
{
	uint16_t motor_id;
};

struct StartMotorParams
{
	uint16_t motor_id;
	float speed;
};

struct MoveByTimeParams
{
	float time_s;
	float x_speed;
	float y_speed;
	float rotation_speed;
	float center_x_mm;
	float center_y_mm;
	bool rotate_chassis;
};

struct MoveByDistanceParams
{
	float distance_mm;
	float x_speed;
	float y_speed;
	float rotation_speed;
	float center_x_mm;
	float center_y_mm;
	bool rotate_chassis;
};

struct MoveAtSpeedParams
{
	float x_speed;
	float y_speed;
	float rotation_speed;
	float center_x_mm;
	float center_y_mm;
	bool rotate_chassis;
};

using CommandParams = std::variant<
	MoveByDistanceRawParams,
	MoveByTimeRawParams,
	MoveAtSpeedRawParams,
	StopParams,
	MoveAtSpeedMotorsParams,
	RunMotorForTimeParams,
	RunMotorForDistanceParams,
	StopMotorParams,
	StartMotorParams,
	MoveByTimeParams,
	MoveByDistanceParams,
	MoveAtSpeedParams
>;


struct Command
{
	uint32_t id = -1;
	CommandParams params = {};
	static Command Create(const MsgHeader& header, const uint8_t* data, const size_t size);
};




#endif // !COMMANDS_HPP
