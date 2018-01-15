#include "ProjectSystemSimulator.h"

const char * ProjectSystemSimulator::getTestCasesStr()
{
	return nullptr;
}

void ProjectSystemSimulator::reset()
{
}

void ProjectSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
}

void ProjectSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
}

void ProjectSystemSimulator::externalForcesCalculations(float timeElapsed)
{
}

void ProjectSystemSimulator::simulateTimestep(float timeStep)
{
}

void ProjectSystemSimulator::notifyCaseChanged(int testCase)
{
}

void ProjectSystemSimulator::onClick(int x, int y)
{
}

void ProjectSystemSimulator::onMouse(int x, int y)
{
}
