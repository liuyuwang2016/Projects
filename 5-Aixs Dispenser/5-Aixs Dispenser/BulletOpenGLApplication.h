#ifndef BULLETOPENGL_BULLETOPENGLAPPLICATION_H
#define BULLETOPENGL_BULLETOPENGLAPPLICATION_H

#include <GL/freeglut.h>
#pragma managed (push,off)
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
// 为了包含凸体Convex Hull所需要的函数库
#include <BulletCollision/CollisionShapes/btConvexPolyhedron.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#pragma managed (pop)

// 这个是为了包含物体的motion state
#include "OpenGLMotionState.h"

#include "GameObject.h"
#include <vector>
#include <set>
#include <iterator>
#include <algorithm>

// debug模式渲染,debug模式具体解释见Bullet的书
#include "DebugDrawer.h"

// 在空间中保存多个物体，使用STL Vector容器
typedef std::vector<GameObject*> GameObjects;

//碰撞事件的 typedef
typedef std::pair<const btRigidBody*, const btRigidBody*> CollisionPair;
typedef std::set<CollisionPair> CollisionPairs;

// 用来存储光线追踪的结果的 struct
struct RayResult {
	btRigidBody* pBody;
	btVector3 hitPoint;
};

class BulletOpenGLApplication {
public:
	BulletOpenGLApplication();
	~BulletOpenGLApplication();
	void Initialize();
	// FreeGLUT 的 callbacks 
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

	// 渲染，可以被派生类重载 
	virtual void RenderBulletScene();

	// 场景更新，可以被派生类渲染
	virtual void UpdateBulletScene(float dt);
	// 物理属性函数，可以被派生类重载
	virtual void InitializePhysics(){};
	virtual void ShutDownPhysics(){};

	// 画各种虚拟物体的函数
	void DrawBox(const btVector3 &halfSize);
	void DrawSphere(const btScalar &radius);
	void DrawCylinder(const btScalar &radius, const btScalar &halfHeight);
	void DrawConvexHull(const btCollisionShape* shape);
	void DrawShape(btScalar* transform, const btCollisionShape* pShape, const btVector3 &color);

	// 物体函数
	GameObject* CreateGameObject(btCollisionShape* pShape,
		const float &mass,
		const btVector3 &color = btVector3(1.0f, 1.0f, 1.0f),
		const btVector3 &initialPosition = btVector3(0.0f, 0.0f, 0.0f),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));
	
	//这个可以通过滑鼠控制射出方块
	void ShootBox(const btVector3 &direction);

	//光线追踪后可以消除物体
	void DestroyGameObject(btRigidBody* pBody);

	//是为了用搜索树搜索空间中存在的虚拟物体，然后进行碰撞侦测
	GameObject* FindGameObject(btRigidBody* pBody);
	
	// 光线追踪选择函数
	btVector3 GetPickingRay(int x, int y);
	bool Raycast(const btVector3 &startPosition, const btVector3 &direction, RayResult &output);

	// constraint 函数
	void CreatePickingConstraint(int x, int y);
	void RemovePickingConstraint();


	// 碰撞事件函数
	void CheckForCollisionEvents();
	virtual void CollisionEvent(btRigidBody* pBody0, btRigidBody * pBody1);
	virtual void SeparationEvent(btRigidBody * pBody0, btRigidBody * pBody1);

public:
	//-------相机模型的参数-------//
	btVector3 m_cameraPosition; // the camera's current position
	btVector3 m_cameraTarget;	 // the camera's lookAt target
	btVector3 m_upVector; // keeps the camera rotated correctly

	btVector3 m_modelPosition;//跟随移动的model的位置

	float m_nearPlane; // minimum distance the camera will render
	float m_farPlane; // farthest distance the camera will render
	
	int m_screenWidth;
	int m_screenHeight;
	
	// Bullet 的核心元素, 具体解释参照给你的Bullet书
	btBroadphaseInterface* m_pBroadphase;
	btCollisionConfiguration* m_pCollisionConfiguration;
	btCollisionDispatcher* m_pDispatcher;
	btConstraintSolver* m_pSolver;
	btDynamicsWorld* m_pWorld;

	// our custom motion state
	OpenGLMotionState* m_pMotionState;

	// 计时的 clock
	btClock m_clock;
	// 存放多个虚拟物体的 array
	GameObjects m_objects;

	// debug 模式的渲染器
	DebugDrawer* m_pDebugDrawer;

	// constraint 变量
	btRigidBody* m_pPickedBody;				// the body we picked up
	btTypedConstraint*  m_pPickConstraint;	// the constraint the body is attached to
	btScalar m_oldPickingDist;

	//碰撞事件
	CollisionPairs m_pairsLastUpdate;
};


#endif //BULLETOPENGL_BULLETOPENGLAPPLICATION_H
