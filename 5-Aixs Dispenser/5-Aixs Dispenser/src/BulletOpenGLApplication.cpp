//
// Created by AICDG on 2017/8/8.
//

#include "BulletOpenGLApplication.h"

BulletOpenGLApplication::BulletOpenGLApplication() :
		m_pBroadphase(nullptr),
		m_pCollisionConfiguration(nullptr),
		m_pDispatcher(nullptr),
		m_pSolver(nullptr),
		m_pWorld(nullptr),
		m_pMotionState(nullptr)
{
}
BulletOpenGLApplication::~BulletOpenGLApplication()
{
	ShutDownPhysics();
}
void BulletOpenGLApplication::Initialize()
{
	InitializePhysics();
}

void BulletOpenGLApplication::Keyboard(unsigned char key, int x, int y) {}
void BulletOpenGLApplication::KeyboardUp(unsigned char key, int x, int y) {}
void BulletOpenGLApplication::Special(int key, int x, int y) {}
void BulletOpenGLApplication::SpecialUp(int key, int x, int y) {}
void BulletOpenGLApplication::Reshape(int w, int h) {}

void BulletOpenGLApplication::Idle()
{
	// get the time since the last iteration
	float dt = m_clock.getTimeMilliseconds();
	// reset the clock to 0
	m_clock.reset();
	// update the scene (convert ms to s)
	UpdateBulletScene(dt / 1000.0f);
	RenderBulletScene();
}

void BulletOpenGLApplication::Mouse(int button, int state, int x, int y) {}
void BulletOpenGLApplication::PassiveMotion(int x, int y) {}
void BulletOpenGLApplication::Motion(int x, int y) {}
void BulletOpenGLApplication::Display() {}

void BulletOpenGLApplication::DrawBox(btScalar* transform, const btVector3 &halfSize, const btVector3 &color /*= btVector3(1.0f, 1.0f, 1.0f)*/)
{
	glPushMatrix();
	glMultMatrixf(transform);

	float halfWidth = halfSize.x();
	float halfHeight = halfSize.y();
	float halfDepth = halfSize.z();

	// set the object's color
	glColor3f(color.x(), color.y(), color.z());

	// create the vertex positions
	btVector3 vertices[8] = {
		btVector3(halfWidth, halfHeight, halfDepth),
		btVector3(-halfWidth, halfHeight, halfDepth),
		btVector3(halfWidth, -halfHeight, halfDepth),
		btVector3(-halfWidth, -halfHeight, halfDepth),
		btVector3(halfWidth, halfHeight, -halfDepth),
		btVector3(-halfWidth, halfHeight, -halfDepth),
		btVector3(halfWidth, -halfHeight, -halfDepth),
		btVector3(-halfWidth, -halfHeight, -halfDepth) };

	// create the indexes for each triangle, using the
	// vertices above. Make it static so we don't waste 
	// processing time recreating it over and over again
	static int indices[36] = {
		0, 1, 2,
		3, 2, 1,
		4, 0, 6,
		6, 0, 2,
		5, 1, 4,
		4, 1, 0,
		7, 3, 1,
		7, 1, 5,
		5, 4, 7,
		7, 4, 6,
		7, 2, 3,
		7, 6, 2 };

	// start processing vertices as triangles
	glBegin(GL_TRIANGLES);

	// increment the loop by 3 each time since we create a
	// triangle with 3 vertices at a time.

	for (int i = 0; i < 36; i += 3) {
		// get the three vertices for the triangle based
		// on the index values set above
		// use const references so we don't copy the object
		// (a good rule of thumb is to never allocate/deallocate
		// memory during *every* render/update call. This should 
		// only happen sporadically)
		const btVector3 &vert1 = vertices[indices[i]];
		const btVector3 &vert2 = vertices[indices[i + 1]];
		const btVector3 &vert3 = vertices[indices[i + 2]];

		// create a normal that is perpendicular to the
		// face (use the cross product)
		btVector3 normal = (vert3 - vert1).cross(vert2 - vert1);
		normal.normalize();

		// set the normal for the subsequent vertices
		glNormal3f(normal.getX(), normal.getY(), normal.getZ());

		// create the vertices
		glVertex3f(vert1.x(), vert1.y(), vert1.z());
		glVertex3f(vert2.x(), vert2.y(), vert2.z());
		glVertex3f(vert3.x(), vert3.y(), vert3.z());
	}

	// stop processing vertices
	glEnd();
	// pop the transform from the stack in preparation
	// for the next object
	glPopMatrix();
}


void BulletOpenGLApplication::RenderBulletScene()
{
	// create an array of 16 floats (representing a 4x4 matrix)
	btScalar transform[16];
	if (m_pMotionState) {
		// get the world transform from our motion state
		m_pMotionState->GetWorldTransform(transform);
		// feed the data into DrawBox
		glTranslatef(0, 0, 10);
		DrawBox(transform, btVector3(1, 1, 1), btVector3(1.0f, 0.2f, 0.2f));
	}
}


void BulletOpenGLApplication::UpdateBulletScene(float dt)
{
	// check if the world object exists
	if (m_pWorld) {
		// step the simulation through time. This is called
		// every update and the amount of elasped time was 
		// determined back in ::Idle() by our clock object.
		m_pWorld->stepSimulation(dt);
	}
}

