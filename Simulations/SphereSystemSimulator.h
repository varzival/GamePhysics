#ifndef SPHSYSTEMSIMULATOR_h
#define SPHSYSTEMSIMULATOR_h
#include "Simulator.h"
#include "spheresystem.h"
#include <vector>

#define NAIVEACC 0
#define GRIDACC 1

class SphereSystemSimulator :public Simulator {
public:
	// Construtors
	SphereSystemSimulator();
	~SphereSystemSimulator();
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

protected:
	// Attributes
	Vec3 m_externalForce;
	Point2D m_mouse;
	Point2D m_trackmouse;
	Point2D m_oldtrackmouse;
	float m_fMass;
	float m_fRadius;
	float m_fForceScaling;
	float m_fDamping;
	int   m_iNumSpheres;

	bool m_simulateGridSystem;
	bool m_gravityOn;
	bool m_gotMouseStuff;
	int m_ioldNumSpheres;

	int m_iCollision = 0;//to switch between methods might be the same as iAccelerator
	int m_iKernel; // index of the m_Kernels[5], more detials in SphereSystemSimulator.cpp
	static std::function<float(float)> m_Kernels[5];

	int   m_iAccelerator; // switch between NAIVEACC and GRIDACC, (optionally, KDTREEACC, 2)

	SphereSystem * m_pSphereSystem; // add your own sphere system member!
	// for Demo 3 only:
	// you will need multiple SphereSystem objects to do comparisons in Demo 3
	// m_iAccelerator should be ignored.
	//TODO initialisieren
	SphereSystem * m_pSphereSystemGrid;

	//Performace mesure variables
	int measureNum = 0;
	double accTime = 0.0;

	int measureNum2 = 0;
	double accTime2 = 0.0;
	int max = 1000;

	void drawSystem(SphereSystem * system);
	void simulateSystem(SphereSystem * system, float timeStep);
	void applyExternalForce(SphereSystem * system);
	void populateSystem(SphereSystem * system);
	void checkCollisionsNaive(SphereSystem * system);
	void checkCollisionsUniform(SphereSystem * system);
	void collisionBetweenTwoSpheres(std::vector<Point>::iterator a, Point b);
	int ThreeDToOneD(Vec3 pos);
	int surroundingCubeOffset(int i);
	void notifyNumberChanged();
};

#endif