//
// Created by AICDG on 2017/8/9.
//

#ifndef BULLETOPENGL_BASICDEMO_H
#define BULLETOPENGL_BASICDEMO_H

#include "BulletOpenGLApplication.h"

#pragma managed (push,off)
#include <btBulletDynamicsCommon.h>
#pragma managed (pop)

class BasicDemo : public BulletOpenGLApplication {
public:
	virtual void InitializePhysics() override;
	virtual void ShutDownPhysics() override;

	void CreateObjects();
};


#endif //BULLETOPENGL_BASICDEMO_H
