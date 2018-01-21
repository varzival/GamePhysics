#pragma once

#include "Simulator.h"

#define EULER 0
#define LEAPFROG 1
#define MIDPOINT 2

//Rendering
#define SPHERECOLOR Vec3(0.0f, 1.0f, 0.0f)
#define SPRINGCOLOR1 Vec3(1.0f, 1.0f, 0.0f)
#define SPRINGCOLOR2 Vec3(1.0f, 0.0f, 0.0f)
#define SPRINGCOLORFACTOR 2.0f;


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
	bool fixed;
	float radius;
};

class ProjectSystemSimulator : public Simulator
{
public:
	// Attributes
	int m_iTestCase;
	DrawingUtilitiesClass * DUC;
	// Functions

	const char * getTestCasesStr();

	void reset();

	ProjectSystemSimulator();

	/*
	This Function is used to initialize the GUI with customized componnents per simulations
	input: DUC pointer for the DrawingUtilities to setup the UI componnents
	**for more info on how to use this function take a look at the template simulator
	*/
	void initUI(DrawingUtilitiesClass * DUC);
	/*
	This Function is used to render one frame from the simulation called by the simulator enviroment
	input: pd3dImmediateContext pointer used only in case of drawing
	an object from scratch but usually we have all the needed
	drawing functions in the DrawingUtilityClass
	**for more info on what functions avaialble to use take a look at the DrawingUtilities Class
	*/
	void drawFrame(ID3D11DeviceContext* pd3dImmediateContext);
	void computeElasticForces(Spring s);
	void applyExternalForce();
	/*
	This Function is used to calculate  called by the simulator enviroment
	input: fElapsedTime is the elapsed time since the last timestep
	*/
	void externalForcesCalculations(float timeElapsed);
	void checkCollisionsNaive();
	/*
	This Function is used to simulate one step of the simulation called by the simulator enviroment
	input: timeStep is the time step of the stimulation
	*/
	void simulateTimestep(float timeStep);
	/*
	This Function is used to notify the simulator that the scene test case is changed
	so that the needed changes can be handed here
	**for more info on how to use this function take a look at the template simulator
	*/
	void notifyCaseChanged(int testCase);

	/*
	this function is used to receive the input from the mouse
	input x,y: the click position in pixels
	*/
	void onClick(int x, int y);

	/*
	this function is used to receive the input from the mouse
	input x,y: the mouse position in pixels
	*/
	void onMouse(int x, int y);
	int addMassPoint(Vec3 position, Vec3 Velocity, float radius, float mass, bool isFixed);
	void addSpring(int masspoint1, int masspoint2, float initialLength);
	void addSpringToVector(int masspoint1, int masspoint2, float initialLength, std::vector<Spring>* springVector);
	int getNumberOfMassPoints();
	int getNumberOfSprings();
	Vec3 getPositionOfMassPoint(int index);
	Vec3 getVelocityOfMassPoint(int index);
	void makeBlanket(float radius, float mass, float length, float level, int num);


private:
	int m_iKernel;
	float m_fForceScaling;
	float m_fStiffness;
	float m_fDamping;
	int m_iIntegrator;

	// UI Attributes
	Vec3 m_externalForce;
	Point2D m_mouse;
	Point2D m_trackmouse;
	Point2D m_oldtrackmouse;
	int m_setupNr;
	int m_setupChoice;
	bool m_demo1StepSimulated;
	bool m_gravityOn;
	bool m_collisionsOn;
	bool m_gotMouseStuff;

	std::vector<Spring> Springs;
	std::vector<Point> Points;

	static std::function<float(float)> m_Kernels[5];

};
