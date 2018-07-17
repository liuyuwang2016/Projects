//
// Created by Yvan om 2018/05/07
// 这个是在 main函数里面需要调试除了
// 虚拟物体之外其他功能的一个EmptyDemo.
//

#ifndef BULLETOPENGL_EMPTYDEMO_H
#define BULLETOPENGL_EMPTYDEMO_H

#include "BulletOpenGLApplication.h"

#pragma managed (push,off)
#include <btBulletDynamicsCommon.h>
#pragma managed (pop)

class EmptyDemo : public BulletOpenGLApplication {
public:
	virtual void InitializePhysics() override;
	virtual void ShutDownPhysics() override;
};
#endif //BULLETOPENGL_BASICDEMO_H
