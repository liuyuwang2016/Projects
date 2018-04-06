#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QKeyEvent>
#include <GL/glu.h>
#include <QMessageBox>
#include <QApplication>
#include <QTextStream>
#include <math.h>

class GLWidget : public QGLWidget
{
	Q_OBJECT
public:
	//�����α���������һЩ(��������)������ɵĶ���Σ�����ͬʱҲ�����ǵ�������ķ��൥λ��
	//���������OpenGL��������Ȥ�����ݡ�������3D�ռ��е�����ֵ(x,y,z)�Լ����ǵ���������(u,v)�����������ε�ÿ�����㡣
	typedef struct tagVERTEX						// ����Vertex����ṹ
	{
		float x, y, z;							// 3D ����
		float u, v;							// ��������
	} VERTEX;								// ����ΪVERTEX
	//һ��sector(����)������һϵ�еĶ���Σ�������һ��Ŀ�����triangle(���ǽ�ֻ�������Σ�����д���������Щ)��
	typedef struct tagTRIANGLE						// ����Triangle�����νṹ
	{
		VERTEX vertex[3];						// VERTEXʸ�����飬��СΪ3
	}TRIANGLE;// ����Ϊ TRIANGLE
	typedef struct tagSECTOR						// ����Sector���νṹ
	{
		int numtriangles;						// Sector�е������θ���
		TRIANGLE* triangle;						// ָ�����������ָ��
	} SECTOR;								// ����ΪSECTOR

	explicit GLWidget(QWidget *parent = 0);
	~GLWidget();
protected:
	void resizeGL(int w, int h);

	void initializeGL();

	void paintGL();

	void keyPressEvent(QKeyEvent *event);
private:
	void setupWorld();

	void readStr(QTextStream *stream, QString &string);

	void loadTexture();
private:
	bool m_show_full_screen;

	SECTOR m_sector1;

	GLfloat m_yrot;
	GLfloat m_xpos;
	GLfloat m_zpos;
	GLfloat m_heading;
	GLfloat m_walkbias;
	GLfloat m_walkbiasangle;
	GLfloat m_lookupdown;

	GLuint	m_filter;
	GLuint	m_texture[3];

	bool m_blend;
};

#endif // MYGLWIDGET_H
