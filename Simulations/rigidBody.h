#pragma once
#include "matrixbase.h"
#include "quaternion.h"
#include <vector>
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
	matrix4x4<float> inverseInertia();
	matrix4x4<float> master();
	matrix4x4<float> transMat();
	matrix4x4<float> scaleMat();
	matrix4x4<float> rotMat();

	Vec3 pos; //Position of center of mass
	Vec3 vel; //Velocity of center of mass
	Vec3 scale;//h,d,w
	Quaternion<float> rot;
	float mass;

	Vec3 angVel; // angular velocity
	Vec3 angMom; // angular momentum

	std::vector<Force> forces;
};

