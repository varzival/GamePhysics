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
};

class SphereSystem
{
public:
	SphereSystem(Vec3 color);
	~SphereSystem();

	Vec3 sphereColor;
	vector<Point> spheres;

};