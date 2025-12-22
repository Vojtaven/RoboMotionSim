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

class PhysicsHandle
{
public:
	ObjectPosition position;
	ObjectSpeed speed;
	void updatePosition(float dt) {
		position.x += speed.x * dt;
		position.y += speed.y * dt;
		position.angle += speed.angular_velocity * dt;
	}

};

#endif // !PHYSICS_OBJECT_HPP
