#include "windows.h"
#include <gl/glut.h>
#include <time.h>
#include <stdio.h>
static int spin = 0;

void init()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

}

double CalFrequency()
{
	static int count;
	static double save;
	static clock_t last, current;
	double timegap;
	++count;
	if (count <= 50)
		return save;
	count = 0;
	last = current;
	current = clock();
	timegap = (current - last) / (double)CLK_TCK;
	save = 50.0 / timegap;
}

void display()
{
	double FPS = CalFrequency();
	printf("FPS=%f\n", FPS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat m[16];

	glMatrixMode(GL_MODELVIEW);

	// 正规化
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("正规化\n");
	printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8], m[12]);
	printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9], m[13]);
	printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
	printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", m[3], m[7], m[11], m[15]);
	// 移动
	glLoadIdentity();
	glTranslatef(2.0f, 3.0f, 5.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("移动\n");
	printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8], m[12]);
	printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9], m[13]);
	printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
	printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", m[3], m[7], m[11], m[15]);
	// 放大缩小
	glLoadIdentity();
	glScalef(2.0f, 0.5f, 1.5f);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("拡大・縮小\n");
	printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8], m[12]);
	printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9], m[13]);
	printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
	printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", m[3], m[7], m[11], m[15]);
	//X轴旋转
	glLoadIdentity();
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("X轴旋转\n");
	printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8], m[12]);
	printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9], m[13]);
	printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
	printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", m[3], m[7], m[11], m[15]);
	// Y轴旋转
	glLoadIdentity();
	glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("Y轴旋转\n");
	printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8], m[12]);
	printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9], m[13]);
	printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
	printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", m[3], m[7], m[11], m[15]);
	//Z轴旋转
	glLoadIdentity();
	glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	printf("Z轴旋转\n");
	printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8], m[12]);
	printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9], m[13]);
	printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
	printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", m[3], m[7], m[11], m[15]);
	
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			spin = (spin + 30) % 360;
			glutPostRedisplay();
		}
		break;
	default:
		break;
	}
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}