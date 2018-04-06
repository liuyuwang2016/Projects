#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GL/glut.h"
#include "glm.h"

void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void timer(int value);

void process_pick(float x, float y);
void pick_func(float x, float y);
void pick_change(GLMtriangle *t0, GLMtriangle *t1);

GLuint list_plane;
GLuint list_ground;

GLMmodel *glm_ground;
GLMtriangle *pathface[1024];
int n_pathface = 0;

int drag_state = -1;
int flying = false;
float plane_pos;
float plane_vec[3] = { 0, 0, 0 };



void main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(640, 640);
	glutCreateWindow("glutTest11");

	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	{
		glm_ground = glmReadOBJ("world_curved.obj");
		glmUnitize(glm_ground);
		glmScale(glm_ground, 1.2);
		list_ground = glmList(glm_ground, GLM_SMOOTH);
	}

	{
		GLMmodel *glm_model;
		glm_model = glmReadOBJ("soccerball.obj");
		glmUnitize(glm_model);
		glmScale(glm_model, .05);
		list_plane = glmList(glm_model, GLM_MATERIAL | GLM_SMOOTH);
		glmDelete(glm_model);
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
}

void display()
{
	glClearColor(.2, .3, .4, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(viewport[2]) / viewport[3], 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

	glDisable(GL_LIGHTING);

	int i;

	if (drag_state != -1)
	for (i = 0; i < n_pathface; i++)
	{
		glColor3f(0, (i + 1.0) / n_pathface, 0);
		glBegin(GL_TRIANGLES);
		glVertex3fv(&glm_ground->vertices[3 * pathface[i]->vindices[0]]);
		glVertex3fv(&glm_ground->vertices[3 * pathface[i]->vindices[1]]);
		glVertex3fv(&glm_ground->vertices[3 * pathface[i]->vindices[2]]);
		glEnd();
	}

	glEnable(GL_LIGHTING);
	glCallList(list_ground);

	if (flying)
	{
		flying = false;
		float *v0, *v1;
		float d, cd, r;

		cd = 0;
		for (i = 0; i<n_pathface - 1; i++)
		{
			v0 = &glm_ground->vertices[3 * pathface[i]->vindices[0]];
			v1 = &glm_ground->vertices[3 * pathface[i + 1]->vindices[0]];

			d = sqrtf(
				(v0[0] - v1[0])*(v0[0] - v1[0]) +
				(v0[1] - v1[1])*(v0[1] - v1[1]) +
				(v0[2] - v1[2])*(v0[2] - v1[2]));

			if (cd + d > plane_pos)
			{
				r = (plane_pos - cd) / d;
				plane_vec[0] = v0[0] * (1 - r) + v1[0] * r;
				plane_vec[1] = v0[1] * (1 - r) + v1[1] * r;
				plane_vec[2] = v0[2] * (1 - r) + v1[2] * r;
				plane_pos += .02;
				flying = true;
				glutTimerFunc(30, timer, 0);
				break;
			}
			cd += d;
		}
	}

	glTranslatef(plane_vec[0], plane_vec[1], plane_vec[2]);
	glCallList(list_plane);

	glutSwapBuffers();
}

void timer(int value)
{
	glutPostRedisplay();
}

void pick_change(GLMtriangle *t0, GLMtriangle *t1)
{
	if (t1)
	{
		pathface[n_pathface] = t1;
		n_pathface++;
	}
}

#define BUFSIZE 1024
typedef struct _GSelect
{
	unsigned int n_name;
	unsigned int d0, d1;
	unsigned int name[1];
}GSelect;
GLMtriangle *prev_face = NULL;
GLMtriangle *active_face = NULL;

void process_pick(float x, float y)
{
	GLuint selectBuf[BUFSIZE];
	unsigned int hit;
	unsigned int i;

	glSelectBuffer(BUFSIZE, selectBuf);
	glInitNames();
	glRenderMode(GL_SELECT);
	pick_func(x, y);
	hit = glRenderMode(GL_RENDER);

	active_face = NULL;
	if (hit)
	{
		GLuint *ptr;
		GLuint dmin = -1;

		ptr = selectBuf;
		for (i = 0; i < hit; i++)
		{
			GSelect &sel = *((GSelect*)ptr);
			if (sel.d0 < dmin)
			{
				dmin = sel.d0;
				active_face = (GLMtriangle*)sel.name[0];
			}
			ptr += 3 + sel.n_name;
		}
	}

	if (active_face != prev_face)
	{
		pick_change(prev_face, active_face);
		prev_face = active_face;
	}
}
void pick_func(float x, float y)
{
	unsigned int i;
	GLMmodel* model;
	GLMgroup* group;
	GLMtriangle* triangle;

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(x*viewport[2], y*viewport[3], .1, .1, viewport);
	gluPerspective(45, double(viewport[2]) / viewport[3], 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

	model = glm_ground;
	group = model->groups;
	while (group)
	{
		for (i = 0; i < group->numtriangles; i++)
		{
			triangle = &model->triangles[group->triangles[i]];
			glPushName((unsigned int)triangle);
			glBegin(GL_TRIANGLES);
			glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
			glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
			glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
			glEnd();
			glPopName();
		}
		group = group->next;
	}
}

void mouse(int button, int state, int x, int y)
{
	if (flying)
		return;

	int w, h;
	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			drag_state = GLUT_LEFT_BUTTON;
			n_pathface = 0;
			process_pick(float(x) / w, float(h - y) / h);
			glutPostRedisplay();
		}

		if (state == GLUT_UP && drag_state == GLUT_LEFT_BUTTON)
		{
			drag_state = -1;
			process_pick(float(x) / w, float(h - y) / h);

			plane_pos = 0;
			flying = true;

			glutPostRedisplay();
		}
	}
}

void motion(int x, int y)
{
	if (flying)
		return;

	int w, h;
	w = glutGet(GLUT_WINDOW_WIDTH);
	h = glutGet(GLUT_WINDOW_HEIGHT);

	if (drag_state == GLUT_LEFT_BUTTON)
	{
		process_pick(float(x) / w, float(h - y) / h);
		glutPostRedisplay();
	}
}