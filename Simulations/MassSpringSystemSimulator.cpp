#include "MassSpringSystemSimulator.h"

void MassSpringSystemSimulator::applyExternalForce(Vec3 force) {
	std::list<Point>::iterator Pit = Points.begin();
	while (Pit != Points.end()) {
		Pit->force = force;
	}
}

//folgende beiden Funktionen muss man irgendwie schöne machen können, nur finde ich keine list.get() oder sowas
Point* MassSpringSystemSimulator::getP(int x) {
	std::list<Point>::iterator Pit = Points.begin();
	while (x > 0) {
		Pit++;
		x--;
	}
	return &*Pit;
}
Spring* MassSpringSystemSimulator::getS(int x) {
	std::list<Spring>::iterator Sit = Springs.begin();
	while (x > 0) {
		Sit++;
		x--;
	}
	return &*Sit;
}

void MassSpringSystemSimulator::computeElasticForces(Spring s) {
	Point* p1 = getP(s.point1);
	Point *p2 = getP(s.point2);
	Vec3 diff1 = p1->position - p2->position;
	Vec3 diff2 = p2->position - p1->position;
	p1->force = p1->force + s.stiffness*diff1 / s.initialLength;
	p2->force = p2->force + s.stiffness*diff2 / s.initialLength;
}

MassSpringSystemSimulator::MassSpringSystemSimulator()
{
	addMassPoint(Vec3(0.5, 0.5, 0.5), Vec3(), true);
	addMassPoint(Vec3(0.1, -0.2, 0.1), Vec3(), true);
	addMassPoint(Vec3(0.0, 0.2, 0.4), Vec3(), true);
	addMassPoint(Vec3(-0.5, -0.5, 0.5), Vec3(), true);
	addMassPoint(Vec3(-0.5, 0.4, 0.1), Vec3(), true);

	addSpring(0, 1, 1);
	addSpring(2, 3, 1);

	m_iIntegrator = LEAPFROG;
}

const char * MassSpringSystemSimulator::getTestCasesStr()
{
	return nullptr;
}

void MassSpringSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
}

void MassSpringSystemSimulator::reset()
{
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
}

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
		Point* p1 = getP(spring.point1);
		Point* p2 = getP(spring.point2);

		DUC->setUpLighting(Vec3(), Vec3(), 0, SPRINGCOLOR);
		DUC->beginLine();
		DUC->drawLine(p1->position, SPRINGCOLOR, p2->position, SPRINGCOLOR);
		DUC->endLine();
	}
}

void MassSpringSystemSimulator::notifyCaseChanged(int testCase)
{
}

void MassSpringSystemSimulator::externalForcesCalculations(float timeElapsed)
{
}

void MassSpringSystemSimulator::simulateTimestep(float timeStep) {
	std::list<Spring>::iterator Sit = Springs.begin();
	std::list<Point>::iterator Pit = Points.begin();
	switch (m_iIntegrator) {
	case 0:
		//EULER

		//1.Forces
		applyExternalForce(m_externalForce);
		while (Sit != Springs.end()) {
			std::cout << "iterating springs for force.." << std::endl;
			computeElasticForces(*Sit);
		}
		//2.Movement by velocity
		while (Pit != Points.end()) {
			std::cout << "iterating points for position.." << std::endl;

			Pit->position = Pit->position + (Pit->velocity*timeStep);
		}
		//3.Velocity changes by forces
		Pit = Points.begin();
		while (Pit != Points.end()) {
			std::cout << "iterating points for velocity.." << std::endl;

			Pit->velocity = Pit->velocity + (Pit->force / Pit->mass *timeStep);
		}
		break;
	case 1:
		//Leapfrog
		break;
	case 2:
		//Midpoint

		break;
	default:
		std::cout << "This shouldnt happen" << std::endl;
		break;
	}
}

void MassSpringSystemSimulator::onClick(int x, int y)
{
}

void MassSpringSystemSimulator::onMouse(int x, int y)
{
}

void MassSpringSystemSimulator::setMass(float mass)
{
}

void MassSpringSystemSimulator::setStiffness(float stiffness)
{
}

void MassSpringSystemSimulator::setDampingFactor(float damping)
{
}

int MassSpringSystemSimulator::addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed)
{
	Point p;
	p.position = position;
	p.velocity = Velocity;
	p.mass = 1;
	p.damping = 0;
	p.force = 0;
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
	s.stiffness = 1;
	s.point1 = masspoint1;
	s.point2 = masspoint2;
	Springs.push_back(s);

}

int MassSpringSystemSimulator::getNumberOfMassPoints()
{
	return 0;
}

int MassSpringSystemSimulator::getNumberOfSprings()
{
	return 0;
}

Vec3 MassSpringSystemSimulator::getPositionOfMassPoint(int index)
{
	return Vec3();
}

Vec3 MassSpringSystemSimulator::getVelocityOfMassPoint(int index)
{
	return Vec3();
}
