//
// Created by Yvan om 2018/06/07
// ����ģ�͵�λ��
//

#ifndef BULLETOPENGL_BASICDEMO_H
#define BULLETOPENGL_BASICDEMO_H
#include "BulletOpenGLApplication.h"
#include <iostream>
#include <fstream>
#include <stdlib.h> 
#include <string>
#pragma managed (push,off)
#include <btBulletDynamicsCommon.h>
#pragma managed (pop)

using namespace std;

class BasicDemo : public BulletOpenGLApplication {
public:
	virtual void InitializePhysics() override;
	virtual void ShutDownPhysics() override;

	void CreateObjects();
	void ReadBuffer();


};
#endif //BULLETOPENGL_BASICDEMO_H
