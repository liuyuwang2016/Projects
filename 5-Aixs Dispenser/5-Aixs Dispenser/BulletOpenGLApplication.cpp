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
	// 在这里遍历我们的虚拟世界中的所有虚拟物体
	for (GameObjects::iterator i = m_objects.begin(); i != m_objects.end(); ++i) {
		// get the object from the iterator
		GameObject* pObj = *i;
		// 在这里读取每个GameObject的transform矩阵，决定要把GameObject渲染到哪个地方
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
		CheckForCollisionEvents();
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
	case SPHERE_SHAPE_PROXYTYPE:
	{
		//假定形状是一个球型，那就转化它
		const btSphereShape* sphere = static_cast<const btSphereShape*>(pShape);
		//从形状拿到球体的大小
		float radius = sphere->getMargin();
		//将球体画出来
		DrawSphere(radius);
		break;
	}
	case CYLINDER_SHAPE_PROXYTYPE:
	{
		// assume the object is a cylinder
		const btCylinderShape* pCylinder = static_cast<const btCylinderShape*>(pShape);
		// get the relevant data
		float radius = pCylinder->getRadius();
		float halfHeight = pCylinder->getHalfExtentsWithMargin()[1];
		// draw the cylinder
		DrawCylinder(radius, halfHeight);

		break;
	}
		case CONVEX_HULL_SHAPE_PROXYTYPE:
	{
		// draw the convex hull shape...whatever it is
		DrawConvexHull(pShape);
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


void BulletOpenGLApplication::CreatePickingConstraint(int x, int y) {
	if (!m_pWorld)
		return;

	// perform a raycast and return if it fails
	RayResult output;
	if (!Raycast(m_cameraPosition, GetPickingRay(x, y), output))
		return;

	// store the body for future reference
	m_pPickedBody = output.pBody;

	// prevent the picked object from falling asleep
	m_pPickedBody->setActivationState(DISABLE_DEACTIVATION);

	// get the hit position relative to the body we hit
	btVector3 localPivot = m_pPickedBody->getCenterOfMassTransform().inverse() * output.hitPoint;

	// create a transform for the pivot point
	btTransform pivot;
	pivot.setIdentity();
	pivot.setOrigin(localPivot);

	// create our constraint object
	btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*m_pPickedBody, pivot, true);
	bool bLimitAngularMotion = true;
	if (bLimitAngularMotion) {
		dof6->setAngularLowerLimit(btVector3(0, 0, 0));
		dof6->setAngularUpperLimit(btVector3(0, 0, 0));
	}

	// add the constraint to the world
	m_pWorld->addConstraint(dof6, true);

	// store a pointer to our constraint
	m_pPickConstraint = dof6;

	// define the 'strength' of our constraint (each axis)
	float cfm = 0.5f;
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 0);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 1);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 2);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 3);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 4);
	dof6->setParam(BT_CONSTRAINT_STOP_CFM, cfm, 5);

	// define the 'error reduction' of our constraint (each axis)
	float erp = 0.5f;
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 0);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 1);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 2);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 3);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 4);
	dof6->setParam(BT_CONSTRAINT_STOP_ERP, erp, 5);

	// save this data for future reference
	m_oldPickingDist = (output.hitPoint - m_cameraPosition).length();
}

void BulletOpenGLApplication::RemovePickingConstraint() {
	// exit in erroneous situations
	if (!m_pPickConstraint || !m_pWorld)
		return;

	// remove the constraint from the world
	m_pWorld->removeConstraint(m_pPickConstraint);

	// delete the constraint object
	delete m_pPickConstraint;

	// reactivate the body
	m_pPickedBody->forceActivationState(ACTIVE_TAG);
	m_pPickedBody->setDeactivationTime(0.f);

	// clear the pointers
	m_pPickConstraint = 0;
	m_pPickedBody = 0;
}

void BulletOpenGLApplication::CheckForCollisionEvents() {
	// keep a list of the collision pairs we
	// found during the current update
	CollisionPairs pairsThisUpdate;

	// iterate through all of the manifolds in the dispatcher
	for (int i = 0; i < m_pDispatcher->getNumManifolds(); ++i) {
		// get the manifold
		btPersistentManifold* pManifold = m_pDispatcher->getManifoldByIndexInternal(i);

		// ignore manifolds that have 
		// no contact points.
		if (pManifold->getNumContacts() > 0) {
			// get the two rigid bodies involved in the collision
			const btRigidBody* pBody0 = static_cast<const btRigidBody*>(pManifold->getBody0());
			const btRigidBody* pBody1 = static_cast<const btRigidBody*>(pManifold->getBody1());

			// always create the pair in a predictable order
			// (use the pointer value..)
			bool const swapped = pBody0 > pBody1;
			const btRigidBody* pSortedBodyA = swapped ? pBody1 : pBody0;
			const btRigidBody* pSortedBodyB = swapped ? pBody0 : pBody1;

			// create the pair
			CollisionPair thisPair = std::make_pair(pSortedBodyA, pSortedBodyB);

			// insert the pair into the current list
			pairsThisUpdate.insert(thisPair);

			// if this pair doesn't exist in the list
			// from the previous update, it is a new
			// pair and we must send a collision event
			if (m_pairsLastUpdate.find(thisPair) == m_pairsLastUpdate.end()) {
				CollisionEvent((btRigidBody*)pBody0, (btRigidBody*)pBody1);
			}
		}
	}

	// create another list for pairs that
	// were removed this update
	CollisionPairs removedPairs;

	// this handy function gets the difference beween
	// two sets. It takes the difference between
	// collision pairs from the last update, and this 
	// update and pushes them into the removed pairs list
	std::set_difference(m_pairsLastUpdate.begin(), m_pairsLastUpdate.end(),
		pairsThisUpdate.begin(), pairsThisUpdate.end(),
		std::inserter(removedPairs, removedPairs.begin()));

	// iterate through all of the removed pairs
	// sending separation events for them
	for (CollisionPairs::const_iterator iter = removedPairs.begin(); iter != removedPairs.end(); ++iter) {
		SeparationEvent((btRigidBody*)iter->first, (btRigidBody*)iter->second);
	}

	// in the next iteration we'll want to
	// compare against the pairs we found
	// in this iteration
	m_pairsLastUpdate = pairsThisUpdate;
}
//碰撞到了都设置成白色
void BulletOpenGLApplication::CollisionEvent(btRigidBody * pBody0, btRigidBody * pBody1) {
	// find the two colliding objects
	GameObject* pObj0 = FindGameObject(pBody0);
	GameObject* pObj1 = FindGameObject(pBody1);

	// exit if we didn't find anything
	if (!pObj0 || !pObj1) return;

	// set their colors to white
	pObj0->SetColor(btVector3(1.0, 1.0, 1.0));
	pObj1->SetColor(btVector3(1.0, 1.0, 1.0));
}

//分开了同时设置成紫色
void BulletOpenGLApplication::SeparationEvent(btRigidBody * pBody0, btRigidBody * pBody1) {
	// get the two separating objects
	GameObject* pObj0 = FindGameObject((btRigidBody*)pBody0);
	GameObject* pObj1 = FindGameObject((btRigidBody*)pBody1);

	// exit if we didn't find anything
	if (!pObj0 || !pObj1) return;

	// set their colors to black
	pObj0->SetColor(btVector3(1.0, 0.2, 0.2));
	pObj1->SetColor(btVector3(1.0, 0.2, 0.2));
}

GameObject* BulletOpenGLApplication::FindGameObject(btRigidBody* pBody) {
	// search through our list of gameobjects finding
	// the one with a rigid body that matches the given one
	for (GameObjects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter) {
		if ((*iter)->GetRigidBody() == pBody) {
			// found the body, so return the corresponding game object
			return *iter;
		}
	}
	return 0;
}

void BulletOpenGLApplication::DrawSphere(const btScalar &radius) {
	// some constant values for more many segments to build the sphere from
	static int lateralSegments = 25;
	static int longitudinalSegments = 25;

	// iterate laterally
	for (int i = 0; i <= lateralSegments; i++) {
		// do a little math to find the angles of this segment
		btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar)(i - 1) / lateralSegments);
		btScalar z0 = radius*sin(lat0);
		btScalar zr0 = radius*cos(lat0);

		btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar)i / lateralSegments);
		btScalar z1 = radius*sin(lat1);
		btScalar zr1 = radius*cos(lat1);

		// start rendering strips of quads (polygons with 4 poins)
		glBegin(GL_QUAD_STRIP);
		// iterate longitudinally
		for (int j = 0; j <= longitudinalSegments; j++) {
			// determine the points of the quad from the lateral angles
			btScalar lng = 2 * SIMD_PI * (btScalar)(j - 1) / longitudinalSegments;
			btScalar x = cos(lng);
			btScalar y = sin(lng);
			// draw the normals and vertices for each point in the quad
			// since it is a STRIP of quads, we only need to add two points
			// each time to create a whole new quad

			// calculate the normal
			btVector3 normal = btVector3(x*zr0, y*zr0, z0);
			normal.normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			// create the first vertex
			glVertex3f(x * zr0, y * zr0, z0);

			// calculate the next normal
			normal = btVector3(x*zr1, y*zr1, z1);
			normal.normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			// create the second vertex
			glVertex3f(x * zr1, y * zr1, z1);

			// and repeat...
		}
		glEnd();
	}
}

void BulletOpenGLApplication::DrawCylinder(const btScalar &radius, const btScalar &halfHeight) {
	static int slices = 15;
	static int stacks = 10;
	// tweak the starting position of the
	// cylinder to match the physics object
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, -halfHeight);
	// create a quadric object to render with
	GLUquadricObj *quadObj = gluNewQuadric();
	// set the draw style of the quadric
	gluQuadricDrawStyle(quadObj, (GLenum)GLU_FILL);
	gluQuadricNormals(quadObj, (GLenum)GLU_SMOOTH);
	// create a disk to cap the cylinder
	gluDisk(quadObj, 0, radius, slices, stacks);
	// create the main hull of the cylinder (no caps)
	gluCylinder(quadObj, radius, radius, 2.f*halfHeight, slices, stacks);
	// shift the position and rotation again
	glTranslatef(0.0f, 0.0f, 2.f*halfHeight);
	glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
	// draw the cap on the other end of the cylinder
	gluDisk(quadObj, 0, radius, slices, stacks);
	// don't need the quadric anymore, so remove it
	// to save memory
	gluDeleteQuadric(quadObj);
}

void BulletOpenGLApplication::DrawConvexHull(const btCollisionShape* shape) {
	// get the polyhedral data from the convex hull
	const btConvexPolyhedron* pPoly = shape->isPolyhedral() ? ((btPolyhedralConvexShape*)shape)->getConvexPolyhedron() : 0;
	if (!pPoly) return;

	// begin drawing triangles
	glBegin(GL_TRIANGLES);

	// iterate through all faces
	for (int i = 0; i < pPoly->m_faces.size(); i++) {
		// get the indices for the face
		int numVerts = pPoly->m_faces[i].m_indices.size();
		if (numVerts > 2)	{
			// iterate through all index triplets
			for (int v = 0; v < pPoly->m_faces[i].m_indices.size() - 2; v++) {
				// grab the three vertices
				btVector3 v1 = pPoly->m_vertices[pPoly->m_faces[i].m_indices[0]];
				btVector3 v2 = pPoly->m_vertices[pPoly->m_faces[i].m_indices[v + 1]];
				btVector3 v3 = pPoly->m_vertices[pPoly->m_faces[i].m_indices[v + 2]];
				// calculate the normal
				btVector3 normal = (v3 - v1).cross(v2 - v1);
				normal.normalize();
				// draw the triangle
				glNormal3f(normal.getX(), normal.getY(), normal.getZ());
				glVertex3f(v1.x(), v1.y(), v1.z());
				glVertex3f(v2.x(), v2.y(), v2.z());
				glVertex3f(v3.x(), v3.y(), v3.z());
			}
		}
	}
	// done drawing
	glEnd();
}