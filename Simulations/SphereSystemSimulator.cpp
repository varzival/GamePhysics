#include "SphereSystemSimulator.h"

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
	m_pSphereSystem = new SphereSystem(Vec3(0.0f, 1.0f, 0.0f));

}

SphereSystemSimulator::~SphereSystemSimulator()
{
	free(m_pSphereSystem);
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
	TwAddVarRW(DUC->g_pTweakBar, "Sphere Mass", TW_TYPE_FLOAT, &m_fMass, "step=0.01 min=0.01");
	TwAddVarRW(DUC->g_pTweakBar, "Gravity", TW_TYPE_BOOLCPP, &m_gravityOn, "");
}

void SphereSystemSimulator::reset()
{
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
}

void SphereSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	drawSystem(m_pSphereSystem);
	if (m_simulateGridSystem) drawSystem(m_pSphereSystemGrid);
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
		break;
	case 1:
		cout << "DEMO 2\n";
		break;
	case 2:
		cout << "DEMO 3\n";
		break;
	default:
		cout << "Empty Test!\n";
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
}

void SphereSystemSimulator::applyExternalForce(SphereSystem * system)
{
	for (std::vector<Point>::iterator iterator = system->spheres.begin(), end = system->spheres.end(); iterator != end; ++iterator) {
		if (m_gravityOn && !m_gotMouseStuff) {
			iterator->force = iterator->mass*9.81*Vec3(0, -0.1, 0);
		}
		else {
			iterator->force = m_externalForce;
		}
	}
	
}

void SphereSystemSimulator::simulateTimestep(float timeStep)
{
	simulateSystem(m_pSphereSystem, timeStep);
	if (m_simulateGridSystem) simulateSystem(m_pSphereSystemGrid, timeStep);
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
			iterator->velocity = iterator->velocity + ((iterator->force / iterator->mass) *timeStep / 2);
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
			iterator->velocity = varray[i++] + ((iterator->force / iterator->mass) *timeStep);
		//}
	}
	free(varray);
	free(xarray);
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

