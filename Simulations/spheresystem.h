#pragma once
#include "util/vectorbase.h"
#include <vector>

using namespace GamePhysics;
using namespace std;

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

	Vec3 sphereColor;
	vector<Point> spheres;

};