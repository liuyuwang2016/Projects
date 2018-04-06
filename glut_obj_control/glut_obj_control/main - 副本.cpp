#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>
#include <iostream>

float M_Cubic_inv[16] = {				//Machine Coordinate rotation inversion
	-0.9989819272793313, -0.003748666227865277, -0.04495616165538265, 0,
	-0.02745696980813679, 0.8412283976366035, 0.5399823125054257, 0,
	0.03579418637471586, 0.540666931216682, -0.8404748929685572, 0,
	0, 0, 0, 1 };

float mo[16];

void init_mo();
void update_mo(float angle, float x, float y, float z);

void display();
void keyboard(unsigned char key, int x, int y);

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);

	glutCreateWindow("Yuwang");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	init_mo();

	glutMainLoop();
}

void display()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glClearColor(.1, .2, .3, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(viewport[2]) / viewport[3], 0.1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
    
	GLfloat TransM_toMechCoord[16] = { 0 };
	
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, -0.035, 0.12);
	glRotatef(-90, 1, 0, 0);
	//glMultMatrixf(M_Cubic_inv);
	//glTranslatef(-0.4450837313772593, 0.2388306799026206, 1);
	glGetFloatv(GL_MODELVIEW_MATRIX, TransM_toMechCoord);
	glPopMatrix();
	
	glMultMatrixf(TransM_toMechCoord);
	glutSolidTeapot(.17);
	
	glutSwapBuffers();
}

void init_mo()
{
	memset(mo, 0, sizeof(mo));
	mo[0] = mo[5] = mo[10] = mo[15] = 1;
	glutPostRedisplay();
}

void update_mo(float angle, float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, x, y, z);
	glMultMatrixf(mo);
	glGetFloatv(GL_MODELVIEW_MATRIX, mo);
	glPopMatrix();

	glutPostRedisplay();
}

void move_mo(float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, z);
	glMultMatrixf(mo);
	glGetFloatv(GL_MODELVIEW_MATRIX, mo);
	glPopMatrix();
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		init_mo();
		break;
	case '1':
		update_mo(-5, 1, 0, 0);
		break;
	case '2':
		update_mo(5, 1, 0, 0);
		break;

	case '3':
		update_mo(-5, 0, 1, 0);
		break;
	case '4':
		update_mo(5, 0, 1, 0);
		break;

	case '5':
		update_mo(-5, 0, 0, 1);
		break;
	case '6':
		update_mo(5, 0, 0, 1);
		break;
	case '7':
		move_mo(0.1, 0, 0);
		break;
	}
}