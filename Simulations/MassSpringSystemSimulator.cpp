#include "MassSpringSystemSimulator.h"

void MassSpringSystemSimulator::applyExternalForce(Vec3 force) {
	for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
		iterator->force = force;
	}
}

void MassSpringSystemSimulator::computeElasticForces(Spring s) {
	Point * p1 = &Points[s.point1];
	Point * p2 = &Points[s.point2];
	Vec3 diff1 = p1->position - p2->position;
	Vec3 diff2 = p2->position - p1->position;
	p1->force = p1->force + s.stiffness*diff2 / s.initialLength;
	p2->force = p2->force + s.stiffness*diff1 / s.initialLength;
	//p1->force *= m_fDamping;
	//p2->force *= m_fDamping;
}

MassSpringSystemSimulator::MassSpringSystemSimulator()
{
	/*
	addMassPoint(Vec3(0.5, 0.5, 0.5), Vec3(), false);
	addMassPoint(Vec3(0.1, -0.2, 0.1), Vec3(), false);
	addMassPoint(Vec3(0.0, 0.2, 0.4), Vec3(), false);
	addMassPoint(Vec3(-0.5, -0.5, 0.5), Vec3(), false);
	addMassPoint(Vec3(-0.5, 0.4, 0.1), Vec3(), false);

	addSpring(0, 1, 1);
	addSpring(2, 3, 1);
	*/

	//basic setup
	addMassPoint(Vec3(0, 0, 0), Vec3(-1, 0, 0), false);
	addMassPoint(Vec3(0, 2, 0), Vec3(1, 0, 0), false);
	addSpring(0, 1, 1.0f);
	setMass(10.0f);
	setStiffness(40.0f);


	//m_iIntegrator = LEAPFROG;
	//Funktioniert noch nicht.
	m_iIntegrator = EULER;
}
//Bernhards Job
const char * MassSpringSystemSimulator::getTestCasesStr()
{
	return "Euler, Leapfrog, Midpoint";
}
//Bernhards Job
void MassSpringSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
}
//Bernhards Job
void MassSpringSystemSimulator::reset()
{
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
}
//Bernhards Job
void MassSpringSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	Vec3 zero = Vec3(0, 0, 0);

	for each (Point point in Points)
	{
		DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, SPHERECOLOR);
		DUC->drawSphere(point.position, SPHERESIZE);
	}
	for each (Spring spring in Springs)
	{
		Point p1 = Points[spring.point1];
		Point p2 = Points[spring.point2];

		DUC->setUpLighting(Vec3(), Vec3(), 0, SPRINGCOLOR);
		DUC->beginLine();
		DUC->drawLine(p1.position, SPRINGCOLOR, p2.position, SPRINGCOLOR);
		DUC->endLine();
	}
}
//Bernhards Job
void MassSpringSystemSimulator::notifyCaseChanged(int testCase)
{
	m_iTestCase = testCase;
	switch (m_iTestCase)
	{
	case 0:
		cout << "Euler\n";
		m_iIntegrator = EULER;
		break;
	case 1:
		cout << "Leapfrog\n";
		m_iIntegrator = LEAPFROG;
		break;
	case 2:
		cout << "Midpoint\n";
		m_iIntegrator = MIDPOINT;
		break;
	default:
		cout << "Empty Test!\n";
		break;
	}
}
//Bernhards Job
void MassSpringSystemSimulator::externalForcesCalculations(float timeElapsed)
{
	Point2D mouseDiff;
	mouseDiff.x = m_trackmouse.x - m_oldtrackmouse.x;
	mouseDiff.y = m_trackmouse.y - m_oldtrackmouse.y;
	if (mouseDiff.x != 0 || mouseDiff.y != 0)
	{
		Mat4 worldViewInv = Mat4(DUC->g_camera.GetWorldMatrix() * DUC->g_camera.GetViewMatrix());
		worldViewInv = worldViewInv.inverse();
		Vec3 inputView = Vec3((float)mouseDiff.x, (float)-mouseDiff.y, 0);
		Vec3 inputWorld = worldViewInv.transformVectorNormal(inputView);
		// find a proper scale!
		float inputScale = 0.001f;
		inputWorld = inputWorld * inputScale;
		m_externalForce = inputWorld;
		/*for (int i = 0; i < Points.size(); i++)
		{
			Points[i].force = inputWorld;
		}
		*/
	}
	else
	{
		m_externalForce = Vec3(0, 0, 0);
		/*for (int i = 0; i < Points.size(); i++)
		{
			Points[i].force = Vec3(0.0, 0.0, 0.0);
		}
		*/
	}
}
//Bastis Job
void MassSpringSystemSimulator::simulateTimestep(float timeStep) {
	int i, p;
	switch (m_iIntegrator) {
	case 0:
		//EULER
		//1.Forces
		applyExternalForce(m_externalForce);
		for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
			computeElasticForces(*iterator);
		}
		//2.Movement by velocity
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			iterator->position = (Vec3)(iterator->position + (iterator->velocity*timeStep));
		}
		//3.Velocity changes by forces
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			iterator->velocity = iterator->velocity + (iterator->force / iterator->mass *timeStep);
		}
		break;
	case 1:
		//Leapfrog
		break;
	case 2:
		//Midpoint
		//saving original values
		p = Points.size();
		Vec3 *xarray, *varray;
		xarray = (Vec3*)malloc(sizeof(Vec3)*p);
		varray = (Vec3*)malloc(sizeof(Vec3)*p);
		//1.Forces
		applyExternalForce(m_externalForce);
		for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
			computeElasticForces(*iterator);
		}
		//2.Movement by velocity
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			xarray[i++] = iterator->position;
			iterator->position = iterator->position + (iterator->velocity*timeStep / 2);
		}
		//3.Velocity changes by forces
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			varray[i++] = iterator->velocity;
			iterator->velocity = iterator->velocity + (iterator->force / iterator->mass *timeStep / 2);
		}
		//4.Forces
		applyExternalForce(m_externalForce);
		for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
			computeElasticForces(*iterator);
		}
		//5.Movement by velocity
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			iterator->position = xarray[i++] + (iterator->velocity*timeStep);
		}
		//6.Velocity changes by forces
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			iterator->velocity = varray[i++] + (iterator->force / iterator->mass *timeStep);
		}
		free(varray);
		free(xarray);
		break;
	default:
		std::cout << "This shouldnt happen!!!!!11ELF" << std::endl;
		break;
	}
}
//Bernhards Job linksklick
void MassSpringSystemSimulator::onClick(int x, int y)
{
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}
//Bernhards Job andere klicks und mausbewegung
void MassSpringSystemSimulator::onMouse(int x, int y)
{
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

void MassSpringSystemSimulator::setMass(float mass)
{
	m_fMass = mass;
	for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
		iterator->mass = mass;
	}
}

void MassSpringSystemSimulator::setStiffness(float stiffness)
{
	m_fStiffness = stiffness;
	for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
		iterator->stiffness = stiffness;
	}
}

void MassSpringSystemSimulator::setDampingFactor(float damping)
{
	m_fDamping = damping;
}

int MassSpringSystemSimulator::addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed)
{
	Point p;
	p.position = position;
	p.velocity = Velocity;
	p.mass = m_fMass;
	p.damping = 0;
	p.force = Vec3(0.0, 0.0, 0.0);
	Points.push_back(p);
	return Points.size() - 1;
}

void MassSpringSystemSimulator::addSpring(int masspoint1, int masspoint2, float initialLength)
{
	if (masspoint1 > (Points.size() - 1))
	{
		std::cout << "Could not find point on index " << masspoint1 << "\n";
		return;
	}
	if (masspoint2 > (Points.size() - 1))
	{
		std::cout << "Could not find point on index " << masspoint2 << "\n";
		return;
	}

	Spring s;
	s.initialLength = initialLength;
	s.stiffness = m_fStiffness;
	s.point1 = masspoint1;
	s.point2 = masspoint2;
	Springs.push_back(s);

}

int MassSpringSystemSimulator::getNumberOfMassPoints()
{
	return Points.size();
}

int MassSpringSystemSimulator::getNumberOfSprings()
{
	return Springs.size();
}

Vec3 MassSpringSystemSimulator::getPositionOfMassPoint(int index)
{
	return Points.at(index).position;
}

Vec3 MassSpringSystemSimulator::getVelocityOfMassPoint(int index)
{
	return Points.at(index).velocity;
}
