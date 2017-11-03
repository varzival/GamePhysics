#include "MassSpringSystemSimulator.h"

void MassSpringSystemSimulator::applyExternalForce(Vec3 force) {
	std::list<Point>::iterator Pit = Points.begin();
	while (Pit != Points.end()) {
		Pit->force = force;
	}
}

//folgende beiden Funktionen muss man irgendwie schöne machen können, nur finde ich keine list.get() oder sowas
Point MassSpringSystemSimulator::getP(int x) {
	std::list<Point>::iterator Pit = Points.begin();
	while (x > 0) {
		Pit++;
		x--;
	}
	return *Pit;
}
Spring MassSpringSystemSimulator::getS(int x) {
	std::list<Spring>::iterator Sit = Springs.begin();
	while (x > 0) {
		Sit++;
		x--;
	}
	return *Sit;
}

void MassSpringSystemSimulator::computeElasticForces(float t, Spring s) {
	Point p1 = getP(s.point1);
	Point p2 = getP(s.point2);
	//TODO

}

void MassSpringSystemSimulator::simulateTimestep(float timeStep) {
	std::list<Spring>::iterator Sit = Springs.begin();
	std::list<Point>::iterator Pit = Points.begin();
	switch (m_iIntegrator) {
	case 0:
		//EULER
		applyExternalForce(m_externalForce);
		while (Sit != Springs.end()) {
			computeElasticForces(timeStep, *Sit);
		}
		while (Pit != Points.end()) {
			Pit->position = Pit->position + (Pit->velocity*timeStep);
		}
		Pit = Points.begin();
		while (Pit != Points.end()) {
			Pit->velocity = Pit->velocity + (Pit->force*timeStep);
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