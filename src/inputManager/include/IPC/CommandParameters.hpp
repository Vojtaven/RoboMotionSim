// Author: Vojtech Venzara
// Date: 2026-03-24
// Description: Parameter structures for various IPC commands with validation

#ifndef COMMAND_ARGUMENTS_HPP
#define COMMAND_ARGUMENTS_HPP

#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <cstring>

// Base class for constraining command parameters in templates
struct CommandParameters {
	virtual ~CommandParameters() = default;

	template<typename T>
	static T parseParams(const uint8_t* data, size_t size)
		requires requires { typename T::command_parameters_tag; }
	{
		if (size != sizeof(T))
			throw std::runtime_error("Invalid command payload: incorrect size for expected parameters");
		T params;
		std::memcpy(&params, data, sizeof(T));
		return params;
	}
};

#pragma pack(push,1)
struct MoveByDistanceRawParams 
{
	using command_parameters_tag = void;

	float distance_mm;
	float x_speed;
	float y_speed;
	float rotation_speed;
	float front_rotation_speed;
};

struct MoveByTimeRawParams 
{
	using command_parameters_tag = void;

	float time_s;
	float x_speed;
	float y_speed;
	float rotation_speed;
	float front_rotation_speed;
};

struct MoveAtSpeedRawParams 
{
	using command_parameters_tag = void;

	float x_speed;
	float y_speed;
	float rotation_speed;
	float front_rotation_speed;
};

struct StopParams  {
	using command_parameters_tag = void;
};

struct MoveByTimeParams 
{
	using command_parameters_tag = void;

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
	using command_parameters_tag = void;

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
	using command_parameters_tag = void;

	float x_speed;
	float y_speed;
	float rotation_speed;
	float center_x_mm;
	float center_y_mm;
	bool rotate_chassis;
};

struct MoveByAngleRawParams{
	using command_parameters_tag = void;

	float angle_rad;      // Relative change in heading
	float x_speed;
	float y_speed;
	float rotation_speed; // Speed of rotation
	float front_rotation_speed;
};

struct MoveByAngleParams
{
	using command_parameters_tag = void;
	float x_speed;
	float y_speed;
	float angle_rad;      // Relative change in heading
	float rotation_speed;
	float center_x_mm;    // Pivot point offset X
	float center_y_mm;    // Pivot point offset Y
	bool rotate_chassis;
};

struct RunMotorForTimeParams 
{
	using command_parameters_tag = void;

	uint16_t motor_id;
	float speed;
	float time_s;
};

struct RunMotorForDistanceParams 
{
	using command_parameters_tag = void;

	uint16_t motor_id;
	float speed;
	float distance_mm;
};

struct StopMotorParams 
{
	using command_parameters_tag = void;

	uint16_t motor_id;
};

struct StartMotorParams 
{
	using command_parameters_tag = void;

	uint16_t motor_id;
	float speed;
};
#pragma pack(pop)
#endif // !COMMAND_ARGUMENTS_HPP
