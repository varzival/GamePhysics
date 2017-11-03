#include "MassSpringSystemSimulator.h"

extern void computeElasticForces(float t,Spring s) {
	//TODO
	Point p1 = Springs.get(s.point1);
	Point p2 = Springs.get(s.point2);

}

extern void simulateTimestep(float timeStep) {
	std::list<Spring>::iterator Sit = Springs.begin();
	std::list<Point>::iterator Pit = Points.begin();
	switch (m_iIntegrator) {
	case 0:
		//EULER
		while (Sit != Springs.end()) {
			computeElasticForces(timeStep,Sit);
		}
		while (Pit != Points.end()) {
			Pit->position = Pit->position + (Pit->velocity*timestep);
		}
		Pit = Points.begin();
		while (Pit != Points.end()) {
			Pit->velocity = Pit->velocity + (Pit->force*timestep);
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