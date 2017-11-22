#include "RigidBodySystemSimulator.h"

RigidBodySystemSimulator::RigidBodySystemSimulator()
{
}

const char * RigidBodySystemSimulator::getTestCasesStr()
{
	return nullptr;
}

void RigidBodySystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
}

void RigidBodySystemSimulator::reset()
{
}

void RigidBodySystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
}

void RigidBodySystemSimulator::notifyCaseChanged(int testCase)
{
}

void RigidBodySystemSimulator::externalForcesCalculations(float timeElapsed)
{
}

void RigidBodySystemSimulator::simulateTimestep(float timeStep)
{
	for (std::vector<rigidBody>::iterator it = rigidBodies.begin(); it != rigidBodies.end; it++)
	{
		Vec3 totalForce(0.0f, 0.0f, 0.0f);
		Vec3 torque(0.0f, 0.0f, 0.0f);
		for (std::vector<Force>::iterator itforce = it->forces.begin(); itforce != it->forces.end(); itforce++)
		{
			Force f = *itforce;
			totalForce = totalForce + f.forceDir;
			Vec3 vecToPoint = f.forcePos - it->pos;
			torque = torque += cross(vecToPoint, f.forceDir);
		}

		//Standard Euler Step
		it->pos = it->pos + timeStep * it->vel;
		it->vel = timeStep * (totalForce / it->mass);

		//Rotation calculations
		it->rot = it->rot + 0.5f * Quaternion<float>(0.0f, it->angVel[0], it->angVel[1], it->angVel[2]) * it->rot;
		it->angMom = it->angMom + timeStep * torque;
		//INSERT inverse inertia here
	}
}

void RigidBodySystemSimulator::onClick(int x, int y)
{
}

void RigidBodySystemSimulator::onMouse(int x, int y)
{
}

int RigidBodySystemSimulator::getNumberOfRigidBodies()
{
	return 0;
}

Vec3 RigidBodySystemSimulator::getPositionOfRigidBody(int i)
{
	return Vec3();
}

Vec3 RigidBodySystemSimulator::getLinearVelocityOfRigidBody(int i)
{
	return Vec3();
}

Vec3 RigidBodySystemSimulator::getAngularVelocityOfRigidBody(int i)
{
	return Vec3();
}

void RigidBodySystemSimulator::applyForceOnBody(int i, Vec3 loc, Vec3 force)
{
}

void RigidBodySystemSimulator::addRigidBody(Vec3 position, Vec3 size, int mass)
{
}

void RigidBodySystemSimulator::setOrientationOf(int i, Quat orientation)
{
}

void RigidBodySystemSimulator::setVelocityOf(int i, Vec3 velocity)
{
}
