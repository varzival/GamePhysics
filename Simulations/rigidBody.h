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
	matrix4x4<float> transMat();
	Vec3 scale;//h,d,w
	matrix4x4<float> scaleMat();
	Quaternion<float> rot;
	matrix4x4<float> rotMat();
	float mass;
	matrix4x4<float> inertia();
	matrix4x4<float> master();

	//TODO
	//liste aus kräften mit richtungsvektor und ortsvektor wo sie wirkt


};

