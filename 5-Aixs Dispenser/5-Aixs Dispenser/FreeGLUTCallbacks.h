#ifndef BULLETOPENGL_FREEGLUTCALLBACKS_H
#define BULLETOPENGL_FREEGLUTCALLBACKS_H

#include "Function.h"
#include "BulletOpenGLApplication.h"
GameObject* m_modelObject;
int modelCount = 0;

//// global pointer to our application object
static BulletOpenGLApplication* g_pApp;
void loadTipModel(float model_x, float model_y, float model_z);
// Various static functions that will be handed to FreeGLUT to be called
// during various events (our callbacks). Each calls an equivalent function
// in our (global) application object.
static void KeyboardCallback(unsigned char key, int x, int y) {
	g_pApp->Keyboard(key, x, y);
	DispenserKeyboard(key, x, y);
}

static void KeyboardUpCallback(unsigned char key, int x, int y) {
	//g_pApp->KeyboardUp(key, x, y);
}

static void SpecialCallback(int key, int x, int y) {
	//g_pApp->Special(key, x, y);
	SpecialKeys(key, x, y);
}

static void SpecialUpCallback(int key, int x, int y) {
	//g_pApp->SpecialUp(key, x, y);
}

static void ReshapeCallback(int w, int h) {
	g_pApp->Reshape(w, h);
}

static void IdleCallback() {
	KinectUpdate();
	/*
	 * ����ǿ���ץ��λ�ò��һ������������function�����������ﻭ�ϵ��������岻���Ը����ƶ������ǻ������ƶ���������������壬
	 * ��ԭ��ΪcreateGameObjectֻ�Ǵ������һ���������壬Ȼ������������bullet�ڲ����ƶ���ʽΪ���̶����������崫��transform�ľ���
	 * ������Ϊ�˴���freeglut��mainloop���У����ÿѭ��һ�ξͻ����һ���������壬�������Ϊʹ��constraints����������ƶ�����bullet�������
	 * ���������ⲿ����һ�����壬���ڴ˴�ֻ�ǲ��ϸı�������transform��ֵ��ʧ�ܣ�
	 */
	
	//if (ROIDepthCount != 0 && modelCount <= 1)
	//{
	//	m_modelObject = g_pApp->CreateGameObject(new btBoxShape(btVector3(0.01, 0.02, 0.02)), 0, btVector3(0.0f, 0.2f, 0.2f), btVector3(tipModel.x, tipModel.y, tipModel.z));
	//	modelCount++;
	//}
	//if (ROIDepthCount != 0)
	//{
	//	loadTipModel(tipModel.x, tipModel.y, tipModel.z);
	//}
}

static void MouseCallBack(int button, int state, int x, int y) {
	g_pApp->Mouse(button, state, x, y);
}

static void MotionCallback(int x, int y) {
	//g_pApp->Motion(x, y);
}

static void DisplayCallback(void) {
	//g_pApp->Display();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderScene();
	g_pApp->Idle();
	if (Finish_Without_Update)
		glFinish();
	else
		glutSwapBuffers();
	FPS_RS++;
}

void loadTipModel(float model_x, float model_y, float model_z)
{
	btVector3 model = btVector3(model_x, model_y, model_z);

	//g_pApp->DestroyGameObject(pObject->GetRigidBody());
	btTransform modelTransform;
	modelTransform.setOrigin(model);
	m_modelObject->GetRigidBody()->getMotionState()->setWorldTransform(modelTransform);
}


// our custom-built 'main' function, which accepts a reference to a
// BulletOpenGLApplication object.
int glutmain(int argc, char **argv, int width, int height, const char* title, BulletOpenGLApplication* pApp)
{
	g_pApp = pApp;
	// store the application object so we can
	// access it globally

	init();
	// initialize the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow(title);

	// perform custom initialization our of application
	g_pApp->Initialize();

	glutFullScreen();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	//glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	/*--------------default camera--------------*/
	g_Camera.vPosition = Vector3(-0.064403, 0.000998867, 0.0897838);
	g_Camera.vCenter = Vector3(-0.0494023, 0.0210018, 1.08971);
	g_Camera.vUpper = Vector3(-0.000199958, 0.99968, -0.019995);
	// perform custom initialization our of application

	// give our static

	glutDisplayFunc(DisplayCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(SpecialCallback);

	glutKeyboardUpFunc(KeyboardUpCallback);
	glutSpecialUpFunc(SpecialUpCallback);
	glutReshapeFunc(ReshapeCallback);
	glutMouseFunc(MouseCallBack);
	glutPassiveMotionFunc(MotionCallback);
	glutMotionFunc(MotionCallback);


	//setShaders();
	loadOBJModel();
	Texture();
	/*--------------above this do not need check again--------------*/
	glutTimerFunc(1000, timer, 0);

	BuildPopupMenu();
	// perform one render before we launch the application
	//g_pApp->Idle();

	// perform one render before we launch the application
	g_pApp->Idle();
	// hand application control over to the FreeGLUT library.
	// This function remains in a while-loop until the
	// application is exited.
	////����ڴ�й©
	//_CrtDumpMemoryLeaks();
	glutMainLoop();
	return 0;
}

#endif //BULLETOPENGL_FREEGLUTCALLBACKS_H
