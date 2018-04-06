#include "glwidget.h"
#include <GL/glu.h>
#include <GL/glut.h>

GLWidget::GLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	fullScreen = false;
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
	glClearColor(0, 0, 0, 1);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);                            //启用深度测试  
	glDepthFunc(GL_LEQUAL);                             //所作深度测试的类型  
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void GLWidget::resizeGL(int w, int h)
{
	if (h == 0)
	{
		h == 1;
	}
	glViewport(0, 0, (GLint)w, (GLint)h);               //重置当前的视口  
	glMatrixMode(GL_PROJECTION);                        //选择投影矩阵  
	glLoadIdentity();                                   //重置投影矩阵  
	//设置视口的大小  
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);                         //选择模型观察矩阵  
	glLoadIdentity();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
	switch (event->key())
	{
		//F1为全屏和普通屏的切换键  
	case Qt::Key_F1:
		fullScreen = !fullScreen;
		if (fullScreen)
		{
			showFullScreen();
		}
		else
		{
			showNormal();
		}
		//updateGL();
		break;
		//ESC为退出键  
	case Qt::Key_Escape:
		close();
}
