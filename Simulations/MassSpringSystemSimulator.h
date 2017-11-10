#ifndef MASSSPRINGSYSTEMSIMULATOR_h
#define MASSSPRINGSYSTEMSIMULATOR_h
#include "Simulator.h"
#include <list>
#include <iterator>
#include <math.h>

// Do Not Change
#define EULER 0
#define LEAPFROG 1
#define MIDPOINT 2
// Do Not Change

//Rendering
#define SPHERECOLOR Vec3(0.0f, 1.0f, 0.0f)
#define SPHERESIZE Vec3(0.05f, 0.05f, 0.05f)
#define SPRINGCOLOR Vec3(1.0f, 1.0f, 0.0f)


//Represents a single Spring
struct Spring {
	int point1;
	int point2;
	float stiffness;
	float initialLength;
};
//Represents a single Point
struct Point
{
	Vec3 position;
	Vec3 velocity;
	Vec3 force;
	float mass;
	float damping;
};

//Represents a Spring/Masspoint constellation
struct Setup
{
	std::vector<Point>* Points;
	std::vector<Spring>* Springs;
	float mass;
	float stiffness;
	float damping;
};

class MassSpringSystemSimulator :public Simulator {
public:
	// Construtors
	MassSpringSystemSimulator();

	// UI Functions
	const char * getTestCasesStr();
	void initUI(DrawingUtilitiesClass * DUC);
	void reset();
	void drawFrame(ID3D11DeviceContext* pd3dImmediateContext);
	void notifyCaseChanged(int testCase);
	void externalForcesCalculations(float timeElapsed);
	void simulateTimestep(float timeStep);
	void onClick(int x, int y);
	void onMouse(int x, int y);
	void notifySetupChanged(int setupNr);
	void loadSetup(int setupNr);

	// Specific Functions
	void setMass(float mass);
	void setStiffness(float stiffness);
	void setDampingFactor(float damping);
	int addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed);
	int addMassPointToVector(Vec3 position, Vec3 Velocity, bool isFixed, std::vector<Point>* massVector);
	void addSpring(int masspoint1, int masspoint2, float initialLength);
	void addSpringToVector(int masspoint1, int masspoint2, float initialLength, std::vector<Spring>* springVector);
	int getNumberOfMassPoints();
	int getNumberOfSprings();
	Vec3 getPositionOfMassPoint(int index);
	Vec3 getVelocityOfMassPoint(int index);
	void applyExternalForce(Vec3 force);

	//should add Forces to Points in Spring
	void computeElasticForces(Spring s);


	// Do Not Change
	void setIntegrator(int integrator) {
		m_iIntegrator = integrator;
	}

private:
	// Data Attributes
	float m_fMass;
	float m_fStiffness;
	float m_fDamping;
	int m_iIntegrator;

	// UI Attributes
	Vec3 m_externalForce;
	Point2D m_mouse;
	Point2D m_trackmouse;
	Point2D m_oldtrackmouse;
	int m_setupNr;
	int m_msetupChoice;

	//Variables
	std::vector<Spring> Springs;
	std::vector<Point> Points;

	Setup* setups;

};
#endif