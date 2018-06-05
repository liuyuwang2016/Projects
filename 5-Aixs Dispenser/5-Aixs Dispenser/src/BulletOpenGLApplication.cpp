//
// Created by AICDG on 2017/8/8.
//

#include "BulletOpenGLApplication.h"
// Some constants for 3D math and the camera speed
#define RADIANS_PER_DEGREE 0.01745329f
#define CAMERA_STEP_SIZE 5.0f

BulletOpenGLApplication::BulletOpenGLApplication() :
		m_cameraPosition(-0.064403, 0.000998867, 0.0897838),
		m_cameraTarget(-0.0494023, 0.0210018, 1.08971),
		m_upVector(-0.000199958, 0.99968, -0.019995),
		m_nearPlane(0.01f),
		m_farPlane(50.0f),
		m_pBroadphase(nullptr),
		m_pCollisionConfiguration(nullptr),
		m_pDispatcher(nullptr),
		m_pSolver(nullptr),
		m_pWorld(nullptr)
{
}

BulletOpenGLApplication::~BulletOpenGLApplication()
{
	ShutDownPhysics();
}

void BulletOpenGLApplication::Initialize()
{
	InitializePhysics();
	// create the debug drawer
	m_pDebugDrawer = new DebugDrawer();
	// set the initial debug level to 0
	m_pDebugDrawer->setDebugMode(0);
	// add the debug drawer to the world
	m_pWorld->setDebugDrawer(m_pDebugDrawer);
}

void BulletOpenGLApplication::Keyboard(unsigned char key, int x, int y)
{
	switch (key){
	case 'n':
		// toggle wireframe debug drawing
		m_pDebugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);
		break;
	case 'm':
		// toggle AABB debug drawing
		m_pDebugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawAabb);
		break;
	case 'd':
		// create a temp object to store the raycast result
		RayResult result;
		// perform the raycast
		if (!Raycast(m_cameraPosition, GetPickingRay(x, y), result))
			return; // return if the test failed
		// destroy the corresponding game object
		DestroyGameObject(result.pBody);
		break;
	}
}

void BulletOpenGLApplication::KeyboardUp(unsigned char key, int x, int y) {}

void BulletOpenGLApplication::Special(int key, int x, int y) {}

void BulletOpenGLApplication::SpecialUp(int key, int x, int y) {}

void BulletOpenGLApplication::Reshape(int w, int h)
{
	// grab the screen width/height
	m_screenWidth = w;
	m_screenHeight = h;
	// set the viewport
	glViewport(0, 0, w, h);
}

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

void BulletOpenGLApplication::Mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case 1://middle mouse button
		{
			if (state == 0)//pressed down
			{
				//shooting a box
				ShootBox(GetPickingRay(x, y));
			}
			break;
		}
	}
}

void BulletOpenGLApplication::PassiveMotion(int x, int y) {}

void BulletOpenGLApplication::Motion(int x, int y)
{
	// did we pick a body with the LMB?
	if (m_pPickedBody) {
		btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>(m_pPickConstraint);
		if (!pickCon)
			return;

		// use another picking ray to get the target direction
		btVector3 dir = GetPickingRay(x, y) - m_cameraPosition;
		dir.normalize();

		// use the same distance as when we originally picked the object
		dir *= m_oldPickingDist;
		btVector3 newPivot = m_cameraPosition + dir;

		// set the position of the constraint
		pickCon->getFrameOffsetA().setOrigin(newPivot);
	}
}

void BulletOpenGLApplication::Display() {}

void BulletOpenGLApplication::DrawBox(const btVector3 &halfSize)
{
	float halfWidth = halfSize.x();
	float halfHeight = halfSize.y();
	float halfDepth = halfSize.z();

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
}

void BulletOpenGLApplication::RenderBulletScene()
{
	// create an array of 16 floats (representing a 4x4 matrix)
	btScalar transform[16];
	// iterate through all of the objects in our world
	for (GameObjects::iterator i = m_objects.begin(); i != m_objects.end(); ++i) {
		// get the object from the iterator
		GameObject* pObj = *i;
		// read the transform
		pObj->GetTransform(transform);

		// get data from the object and draw it
		DrawShape(transform, pObj->GetShape(), pObj->GetColor());
	}
	// after rendering all game objects, perform debug rendering
	// Bullet will figure out what needs to be drawn then call to
	// our DebugDrawer class to do the rendering for us
	m_pWorld->debugDrawWorld();
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

void BulletOpenGLApplication::DrawShape(btScalar* transform, const btCollisionShape* pShape, const btVector3 &color) {
	// set the color
	glColor3f(color.x(), color.y(), color.z());

	// push the matrix stack
	glPushMatrix();
	glMultMatrixf(transform);

	// make a different draw call based on the object type
	switch (pShape->getShapeType()) {
		// an internal enum used by Bullet for boxes
	case BOX_SHAPE_PROXYTYPE:
	{
		// assume the shape is a box, and typecast it
		const btBoxShape* box = static_cast<const btBoxShape*>(pShape);
		// get the 'halfSize' of the box
		btVector3 halfSize = box->getHalfExtentsWithMargin();
		// draw the box
		DrawBox(halfSize);
		break;
	}
	default:
		// unsupported type
		break;
	}

	// pop the stack
	glPopMatrix();
}

GameObject* BulletOpenGLApplication::CreateGameObject(btCollisionShape* pShape, const float &mass, const btVector3 &color, const btVector3 &initialPosition, const btQuaternion &initialRotation) {
	// create a new game object
	GameObject* pObject = new GameObject(pShape, mass, color, initialPosition, initialRotation);

	// push it to the back of the list
	m_objects.push_back(pObject);

	// check if the world object is valid
	if (m_pWorld) {
		// add the object's rigid body to the world
		m_pWorld->addRigidBody(pObject->GetRigidBody());
	}
	return pObject;
}

btVector3 BulletOpenGLApplication::GetPickingRay(int x, int y) {
	// calculate the field-of-view
	float tanFov = 1.0f / m_nearPlane;
	float fov = btScalar(2.0) * btAtan(tanFov);

	// get a ray pointing forward from the
	// camera and extend it to the far plane
	btVector3 rayFrom = m_cameraPosition;
	btVector3 rayForward = (m_cameraTarget - m_cameraPosition);
	rayForward.normalize();
	rayForward *= m_farPlane;

	// find the horizontal and vertical vectors
	// relative to the current camera view
	btVector3 ver = m_upVector;
	btVector3 hor = rayForward.cross(ver);
	hor.normalize();
	ver = hor.cross(rayForward);
	ver.normalize();
	hor *= 2.f * m_farPlane * tanFov;
	ver *= 2.f * m_farPlane * tanFov;

	// calculate the aspect ratio
	btScalar aspect = m_screenWidth / (btScalar)m_screenHeight;

	// adjust the forward-ray based on
	// the X/Y coordinates that were clicked
	hor *= aspect;
	btVector3 rayToCenter = rayFrom + rayForward;
	btVector3 dHor = hor * 1.f / float(m_screenWidth);
	btVector3 dVert = ver * 1.f / float(m_screenHeight);
	btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * ver;
	
	rayTo += btScalar(x) * dHor;
	rayTo -= btScalar(y) * dVert;

	// return the final result
	return rayTo;
}

void BulletOpenGLApplication::ShootBox(const btVector3 &direction) {
	// create a new box object
	GameObject* pObject = CreateGameObject(new btBoxShape(btVector3(1, 1, 1)), 1, btVector3(0.4f, 0.f, 0.4f), m_cameraPosition);

	// calculate the velocity
	btVector3 velocity = direction;
	velocity.normalize();
	velocity *= 25.0f;

	// set the linear velocity of the box
	pObject->GetRigidBody()->setLinearVelocity(velocity);
}

bool BulletOpenGLApplication::Raycast(const btVector3 &startPosition, const btVector3 &direction, RayResult &output) {
	if (!m_pWorld)
		return false;

	// get the picking ray from where we clicked
	btVector3 rayTo = direction;
	btVector3 rayFrom = m_cameraPosition;

	// create our raycast callback object
	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);

	// perform the raycast
	m_pWorld->rayTest(rayFrom, rayTo, rayCallback);

	// did we hit something?
	if (rayCallback.hasHit())
	{
		// if so, get the rigid body we hit
		btRigidBody* pBody = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
		if (!pBody)
			return false;

		// prevent us from picking objects
		// like the ground plane
		if (pBody->isStaticObject() || pBody->isKinematicObject())
			return false;

		// set the result data
		output.pBody = pBody;
		output.hitPoint = rayCallback.m_hitPointWorld;
		return true;
	}

	// we didn't hit anything
	return false;
}

void BulletOpenGLApplication::DestroyGameObject(btRigidBody* pBody) {
	// we need to search through the objects in order to 
	// find the corresponding iterator (can only erase from 
	// an std::vector by passing an iterator)
	for (GameObjects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter) {
		if ((*iter)->GetRigidBody() == pBody) {
			GameObject* pObject = *iter;
			// remove the rigid body from the world
			m_pWorld->removeRigidBody(pObject->GetRigidBody());
			// erase the object from the list
			m_objects.erase(iter);
			// delete the object from memory
			delete pObject;
			// done
			return;
		}
	}
} 