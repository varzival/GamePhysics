#include "MassSpringSystemSimulator.h"


void simulateTimestep(float timeStep) {
	std::list<Spring>::iterator Sit = Springs.begin();
	std::list<Points>::iterator Pit = Points.begin();
	switch (m_iIntegrator) {
	case 0:
		//EULER
		
		while (Sit != Springs.end()) {

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