#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include <maths/math_utils.h>

enum OBJECT_TYPE
{
	PLAYER,
	ENEMY,
	BULLET,
	WALL
};

class GameObject : public gef::MeshInstance
{
public:
	void UpdateFromSimulation(const b2Body* body);
	void UpdateFromSimulation(const b2Body* body, float player);
	void MyCollisionResponse();

	inline void set_type(OBJECT_TYPE type) { type_ = type; }
	inline OBJECT_TYPE type() { return type_; }
private:
	OBJECT_TYPE type_;
};

class Player : public GameObject
{
public:
	Player();
	void DecrementHealth();
};

#endif // _GAME_OBJECT_H