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
	matrix4x4<Real> inertia();
	matrix4x4<Real> inverseInertia();
	matrix4x4<Real> worldMat();
	matrix4x4<Real> transMat();
	matrix4x4<Real> scaleMat();
	matrix4x4<Real> rotMat();

	Vec3 pos; //Position of center of mass
	Vec3 vel; //Velocity of center of mass
	Vec3 scale;//h,d,w
	Quat rot;
	Real mass;

	Vec3 angVel; // angular velocity
	Vec3 angMom; // angular momentum

	Vec3 totalForce;
	Vec3 torque;
};

