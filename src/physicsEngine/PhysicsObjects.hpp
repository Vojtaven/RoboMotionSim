#ifndef PHYSICS_OBJECT_HPP
#define PHYSICS_OBJECT_HPP

struct  ObjectSpeed
{
	float x =0;
	float y = 0;
	float z = 0;
	float angular_velocity = 0;
};

struct ObjectPosition
{
	float x = 0;
	float y = 0;
	float angle = 0;
};

struct PhysicsHandle
{
	ObjectPosition position;
	ObjectSpeed speed;
};

#endif // !PHYSICS_OBJECT_HPP
