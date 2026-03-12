#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
#include <cstdint>

enum class MsgType : uint8_t
{
	HANDSHAKE = 0x01,
	HANDSHAKE_ACK = 0x02,
	HEARTBEAT = 0x03,
	HEARTBEAT_ACK = 0x04,
	MOTOR_COUNT = 0x05, // Sent by client after handshake, parameters: motor_count

	TELEMETRY = 0x06,
	COMMAND = 0x07,

	CMD_ACK = 0x08,
	CMD_ERROR = 0x09,
	CMD_COMPLETE = 0x0A,

	CLEAR_COMMAND_QUEUE = 0x0B, // Clear all commands in the queue
	DISCONNECT_ACK = 0xFE,
	DISCONNECT = 0xFF
};

// ---- Command type - what TO DO (only relevant when MsgType == COMMAND) ----
enum class CommandType : uint16_t
{
	MOVE_BY_DISTANCE_RAW = 0x01, // Parameters: distance_mm, x_speed, y_speed, rotation_speed, front_rotation_speed
	MOVE_BY_TIME_RAW = 0x02, // Parameters: time, x_speed, y_speed, rotation_speed, front_rotation_speed
	MOVE_AT_SPEED_RAW = 0x03, // Parameters: x_speed, y_speed, rotation_speed, front_rotation_speed
	STOP = 0x04, // Skips queue and stops all motors immediately
	MOVE_AT_SPEED_MOTORS = 0x05, // Parameters: motor_count. foreach motor: speed 
	RUN_MOTOR_FOR_TIME = 0x06,  // Parameters: motor_id, speed, time_s
	RUN_MOTOR_FOR_DISTANCE = 0x07, // Parameters: motor_id, speed, distance_mm
	STOP_MOTOR = 0x08, // Parameters: motor_id
	START_MOTOR = 0x09, // Parameters: motor_id, speed
	MOVE_BY_TIME = 0x0A, // Parameters: time_s, x_speed, y_speed, rotation_speed, center_x_mm, center_y_mm, rotate_chassis
	MOVE_BY_DISTANCE = 0x0B, // Parameters: distance_mm, x_speed, y_speed, rotation_speed, center_x_mm, center_y_mm, rotate_chassis
	MOVE_AT_SPEED = 0x0C,	// Parameters: x_speed, y_speed, rotation_speed, center_x_mm, center_y_mm, rotate_chassis
	MOVE_BY_ANGLE = 0x0D, // Parameters: angle_rad, rotation_speed, center_x_mm, center_y_mm, rotate_chassis
	MOVE_BY_ANGLE_RAW = 0x0E, // Parameters: angle_rad, x_speed, y_speed, rotation_speed, front_rotation_speed
};


#pragma pack(push, 1)
struct MsgHeader
{
	uint32_t id;            // message id must start at 1 and increment for each new message sent by the client, used for ACKs and matching responses to requests
	uint16_t payload_size;  // bytes following header
	MsgType  type;          // envelope
};

struct TelemetryOdometry
{
	uint64_t timestamp_ms;
	float distanceTraveledX;
	float distanceTraveledY;
	float localVelocityX;
	float localVelocityY;
	float frontAngle;
	float chassisAngle;
	float chassisAngularVelocity;
	uint8_t wheelCount;         // how many TelemetryWheelState follow
};

struct TelemetryWheelState
{
	float speed;
	float distanceTraveled;
};
#pragma pack(pop)


#endif // !PROTOCOL_HPP
