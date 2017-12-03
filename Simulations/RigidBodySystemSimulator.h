#ifndef RIGIDBODYSYSTEMSIMULATOR_h
#define RIGIDBODYSYSTEMSIMULATOR_h
#include "Simulator.h"
#include "matrixbase.h"
#include "quaternion.h"
#include "rigidBody.h"
#include "collisionDetect.h"

//add your header for your rigid body system, for e.g.,
//#include "rigidBodySystem.h" 

#define TESTCASEUSEDTORUNTEST 2

#define BOXCOLOR Vec3(0.0f, 1.0f, 0.0f)
#define FORCEPOINTCOLOR Vec3(0.0f, 0.0f, 1.0f)
#define FORCELINECOLOR Vec3(1.0f, 1.0f, 0.0f)

#define FORCEPOINTRADIUS 0.05f
#define FORCETTL 3.0f

struct ForceVisual
{
	Force force;
	float ttl;
};

class RigidBodySystemSimulator :public Simulator {
public:
	float * timeStep;
	// Construtors
	RigidBodySystemSimulator();

	// Functions
	const char * getTestCasesStr();
	void initUI(DrawingUtilitiesClass * DUC);
	void reset();
	void drawFrame(ID3D11DeviceContext* pd3dImmediateContext);
	void notifyCaseChanged(int testCase);
	void externalForcesCalculations(float timeElapsed);
	void simulateTimestep(float timeStep);
	void onClick(int x, int y);
	void onMouse(int x, int y);

	// ExtraFunctions
	int getNumberOfRigidBodies();
	Vec3 getPositionOfRigidBody(int i);
	Vec3 getLinearVelocityOfRigidBody(int i);
	Vec3 getAngularVelocityOfRigidBody(int i);
	void applyForceOnBody(int i, Vec3 loc, Vec3 force);
	void addRigidBody(Vec3 position, Vec3 size, int mass);
	void setOrientationOf(int i, Quat orientation);
	void setVelocityOf(int i, Vec3 velocity);

	//UI
	void loadDemo();

private:
	// Attributes
	// add your RigidBodySystem data members, for e.g.,
	// RigidBodySystem * m_pRigidBodySystem; 
	Vec3 m_externalForce;

	// UI Attributes
	Point2D m_mouse;
	Point2D m_trackmouse;
	Point2D m_oldtrackmouse;

	vector<rigidBody> rigidBodies;
	//visualize forces
	bool m_forceVisalsOn;
	bool m_bodyVisualsOn;
	vector<ForceVisual> forceVisuals;

	//Gibt an welche Demo abgespielt werden soll
	int demoChoice = 0;
	int oldDemoChoice;
	bool demo1Printed = false;
};
#endif

