#pragma once
#include "matrixbase.h"
#include "quaternion.h"
using namespace GamePhysics;

struct Force {
	Vec3 forcePos;
	Vec3 forceDir;
};

class rigidBody
{
public:
	rigidBody();
	~rigidBody();
	matrix4x4<float> inertia();

	Vec3 pos; //Position of center of mass
	Vec3 vel; //Velocity of center of mass
	Vec3 scale;//h,d,w
	Quaternion<float> rot;
	float mass;
	matrix4x4<float> inertia();
	matrix4x4<float> master;

	Vec3 angVel; // angular velocity
	Vec3 angMom;

	std::vector<Force> forces;
};

