#include "BasicDemo.h"

void BasicDemo::InitializePhysics() {
	// create the collision configuration
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	// create the dispatcher
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
	// create the broadphase
	m_pBroadphase = new btDbvtBroadphase();
	// create the constraint solver
	m_pSolver = new btSequentialImpulseConstraintSolver();
	// create the world
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);
	// create our scene's physics objects
	CreateObjects();
}

void BasicDemo::ShutDownPhysics() {
	delete m_pWorld;
	delete m_pSolver;
	delete m_pBroadphase;
	delete m_pDispatcher;
	delete m_pCollisionConfiguration;
}



//这个是从存取的position.txt的值来读取x,y,z
void BasicDemo::ReadBuffer()
{
	//ifstream in("position.txt");
	//if (in)
	//{
	//	float pos[3];
	//	string line;
	//	for (unsigned dim = 0; dim < 3; dim++)
	//	{
	//		in >> line;
	//		pos[dim] = atof(line.c_str());
	//	}
	//	probeModel.x = pos[0];
	//	probeModel.y = pos[1];
	//	probeModel.z = pos[2];
	//}
}
