
#ifdef _APPLE_
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//初始化要画的图形位置和大小
GLfloat x1 = 0.0f;
GLfloat y1 = 0.0f;
GLfloat rsize = 100;
//每次要移动的像素值
GLfloat xStep = 1.0f;
GLfloat yStep = 1.0f;

GLfloat windowWidth;
GLfloat windowHeight;


void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);
	glRectf(x1, y1, x1 + rsize, y1 - rsize);
	glutSwapBuffers();
}

void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat ratio;

	//防止被除数等于0
	if (h == 0)
		h = 1;
	//设置观察口
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ratio = (GLfloat)w / (GLfloat)h;
	if (w <= h)
		glOrtho(-100.0, 100.0, -100 / ratio, 100.0 / ratio, 1.0, -1.0);
	else
		glOrtho(-100.0 * ratio, 100.0 * ratio, -100, 100.0, 1.0, -1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void TimerFunc(int value)
{
	if (x1 > windowWidth - rsize || x1 < -windowWidth)
		xStep = -xStep;
	if (y1>windowHeight || y1 < -windowHeight + rsize)
		yStep = -yStep;
	x1 += xStep;
	y1 += yStep;
	if (x1 >(windowWidth - rsize + xStep))
		x1 = windowWidth - rsize - 1;
	else if (x1 < -(windowWidth + xStep))
		x1 = -windowWidth - 1;

	if (y1 >(windowHeight + yStep))
		y1 = windowHeight - 1;
	else if (y1 < -(windowHeight -rsize + yStep))
		y1 = -windowHeight + rsize - 1;
	glutPostRedisplay();
	glutTimerFunc(33, TimerFunc, 1);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Bounce");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutTimerFunc(33, TimerFunc, 1);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glutMainLoop();
	return 0;
}