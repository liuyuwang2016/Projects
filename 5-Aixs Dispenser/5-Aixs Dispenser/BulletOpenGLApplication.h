#ifndef BULLETOPENGL_BULLETOPENGLAPPLICATION_H
#define BULLETOPENGL_BULLETOPENGLAPPLICATION_H

#include <GL/freeglut.h>
#pragma managed (push,off)
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#pragma managed (pop)

// include our custom Motion State object
#include "OpenGLMotionState.h"

#include "GameObject.h"
#include <vector>

// debug模式渲染
#include "DebugDrawer.h"

// 在空间中保存多个物体，使用STL Vector容器
typedef std::vector<GameObject*> GameObjects;

// struct to store our raycasting results
struct RayResult {
	btRigidBody* pBody;
	btVector3 hitPoint;
};

class BulletOpenGLApplication {
public:
	BulletOpenGLApplication();
	~BulletOpenGLApplication();
	void Initialize();
	// FreeGLUT callbacks //
	virtual void Keyboard(unsigned char key, int x, int y);
	virtual void KeyboardUp(unsigned char key, int x, int y);
	virtual void Special(int key, int x, int y);
	virtual void SpecialUp(int key, int x, int y);
	virtual void Reshape(int w, int h);
	virtual void Idle();
	virtual void Mouse(int button, int state, int x, int y);
	virtual void PassiveMotion(int x, int y);
	virtual void Motion(int x, int y);
	virtual void Display();

	// rendering. Can be overrideen by derived classes
	virtual void RenderBulletScene();

	// scene updating. Can be overridden by derived classes
	virtual void UpdateBulletScene(float dt);
	//物理属性函数，可以被派生类重载
	virtual void InitializePhysics(){};
	virtual void ShutDownPhysics(){};

	//drawing functions
	void DrawBox(const btVector3 &halfSize);
	void DrawShape(btScalar* transform, const btCollisionShape* pShape, const btVector3 &color);

	// object functions
	GameObject* CreateGameObject(btCollisionShape* pShape,
		const float &mass,
		const btVector3 &color = btVector3(1.0f, 1.0f, 1.0f),
		const btVector3 &initialPosition = btVector3(0.0f, 0.0f, 0.0f),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));

	void ShootBox(const btVector3 &direction);
	void DestroyGameObject(btRigidBody* pBody);

	// picking functions
	btVector3 GetPickingRay(int x, int y);
	bool Raycast(const btVector3 &startPosition, const btVector3 &direction, RayResult &output);
protected:
	btVector3 m_cameraPosition; // the camera's current position
	btVector3 m_cameraTarget;	 // the camera's lookAt target
	btVector3 m_upVector; // keeps the camera rotated correctly
	float m_nearPlane; // minimum distance the camera will render
	float m_farPlane; // farthest distance the camera will render
	
	int m_screenWidth;
	int m_screenHeight;
	
	 //core Bullet components Bullet的核心元素
	btBroadphaseInterface* m_pBroadphase;
	btCollisionConfiguration* m_pCollisionConfiguration;
	btCollisionDispatcher* m_pDispatcher;
	btConstraintSolver* m_pSolver;
	btDynamicsWorld* m_pWorld;

	// our custom motion state
	OpenGLMotionState* m_pMotionState;

	// a simple clock for counting time
	btClock m_clock;
	// an array of our game objects
	GameObjects m_objects;

	// debug renderer
	DebugDrawer* m_pDebugDrawer;

	// constraint variables
	btRigidBody* m_pPickedBody;				// the body we picked up
	btTypedConstraint*  m_pPickConstraint;	// the constraint the body is attached to
	btScalar m_oldPickingDist;
};


#endif //BULLETOPENGL_BULLETOPENGLAPPLICATION_H
