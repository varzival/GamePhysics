#include "RigidBodySystemSimulator.h"
#include <math.h>

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
		*timeStep = 0.001f;
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
						Vec3 relVel = (it->vel) - (it2->vel);
						Vec3 out = (relVel * info.normalWorld);
						double end = out.x + out.y + out.z;
						//Boxes are seperating
						if (end > 0.0)
						{
							//Hier muss man glaub gar nichts machen
							cout << "Boxes are seperating\n";
						}
						//Boxes are colliding
						else if (end < 0.0)
						{
							//TODO
							//Inertia Tensor berechnen
							/*matrix4x4<Real> I = matrix4x4<Real>(
								info.collisionPointWorld.x, 0, 0, 0,
								0, 1, 0, 0,
								0, 0, 1, 0,
								0, 0, 0, 0);
							//Impuls berechnen
							double j = (-1 * end)/(1/it->mass + 1/it2->mass + []);*/
							cout << "Boxes are colliding\n";
						}
						else
						{
							cout << "Boxes are sliding\n ";
						}
						//Kollision erkannt einfluss hier berechnen
						
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
		//rotMat.transpose(); transpose back

		it->angVel = invIns.transformVector(it->angMom);

		//apply rotation
		//does it make sense to update position and velocity of every point with rotation?

		//reset force and torque
		it->totalForce = Vec3(0.0f, 0.0f, 0.0f);
		it->torque = Vec3(0.0f, 0.0f, 0.0f);
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
