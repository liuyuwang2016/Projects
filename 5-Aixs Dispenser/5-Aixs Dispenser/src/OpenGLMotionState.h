//
// Created by AICDG on 2017/8/9.
//

#ifndef BULLETOPENGL_OPENGLMOTIONSTATE_H
#define BULLETOPENGL_OPENGLMOTIONSTATE_H
#pragma managed (push,off)
#include <btBulletCollisionCommon.h>
#pragma managed (pop)


class OpenGLMotionState : public btDefaultMotionState {
public:
    OpenGLMotionState(const btTransform &transform) : btDefaultMotionState(transform) {}

	void GetWorldTransform(btScalar* transform) {
		btTransform trans;
		getWorldTransform(trans);
		trans.getOpenGLMatrix(transform);
	}
};

#endif //BULLETOPENGL_OPENGLMOTIONSTATE_H
