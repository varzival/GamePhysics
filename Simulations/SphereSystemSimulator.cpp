﻿#include "SphereSystemSimulator.h"
#include <ctime>

#define GRIDCOLOR Vec3(1.0f, 1.0f, 0.0f)
std::function<float(float)> SphereSystemSimulator::m_Kernels[5] = {
	[](float x) {return 1.0f; },              // Constant, m_iKernel = 0
	[](float x) {return 1.0f - x; },          // Linear, m_iKernel = 1, as given in the exercise Sheet, x = d/2r
	[](float x) {return (1.0f - x)*(1.0f - x); }, // Quadratic, m_iKernel = 2
	[](float x) {return 1.0f / (x)-1.0f; },     // Weak Electric Charge, m_iKernel = 3
	[](float x) {return 1.0f / (x*x) - 1.0f; },   // Electric Charge, m_iKernel = 4
};

// SphereSystemSimulator member functions

SphereSystemSimulator::SphereSystemSimulator()
{
	m_simulateGridSystem = false;
	m_iNumSpheres = 20;
	m_ioldNumSpheres = m_iNumSpheres;
	m_fMass = 1.0f;
	m_fRadius = 0.1f;
	m_fDamping = 0.0f;
	m_fForceScaling = 900.0f;
	m_iTestCase = 0;
	m_iKernel = 1;

	m_pSphereSystem = new SphereSystem(Vec3(0.0f, 1.0f, 0.0f));
	m_pSphereSystemGrid = new SphereSystem(Vec3(1.0f, 1.0f, 0.0f));
	reset();
}

SphereSystemSimulator::~SphereSystemSimulator()
{
	free(m_pSphereSystem);
	free(m_pSphereSystemGrid);
}

const char * SphereSystemSimulator::getTestCasesStr()
{
	return "Demo1, Demo2, Demo3";
}

void SphereSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
	TwAddVarRW(DUC->g_pTweakBar, "Sphere Number", TW_TYPE_INT32, &m_iNumSpheres, "min=1");
	TwAddVarRW(DUC->g_pTweakBar, "Sphere Radius", TW_TYPE_FLOAT, &m_fRadius, "step=0.01 min=0.01");
	TwAddVarRW(DUC->g_pTweakBar, "Sphere Mass", TW_TYPE_FLOAT, &m_fMass, "step=1.0 min=0.1");
	TwAddVarRW(DUC->g_pTweakBar, "Gravity", TW_TYPE_BOOLCPP, &m_gravityOn, "");
	TwType TW_TYPE_KERNEL = TwDefineEnumFromString("Collision Force Kernel", "Constant, Linear, Quadratic");
	TwAddVarRW(DUC->g_pTweakBar, "Collision Force Kernel", TW_TYPE_KERNEL, &m_iKernel, "");
	TwType TW_TYPE_COLLISION = TwDefineEnumFromString("Collision Detection Mode", "Naive, UniformGrid");
	TwAddVarRW(DUC->g_pTweakBar, "Collision Detection Mode", TW_TYPE_COLLISION, &m_iCollision, "");
}

void SphereSystemSimulator::reset()
{
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
	gridSize = 1 / (2 * m_fRadius);
	grid = std::vector<std::vector<std::vector<std::vector<std::vector<Point>::iterator>>>>(gridSize, std::vector<std::vector<std::vector<std::vector<Point>::iterator>>>(gridSize, std::vector<std::vector<std::vector<Point>::iterator>>(gridSize, std::vector<std::vector<Point>::iterator>(0))));

	populateSystem(m_pSphereSystem);
	if (m_simulateGridSystem)
	{
		populateSystem(m_pSphereSystemGrid);
	}
}

void SphereSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	drawSystem(m_pSphereSystem);
	if (m_simulateGridSystem)
	{
		drawSystem(m_pSphereSystemGrid);
	}
}

void SphereSystemSimulator::drawSystem(SphereSystem * system)
{
	for (vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator)
	{
		DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, system->sphereColor);
		DUC->drawSphere(iterator->position, m_fRadius);
	}
}

void SphereSystemSimulator::notifyCaseChanged(int testCase)
{
	m_iTestCase = testCase;

	switch (m_iTestCase)
	{
	case 0:
		cout << "DEMO 1\n";
		m_fRadius = 0.1f;
		m_simulateGridSystem = false;
		m_iCollision = 0;
		m_iNumSpheres = 20;
		m_pSphereSystemGrid->spheres.clear();
		measureNum = 0;
		accTime = 0.0;
		measureNum2 = 0;
		accTime2 = 0.0;
		max = 1000;
		reset();
		break;
	case 1:
		cout << "DEMO 2\n";
		m_fRadius = 0.1f;
		m_simulateGridSystem = false;
		m_iCollision = 1;
		m_iNumSpheres = 20;
		m_pSphereSystemGrid->spheres.clear();
		measureNum = 0;
		accTime = 0.0;
		measureNum2 = 0;
		accTime2 = 0.0;
		max = 1000;
		reset();
		break;
	case 2:
		cout << "DEMO 3\n";
		m_fRadius = 0.05f;
		m_iNumSpheres = 100;
		m_iCollision = 0;
		m_simulateGridSystem = true;
		measureNum = 0;
		accTime = 0.0;
		measureNum2 = 0;
		accTime2 = 0.0;
		//Bei sehr vielen Kugeln diesen wert verringern um in endlicher Zeit eine Messung zu bekommen
		max = 100;
		reset();
		break;
	default:
		cout << "Empty Test!\n";
		reset();
		break;
	}
}

void SphereSystemSimulator::externalForcesCalculations(float timeElapsed)
{
	Point2D mouseDiff;
	mouseDiff.x = m_trackmouse.x - m_oldtrackmouse.x;
	mouseDiff.y = m_trackmouse.y - m_oldtrackmouse.y;
	if (mouseDiff.x != 0 || mouseDiff.y != 0)
	{
		m_gotMouseStuff = true;
		Mat4 worldViewInv = Mat4(DUC->g_camera.GetWorldMatrix() * DUC->g_camera.GetViewMatrix());
		worldViewInv = worldViewInv.inverse();
		Vec3 inputView = Vec3((float)mouseDiff.x, (float)-mouseDiff.y, 0);
		Vec3 inputWorld = worldViewInv.transformVectorNormal(inputView);
		// find a proper scale!
		float inputScale = 4.f;
		inputWorld = inputWorld * inputScale;
		m_externalForce = inputWorld;
	}
	else
	{
		m_gotMouseStuff = false;
		//Gravity
		if (m_gravityOn) {
			//this value should never be actually used
			m_externalForce = Vec3(0, -100.0, 0);
		}
		//else m_externalForce = Vec3();
	}

	notifyNumberChanged();
}

void SphereSystemSimulator::applyExternalForce(SphereSystem * system)
{
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator)
	{
		if (m_gravityOn && !m_gotMouseStuff)
		{
			iterator->force += m_fMass*9.81*Vec3(0, -0.1, 0);
		}
		else
		{
			iterator->force += m_externalForce;
		}
	}
}

void SphereSystemSimulator::populateSystem(SphereSystem * system)
{
	system->spheres.clear();

	std::mt19937 eng;
	uniform_real_distribution<float> randPos(-0.5f, 0.5f);
	for (int i = 0; i < m_iNumSpheres; i++)
	{
		Point p = { Vec3(randPos(eng), randPos(eng), randPos(eng)), Vec3(), Vec3() };
		system->spheres.push_back(p);
	}

}

void SphereSystemSimulator::checkCollisionsNaive(SphereSystem * system)
{
	//Performance measure
	clock_t begin = clock();
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator)
	{
		for (std::vector<Point>::iterator iterator2 = system->spheres.begin(), end = system->spheres.end(); iterator2 != end; ++iterator2)
		{
			float distance = norm(iterator->position - iterator2->position);
			if (distance < 2.0f * m_fRadius)
			{
				float x = distance / (2.0f * m_fRadius);
				float forceFactor = m_fForceScaling * m_Kernels[m_iKernel](x);
				Vec3 force = forceFactor * (iterator->position - iterator2->position);
				iterator->force += force;
			}
		}
	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	accTime += elapsed_secs;
	measureNum++;
	if (measureNum >= max)
	{
		measureNum = 0;
		cout << "Naive time measurement with " << max << " timesteps, " << m_iNumSpheres << " spheres: " << accTime << " \n";
		accTime = 0;
	}
}

Vec3 SphereSystemSimulator::scaleVec(Vec3 in, int gridsize) 
{
	return Vec3((in.x + 0.5)*gridsize, (in.y + 0.5)*gridsize, (in.z + 0.5)*gridsize);
}
void SphereSystemSimulator::checkCollisionsUniform(SphereSystem * system)
{
	//Performance measure
	clock_t begin = clock();
	//adding spheres to grid with index
	int i = 0;
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator)
	{
		//mapping pos to (0,1) and then calculating position in grid, then saving index to sphere there
		Vec3 relpos = scaleVec(iterator->position, gridSize - 1);
		grid[(int)relpos.x][(int)relpos.y][(int)relpos.z].push_back(iterator);
	}

	//check for collisions
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator)
	{
		Vec3 relpos = scaleVec(iterator->position, gridSize - 1);
		for (int x = -1; x < 2; x++)
		{
			int x1 = x + (int)relpos.x;
			if (x1 >= 0 && x1 < gridSize)
			{
				for (int y = -1; y < 2; y++)
				{
					int y1 = y + (int)relpos.y;
					if(y1 >= 0 && y1 < gridSize) 
					{
						for (int z = -1; z < 2; z++)
						{
							int z1 = z + (int)relpos.z;
							if (z1 >= 0 && z1 < gridSize)
							{
								if (!grid[x1][y1][z1].empty())
								{
									for (std::vector<std::vector<Point>::iterator>::iterator it = grid[x1][y1][z1].begin(), end = grid[x1][y1][z1].end(); it != end; ++it)
									{
										collisionBetweenTwoSpheres(iterator, *it);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//since vector is defined in setup we clear it here
	for (auto &gridx : grid)
	{
		for (auto &gridy : gridx)
		{
			for (auto &gridz : gridy)
			{
				gridz.clear();
			}
		}
	}

	//Performance measure
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	accTime2 += elapsed_secs;
	measureNum2++;
	if (measureNum2 >= max)
	{
		measureNum2 = 0;
		cout << "Uniform Grid time measurement with "<< max << " timesteps, " << m_iNumSpheres << " spheres: " << accTime2 << " \n";
		accTime2 = 0;
	}
}



void SphereSystemSimulator::collisionBetweenTwoSpheres(std::vector<Point>::iterator a, std::vector<Point>::iterator b)
{
	float distance = norm(a->position - b->position);
	if (distance < 2.0f * m_fRadius)
	{
		float x = distance / (2.0f * m_fRadius);
		float forceFactor = m_fForceScaling * m_Kernels[m_iKernel](x);
		Vec3 force = forceFactor * (a->position - b->position);
		a->force += force;
	}
}

void SphereSystemSimulator::notifyNumberChanged()
{
	if (m_iNumSpheres != m_ioldNumSpheres)
	{
		m_ioldNumSpheres = m_iNumSpheres;
		reset();
	}
}

void SphereSystemSimulator::simulateTimestep(float timeStep)
{
	switch (m_iCollision)
	{
	case 0:
		checkCollisionsNaive(m_pSphereSystem);
		break;
	case 1:
		checkCollisionsUniform(m_pSphereSystem);
		break;
	default:
		checkCollisionsNaive(m_pSphereSystem);
		break;
	}
	if (m_simulateGridSystem)
	{
		checkCollisionsUniform(m_pSphereSystemGrid);
	}
	simulateSystem(m_pSphereSystem, timeStep);
	if (m_simulateGridSystem)
	{
		simulateSystem(m_pSphereSystemGrid, timeStep);
	}
}

void SphereSystemSimulator::simulateSystem(SphereSystem * system, float timeStep)
{
	//Midpoint
	//saving original values
	int p = system->spheres.size();
	Vec3 *xarray, *varray;
	xarray = (Vec3*)malloc(sizeof(Vec3)*p);
	varray = (Vec3*)malloc(sizeof(Vec3)*p);
	//1.Forces
	applyExternalForce(system);
	//2.Movement by velocity
	int i = 0;
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator) {
		//if (!(iterator->fixed)) {
		xarray[i++] = iterator->position;
		iterator->position = iterator->position + (iterator->velocity*timeStep / 2);
		//}
	}
	//3.Velocity changes by forces
	i = 0;
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator) {
		//if (!(iterator->fixed)) {
		varray[i++] = iterator->velocity;
		iterator->velocity = iterator->velocity + ((iterator->force / m_fMass) *timeStep / 2);
		//}
	}
	//4.Forces
	applyExternalForce(system);
	//5.Movement by velocity
	i = 0;
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator) {
		//if (!(iterator->fixed)) {
		iterator->position = xarray[i++] + (iterator->velocity*timeStep);
		//}
	}
	//6.Velocity changes by forces
	i = 0;
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator) {
		//if (!(iterator->fixed)) {
		iterator->velocity = varray[i++] + ((iterator->force / m_fMass) *timeStep);
		//}

		//reset force
		iterator->force = Vec3();
	}
	free(varray);
	free(xarray);

	//"Kollision" mit der Box
	for (vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator)
	{
		if (iterator->position.x < -0.5)
		{
			iterator->position.x = -0.5;
			iterator->velocity.x *= -0.5;
		}
		if (iterator->position.x > 0.5)
		{
			iterator->position.x = 0.5;
			iterator->velocity.x *= -0.5;
		}
		if (iterator->position.y < -0.5)
		{
			iterator->position.y = -0.5;
			iterator->velocity.y *= -0.5;
		}
		if (iterator->position.y > 0.5)
		{
			iterator->position.y = 0.5;
			iterator->velocity.y *= -0.5;
		}
		if (iterator->position.z < -0.5)
		{
			iterator->position.z = -0.5;
			iterator->velocity.z *= -0.5;
		}
		if (iterator->position.z > 0.5)
		{
			iterator->position.z = 0.5;
			iterator->velocity.z *= -0.5;
		}

	}
}

void SphereSystemSimulator::onClick(int x, int y)
{
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

void SphereSystemSimulator::onMouse(int x, int y)
{
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

