#pragma once
#include "matrixbase.h"
#include "quaternion.h"
using namespace GamePhysics;
class rigidBody
{
public:
	rigidBody();
	~rigidBody();

	Vec3 pos;
	Vec3 scale;//h,d,w
	Quaternion rot;
	float mass;
	matrix4x4 inertia();
	matrix4x4 master;


};

