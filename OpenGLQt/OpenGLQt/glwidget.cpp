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
	glEnable(GL_DEPTH_TEST);                            //������Ȳ���  
	glDepthFunc(GL_LEQUAL);                             //������Ȳ��Ե�����  
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
	glViewport(0, 0, (GLint)w, (GLint)h);               //���õ�ǰ���ӿ�  
	glMatrixMode(GL_PROJECTION);                        //ѡ��ͶӰ����  
	glLoadIdentity();                                   //����ͶӰ����  
	//�����ӿڵĴ�С  
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);                         //ѡ��ģ�͹۲����  
	glLoadIdentity();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
	switch (event->key())
	{
		//F1Ϊȫ������ͨ�����л���  
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
		//ESCΪ�˳���  
	case Qt::Key_Escape:
		close();
}
