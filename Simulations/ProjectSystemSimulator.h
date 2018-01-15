#pragma once

#include "Simulator.h"

class ProjectSystemSimulator : public Simulator
{

public:
	// Attributes
	int m_iTestCase;
	DrawingUtilitiesClass * DUC;
	// Functions

	const char * getTestCasesStr();

	void reset();

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
	/*
	This Function is used to calculate  called by the simulator enviroment
	input: fElapsedTime is the elapsed time since the last timestep
	*/
	void externalForcesCalculations(float timeElapsed);
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

};
