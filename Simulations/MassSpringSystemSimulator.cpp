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

void MassSpringSystemSimulator::reset()
{
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
}

void MassSpringSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	Vec3 zero = Vec3(0, 0, 0);
	DUC->setUpLighting(zero, zero, 0, SPHERECOLOR);

	for each (Point point in Points)
	{
		DUC->drawSphere(point.position, SPHERESIZE);
	}

	DUC->setUpLighting(zero, zero, 0, SPHERECOLOR);
	for each (Spring spring in Springs)
	{
		Point* p1 = getP(spring.point1);
		Point* p2 = getP(spring.point2);
		
		DUC->beginLine();
		DUC->drawLine(p1->position, SPRINGCOLOR, p2->position, SPRINGCOLOR);
		DUC->endLine();
	}
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
			computeElasticForces(*Sit);
		}
		//2.Movement by velocity
		while (Pit != Points.end()) {
			Pit->position = Pit->position + (Pit->velocity*timeStep);
		}
		//3.Velocity changes by forces
		Pit = Points.begin();
		while (Pit != Points.end()) {
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