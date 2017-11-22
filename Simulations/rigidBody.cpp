#include "rigidBody.h"



rigidBody::rigidBody()
{
}


rigidBody::~rigidBody()
{
}

matrix4x4<float> rigidBody::transMat()
{
	return matrix4x4<float>(
		1, 0, 0, pos.x,
		0, 1, 0, pos.y,
		0, 0, 1, pos.z,
		0, 0, 0, 1);
}

matrix4x4<float> rigidBody::scaleMat()
{
	return matrix4x4<float>(
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1);
}

matrix4x4<float> rigidBody::rotMat()
{
	return rot.getRotMat();
}

matrix4x4<float> rigidBody::inertia()
{
	return matrix4x4<float>(
		1.0f / 12.0f * mass*(pow(scale.x, 2) + pow(scale.y, 2)), 0, 0, 0,
		0, 1.0f / 12.0f * mass*(pow(scale.z, 2) + pow(scale.y, 2)), 0, 0,
		0, 0, 1.0f / 12.0f * mass*(pow(scale.z, 2) + pow(scale.x, 2)), 0,
		0, 0, 0, 1);

}

matrix4x4<float> rigidBody::master()
{
	return scaleMat()*rotMat()*transMat();
}
