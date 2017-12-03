#include "rigidBody.h"
#include "vectorbase.h"

rigidBody::rigidBody()
{
	pos = Vec3(0.0f, 0.0f, 0.0f);
	vel = Vec3(0.0f, 0.0f, 0.0f);
	angVel = Vec3(0.0f, 0.0f, 0.0f);
	angMom = Vec3(0.0f, 0.0f, 0.0f);
	scale = Vec3(1.0f, 1.0f, 1.0f);
	mass = 1.0f;
	rot = Quat(0.0f, 0.0f, 0.0f, 1.0f); //no rotation
}


rigidBody::~rigidBody()
{
}

matrix4x4<Real> rigidBody::transMat()
{
	matrix4x4<Real> m = matrix4x4<Real>(
		1, 0, 0, pos.x,
		0, 1, 0, pos.y,
		0, 0, 1, pos.z,
		0, 0, 0, 1);
	m.transpose();
	return m;
}

matrix4x4<Real> rigidBody::scaleMat()
{
	matrix4x4<Real> m = matrix4x4<Real>(
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1);
	m.transpose();
	return m;
}

matrix4x4<Real> rigidBody::rotMat()
{
	return rot.getRotMat();
}

Vec3 rigidBody::pointVelocity(Vec3 point)
{
	Vec3 midToPoint = point - pos;
	return vel + cross(angVel, midToPoint);
}

matrix4x4<Real> rigidBody::inertia()
{
	matrix4x4<Real> m = matrix4x4<Real>(
		1.0f / 12.0f * mass*(pow(scale.x, 2) + pow(scale.y, 2)), 0, 0, 0,
		0, 1.0f / 12.0f * mass*(pow(scale.z, 2) + pow(scale.y, 2)), 0, 0,
		0, 0, 1.0f / 12.0f * mass*(pow(scale.z, 2) + pow(scale.x, 2)), 0,
		0, 0, 0, 1);
	m.transpose();
	return m;

}

matrix4x4<Real> rigidBody::inverseInertia()
{
	matrix4x4<Real> m = matrix4x4<Real>(
		1.0f / (1.0f / 12.0f * mass*(pow(scale.x, 2) + pow(scale.y, 2))), 0, 0, 0,
		0, 1.0f / (1.0f / 12.0f * mass*(pow(scale.z, 2) + pow(scale.y, 2))), 0, 0,
		0, 0, 1.0f / (1.0f / 12.0f * mass*(pow(scale.z, 2) + pow(scale.x, 2))), 0,
		0, 0, 0, 1.0f);
	m.transpose();
	return m;
}

matrix4x4<Real> rigidBody::worldMat()
{
	return scaleMat()*rotMat()*transMat();
}
