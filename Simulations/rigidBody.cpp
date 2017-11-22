#include "rigidBody.h"



rigidBody::rigidBody()
{
}


rigidBody::~rigidBody()
{
}

matrix4x4<float> rigidBody::inertia()
{
	return matrix4x4<float>(
		1 / 12 * mass*(pow(scale.x, 2) + pow(scale.y, 2)), 0, 0, 0,
		0, 1 / 12 * mass*(pow(scale.z, 2) + pow(scale.y, 2)), 0, 0,
		0, 0, 1 / 12 * mass*(pow(scale.z, 2) + pow(scale.x, 2)), 0,
		0, 0, 0, 1);

}
