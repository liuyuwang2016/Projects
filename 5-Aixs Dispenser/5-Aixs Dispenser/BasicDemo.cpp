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

void BasicDemo::CreateObjects() {
	//ReadBuffer();

	// create our original red box
	CreateGameObject(new btBoxShape(btVector3(0.03, 0.1, 0.1)), 0, btVector3(1.0f, 0.2f, 0.2f), btVector3(0.0124319, -0.124894, 0.780611));

	// create a second box
	//CreateGameObject(new btBoxShape(btVector3(0.01, 0.01, 0.01)), 0, btVector3(0.0f, 0.2f, 0.8f), btVector3(0.0124319, -0.124894, 0.780611));
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


