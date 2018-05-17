#include <iostream>

#include <GL/freeglut.h>
//#include <GL/glew.h>
#include "gjk_epa.hpp"
#include "glm.h"

using namespace std;
// Shader attributes
GLint iLocposition;
GLint iLocColor;
GLint iLocMVP;

#define MAX_TEXTURE_NUM 50
#define MAXSIZE 200000
#define MAXGROUPSIZE 50
#define myMax(a,b) (((a)>(b))?(a):(b))
float vertices[MAXGROUPSIZE][MAXSIZE];
float normals[MAXGROUPSIZE][MAXSIZE];
float vtextures[MAXGROUPSIZE][MAXSIZE];
#define GLUT_WHEEL_UP   3
#define GLUT_WHEEL_DOWN 4

class Model
{
public:
	Vector3 position;
	Model(){}
	~Model(){}
	
	virtual Vector3 support_world(Vector3& direction)
	{
		Vector3 res;
		return res;
	}
	virtual void drawModel(){}
};

class probeTipModel : public Model
{
public:
	GLMmodel* obj;
	probeTipModel()
	{
	}
	~probeTipModel()
	{
	}

	Vector3 support_world(Vector3& direction)
	{
		Vector3 res;
		return res;
	}

	void drawModel(GLMmodel* obj);
};

void probeTipModel::drawModel(GLMmodel* obj)
{
	if (obj != NULL) {
		free(obj);
	}
	obj = glmReadOBJ("model/bunny.obj");
	glmUnitize(obj);
		
	GLMgroup* group = obj->groups;

	float maxx, maxy, maxz;
	float minx, miny, minz;
	float dx, dy, dz;
	//glm中，vertices3,4,5是第一个点
	maxx = minx = obj->vertices[3];
	maxy = miny = obj->vertices[4];
	maxz = minz = obj->vertices[5];

	//3,4,5的值为第一组v的值
	//找最大最小值：三个维度,遍历模型中顶点的数目
	for (unsigned int i = 2; i <= obj->numvertices; i++)
	{
		GLfloat vx, vy, vz;

		vx = obj->vertices[i * 3 + 0];
		vy = obj->vertices[i * 3 + 1];
		vz = obj->vertices[i * 3 + 2];

		if (vx > maxx)
			maxx = vx;
		if (vx < minx)
			minx = vx;
		if (vy > maxy)
			maxy = vy;
		if (vy < miny)
			miny = vy;
		if (vz > maxz)
			maxz = vz;
		if (vz < minz)
			minz = vz;
	}
	//printf("max\n%f %f, %f %f, %f %f\n", maxx, minx, maxy, miny, maxz, minz);
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;
	//找到模型的顶点中X,Y,Z的值最大的三个点和X,Y,Z值最小的三个点，从而算出模型的长宽高
	//printf("dx,dy,dz = %f %f %f\n", dx, dy, dz);
	//GLfloat normalizationScale = myMax(myMax(dx, dy), dz) / 2;
	//算出模型的长宽高之后，拿到模型中心点所在的位置
	obj->position[0] = (maxx + minx) / 2;
	obj->position[1] = (maxy + miny) / 2;
	obj->position[2] = (maxz + minz) / 2;

	int gCount = 0;
	while (group) {
		for (unsigned int i = 0; i < group->numtriangles; i++) {

			// triangle index
			int triangleID = group->triangles[i];

			// the index of each vertex vindices是三角形的顶点
			int indv1 = obj->triangles[triangleID].vindices[0];
			int indv2 = obj->triangles[triangleID].vindices[1];
			int indv3 = obj->triangles[triangleID].vindices[2];

			//在这里可以得到，见文件triangle.txt
			/*cout << "obj->triangles["<<triangleID<<"].vindices[0] = " << obj->triangles[triangleID].vindices[0] << endl;
			cout << "obj->triangles["<<triangleID<<"].vindices[1] = " << obj->triangles[triangleID].vindices[1] << endl;
			cout << "obj->triangles["<<triangleID<<"].vindices[2] = " << obj->triangles[triangleID].vindices[2] << endl;*/

			// vertices
			GLfloat vx, vy, vz;
			double scale = 0.001;
			//抓取的三角形的第一个顶点的数据X,Y,Z
			vx = obj->vertices[indv1 * 3];
			vy = obj->vertices[indv1 * 3 + 1];
			vz = obj->vertices[indv1 * 3 + 2];

			//读三角形三个点
			//printf("vertices1 %f %f %f\n", vx, vy, vz);
			//这里是把原来的 vertices 转移到新的二维数组中，原来的单位是米，我们转变后单位是毫米
			/* The model size's unit is mm but glm.h's size unit is m, so we use a scale to resize it\n*/
			//让真实的三角形的第一个顶点的数据等于模型中的数据
			obj->vertices[gCount][i * 9 + 0] = vx * scale;
			obj->vertices[gCount][i * 9 + 1] = vy * scale;
			obj->vertices[gCount][i * 9 + 2] = vz * scale;

			//在这里可以得到，见文件vertices.txt
			/*cout << "vertices[" << gCount << "][" << i  << " * 9]+0 = " << vertices[gCount][i * 9 + 0] << endl;
			cout << "vertices[" << gCount << "][" << i << " * 9]+1 = " << vertices[gCount][i * 9 + 1] << endl;
			cout << "vertices[" << gCount << "][" << i  << " * 9]+2 = " << vertices[gCount][i * 9 + 2] << endl;*/
			//抓取的三角形的第二个顶点的数据
			vx = obj->vertices[indv2 * 3];
			vy = obj->vertices[indv2 * 3 + 1];
			vz = obj->vertices[indv2 * 3 + 2];

			obj->vertices[gCount][i * 9 + 3] = vx * scale;
			obj->vertices[gCount][i * 9 + 4] = vy * scale;
			obj->vertices[gCount][i * 9 + 5] = vz * scale;
			//抓取的三角形的第三个顶点的数据
			vx = obj->vertices[indv3 * 3];
			vy = obj->vertices[indv3 * 3 + 1];
			vz = obj->vertices[indv3 * 3 + 2];

			obj->vertices[gCount][i * 9 + 6] = vx * scale;
			obj->vertices[gCount][i * 9 + 7] = vy * scale;
			obj->vertices[gCount][i * 9 + 8] = vz * scale;

			//抓取的三角形的三个点的法向量
			int indn1 = obj->triangles[triangleID].nindices[0];
			int indn2 = obj->triangles[triangleID].nindices[1];
			int indn3 = obj->triangles[triangleID].nindices[2];

			// 三个点的法向量的X,Y,Z数据
			obj->normals[gCount][i * 9 + 0] = obj->normals[indn1 * 3 + 0];
			obj->[gCount][i * 9 + 1] = obj->normals[indn1 * 3 + 1];
			obj->[gCount][i * 9 + 2] = obj->normals[indn1 * 3 + 2];

			obj->normals[gCount][i * 9 + 3] = obj->normals[indn2 * 3 + 0];
			obj->normals[gCount][i * 9 + 4] = obj->normals[indn2 * 3 + 1];
			obj->normals[gCount][i * 9 + 5] = obj->normals[indn2 * 3 + 2];

			obj->normals[gCount][i * 9 + 6] = obj->normals[indn3 * 3 + 0];
			obj->normals[gCount][i * 9 + 7] = obj->normals[indn3 * 3 + 1];
			obj->normals[gCount][i * 9 + 8] = obj->normals[indn3 * 3 + 2];

			//三角形的三个点的纹理坐标
			int indt1 = obj->triangles[triangleID].tindices[0];
			int indt2 = obj->triangles[triangleID].tindices[1];
			int indt3 = obj->triangles[triangleID].tindices[2];
			//三角形的三个点的纹理坐标的X,Y值
			obj->vtextures[gCount][i * 6 + 0] = obj->texcoords[indt1 * 2 + 0];
			obj->vtextures[gCount][i * 6 + 1] = obj->texcoords[indt1 * 2 + 1];

			vtextures[gCount][i * 6 + 2] = obj->texcoords[indt2 * 2 + 0];
			vtextures[gCount][i * 6 + 3] = obj->texcoords[indt2 * 2 + 1];

			vtextures[gCount][i * 6 + 4] = obj->texcoords[indt3 * 2 + 0];
			vtextures[gCount][i * 6 + 5] = obj->texcoords[indt3 * 2 + 1];
		}
		//指向下一组模型数据
		group = group->next;
		gCount++;
		//vertices;
	}
	cout << "gcount " << gCount << endl;


}

class virtualModel :public Model
{
public:
	GLMmodel* obj;
	virtualModel()
	{
	}
	~virtualModel()
	{
	}
	Vector3 support_world(Vector3& direction)
	{
		Vector3 res;
		return res;
	}
	void drawModel(GLMmodel* obj);
};

void virtualModel::drawModel(GLMmodel* obj)
{
	if (obj != NULL) {
		free(obj);
	}
	obj = glmReadOBJ("model/elephant.obj");
	
	glmUnitize(obj);

	GLMgroup* group = obj->groups;

	float maxx, maxy, maxz;
	float minx, miny, minz;
	float dx, dy, dz;
	//glm中，vertices3,4,5是第一个点
	maxx = minx = obj->vertices[3];
	maxy = miny = obj->vertices[4];
	maxz = minz = obj->vertices[5];

	//3,4,5的值为第一组v的值
	//找最大最小值：三个维度,遍历模型中顶点的数目
	for (unsigned int i = 2; i <= obj->numvertices; i++)
	{
		GLfloat vx, vy, vz;

		vx = obj->vertices[i * 3 + 0];
		vy = obj->vertices[i * 3 + 1];
		vz = obj->vertices[i * 3 + 2];

		if (vx > maxx)
			maxx = vx;
		if (vx < minx)
			minx = vx;
		if (vy > maxy)
			maxy = vy;
		if (vy < miny)
			miny = vy;
		if (vz > maxz)
			maxz = vz;
		if (vz < minz)
			minz = vz;
	}
	//printf("max\n%f %f, %f %f, %f %f\n", maxx, minx, maxy, miny, maxz, minz);
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;
	//找到模型的顶点中X,Y,Z的值最大的三个点和X,Y,Z值最小的三个点，从而算出模型的长宽高
	//printf("dx,dy,dz = %f %f %f\n", dx, dy, dz);
	//GLfloat normalizationScale = myMax(myMax(dx, dy), dz) / 2;
	//算出模型的长宽高之后，拿到模型中心点所在的位置
	obj->position[0] = (maxx + minx) / 2;
	obj->position[1] = (maxy + miny) / 2;
	obj->position[2] = (maxz + minz) / 2;

	int gCount = 0;
	while (group) {
		for (unsigned int i = 0; i < group->numtriangles; i++) {

			// triangle index
			int triangleID = group->triangles[i];

			// the index of each vertex vindices是三角形的顶点
			int indv1 = obj->triangles[triangleID].vindices[0];
			int indv2 = obj->triangles[triangleID].vindices[1];
			int indv3 = obj->triangles[triangleID].vindices[2];

			//在这里可以得到，见文件triangle.txt
			/*cout << "obj->triangles["<<triangleID<<"].vindices[0] = " << obj->triangles[triangleID].vindices[0] << endl;
			cout << "obj->triangles["<<triangleID<<"].vindices[1] = " << obj->triangles[triangleID].vindices[1] << endl;
			cout << "obj->triangles["<<triangleID<<"].vindices[2] = " << obj->triangles[triangleID].vindices[2] << endl;*/

			// vertices
			GLfloat vx, vy, vz;
			double scale = 0.001;
			//抓取的三角形的第一个顶点的数据X,Y,Z
			vx = obj->vertices[indv1 * 3];
			vy = obj->vertices[indv1 * 3 + 1];
			vz = obj->vertices[indv1 * 3 + 2];

			//读三角形三个点
			//printf("vertices1 %f %f %f\n", vx, vy, vz);
			//这里是把原来的 vertices 转移到新的二维数组中，原来的单位是米，我们转变后单位是毫米
			/* The model size's unit is mm but glm.h's size unit is m, so we use a scale to resize it\n*/
			//让真实的三角形的第一个顶点的数据等于模型中的数据
			vertices[gCount][i * 9 + 0] = vx * scale;
			vertices[gCount][i * 9 + 1] = vy * scale;
			vertices[gCount][i * 9 + 2] = vz * scale;

			//在这里可以得到，见文件vertices.txt
			/*cout << "vertices[" << gCount << "][" << i  << " * 9]+0 = " << vertices[gCount][i * 9 + 0] << endl;
			cout << "vertices[" << gCount << "][" << i << " * 9]+1 = " << vertices[gCount][i * 9 + 1] << endl;
			cout << "vertices[" << gCount << "][" << i  << " * 9]+2 = " << vertices[gCount][i * 9 + 2] << endl;*/
			//抓取的三角形的第二个顶点的数据
			vx = obj->vertices[indv2 * 3];
			vy = obj->vertices[indv2 * 3 + 1];
			vz = obj->vertices[indv2 * 3 + 2];

			vertices[gCount][i * 9 + 3] = vx * scale;
			vertices[gCount][i * 9 + 4] = vy * scale;
			vertices[gCount][i * 9 + 5] = vz * scale;
			//抓取的三角形的第三个顶点的数据
			vx = obj->vertices[indv3 * 3];
			vy = obj->vertices[indv3 * 3 + 1];
			vz = obj->vertices[indv3 * 3 + 2];

			vertices[gCount][i * 9 + 6] = vx * scale;
			vertices[gCount][i * 9 + 7] = vy * scale;
			vertices[gCount][i * 9 + 8] = vz * scale;

			//抓取的三角形的三个点的法向量
			int indn1 = obj->triangles[triangleID].nindices[0];
			int indn2 = obj->triangles[triangleID].nindices[1];
			int indn3 = obj->triangles[triangleID].nindices[2];

			// 三个点的法向量的X,Y,Z数据
			normals[gCount][i * 9 + 0] = obj->normals[indn1 * 3 + 0];
			normals[gCount][i * 9 + 1] = obj->normals[indn1 * 3 + 1];
			normals[gCount][i * 9 + 2] = obj->normals[indn1 * 3 + 2];

			normals[gCount][i * 9 + 3] = obj->normals[indn2 * 3 + 0];
			normals[gCount][i * 9 + 4] = obj->normals[indn2 * 3 + 1];
			normals[gCount][i * 9 + 5] = obj->normals[indn2 * 3 + 2];

			normals[gCount][i * 9 + 6] = obj->normals[indn3 * 3 + 0];
			normals[gCount][i * 9 + 7] = obj->normals[indn3 * 3 + 1];
			normals[gCount][i * 9 + 8] = obj->normals[indn3 * 3 + 2];

			//三角形的三个点的纹理坐标
			int indt1 = obj->triangles[triangleID].tindices[0];
			int indt2 = obj->triangles[triangleID].tindices[1];
			int indt3 = obj->triangles[triangleID].tindices[2];
			//三角形的三个点的纹理坐标的X,Y值
			vtextures[gCount][i * 6 + 0] = obj->texcoords[indt1 * 2 + 0];
			vtextures[gCount][i * 6 + 1] = obj->texcoords[indt1 * 2 + 1];

			vtextures[gCount][i * 6 + 2] = obj->texcoords[indt2 * 2 + 0];
			vtextures[gCount][i * 6 + 3] = obj->texcoords[indt2 * 2 + 1];

			vtextures[gCount][i * 6 + 4] = obj->texcoords[indt3 * 2 + 0];
			vtextures[gCount][i * 6 + 5] = obj->texcoords[indt3 * 2 + 1];
		}
		//指向下一组模型数据
		group = group->next;
		gCount++;
		//vertices;
	}
	cout << "gcount " << gCount << endl;
}

struct Shape
{
	Vector3 position;

	Shape()
	{
	}
	~Shape()
	{
		
	}

	virtual Vector3 support_world(Vector3& direction)
	{
		Vector3 res;
		return res;
	}

	virtual void draw() {}
};

struct SphereShape : public Shape
{
	float mRadius;

	SphereShape(float radius)
		: Shape(), mRadius(radius)
	{
	}

	Vector3 support_world(Vector3& direction)
	{
		Vector3 res = direction.getUnit() * mRadius;
		return (res + position);
	}

	void draw()
	{
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glutWireSphere(mRadius, 10, 10);
		glPopMatrix();
	}

};

struct BoxShape : public Shape
{
	Vector3 mExtent;//拉伸的大小

	BoxShape(Vector3& extent)
		: mExtent(extent)
	{

	}

	Vector3 support_world(Vector3& direction)
	{
		Vector3 res = Vector3(direction.x < 0.0 ? -mExtent.x : mExtent.x,
			direction.y < 0.0 ? -mExtent.y : mExtent.y,
			direction.z < 0.0 ? -mExtent.z : mExtent.z);
		return (res + position);
	}

	void draw()
	{
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		glScalef(mExtent.x, mExtent.y, mExtent.z);
		glutWireCube(2.0);
		glPopMatrix();
	}
};

//GLMmodel* shape1 = glmReadOBJ("model/bunny.obj");
//GLMmodel* shape0 = glmReadOBJ("model/elephant.obj");

Model *shape0 = new probeTipModel();
Model *shape1 = new virtualModel();
//Shape *shape1 = new BoxShape(Vector3(2, 1, 2));
//Shape *shape0 = new SphereShape(2);

//=================================  Unit Test ==================================//

float Width = 600;
float Height = 400;

void print_text(int x, int y, string String)
{
	//(x,y) is from the bottom left of the window
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, Width, 0, Height, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glRasterPos2i(x, y);
	for (unsigned int i = 0; i < String.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, String[i]);
	}
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

/// Camera value
Vector3 mEye;
Vector3 mCenter;
Vector3 mUp;

float TimeStep = 1.0 / 60.0;
bool Pause = true;

// Initialization
void initCamera()
{
	float aspect = Width / Height;
	float zNear = 1.0;
	float zFar = 1024;
	float fov = 45.0;

	mEye = Vector3(0, 0, 0);
	mCenter = Vector3(0, 0, 0);
	mUp = Vector3(0, 1, 0);

	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	gluPerspective(fov, aspect, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	//	  gluLookAt(0.0, 0.0, 5.0,  /* eye is at (0,0,5) */
	//	    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
	//	    0.0, 1.0, 0.);      /* up is in positive Y direction */

}

// Display the scene
void display()
{
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glViewport(0, 0, Width, Height);
	glLoadIdentity();
	//	float aspect = 1.0;//Width / Height;
	//	float zNear  = 1.0;
	//	float zFar   = 1024;
	//	float fov    = 45.0;
	//
	//	glMatrixMode(GL_PROJECTION);
	//	gluPerspective( fov , aspect , zNear , zFar );
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(-mEye.x, -mEye.y, -mEye.z, mCenter.x, mCenter.y, mCenter.z, mUp.x, mUp.y, mUp.z);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ContactBasicData Date;
	if (GJKEPAGenerator::CollisionSolveGJK_EPA(shape0, shape1, &Date))
	{
		Date.normal.normalize();

		Vector3 a = Date.point;
		Vector3 b = Date.point + Date.normal * Date.penetration;

		glPushMatrix();
		glColor3f(1, 1, 1);
		glLineWidth(20);
		glBegin(GL_LINES);
		glVertex3f(a.x, a.y, a.z);
		glVertex3f(b.x, b.y, b.z);
		glEnd();
		//这个下面是描述wireframe的线的粗细
		glLineWidth(1);
		glColor3f(0, 1, 0);

		glPopMatrix();

		print_text(20, Height - 60, " COLLISION : Yes");
	}
	else
	{
		glColor3f(1, 1, 1);
		print_text(20, Height - 60, " COLLISION : No");
	}
	shape0->drawModel();
	shape1->drawModel();
	print_text(20, Height - 20, " KEY   : (UP,DOWN,RIGHT,LEFT) : Move-Object");
	print_text(20, Height - 40, " MOUSE : Move-Camera");
	glutSwapBuffers();


}

float zoom_distance = 25;

float oldX = 0;
float oldY = 0;

float angle_X = 0;
float angle_Y = 0;

int   mouse_button = -1;

void UpdateTime(void)
{

	//**********  Camera position *******//
	Vector3 DircetionLeft(1, 0, 0);
	Vector3 DircetionLook(0, 0, zoom_distance);
	DircetionLook.rotateAroundAxis(Vector3(0, 1, 0), angle_X);
	DircetionLeft.rotateAroundAxis(Vector3(0, 1, 0), angle_X);
	DircetionLook.rotateAroundAxis(DircetionLeft, angle_Y);
	mEye = DircetionLook;//+ Vector3(-10,-15,0);
	//************ Render Scene *********//
	display();
};


// Reshape function
void reshape(int width, int height)
{
	Width = width;
	Height = height;
}


// Called when a mouse button event occurs
void mouseButton(int button, int state, int x, int y)
{
	mouse_button = button;

	float aspect = Width / Height;
	float m_x = ((x / Width) - 0.5f) * aspect * 0.834;
	float m_y = ((y / Height) - 0.5f) * 0.834;

	oldX = m_x;
	oldY = m_y;

	if (state == GLUT_UP)
	{
		if (button == GLUT_WHEEL_UP)
		{
			zoom_distance -= 0.5;
		}
		else if (button == GLUT_WHEEL_DOWN)
		{
			zoom_distance += 0.5;
		}
	}
	if (button == GLUT_RIGHT_BUTTON)
	{

	}
}

// Called when a mouse motion event occurs
void mouseMotion(int x, int y)
{
	float aspect = Width / Height;
	float m_x = ((x / Width) - 0.5f) * aspect * 0.834;
	float m_y = ((y / Height) - 0.5f) * 0.834;

	float speedX = (m_x - oldX);
	float speedY = (m_y - oldY);

	if (mouse_button == GLUT_LEFT_BUTTON)
	{
		float coff = 5.5f;
		angle_X += speedX * coff;
		angle_Y += speedY * coff;
	}

	oldX = m_x;
	oldY = m_y;

}

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: exit(0); break;
	case ' ': Pause = !Pause; break;
	}

}


void processSpecialKeys(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP:
		shape0->position = shape0->position + Vector3(0, 0, 0.1);
		break;
	case GLUT_KEY_DOWN:
		shape0->position = shape0->position - Vector3(0, 0, 0.1);
		break;
	case GLUT_KEY_LEFT:
		shape0->position = shape0->position + Vector3(0.1, 0, 0);
		break;
	case GLUT_KEY_RIGHT:
		shape0->position = shape0->position - Vector3(0.1, 0, 0);
		break;
	}
}

// Main function
int main(int argc, char** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// Initialize the size of the GLUT windows
	glutInitWindowSize(Width, Height);
	// Initialize the position of the GLUT windows
	//glutInitWindowposition(0, 0);
	// Initialize the create window
	glutCreateWindow("Demo: GJK-EPA");

	initCamera();

	glutDisplayFunc(display);

	// Glut Idle function that is continuously called
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);

	// here are the new entries
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);


	glutIdleFunc(UpdateTime);

	// Glut main looop
	glutMainLoop();


	/**/
	return 0;
}