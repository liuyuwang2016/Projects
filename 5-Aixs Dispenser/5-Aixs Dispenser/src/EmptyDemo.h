//
// Created by AICDG on 2017/8/9.
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
