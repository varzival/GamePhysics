#include "spheresystem.h"

SphereSystem::SphereSystem(Vec3 color)
{
	this->sphereColor = color;
	this->spheres = vector<Point>();
}

SphereSystem::~SphereSystem()
{
	spheres.clear();
}
