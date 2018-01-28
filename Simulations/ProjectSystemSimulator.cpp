#include "ProjectSystemSimulator.h"
#include <time.h>
#include <random>

std::function<float(float)> ProjectSystemSimulator::m_Kernels[5] = {
	[](float x) {return 1.0f; },              // Constant, m_iKernel = 0
	[](float x) {return 1.0f - x; },          // Linear, m_iKernel = 1, as given in the exercise Sheet, x = d/2r
	[](float x) {return (1.0f - x)*(1.0f - x); }, // Quadratic, m_iKernel = 2
	[](float x) {return 1.0f / (x)-1.0f; },     // Weak Electric Charge, m_iKernel = 3
	[](float x) {return 1.0f / (x*x) - 1.0f; },   // Electric Charge, m_iKernel = 4
};


const char * ProjectSystemSimulator::getTestCasesStr()
{
	return nullptr;
}

void ProjectSystemSimulator::reset()
{
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
}

ProjectSystemSimulator::ProjectSystemSimulator()
{
	m_iIntegrator = EULER;
	m_fDamping = 1.0f;
	m_fForceScaling = 900.0f;
	m_iTestCase = 0;
	m_iKernel = 1;
	m_fStiffness = 40.0f;
	int num = 6;
	makeBlanket(0.05f, 1.0f, 1.0f, 0.0f, num, 3.0f);
	indexToForce = num * num;
}

void ProjectSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;

	TwAddVarRW(DUC->g_pTweakBar, "Sphere Radius", TW_TYPE_DOUBLE, &dropBallRadius, "step=0.01 min=0.01");
	TwAddVarRW(DUC->g_pTweakBar, "Sphere Number", TW_TYPE_INT32, &numberDropBalls, "step=1 min=1");
	TwAddVarRW(DUC->g_pTweakBar, "Sphere Mass", TW_TYPE_FLOAT, &dropBallMass, "step=0.5 min=0.5");
}

void ProjectSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	Vec3 zero = Vec3(0, 0, 0);

	for (vector<Point>::iterator point = Points.begin(), end = Points.end(); point != end; ++point)
	{
		DUC->setUpLighting(Vec3(), Vec3(1.0f, 1.0f, 1.0f), 0.1, SPHERECOLOR);
		DUC->drawSphere(point->position, point->radius);
	}

	for (vector<Spring>::iterator spring = Springs.begin(), end = Springs.end(); spring != end; ++spring)
	{
		Point p1 = Points[spring->point1];
		Point p2 = Points[spring->point2];

		//Fancy Spring color effect
		DirectX::XMVECTOR v1 = p1.position.toDirectXVector();
		DirectX::XMVECTOR v2 = p2.position.toDirectXVector();
		DirectX::XMVECTOR l = XMVector3Length(v1 - v2);
		float diff = XMVectorGetX(l); //distance between Points
		float diffFact = (diff - spring->initialLength) / spring->initialLength; //distance in relation to spring length
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

}

void ProjectSystemSimulator::computeElasticForces(Spring s) {
	Point * p1 = &Points[s.point1];
	Point * p2 = &Points[s.point2];
	Vec3 diff1 = p1->position - p2->position;
	float diff1length = sqrt(diff1[0] * diff1[0] + diff1[1] * diff1[1] + diff1[2] * diff1[2]);
	Vec3 diff1initialLenth = (s.initialLength / diff1length) * diff1;
	p1->force -= m_fStiffness * (diff1 - diff1initialLenth);
	p2->force -= m_fStiffness * -(diff1 - diff1initialLenth);
	p1->force -= p1->velocity*m_fDamping;
	p2->force -= p2->velocity*m_fDamping;
}

void ProjectSystemSimulator::applyExternalForce() {
	for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator) {

		if (m_gravityOn && (!m_gotMouseStuff || (std::distance(Points.begin(), iterator)) < indexToForce)) {
			iterator->force = iterator->mass*9.81*Vec3(0, -0.1, 0);
		}
		else {
			iterator->force = m_externalForce;
		}
	}
}


void ProjectSystemSimulator::externalForcesCalculations(float timeElapsed)
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


void ProjectSystemSimulator::checkCollisionsNaive()
{
	for (std::vector<Point>::iterator iterator = Points.begin(), end = Points.end(); iterator != end; ++iterator)
	{
		for (std::vector<Point>::iterator iterator2 = Points.begin(), end = Points.end(); iterator2 != end; ++iterator2)
		{
			float distance = norm(iterator->position - iterator2->position);
			if (distance < iterator->radius + iterator2->radius)
			{
				float x = distance / (iterator->radius + iterator2->radius);
				float forceFactor = m_fForceScaling * m_Kernels[m_iKernel](x);
				Vec3 force = forceFactor * (iterator->position - iterator2->position);
				iterator->force += force;
			}
		}
	}
}

void ProjectSystemSimulator::simulateTimestep(float timeStep)
{
	int i, p;
	switch (m_iIntegrator) {
	case 0:
		//EULER
		//1.Forces
		applyExternalForce();
		checkCollisionsNaive();
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
		applyExternalForce();
		checkCollisionsNaive();
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
		applyExternalForce();
		checkCollisionsNaive();
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
}

void ProjectSystemSimulator::notifyCaseChanged(int testCase)
{
}

void ProjectSystemSimulator::onClick(int x, int y)
{
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

void ProjectSystemSimulator::onMouse(int x, int y)
{
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}



int ProjectSystemSimulator::addMassPoint(Vec3 position, Vec3 Velocity, float radius, float mass, bool isFixed)
{
	Point p;
	p.position = position;
	p.velocity = Velocity;
	p.mass = mass;
	p.radius = radius;
	p.damping = 0;
	p.force = Vec3(0.0, 0.0, 0.0);
	p.fixed = isFixed;
	Points.push_back(p);
	return Points.size() - 1;
}

void ProjectSystemSimulator::addSpring(int masspoint1, int masspoint2, float initialLength)
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
	Springs.push_back(s);
}

int ProjectSystemSimulator::getNumberOfMassPoints()
{
	return Points.size();
}

int ProjectSystemSimulator::getNumberOfSprings()
{
	return Springs.size();
}

Vec3 ProjectSystemSimulator::getPositionOfMassPoint(int index)
{
	return Points.at(index).position;
}

Vec3 ProjectSystemSimulator::getVelocityOfMassPoint(int index)
{
	return Points.at(index).velocity;
}

void ProjectSystemSimulator::makeBlanket(float radius, float mass, float width, float level, int num, float hangingFactor)
{
	Points.clear();

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			float initX = -(width / 2.0f);
			float initZ = -(width / 2.0f);
			float posX = initX + (width / (float)num) * i;
			float posZ = initZ + (width / (float)num) * j;
			bool fixed;
			if ((i == 0 && j == 0) || (i == num - 1 && j == 0) || (i == 0 && j == num - 1) || (i == num - 1 && j == num - 1)) fixed = true;
			else fixed = false;
			addMassPoint(Vec3(posX, level, posZ), Vec3(), radius, mass, fixed);
		}
	}

	addMassPoint(Vec3(0.0, 0.5, 0.0), Vec3(0.0, -1.0, 0.0), dropBallRadius, dropBallMass, false);

	for (int i = 0; i < numberDropBalls; i++)
	{
		std::mt19937 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist10(0, 10);
		float x = (dist10(rng) - 5.0f) / 10.0f;

		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937::result_type> dist11(0, 10);
		float z = (dist11(rng) - 5.0f) / 10.0f;
		addMassPoint(Vec3(x, 0.5, z), Vec3(0.0, -1.0, 0.0), dropBallRadius, dropBallMass, false);
	}

	int springLength = hangingFactor * (width / (float)num);


	//Springs
	for (int i = 0; i < num*num; i++)
	{
		if (i == 0)
		{
			addSpring(0, 1, springLength);
			addSpring(0, num, springLength);
		}
		else if (i == num - 1)
		{
			addSpring(num - 1, num - 2, springLength);
			addSpring(num - 1, 2 * num - 1, springLength);
		}
		else if (i == num * num - 1)
		{
			addSpring(num*num - 1, num*num - 2, springLength);
			addSpring(num*num - 1, num*num - 1 - num, springLength);
		}
		else if (i == num * num - num)
		{
			addSpring(num*num - num, num*num - num + 1, springLength);
			addSpring(num*num - num, num*num - 2 * num, springLength);
		}
		else if (i % num == 0)
		{
			//left edge
			addSpring(i, i - num, springLength);
			addSpring(i, i + num, springLength);
			addSpring(i, i + 1, springLength);
		}
		else if (i % num == num - 1)
		{
			//right edge
			addSpring(i, i - num, springLength);
			addSpring(i, i + num, springLength);
			addSpring(i, i - 1, springLength);
		}
		else if (i / num < 1)
		{
			//bottom edge
			addSpring(i, i - 1, springLength);
			addSpring(i, i + 1, springLength);
			addSpring(i, i + num, springLength);
		}
		else if (i / num >= num - 1)
		{
			//top edge
			addSpring(i, i - 1, springLength);
			addSpring(i, i + 1, springLength);
			addSpring(i, i - num, springLength);
		}
		else
		{
			addSpring(i - 1, i, springLength);
			addSpring(i + 1, i, springLength);
			addSpring(i + num, i, springLength);
			addSpring(i - num, i, springLength);
		}
	}

}
