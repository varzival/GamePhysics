#include "RigidBodySystemSimulator.h"

RigidBodySystemSimulator::RigidBodySystemSimulator()
{
	//basic setup
	addRigidBody(Vec3(0, 0, 0), Vec3(1, 0.6, 0.5), 2);
	Quat deg90 = Quat(Vec3(0, 0, 1), M_PI/2.0);
	setOrientationOf(0, deg90);
	applyForceOnBody(0, Vec3(0.3, 0.5, 0.25), Vec3(1, 1, 0));
}

const char * RigidBodySystemSimulator::getTestCasesStr()
{
	return nullptr;
}

void RigidBodySystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
}

void RigidBodySystemSimulator::reset()
{
}

void RigidBodySystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	for (std::vector<rigidBody>::iterator it = rigidBodies.begin(); it != rigidBodies.end(); it++)
	{
		DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, BOXCOLOR);
		DUC->drawRigidBody(it->worldMat());
	}

}

void RigidBodySystemSimulator::notifyCaseChanged(int testCase)
{
}

void RigidBodySystemSimulator::externalForcesCalculations(float timeElapsed)
{
}

void RigidBodySystemSimulator::simulateTimestep(float timeStep)
{
	for (std::vector<rigidBody>::iterator it = rigidBodies.begin(); it != rigidBodies.end(); it++)
	{
		//Standard Euler Step
		it->pos = it->pos + timeStep * it->vel;
		it->vel = it->vel + timeStep * (it->totalForce / it->mass);

		//Rotation calculations
		it->rot = it->rot + 0.5f * Quat(it->angVel[0], it->angVel[1], it->angVel[2], 0.0f) * it->rot;
		it->angMom = it->angMom + timeStep * it->torque;
		

		//Inertia
		matrix4x4<Real> rotMat = it->rotMat();
		matrix4x4<Real> invIns = rotMat * it->inverseInertia();
		rotMat.transpose();
		invIns = invIns * rotMat;
		//rotMat.transpose(); transpose back

		it->angVel = invIns.transformVector(it->angMom);

		//apply rotation
		//does it make sense to update position and velocity of every point with rotation?

		//reset force and torque
		it->totalForce = Vec3(0.0f, 0.0f, 0.0f);
		it->torque = Vec3(0.0f, 0.0f, 0.0f);
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
	return rigidBodies.size();
}

Vec3 RigidBodySystemSimulator::getPositionOfRigidBody(int i)
{
	return rigidBodies[i].pos;
}

Vec3 RigidBodySystemSimulator::getLinearVelocityOfRigidBody(int i)
{
	return rigidBodies[i].vel;
}

Vec3 RigidBodySystemSimulator::getAngularVelocityOfRigidBody(int i)
{
	return rigidBodies[i].angVel;
}

void RigidBodySystemSimulator::applyForceOnBody(int i, Vec3 loc, Vec3 force)
{
	rigidBody* rbp = &rigidBodies.at(i);
	rbp->totalForce = rbp->totalForce + force;
	Vec3 vecToPoint = loc - rbp->pos;
	rbp->torque = rbp->torque + cross(vecToPoint, force);
}

void RigidBodySystemSimulator::addRigidBody(Vec3 position, Vec3 size, int mass)
{
	rigidBody rb;
	rb.pos = position;
	rb.scale = size;
	rb.mass = mass;
	rigidBodies.push_back(rb);
}

void RigidBodySystemSimulator::setOrientationOf(int i, Quat orientation)
{
	rigidBodies[i].rot = orientation;
}

void RigidBodySystemSimulator::setVelocityOf(int i, Vec3 velocity)
{
	rigidBodies[i].vel = velocity;
}