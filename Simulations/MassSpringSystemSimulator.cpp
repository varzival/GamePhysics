#include "MassSpringSystemSimulator.h"

void MassSpringSystemSimulator::applyExternalForce(Vec3 force) {
	for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {

		if (m_gravityOn && !m_gotMouseStuff) {
			iterator->force = iterator->mass*9.81*Vec3(0, -0.1, 0);
		}
		else {
			iterator->force = force;
		}
	}
}

void MassSpringSystemSimulator::printVec3(Vec3 vec)
{
	cout << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

void MassSpringSystemSimulator::computeElasticForces(Spring s) {
	Point * p1 = &Points[s.point1];
	Point * p2 = &Points[s.point2];
	Vec3 diff1 = p1->position - p2->position;
	float diff1length = sqrt(diff1[0] * diff1[0] + diff1[1] * diff1[1] + diff1[2] * diff1[2]);
	Vec3 diff1initialLenth = (s.initialLength / diff1length) * diff1;
	p1->force -= s.stiffness*(diff1 - diff1initialLenth);
	p2->force -= s.stiffness*-(diff1 - diff1initialLenth);
	p1->force -= p1->velocity*m_fDamping;
	p2->force -= p2->velocity*m_fDamping;
}

MassSpringSystemSimulator::MassSpringSystemSimulator()
{
	std::vector<Point>* points_s1 = new std::vector<Point>();
	std::vector<Spring>* springs_s1 = new std::vector<Spring>();
	std::vector<Point>* points_s2 = new std::vector<Point>();
	std::vector<Spring>* springs_s2 = new std::vector<Spring>();
	std::vector<Point>* points_s3 = new std::vector<Point>();
	std::vector<Spring>* springs_s3 = new std::vector<Spring>();


	//basic setup
	addMassPointToVector(Vec3(0, 0, 0), Vec3(-1, 0, 0), false, points_s1);
	addMassPointToVector(Vec3(0, 2, 0), Vec3(1, 0, 0), false, points_s1);
	addSpringToVector(0, 1, 1.0f, springs_s1);

	addMassPointToVector(Vec3(0.5, 0.5, 0.5), Vec3(), true, points_s2);
	addMassPointToVector(Vec3(0.1, -0.2, 0.1), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(0.0, 0.2, 0.4), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(-0.5, -0.5, 0.5), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(-0.5, 0.4, 0.1), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(-0.5, 0.1, 0.1), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(-0.5, 0.4, 0.5), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(-0.1, 0.1, 0.1), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(0.0, 0.0, 0.0), Vec3(), false, points_s2);
	addMassPointToVector(Vec3(0.1, 0.1, 0.1), Vec3(), false, points_s2);

	addSpringToVector(0, 1, 0.3, springs_s2);
	addSpringToVector(2, 3, 0.3, springs_s2);
	addSpringToVector(3, 4, 0.3, springs_s2);
	addSpringToVector(4, 5, 0.5, springs_s2);
	addSpringToVector(4, 6, 0.5, springs_s2);
	addSpringToVector(6, 7, 0.5, springs_s2);
	addSpringToVector(7, 8, 0.5, springs_s2);
	addSpringToVector(8, 9, 0.5, springs_s2);
	addSpringToVector(3, 8, 0.5, springs_s2);
	addSpringToVector(3, 7, 0.5, springs_s2);

	addMassPointToVector(Vec3(-0.5, 0.0, 0.5), Vec3(), false, points_s3);
	addMassPointToVector(Vec3(-0.1, 0.0, 0.5), Vec3(), false, points_s3);
	addMassPointToVector(Vec3(-0.3, 0.0, 0.1), Vec3(), false, points_s3);
	addMassPointToVector(Vec3(-0.3, 0.4, 0.3), Vec3(), true, points_s3);

	addSpringToVector(0, 1, 0.4, springs_s3);
	addSpringToVector(0, 2, 0.4, springs_s3);
	addSpringToVector(0, 3, 0.4, springs_s3);
	addSpringToVector(1, 2, 0.4, springs_s3);
	addSpringToVector(1, 3, 0.4, springs_s3);
	addSpringToVector(2, 3, 0.4, springs_s3);

	Setup setup1 = { points_s1, springs_s1, 10.0f, 40.0f, 0.0f, false, false };
	Setup setup2 = { points_s2, springs_s2, 10.0f, 40.0f, 2.0f, true, true };
	Setup setup3 = { points_s3, springs_s3, 5.0f, 100.0f, 1.0f, true, true };

	setups[0] = setup1;
	setups[1] = setup2;
	setups[2] = setup3;

	m_demo1StepSimulated = false;
	m_setupChoice = 0;
	m_setupNr = 0;
	loadSetup(0);

	m_iIntegrator = EULER;
}
MassSpringSystemSimulator::~MassSpringSystemSimulator()
{
	for (int i = 0; i < 2; i++)
	{
		setups[i].Points->clear();
		setups[i].Springs->clear();
	}
	free(setups);
}

const char * MassSpringSystemSimulator::getTestCasesStr()
{
	//return "Euler, Leapfrog, Midpoint";
	return "Demo1, Demo2, Demo3, Demo4";
}

void MassSpringSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
	TwType TW_TYPE_TESTCASE = TwDefineEnumFromString("Setup", "Setup1, Setup2, Setup3");
	TwAddVarRW(DUC->g_pTweakBar, "Setup", TW_TYPE_TESTCASE, &m_setupChoice, "");
	TW_TYPE_TESTCASE = TwDefineEnumFromString("Integrator", "Euler, Leapfrog, Midpoint");
	TwAddVarRW(DUC->g_pTweakBar, "Integration Method", TW_TYPE_TESTCASE, &m_iIntegrator, "");
	TwAddVarRW(DUC->g_pTweakBar, "Gravity", TW_TYPE_BOOLCPP, &m_gravityOn, "");
	TwAddVarRW(DUC->g_pTweakBar, "Collisions", TW_TYPE_BOOLCPP, &m_collisionsOn, "");
}

void MassSpringSystemSimulator::reset()
{
	loadSetup(m_setupNr);
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
}

void MassSpringSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	Vec3 zero = Vec3(0, 0, 0);

	for each (Point point in Points)
	{
		DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, SPHERECOLOR);
		DUC->drawSphere(point.position, SPHERESIZE);
	}
	for each (Spring spring in Springs)
	{
		Point p1 = Points[spring.point1];
		Point p2 = Points[spring.point2];

		//Fancy Spring color effect
		DirectX::XMVECTOR v1 = p1.position.toDirectXVector();
		DirectX::XMVECTOR v2 = p2.position.toDirectXVector();
		DirectX::XMVECTOR l = XMVector3Length(v1 - v2);
		float diff = XMVectorGetX(l); //distance between Points
		float diffFact = (diff - spring.initialLength) / spring.initialLength; //distance in relation to spring length
		float cf; //color factor for second color in [0, 1]
		if (diffFact < 0.0f) cf = 0.0f;
		else {
			cf = (diffFact + 1.0f) / SPRINGCOLORFACTOR;
		}
		if (cf > 1.0f) cf = 1.0f;

		Vec3 resultColor = SPRINGCOLOR1 * (1.0f - cf) + SPRINGCOLOR2 * cf; //linear interpolation of colors

		DUC->setUpLighting(Vec3(), Vec3(), 0, resultColor);
		DUC->beginLine();
		DUC->drawLine(p1.position, resultColor, p2.position, resultColor);
		DUC->endLine();
	}


	if (m_collisionsOn)
	{

		//"Kollision" mit der Box
		for (vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator)
		{
			if (iterator->position.x < -0.5)
			{
				iterator->position.x = -0.5;
				iterator->velocity.x *= -0.5;
			}
			if (iterator->position.x > 0.5)
			{
				iterator->position.x = 0.5;
				iterator->velocity.x *= -0.5;
			}
			if (iterator->position.y < -0.5)
			{
				iterator->position.y = -0.5;
				iterator->velocity.y *= -0.5;
			}
			if (iterator->position.y > 0.5)
			{
				iterator->position.y = 0.5;
				iterator->velocity.y *= -0.5;
			}
			if (iterator->position.z < -0.5)
			{
				iterator->position.z = -0.5;
				iterator->velocity.z *= -0.5;
			}
			if (iterator->position.z > 0.5)
			{
				iterator->position.z = 0.5;
				iterator->velocity.z *= -0.5;
			}
		}

	}

	notifySetupChanged(m_setupChoice);
}
void MassSpringSystemSimulator::notifyCaseChanged(int testCase)
{
	m_iTestCase = testCase;

	switch (m_iTestCase)
	{
	case 0:
		cout << "DEMO 1\n";
		m_demo1StepSimulated = false;
		loadSetup(0);
		m_iIntegrator = EULER;
		m_setupChoice = 0;
		m_setupNr = 0;
		*timeStep = 0.1f;
		break;
	case 1:
		cout << "DEMO 2\n";
		loadSetup(0);
		m_iIntegrator = EULER;
		m_setupChoice = 0;
		m_setupNr = 0;
		*timeStep = 0.005f;
		break;
	case 2:
		cout << "DEMO 3\n";
		loadSetup(0);
		m_iIntegrator = MIDPOINT;
		m_setupChoice = 0;
		m_setupNr = 0;
		*timeStep = 0.005f;
		break;
	case 3:
		cout << "DEMO 4\n";
		loadSetup(1);
		m_iIntegrator = EULER;
		m_setupChoice = 1;
		m_setupNr = 1;
		*timeStep = 0.001f;
		break;
	default:
		cout << "Empty Test!\n";
		loadSetup(0);
		m_iIntegrator = EULER;
		m_setupChoice = 0;
		m_setupNr = 0;
		*timeStep = 0.1f;
		break;
	}
}
void MassSpringSystemSimulator::externalForcesCalculations(float timeElapsed)
{
	Point2D mouseDiff;
	mouseDiff.x = m_trackmouse.x - m_oldtrackmouse.x;
	mouseDiff.y = m_trackmouse.y - m_oldtrackmouse.y;
	if (mouseDiff.x != 0 || mouseDiff.y != 0)
	{
		m_gotMouseStuff = true;
		Mat4 worldViewInv = Mat4(DUC->g_camera.GetWorldMatrix() * DUC->g_camera.GetViewMatrix());
		worldViewInv = worldViewInv.inverse();
		Vec3 inputView = Vec3((float)mouseDiff.x, (float)-mouseDiff.y, 0);
		Vec3 inputWorld = worldViewInv.transformVectorNormal(inputView);
		// find a proper scale!
		float inputScale = 4.f;
		inputWorld = inputWorld * inputScale;
		m_externalForce = inputWorld;
	}
	else
	{
		m_gotMouseStuff = false;
		//Gravity
		if (m_gravityOn) {
			//this value should never be actually used
			m_externalForce = Vec3(0, -100.0, 0);
		}
		else m_externalForce = Vec3();
	}
}

void MassSpringSystemSimulator::simulateTimestep(float timeStep) {
	int i, p;
	switch (m_iIntegrator) {
	case 0:
		//EULER
		//1.Forces
		applyExternalForce(m_externalForce);
		for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
			computeElasticForces(*iterator);
		}
		//2.Movement by velocity
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			if (!(iterator->fixed))
				iterator->position = (Vec3)(iterator->position + (iterator->velocity*timeStep));
		}
		//3.Velocity changes by forces
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			iterator->velocity = iterator->velocity + ((iterator->force / iterator->mass) *timeStep);
		}
		break;
	case 1:
		//Leapfrog
		break;
	case 2:
		//Midpoint
		//saving original values
		p = Points.size();
		Vec3 *xarray, *varray;
		xarray = (Vec3*)malloc(sizeof(Vec3)*p);
		varray = (Vec3*)malloc(sizeof(Vec3)*p);
		//1.Forces
		applyExternalForce(m_externalForce);
		for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
			computeElasticForces(*iterator);
		}
		//2.Movement by velocity
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			if (!(iterator->fixed)) {
				xarray[i++] = iterator->position;
				iterator->position = iterator->position + (iterator->velocity*timeStep / 2);
			}
		}
		//3.Velocity changes by forces
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			if (!(iterator->fixed)) {
				varray[i++] = iterator->velocity;
				iterator->velocity = iterator->velocity + ((iterator->force / iterator->mass) *timeStep / 2);
			}
		}
		//4.Forces
		applyExternalForce(m_externalForce);
		for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
			computeElasticForces(*iterator);
		}
		//5.Movement by velocity
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			if (!(iterator->fixed)) {
				iterator->position = xarray[i++] + (iterator->velocity*timeStep);
			}
		}
		//6.Velocity changes by forces
		i = 0;
		for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
			if (!(iterator->fixed)) {
				iterator->velocity = varray[i++] + ((iterator->force / iterator->mass) *timeStep);
			}
		}
		free(varray);
		free(xarray);
		break;
	default:
		std::cout << "This shouldnt happen!!!!!11ELF" << std::endl;
		break;
	}


	if (m_iTestCase == 0 && !m_demo1StepSimulated)
	{
		m_demo1StepSimulated = true;
		cout << "Position & Velocity after 1 Step:\n";

		for (int i = 0; i < Points.size(); i++) {
			cout << "Point " << i << " position: ";
			printVec3(Points.at(i).position);
			cout << "\nPoint " << i << " velocity: ";
			printVec3(Points.at(i).velocity);
			cout << "\n";
		}
	}
}
//linksklick
void MassSpringSystemSimulator::onClick(int x, int y)
{
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}
//andere klicks und mausbewegung
void MassSpringSystemSimulator::onMouse(int x, int y)
{
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

void MassSpringSystemSimulator::notifySetupChanged(int setupNr)
{
	if (m_setupNr != setupNr)
	{
		m_setupNr = setupNr;
		loadSetup(setupNr);
	}
}

void MassSpringSystemSimulator::loadSetup(int setupNr)
{
	Springs.clear();
	Points.clear();
	Setup setup = setups[setupNr];

	Springs = *setup.Springs;
	Points = *setup.Points;

	setMass(setup.mass);
	setDampingFactor(setup.damping);
	setStiffness(setup.stiffness);
	m_gravityOn = setup.gravity;
	m_collisionsOn = setup.collisions;
}

void MassSpringSystemSimulator::setMass(float mass)
{
	m_fMass = mass;
	for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {
		iterator->mass = mass;
	}
}

void MassSpringSystemSimulator::setStiffness(float stiffness)
{
	m_fStiffness = stiffness;
	for (std::vector<Spring>::iterator iterator = Springs.begin(), end = Springs.end(); iterator != end; ++iterator) {
		iterator->stiffness = stiffness;
	}
}

void MassSpringSystemSimulator::setDampingFactor(float damping)
{
	m_fDamping = damping;
}

int MassSpringSystemSimulator::addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed)
{
	return addMassPointToVector(position, Velocity, isFixed, &Points);
}

int MassSpringSystemSimulator::addMassPointToVector(Vec3 position, Vec3 Velocity, bool isFixed, std::vector<Point>* massVector)
{
	Point p;
	p.position = position;
	p.velocity = Velocity;
	p.mass = m_fMass;
	p.damping = 0;
	p.force = Vec3(0.0, 0.0, 0.0);
	p.fixed = isFixed;
	massVector->push_back(p);
	return massVector->size() - 1;
}

void MassSpringSystemSimulator::addSpring(int masspoint1, int masspoint2, float initialLength)
{
	addSpringToVector(masspoint1, masspoint2, initialLength, &Springs);
}

void MassSpringSystemSimulator::addSpringToVector(int masspoint1, int masspoint2, float initialLength, std::vector<Spring>* springVector)
{
	if (masspoint1 > (Points.size() - 1))
	{
		std::cout << "Could not find point on index " << masspoint1 << "\n";
		return;
	}
	if (masspoint2 > (Points.size() - 1))
	{
		std::cout << "Could not find point on index " << masspoint2 << "\n";
		return;
	}

	Spring s;
	s.initialLength = initialLength;
	s.stiffness = m_fStiffness;
	s.point1 = masspoint1;
	s.point2 = masspoint2;
	springVector->push_back(s);
}

int MassSpringSystemSimulator::getNumberOfMassPoints()
{
	return Points.size();
}

int MassSpringSystemSimulator::getNumberOfSprings()
{
	return Springs.size();
}

Vec3 MassSpringSystemSimulator::getPositionOfMassPoint(int index)
{
	return Points.at(index).position;
}

Vec3 MassSpringSystemSimulator::getVelocityOfMassPoint(int index)
{
	return Points.at(index).velocity;
}
