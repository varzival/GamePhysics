#pragma once

#include "vectorbase.h"
#include <vector>

//Rendering
#define SPHERESIZE Vec3(0.05f, 0.05f, 0.05f)

struct Point
{
	Vec3 position;
	Vec3 velocity;
	Vec3 force;
	float mass;
};

class SphereSystem
{
public:
	SphereSystem(Vec3 color);

	//float * m_fRadius;
	//float * m_fMass;
	Vec3 sphereColor;
	vector<Point> spheres;

};