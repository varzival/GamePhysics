#include "MassSpringSystemSimulator.h"


void simulateTimestep(float timeStep) {
	switch (m_iIntegrator) {
	case 0:
		//EULER

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