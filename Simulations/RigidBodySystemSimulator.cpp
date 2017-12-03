#include "RigidBodySystemSimulator.h"
#include <math.h>

#define VELOCITYFACTOR 1.0

RigidBodySystemSimulator::RigidBodySystemSimulator()
{
	m_forceVisalsOn = true;

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
	TwAddVarRW(DUC->g_pTweakBar, "Visualize Forces", TW_TYPE_BOOLCPP, &m_forceVisalsOn, "");
	TwType TW_TYPE_TESTCASE = TwDefineEnumFromString("Demo", "Demo1, Demo2, Demo3, Demo4");
	TwAddVarRW(DUC->g_pTweakBar, "Demo", TW_TYPE_TESTCASE, &demoChoice, "");
}

void RigidBodySystemSimulator::reset()
{
	loadDemo();
}

void RigidBodySystemSimulator::loadDemo()
{
	rigidBodies.clear();
	forceVisuals.clear();
	Quat deg90;
	cout << "load Demo " << demoChoice << "\n";
	switch(demoChoice)
	{
	case 0:
		addRigidBody(Vec3(0, 0, 0), Vec3(1, 0.6, 0.5), 2);
		 deg90 = Quat(Vec3(0, 0, 1), M_PI / 2.0);
		setOrientationOf(0, deg90);
		applyForceOnBody(0, Vec3(0.3, 0.5, 0.25), Vec3(1, 1, 0));
		oldDemoChoice = 0;
		demo1Printed = false;
		*timeStep = 2.0f;
		break;
	case 1:
		addRigidBody(Vec3(0, 0, 0), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0, 0, 1), M_PI / 2.0);
		setOrientationOf(0, deg90);
		applyForceOnBody(0, Vec3(0.3, 0.5, 0.25), Vec3(1, 1, 0));
		oldDemoChoice = 1;
		*timeStep = 0.01f;
		break;
	case 2:
		oldDemoChoice = 2;
		*timeStep = 0.001f;
		//Quader1
		addRigidBody(Vec3(0, 0, 0), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0.2, 1, 0), M_PI / 2.0);
		setOrientationOf(0, deg90);
		setVelocityOf(0, Vec3(0, 0.1, 0));
		//Quader2
		addRigidBody(Vec3(0, 1, 0), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0, 1, 0), M_PI / 2.0);
		setOrientationOf(1, deg90);
		setVelocityOf(1, Vec3(0, -0.1, 0));
		break;
	case 3:
		*timeStep = 0.001f;
		oldDemoChoice = 3;
		//Quader1
		addRigidBody(Vec3(0, 0, 0), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0, 1, 0), M_PI / 2.0);
		setOrientationOf(0, deg90);
		//Quader2
		addRigidBody(Vec3(0, 1, 0), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0, 1, 0), M_PI / 2.0);
		setOrientationOf(1, deg90);
		//Quader3
		addRigidBody(Vec3(1, 0, 0), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0, 1, 0), M_PI / 2.0);
		setOrientationOf(1, deg90);
		//Quader4
		addRigidBody(Vec3(0, 0, 1), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0, 1, 0), M_PI / 2.0);
		setOrientationOf(1, deg90);
		break;
	default:
		addRigidBody(Vec3(0, 0, 0), Vec3(1, 0.6, 0.5), 2);
		deg90 = Quat(Vec3(0, 0, 1), M_PI / 2.0);
		setOrientationOf(0, deg90);
		applyForceOnBody(0, Vec3(0.3, 0.5, 0.25), Vec3(1, 1, 0));
		oldDemoChoice = 0;
		break;
	}
}

void RigidBodySystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{

	if (m_bodyVisualsOn)
	{
		for (std::vector<rigidBody>::iterator it = rigidBodies.begin(); it != rigidBodies.end(); it++)
		{
			DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, BOXCOLOR);
			DUC->drawRigidBody(it->worldMat());
		}
	}
	
	//visualize forces
	if (m_forceVisalsOn)
	{
		for (std::vector<ForceVisual>::iterator it = forceVisuals.begin(); it != forceVisuals.end(); it++)
		{
			DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, FORCEPOINTCOLOR);
			DUC->drawSphere(it->force.forcePos, FORCEPOINTRADIUS);
			DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, FORCELINECOLOR);
			DUC->beginLine();
			DUC->drawLine(it->force.forcePos, FORCELINECOLOR, it->force.forcePos + it->force.forceDir, FORCELINECOLOR);
			DUC->endLine();
		}
	}
	//Falls der Nutzer die Demo aendert wechseln
	if (oldDemoChoice != demoChoice)
	{
		loadDemo();
	}
	CollisionInfo info;
	for (std::vector<rigidBody>::iterator it = rigidBodies.begin(); it != rigidBodies.end(); it++)
	{
		if (rigidBodies.begin() != rigidBodies.end())//Sicherstellen das es mehr als 1 Rigidbody gibt
		{			
			//It mit allen anderen auf Kollisionen pruefen
			for (std::vector<rigidBody>::iterator it2 = rigidBodies.begin(); it2 != rigidBodies.end(); it2++)
			{
				if (it != it2)//Nicht mit sich selbst prüfen
				{
					info = checkCollisionSAT(it->worldMat(), it2->worldMat());
					if (info.isValid)
					{
						//Relative Velocity ausrechnen
						Vec3 relVel = (it->pointVelocity(info.collisionPointWorld)) - (it2->pointVelocity(info.collisionPointWorld));
						Vec3 out = dot(relVel, info.normalWorld);
						//Boxes are seperating
						
						if (out > 0.0)
						{
							//Hier muss man glaub gar nichts machen
							//es kann aber durch die Rotation trotzdem Kollisionen geben, die KÖNNEN wir jedoch nicht berücksichtigen
							//würde vorschlagen, einen großen Faktor auf die resultat-Velocities zu multiplizieren, damit das einfach nie passiert
							cout << "Boxes are seperating\n";
						}
						
						//Boxes are colliding
						if (out < 0.0)
						{
							//probably take this formula: http://www.euclideanspace.com/physics/dynamics/collision/threed/index.htm
							//cause game physics slides really suck

							//Impuls berechnen
							Vec3 r_a = info.collisionPointWorld - it->pos;
							Vec3 r_b = info.collisionPointWorld - it2->pos;
							Vec3 p1 = (it->inverseInertia()*(cross(r_a, info.normalWorld)) * info.collisionPointWorld);
							Vec3 p2 = (it2->inverseInertia()*(cross(r_b, info.normalWorld)) * info.collisionPointWorld);
							Vec3 j = (-1 * out)/(1.0/it->mass + 1.0/it2->mass + dot((p1 + p2), info.normalWorld));
							it->vel = VELOCITYFACTOR * (it->vel + j/it->mass);
							it2->vel = VELOCITYFACTOR * (it2->vel - j/it->mass);

							it->angMom = it->angMom + (cross(info.collisionPointWorld, j));
							it2->angMom = it2->angMom - (cross(info.collisionPointWorld, j));
							cout << "Boxes are colliding\n";
						}
						else
						{
							//wird nie eintreten, da eine Gleitkommazahl nie genau gleich 0 ist
							cout << "Boxes are sliding\n ";
						}
						
					}
				}
			}
		}
	}

}



void RigidBodySystemSimulator::notifyCaseChanged(int testCase)
{
	reset();
}

void RigidBodySystemSimulator::externalForcesCalculations(float timeElapsed)
{
	for (std::vector<ForceVisual>::iterator it = forceVisuals.begin(); it != forceVisuals.end();)
	{
		it->ttl -= timeElapsed;
		if (it->ttl < 0.0f)
		{
			it = forceVisuals.erase(it);
		}
		else
		{
			it++;
		}
	}

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
		float inputScale = 0.00004f;
		inputWorld = inputWorld * inputScale;
		m_externalForce = inputWorld;
		for (std::vector<rigidBody>::iterator it = rigidBodies.begin(); it != rigidBodies.end(); it++)
		{
			it->vel += m_externalForce;
		}
	}
	else
	{
	}

}

void RigidBodySystemSimulator::simulateTimestep(float timeStep)
{
	for (std::vector<rigidBody>::iterator it = rigidBodies.begin(); it != rigidBodies.end(); it++)
	{
		//Standard Euler Step
		it->pos = it->pos + timeStep * it->vel;
		it->vel = it->vel + timeStep * (it->totalForce / it->mass);

		//Rotation calculations
		it->rot = it->rot + timeStep * 0.5 * Quat(it->angVel[0], it->angVel[1], it->angVel[2], 0.0) * it->rot;
		//renormalize
		it->rot /= it->rot.norm();

		it->angMom = it->angMom + timeStep * it->torque;
		
		//Inertia
		matrix4x4<Real> rotMat = it->rotMat();
		matrix4x4<Real> invIns = rotMat * it->inverseInertia();
		rotMat.transpose();
		invIns = invIns * rotMat;
		rotMat.transpose();

		it->angVel = invIns.transformVector(it->angMom);

		//apply rotation
		//does it make sense to update position and velocity of every point with rotation?

		//reset force and torque
		it->totalForce = Vec3(0.0f, 0.0f, 0.0f);
		it->torque = Vec3(0.0f, 0.0f, 0.0f);

		//print
		if (demoChoice == 0 && !demo1Printed)
		{
			Vec3 point = Vec3(0.3, 0.5, 0.25);
			std::cout << "World Space Velocity of " << point << ": " << it->pointVelocity(point) << "\n";
			std::cout << "Linear Velocity of Body: " << it->vel << "\n";
			std::cout << "Angular Velocity of Body: " << it->angVel << "\n";
			demo1Printed = true;
		}
	}
}
//linksklick
void RigidBodySystemSimulator::onClick(int x, int y)
{
	m_trackmouse.x = x;
	m_trackmouse.y = y;

	//Wo die Maus zu jedem Moment ist:
	m_mouse.x = x;
	m_mouse.y = y;

}
//andere klicks und mausbewegung
void RigidBodySystemSimulator::onMouse(int x, int y)
{
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;

	//Wo die Maus zu jedem Moment ist:
	m_mouse.x = x;
	m_mouse.y = y;
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
	Vec3 vecToPoint = rbp->pos - loc;
	rbp->torque = rbp->torque + cross(vecToPoint, force);

	//visualize
	Force f = { loc, force };
	ForceVisual fv = { f, FORCETTL };
	forceVisuals.push_back(fv);
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
