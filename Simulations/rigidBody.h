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
	Quaternion<float> rot;
	float mass;
	matrix4x4<float> inertia();
	matrix4x4<float> master;


};

