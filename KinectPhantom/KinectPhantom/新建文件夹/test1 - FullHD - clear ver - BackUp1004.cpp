/*--------------Standard Library-------------*/
#include <iostream>
#include <math.h>

/*--------------OpenCV-------------*/
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cv.h>

/*--------------Kinect-------------*/
#include <Kinect.h>
#include <windows.h>   

/*--------------OpenGL-------------*/
#include <gl/Gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

#pragma comment(lib,"opengl32.lib")  
#pragma comment(lib,"GLU32.LIB")  
#pragma comment(lib,"GLUT32.LIB")

/*--------------ARTool Kit-------------*/
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

/*--------------OpenHaptics-------------*/
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>

#include <HLU/hlu.h>


using namespace std;
using namespace cv;


/*----------------------------------------------*/
/*----------------OpenGL Declare----------------*/
/*----------------------------------------------*/
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
int FPS = 0;
BYTE* pBufferColor_RGB = nullptr;
GLfloat fAspect;
void GLInit(void);                         //OpenGL Initial
void SpecialKeys(int key, int x, int y);   //OpenGL Key Func
void timer(int value);                     //OpenGL Time Func
void RenderScene(void);                    //OpenGL Render Func
bool BG_IS_OPEN = TRUE;                    //Flag for BG
void SceneWithBackground(void);            //Background Func
void SceneWithoutBackground(void);
int DRAWING_TYPE = 0;                      //Flag for drawing type: 0 = none, 1 = mesh, 2 = point cloud
void DrawPointCloud(void);                 //Drawing Type
void DrawMeshes(void);

//Draw Cubic
float CubicUnitW = 0.1f * 2;
float CubicUnitH = 0.05f * 2;
float CubicUnitT = 0.01f * 2;
Point3f CubicPosi;
Point3f CubicPosiinHLCoord;
bool ROI_IS_COLLIDE = FALSE;   //FLAG of if prob insert into the virtual cube
void DrawCubic(void);        //Draw virtula cube Func

//Draw path
void DrawPath(void);

//Creat Pop Up menu
enum {
	MENU_WTBG_NONE = 1,
	MENU_WTBG_MESH,
	MENU_WTBG_POINTCLOUD,
	MENU_WOBG_MESH,
	MENU_WOBG_POINTCLOUD,
	MENU_PATHGEN_LINE1,
	MENU_PATHGEN_LINE2,
	MENU_PATHGEN_LINE3
};
int menu_value = 0;
int submenuID_BG;
int submenuID_WTBG_DrawType;
int submenuID_WOBG_DrawType;
int submenuID_PathType;
int LINE_TYPE = 0;                      //Flag for line type: 0 = 1st orde, 1 = 2nd order, 2 = 3rd order
void BuildPopupMenu(void);
void menuCB(int menu_value);



/*----------------------------------------------*/
/*----------------Kinect Declare----------------*/
/*----------------------------------------------*/
//Initial (Sensor)
IKinectSensor* pSensor = nullptr;
ICoordinateMapper* Mapper = nullptr;
IDepthFrameReader* pFrameReaderDepth = nullptr;
IColorFrameReader* pFrameReaderColor = nullptr;
void KineticInit(void);           //Kinect Initial
void KineticUpdate(void);         //Kinect Update Frame

//Depth Map
int iWidthDepth = 0;
int iHeightDepth = 0;
UINT depthPointCount = 0;
UINT16* pBufferDepth = nullptr;   //Depth map origin format
UINT16 uDepthMin = 0, uDepthMax = 0;
Mat mDepthImg;                    //Depth map UINT16
Mat mImg8bit;                     //Depth map CV_8U

//Color Map
int iWidthColor = 0;
int iHeightColor = 0;
UINT colorPointCount = 0;
UINT uBufferSizeColor = 0;
BYTE* pBufferColor = nullptr;    //Color map origin format (RGBA)
Mat mColorImg;                   //Color map OpenCV format (BGRA)

//Map to camera space point
CameraSpacePoint* pCSPoints = nullptr;
void ShowImage(void);            //Display Image Func



/*---------------------------------------------------------------*/
/*----------------Region Of Interest(ROI) Declare----------------*/
/*---------------------------------------------------------------*/
//Initial
Rect2i ROI_rect;
Point ROI_p1, ROI_p2;   //start point and end point of ROI
Mat ROI;
bool ROI_S1 = FALSE;    //FLAG of mouse event
bool ROI_S2 = FALSE;    //FLAG of mouse event
void onMouseROI(int event, int x, int y, int flags, void* param);   //Mouse callback Func
void FindROI(void);                                                 //Find ROI Func

//Tracking
int ROIcount = 0;
Point2i ROICenterColorS_Old, ROICenterColorS_New;   //center of tracking object of ROI in Color Space
Point2i* ROIPixel = nullptr;
CameraSpacePoint* ROICameraSP = nullptr;
void MoveROI(void);                                //Tracking ROI Func

//Mapping
int ROIDepthCount = 0;
Point3f ROICenterCameraS;
void CameraSpaceROI(void);                         //Mapping ROI Func

//Storage
CameraSpacePoint* ROICameraSP_Storage = nullptr;
int ROIStorageCount = 0;
bool IS_KEY_F1_UP = FALSE;                        //FLAG of data collecting, see SpecialKeys()
bool IS_KEY_F3_UP = FALSE;
void ROICameraSPStorage(void);                    //Storage ROI Func



/*-------------------------------------------------------*/
/*----------------Path Generation Declare----------------*/
/*-------------------------------------------------------*/
//const int PathNum = 5;
//const int 
bool IS_PathGenProjToPlane = FALSE;       //If raw data is projected to plane
bool IS_PathGenRegression = FALSE;
void PathGenProjToPlane(CameraSpacePoint* Data, int DataNum, double* PlaneCoeff, CameraSpacePoint* Result);
void PathGenCoordWorldToCubic(CameraSpacePoint* Data, int DataNum, CameraSpacePoint* Result);
void PathGenCoordCubicToWorld(CameraSpacePoint* Data, int DataNum, CameraSpacePoint* Result);
void PathGenCalcMean(CameraSpacePoint* Data, int DataNum, double* Result);
void PathGenRegression(CameraSpacePoint* Data, int DataNum, double* Result);



/*----------------------------------------------*/
/*--------------OpenHaptics Declare-------------*/
/*----------------------------------------------*/
static HHD hHD = HD_INVALID_HANDLE;
static HHLRC hHLRC = 0;
void initHL(void);
HLuint spring;




/*------------------------------------------*/
/*--------------OpenGL Function-------------*/
/*------------------------------------------*/
void GLInit()
{
	GLfloat  whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	GLfloat  sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat	 lightPos[] = { -50.f, 25.0f, 250.0f, 0.0f };

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void SpecialKeys(int key, int x, int y)
{
	//Press F1 to start recoard path of the probe
	//Press again to end
	if (key == GLUT_KEY_F1)
	{
		if (IS_KEY_F1_UP)
		{
			IS_KEY_F1_UP = FALSE;
			cout << endl << "End collecting data..." << endl;

		}
		else
		{
			IS_KEY_F1_UP = TRUE;
			cout << endl << "Dtart collecting data..." << endl;

		}
	}
	//Press F2 to clear
	else if (key == GLUT_KEY_F2)
	{
		delete[] ROICameraSP_Storage;
		ROICameraSP_Storage = nullptr;

		ROIStorageCount = 0;

		//IS_KEY_F3_UP = FALSE;
		IS_PathGenProjToPlane = FALSE;
		IS_PathGenRegression = FALSE;

		cout << endl << "Clear ROICameraSP_Storage..." << endl;
	}
	else if (key == GLUT_KEY_F3)
	{
		if (IS_KEY_F3_UP)
		{
			cout << endl << "Calculating Please Wait..." << endl;
		}
		else
		{
			IS_KEY_F3_UP = TRUE;
			cout << endl << "Start Calculating..." << endl;
		}
	}

	glutPostRedisplay();
}

void timer(int value)
{
	switch (value)
	{
	//OpenGL refresh timmer
	case 0:
		//printf("FPS = %d\n", FPS);
		FPS = 0;
		glutTimerFunc(1000, timer, 0);
		break;
	//Probe path sampling timmer
	case 1:
		ROICameraSPStorage();
		glutTimerFunc(125, timer, 1);
		break;
	}
	
}

void BuildPopupMenu()
{
	//3rd layer
	submenuID_WTBG_DrawType = glutCreateMenu(menuCB);
	glutAddMenuEntry("None", MENU_WTBG_NONE);
	glutAddMenuEntry("Mesh", MENU_WTBG_MESH);
	glutAddMenuEntry("Point Cloud", MENU_WTBG_POINTCLOUD);

	submenuID_WOBG_DrawType = glutCreateMenu(menuCB);
	glutAddMenuEntry("Mesh", MENU_WOBG_MESH);
	glutAddMenuEntry("Point Cloud", MENU_WOBG_POINTCLOUD);

	//2nd layer
	submenuID_BG = glutCreateMenu(menuCB);
	glutAddSubMenu("Color Image Background", submenuID_WTBG_DrawType);
	glutAddSubMenu("Empty Background", submenuID_WOBG_DrawType);

	submenuID_PathType = glutCreateMenu(menuCB);
	glutAddMenuEntry("Line 1", MENU_PATHGEN_LINE1);
	glutAddMenuEntry("Line 2", MENU_PATHGEN_LINE2);
	glutAddMenuEntry("Line 3", MENU_PATHGEN_LINE3);

	//1st layer
	glutCreateMenu(menuCB);
	glutAddSubMenu("Background type", submenuID_BG);
	glutAddSubMenu("Path type", submenuID_PathType);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void menuCB(int menu_value)
{
	cout << menu_value << endl;

	switch (menu_value)	{

	case MENU_WTBG_NONE:
		BG_IS_OPEN = TRUE;
		DRAWING_TYPE = 0;
		break;

	case MENU_WTBG_MESH:
		BG_IS_OPEN = TRUE;
		DRAWING_TYPE = 1;
		break;

	case MENU_WTBG_POINTCLOUD:
		BG_IS_OPEN = TRUE;
		DRAWING_TYPE = 2;
		break;

	case MENU_WOBG_MESH:
		BG_IS_OPEN = FALSE;
		DRAWING_TYPE = 1;
		break;

	case MENU_WOBG_POINTCLOUD:
		BG_IS_OPEN = FALSE;
		DRAWING_TYPE = 2;
		break;
	case MENU_PATHGEN_LINE1:
		LINE_TYPE = 0;
		break;
	case MENU_PATHGEN_LINE2:
		LINE_TYPE = 1;
		break;
	case MENU_PATHGEN_LINE3:
		LINE_TYPE = 2;
		break;

	}
}

void SceneWithBackground()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, iWidthColor, iHeightColor);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, iWidthColor, iHeightColor, 0, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Draw backgroung through RGBA image from Kinect
	glDrawPixels(iWidthColor, iHeightColor, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor_RGB);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(54.5f, (double)iWidthColor / (double)iHeightColor, 0.01f, 20000.0f);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	glTranslatef(0.059f, -0.015f, 0.0f);
}

void SceneWithoutBackground()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(54.5f, (double)iWidthColor / (double)iHeightColor, 0.001f, 20000.0f);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

}

void DrawPointCloud()
{
	glPushMatrix();

	glPointSize(1.0f);
	glBegin(GL_POINTS);

	for (int i = 0; i < colorPointCount; i++)
	{
		glColor3ub(pBufferColor[4 * i], pBufferColor[4 * i + 1], pBufferColor[4 * i + 2]);
		GLfloat pX = pCSPoints[i].X;
		GLfloat pY = pCSPoints[i].Y;
		GLfloat pZ = -pCSPoints[i].Z;
		glVertex3f(pX, pY, pZ);
	}

	glEnd();
	glPopMatrix();

}

void DrawMeshes()
{
	if (BG_IS_OPEN)
	{
		glPushMatrix();
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_TRIANGLE_STRIP);
		
		//glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

		for (int i = 0; i < colorPointCount; i++)
		{
			//glColor3ub(pBufferColor[4 * i], pBufferColor[4 * i + 1], pBufferColor[4 * i + 2]);
			glColor4ub(pBufferColor[4 * i], pBufferColor[4 * i + 1], pBufferColor[4 * i + 2], 125);

			if (i % iWidthColor < (iWidthColor - 1) && i < iWidthColor*(iHeightColor - 1))
			{
				glVertex3f(pCSPoints[i].X, pCSPoints[i].Y, -pCSPoints[i].Z);
				glVertex3f(pCSPoints[i + iWidthColor].X, pCSPoints[i + iWidthColor].Y, -pCSPoints[i + iWidthColor].Z);
				glVertex3f(pCSPoints[i + 1].X, pCSPoints[i + 1].Y, -pCSPoints[i + 1].Z);
				glVertex3f(pCSPoints[i + iWidthColor + 1].X, pCSPoints[i + iWidthColor + 1].Y, -pCSPoints[i + iWidthColor + 1].Z);

			}
		}

		glDisable(GL_BLEND);

		glEnd();
		glPopMatrix();

	}
	else
	{
		glPushMatrix();
		glBegin(GL_TRIANGLE_STRIP);

		for (int i = 0; i < colorPointCount; i++)
		{
			glColor3ub(pBufferColor[4 * i], pBufferColor[4 * i + 1], pBufferColor[4 * i + 2]);

			if (i % iWidthColor < (iWidthColor - 1) && i < iWidthColor*(iHeightColor - 1))
			{
				glVertex3f(pCSPoints[i].X, pCSPoints[i].Y, -pCSPoints[i].Z);
				glVertex3f(pCSPoints[i + iWidthColor].X, pCSPoints[i + iWidthColor].Y, -pCSPoints[i + iWidthColor].Z);
				glVertex3f(pCSPoints[i + 1].X, pCSPoints[i + 1].Y, -pCSPoints[i + 1].Z);
				glVertex3f(pCSPoints[i + iWidthColor + 1].X, pCSPoints[i + iWidthColor + 1].Y, -pCSPoints[i + iWidthColor + 1].Z);

			}
		}
		glEnd();
		glPopMatrix();
	}

}

void DrawCubic()
{
	static GLuint displayList = 0;

	if (displayList)
	{
		glCallList(displayList);
	}
	else
	{
		displayList = glGenLists(1);
		glNewList(displayList, GL_COMPILE_AND_EXECUTE);

		glPushMatrix();
		
		glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);
		//glRotated(45, 0, 1, 0);


		glBegin(GL_QUADS);        // Draw The Cube Using quads
		//Front
		glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
		glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
		glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);    // 3
		glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // 4
		//Bottom
		glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
		glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
		glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
		glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
		//Left
		glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
		glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
		glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
		glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // 4
		//Right
		glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
		glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
		glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
		glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);    // 3
		//Top
		glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
		glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
		glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
		glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
		//Down
		glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // 4
		glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
		glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
		glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);    // 3

		glEnd();


		glLineWidth(2.5);
		glBegin(GL_LINE_STRIP);        // Draw The Strip Using Line Strip
		glColor3ub(40, 148, 255);

		//glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);     // 1
		//glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);     // 2
		//glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);     // 3
		//glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);     // 4

		//glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
		//glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
		//glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
		//glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8

		glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
		glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
		glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
		glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
		glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5

		glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
		glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // 4
		glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5

		glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
		glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
		glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);    // 3

		glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
		glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
		glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);    // 3

		glEnd();


		glLineWidth(2.5);
		glBegin(GL_LINE_STRIP);
		glColor3ub(217, 179, 0);

		glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // Top Right Of The Quad (Front)
		glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // Top Left Of The Quad (Front)
		glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // Bottom Left Of The Quad (Front)
		glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);    // Bottom Right Of The Quad (Front)
		glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // Top Right Of The Quad (Front)

		glEnd();

		glPopMatrix();
		glEndList();
	}

}

void DrawPath()
{
	const float T_MIN = -0.19;
	const float T_MAX =  0.19;
	const float T_PRECISION = 0.001;
	
	
	glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);
	//glRotated(45, 0, 1, 0);
	glTranslatef(0, 0, CubicUnitT);

	switch (LINE_TYPE){

	case 0:
		glBegin(GL_LINE_STRIP);
		for (float t = T_MIN; t <= T_MAX; t += T_PRECISION)
		{
			glVertex2d(t, 0.3*t);
		}
		glEnd();
		break;

	case 1:
		glBegin(GL_LINE_STRIP);
		for (float t = T_MIN; t <= T_MAX; t += T_PRECISION)
		{
			glVertex2d(t, 200.0 / 81.0 * pow(t, 2) - 1.0 / 25.0);
		}
		glEnd();
		break;

	case 2:	
		glBegin(GL_LINE_STRIP);
		for (float t = T_MIN; t <= T_MAX; t += T_PRECISION)
		{
			glVertex2d(t, -219.4787 / 8.0 * pow(t, 3) + 2.0 / 3.0 * t);
		}
		glEnd();
		break;

	}




}

void RenderScene()
{
	/*--------------RenderScene Background & Drawing Type-------------*/
	if (BG_IS_OPEN)
	{
		SceneWithBackground();

		switch (DRAWING_TYPE){
		case 0:
			break;
		case 1:
			//cout << endl << "Error in drawing type" << endl;
			DrawMeshes();
			break;
		case 2:
			DrawPointCloud();
			break;
		}
	}
	else
	{
		SceneWithoutBackground();

		switch (DRAWING_TYPE){
		case 0:
			cout << endl << "Error in drawing type" << endl;
			break;
		case 1:
			DrawMeshes();
			break;
		case 2:
			DrawPointCloud();
			break;
		}
	}


	/*--------------ROI Mapping-------------*/
	if (ROIDepthCount != 0)
	{
		glPushMatrix();
		glPointSize(2.0f);
		glBegin(GL_POINTS);
			for (int i = 0; i < ROIDepthCount; i++)
			{
				glColor3ub(0, 255, 0);
				GLfloat pX = ROICameraSP[i].X;
				GLfloat pY = ROICameraSP[i].Y;
				GLfloat pZ = -ROICameraSP[i].Z;
				glVertex3f(pX, pY, pZ);

				/*--------------Touching Detected-------------*/
				if (abs(pX - CubicPosi.x) < CubicUnitW)
				{
					if (abs(pY - CubicPosi.y) < CubicUnitH)
					{
						if (abs(pZ - CubicPosi.z) < CubicUnitT)
						{
							ROI_IS_COLLIDE = TRUE;
						}
					}
				}

			}

		glEnd();
		glPopMatrix();
	}


	/*--------------ROI Storage-------------*/
	//if (IS_KEY_F1_UP)
	//{
	//	ROICameraSPStorage();
	//}
	if (ROIStorageCount != 0)
	{
		glPushMatrix();
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		
		if (IS_PathGenProjToPlane || IS_PathGenRegression)
		{
			glColor3ub(255, 255, 55);
		}
		else
		{
			glColor3ub(255, 0, 137);
		}
		for (int i = 0; i < ROIStorageCount; i++)
		{
			GLfloat pX = ROICameraSP_Storage[i].X;
			GLfloat pY = ROICameraSP_Storage[i].Y;
			GLfloat pZ = ROICameraSP_Storage[i].Z;
			glVertex3f(pX, pY, pZ);

		}

		glEnd();
		glPopMatrix();
	}


	/*--------------Test PathGen Functions-------------*/
	//CameraSpacePoint* test = new CameraSpacePoint[3];
	//test[0].X = 2;
	//test[0].Y = 2;
	//test[0].Z = 2;
	//test[1].X = 1;
	//test[1].Y = 3;
	//test[1].Z = 8;
	//test[2].X = 4;
	//test[2].Y = 5;
	//test[2].Z = 1;
	//double testmean[3] = { 0 };
	//PathGenCalcMean(test, 3, testmean);
	//cout << endl << testmean[0] << " " << testmean[1] << " " << testmean[2] << endl;

	//double Coeff[4] = { 0, 0, 1, -2.5 };
	//CameraSpacePoint* testnew = new CameraSpacePoint[3];
	//PathGenProjToPlane(test, 3, Coeff, testnew);
	//cout << endl << testnew[0].X << " " << testnew[0].Y << " " << testnew[0].Z 
	//	 << endl << testnew[1].X << " " << testnew[1].Y << " " << testnew[1].Z 
	//	 << endl << testnew[2].X << " " << testnew[2].Y << " " << testnew[2].Z << endl;

	if (IS_KEY_F3_UP && ROIStorageCount > 0)
	{
		//ax + by + cz = d = 0
		double Coeff[4] = { 0, 0, 1, - CubicPosi.z - CubicUnitT };
		PathGenProjToPlane(ROICameraSP_Storage, ROIStorageCount, Coeff, ROICameraSP_Storage);

		//cout << endl << CubicPosi.z + CubicUnitT << endl;
		//cout << ROICameraSP_Storage[ROIStorageCount / 2].Z << endl;

		glPushMatrix();
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (int i = 0; i < ROIStorageCount; i++)
		{
			glColor3ub(255, 255, 55);
			GLfloat pX = ROICameraSP_Storage[i].X;
			GLfloat pY = ROICameraSP_Storage[i].Y;
			GLfloat pZ = ROICameraSP_Storage[i].Z;
			glVertex3f(pX, pY, pZ);
		}

		glEnd();
		glPopMatrix();

		IS_PathGenProjToPlane = TRUE;
		IS_KEY_F3_UP = FALSE;

		//cout << endl << "IS_PathGenProjToPlane = TRUE" << endl;
	}

	if (IS_PathGenProjToPlane && ROIStorageCount > 0)
	{
		CameraSpacePoint* ROICameraSP_Regression = new CameraSpacePoint[ROIStorageCount];
		//ordr; y = ax + b
		double RegresCoeff[3] = { 1, 0, 0 };

		PathGenCoordWorldToCubic(ROICameraSP_Storage, ROIStorageCount, ROICameraSP_Regression);
		PathGenRegression(ROICameraSP_Regression, ROIStorageCount, RegresCoeff);
		//PathGenRegression(ROICameraSP_Storage, ROIStorageCount, RegresCoeff);

		//cout << endl << RegresCoeff[1] << " " << RegresCoeff[2] << endl;

		double T_MIN = 0;
		double T_MAX = 0;
		double T_PRECISION = 0;

		for (int i = 0; i < ROIStorageCount; i++)
		{
			if (T_MIN > ROICameraSP_Regression[i].X)
			{
				T_MIN = ROICameraSP_Regression[i].X;
			}
			else if (T_MAX < ROICameraSP_Regression[i].X)
			{
				T_MAX = ROICameraSP_Regression[i].X;
			}
		}

		T_PRECISION = (T_MAX - T_MIN) / 100;

		//const double T_MIN = -0.19;
		//const double T_MAX = 0.19;
		//const double T_PRECISION = 0.001;

		glPushMatrix();
		glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z + CubicUnitT);
		//glRotated(45, 0, 1, 0);

		glColor3ub(0, 255, 0);

		glBegin(GL_LINE_STRIP);
		for (double t = T_MIN; t <= T_MAX; t += T_PRECISION)
		{
			glVertex2d(t, RegresCoeff[1] * t + RegresCoeff[2]);
		}
		glEnd();
		glPopMatrix();

		IS_PathGenRegression = TRUE;

		//cout << endl << "IS_PathGenRegression = TRUE" << endl;
	}
	
	//GLfloat m[16] = { 0 };
	//m[0] = m[5] = m[10] = m[15] = 1;
	//m[12] = 4;
	//m[13] = 2;
	//m[14] = 5;
	//
	//glPushMatrix();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//
	//glTranslatef(0, 0, -10);
	//
	//glMultMatrixf(m);
	//
	//glGetFloatv(GL_MODELVIEW_MATRIX, m);
	//glPopMatrix();
	//	
	//cout << endl << m[0] << " " << m[4] << " " << m[8] << " " << m[12] << endl;
	//cout << m[1] << " " << m[5] << " " << m[9] << " " << m[13] << endl;
	//cout << m[2] << " " << m[6] << " " << m[10] << " " << m[14] << endl;
	//cout << m[3] << " " << m[7] << " " << m[11] << " " << m[15] << endl;



	/*--------------Draw Cubic-------------*/
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	if (ROI_IS_COLLIDE)
	{
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	}
	else
	{
		glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
	}

	DrawCubic();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glPopMatrix();


	/*--------------Draw Path-------------*/
	glPushMatrix();

	if (ROI_IS_COLLIDE)
	{
		glColor3ub(64, 128, 128);
	}
	else
	{
		glColor3ub(247, 80, 0);
	}

	DrawPath();

	glPopMatrix();





	/*-----------------------------------------------------*/
	/*--------------OpenHaptics Force Generate-------------*/
	/*-----------------------------------------------------*/
	hlBeginFrame();
	if (ROI_IS_COLLIDE == TRUE)
	{
		HDdouble anchor[3];
		hlGetDoublev(HL_PROXY_POSITION, anchor);

		hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.8);
		hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 10.0);
		hlEffectdv(HL_EFFECT_PROPERTY_POSITION, anchor);
		hlStartEffect(/*HL_EFFECT_SPRING*/HL_EFFECT_FRICTION, spring);
	}
	else
	{
		hlStopEffect(spring);
	}
	hlEndFrame();

	ROI_IS_COLLIDE = FALSE;


	glutSwapBuffers();
}



/*------------------------------------------*/
/*--------------Kinect Function-------------*/
/*------------------------------------------*/
void KineticInit()
{
	// 1. Sensor related code
	cout << "Try to get default sensor" << endl;
	{
		if (GetDefaultKinectSensor(&pSensor) != S_OK)
		{
			cerr << "Get Sensor failed" << endl;
			return;
		}

		cout << "Try to open sensor" << endl;
		if (pSensor->Open() != S_OK)
		{
			cerr << "Can't open sensor" << endl;
			return;
		}
	}

	// 2. Color related code
	cout << "Try to get color source" << endl;
	{
		// Get frame source
		IColorFrameSource* pFrameSource = nullptr;
		if (pSensor->get_ColorFrameSource(&pFrameSource) != S_OK)
		{
			cerr << "Can't get color frame source" << endl;
			return;
		}

		// Get frame description
		cout << "get color frame description\n" << endl;
		IFrameDescription* pFrameDescription = nullptr;
		if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
		{
			pFrameDescription->get_Width(&iWidthColor);
			pFrameDescription->get_Height(&iHeightColor);

			colorPointCount = iWidthColor * iHeightColor;
			uBufferSizeColor = colorPointCount * 4 * sizeof(BYTE);

			pCSPoints = new CameraSpacePoint[colorPointCount];
			pBufferColor = new BYTE[4 * colorPointCount];
			pBufferColor_RGB = new BYTE[4 * colorPointCount];
		}
		pFrameDescription->Release();
		pFrameDescription = nullptr;

		// get frame reader
		cout << "Try to get color frame reader" << endl;
		if (pFrameSource->OpenReader(&pFrameReaderColor) != S_OK)
		{
			cerr << "Can't get color frame reader" << endl;
			return;
		}

		// release Frame source
		cout << "Release frame source" << endl;
		pFrameSource->Release();
		pFrameSource = nullptr;
	}

	// 3. Depth related code
	cout << "Try to get depth source" << endl;
	{
		// Get frame source
		IDepthFrameSource* pFrameSource = nullptr;
		if (pSensor->get_DepthFrameSource(&pFrameSource) != S_OK)
		{
			cerr << "Can't get depth frame source" << endl;
			return;
		}
		if (pSensor->get_DepthFrameSource(&pFrameSource) == S_OK)
		{
			pFrameSource->get_DepthMinReliableDistance(&uDepthMin);
			pFrameSource->get_DepthMaxReliableDistance(&uDepthMax);
		}

		// Get frame description
		cout << "get depth frame description\n" << endl;
		IFrameDescription* pFrameDescription = nullptr;
		if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
		{
			pFrameDescription->get_Width(&iWidthDepth);
			pFrameDescription->get_Height(&iHeightDepth);

			depthPointCount = iWidthDepth * iHeightDepth;
			pBufferDepth = new UINT16[depthPointCount];
		}
		pFrameDescription->Release();
		pFrameDescription = nullptr;

		// get frame reader
		cout << "Try to get depth frame reader" << endl;
		if (pFrameSource->OpenReader(&pFrameReaderDepth) != S_OK)
		{
			cerr << "Can't get depth frame reader" << endl;
			return;
		}
		// release Frame source
		cout << "Release frame source" << endl;
		pFrameSource->Release();
		pFrameSource = nullptr;
	}

	// 4. Coordinate Mapper
	if (pSensor->get_CoordinateMapper(&Mapper) != S_OK)
	{
		cerr << "get_CoordinateMapper failed" << endl;
		return;
	}

	 mDepthImg = cv::Mat::zeros(iHeightDepth, iWidthDepth, CV_16UC1);
	 mImg8bit = cv::Mat::zeros(iHeightDepth, iWidthDepth, CV_8UC1);
	 mColorImg = cv::Mat::zeros(iHeightColor, iWidthColor, CV_8UC4);

	 CubicPosi.x = 0.069f - 0.05f;
	 CubicPosi.y = -0.0647f + 0.05f;
	 CubicPosi.z = -0.4227f - 0.25f;
}

void KineticUpdate()
{
	/*--------------Read color data-------------*/
	IColorFrame* pFrameColor = nullptr;
	if (pFrameReaderColor->AcquireLatestFrame(&pFrameColor) == S_OK)
	{
		pFrameColor->CopyConvertedFrameDataToArray(uBufferSizeColor, pBufferColor, ColorImageFormat_Rgba);
		pFrameColor->CopyConvertedFrameDataToArray(uBufferSizeColor, mColorImg.data, ColorImageFormat_Bgra);
		
		pFrameColor->Release();
		pFrameColor = nullptr;

		{
			FPS++;
			glutPostRedisplay();
		}
		
	}

	/*--------------Read depth data-------------*/
	IDepthFrame* pDFrameDepth = nullptr;
	if (pFrameReaderDepth->AcquireLatestFrame(&pDFrameDepth) == S_OK)
	{
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, pBufferDepth);
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, reinterpret_cast<UINT16*>(mDepthImg.data));

		pDFrameDepth->Release();
		pDFrameDepth = nullptr;
	}


	/*--------------Mapper Function (Point Cloud)-------------*/
	//Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);
	
	//GaussianBlur(mDepthImg, mDepthImg, Size(3, 3), 0, 0);
	//blur(mDepthImg, mDepthImg, Size(3, 3));

	//pBufferDepth = reinterpret_cast<UINT16*>(mDepthImg.data);

	Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);


	/*--------------OpenGL Background Converting-------------*/
	for (int i = 0; i < iWidthColor; i++)
	{
		for (int j = 0; j < iHeightColor; j++)
		{
			int indexCV = j*iWidthColor + i;
			int indexGL = (iHeightColor - 1 - j)*iWidthColor + i;

			pBufferColor_RGB[4 * indexGL] = pBufferColor[4 * indexCV];
			pBufferColor_RGB[4 * indexGL + 1] = pBufferColor[4 * indexCV + 1];
			pBufferColor_RGB[4 * indexGL + 2] = pBufferColor[4 * indexCV + 2];
			pBufferColor_RGB[4 * indexGL + 3] = pBufferColor[4 * indexCV + 3];

		}
	}

	/*--------------Call Window Function-------------*/
	ShowImage();
}

void ShowImage()
{
	namedWindow("Depth Map");
	namedWindow("Color Map");

	/*--------------Depth Image-------------*/
	// Convert from 16bit to 8bit
	mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);

	/*--------------Set Mouse Callback Function and Find ROI-------------*/
	cvSetMouseCallback("Color Map", onMouseROI, NULL);
	cv::imshow("Depth Map", mImg8bit);


	if (ROI_S1 == false && ROI_S2 == false)
	{
		// Initial State
		ROI = mColorImg.clone();
		cv::imshow("Color Map", ROI);
	}
	else if (ROI_S2 == true)
	{
		// CV_EVENT_LBUTTONUP：ROI選取完畢
		if (ROI_S1 == false)
		{
			int thickness = 2;

			ROI = mColorImg.clone();
			rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), thickness);
			cv::imshow("Color Map", ROI);

			FindROI();
		}
		//CV_EVENT_MOUSEMOVE：只選了左上角
		else
		{
			cv::imshow("Color Map", ROI);
		}

	}

}



/*---------------------------------------*/
/*--------------ROI Function-------------*/
/*---------------------------------------*/
void onMouseROI(int event, int x, int y, int flags, void* param)
{
	int thickness = 2;

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		////当前鼠标位置（x，y）  
		ROI_rect.x = x;
		ROI_rect.y = y;

		ROI_S1 = true;
		ROI_S2 = false;

	}
	else if (ROI_S1&&event == CV_EVENT_MOUSEMOVE)
	{
		ROI_S2 = true;

		ROI_p1 = Point(ROI_rect.x, ROI_rect.y);
		ROI_p2 = Point(x, y);

		ROI = mColorImg.clone();
		rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), 2);
		cv::imshow("Color Map", ROI);

	}
	else if (ROI_S1&&event == CV_EVENT_LBUTTONUP)
	{
		ROI_S1 = false;

		ROI_rect.height = y - ROI_rect.y;
		ROI_rect.width = x - ROI_rect.x;

		ROI_p2 = Point(x, y);

	}
}

void FindROI()
{
	namedWindow("ROI");
	namedWindow("YCrCb");

	/*--------------Find ROI-------------*/
	Mat ROI_Image = mColorImg.colRange(ROI_p1.x, ROI_p2.x + 1).rowRange(ROI_p1.y, ROI_p2.y + 1).clone();
	imshow("ROI", ROI_Image);


	/*--------------BGR to YCrCb-------------*/
	Mat ROI_YCrCb;
	cvtColor(ROI_Image, ROI_YCrCb, CV_BGR2YCrCb);
	imshow("YCrCb", ROI_YCrCb);


	/*-------------- Color Detection and Tracking-------------*/
	int rows = ROI_YCrCb.rows;
	int cols = ROI_YCrCb.cols;
	ROIcount = 0;
	ROIDepthCount = 0;
	
	
	if (ROIPixel != nullptr)
	{
		delete[] ROIPixel;
		ROIPixel = nullptr;
	}
	ROIPixel = new Point2i[1000];

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int IsRed = (int)ROI_YCrCb.at<Vec3b>(i, j)[1];

			//threshold = 150 for fluorescent pink
			//threshold = 170 for red
			if (IsRed > 170)
			{
				ROI_Image.at<Vec4b>(i, j)[0] = 255;


				ROI_Image.at<Vec4b>(i, j)[1] = 0;
				ROI_Image.at<Vec4b>(i, j)[2] = 0;

				ROI.at<Vec4b>(i + ROI_p1.y, j + ROI_p1.x)[0] = 255;
				ROI.at<Vec4b>(i + ROI_p1.y, j + ROI_p1.x)[1] = 0;
				ROI.at<Vec4b>(i + ROI_p1.y, j + ROI_p1.x)[2] = 0;

				ROIPixel[ROIcount].x = j + ROI_p1.x;
				ROIPixel[ROIcount].y = i + ROI_p1.y;

				ROICenterColorS_New.x += ROIPixel[ROIcount].x;
				ROICenterColorS_New.y += ROIPixel[ROIcount].y;

				ROIcount++;
			}
		}
	}

	imshow("ROI", ROI_Image);
	imshow("Color Map", ROI);
	

	if (ROIcount > 0)
	{
		ROICenterColorS_New.x = static_cast<int>(ROICenterColorS_New.x / ROIcount);
		ROICenterColorS_New.y = static_cast<int>(ROICenterColorS_New.y / ROIcount);

		CameraSpaceROI();
		MoveROI();

	}
	else if (ROIcount == 0)
	{
		ROICenterColorS_Old.x = ROICenterColorS_New.x = 0;
		ROICenterColorS_Old.y = ROICenterColorS_New.y = 0;
	}

	ROI_Image.release();
	ROI_YCrCb.release();
	ROI.release();

}

void MoveROI()
{
	if (ROICenterColorS_Old.x == 0 && ROICenterColorS_Old.y == 0)
	{
		ROICenterColorS_Old.x = ROICenterColorS_New.x;
		ROICenterColorS_Old.y = ROICenterColorS_New.y;
	}
	else if (ROICenterColorS_Old.x != 0 && ROICenterColorS_New.y != 0)
	{
		Vec2i Dir;

		Dir.val[0] = ROICenterColorS_New.x - ROICenterColorS_Old.x;
		Dir.val[1] = ROICenterColorS_New.y - ROICenterColorS_Old.y;

		ROI_p1.x = ROI_p1.x + Dir.val[0];
		ROI_p1.y = ROI_p1.y + Dir.val[1];

		ROI_p2.x = ROI_p2.x + Dir.val[0];
		ROI_p2.y = ROI_p2.y + Dir.val[1];

		ROICenterColorS_Old.x = ROICenterColorS_New.x;
		ROICenterColorS_Old.y = ROICenterColorS_New.y;

		ROICenterColorS_New.x = 0;
		ROICenterColorS_New.y = 0;
	}
}

void CameraSpaceROI()
{
	if (ROICameraSP != nullptr)
	{
		delete[] ROICameraSP;
		ROICameraSP = nullptr;
	}

	ROIDepthCount = 0;

	ROICenterCameraS.x = 0;
	ROICenterCameraS.y = 0;
	ROICenterCameraS.z = 0;

	for (int i = 0; i < ROIcount; i++)
	{
		int index1 = ROIPixel[i].x + ROIPixel[i].y * iWidthColor;
		if (pCSPoints[index1].Z != -1 * numeric_limits<float>::infinity())
		{
			ROIDepthCount++;

		}
	}

	ROICameraSP = new CameraSpacePoint[ROIDepthCount];

	int indx1 = 0;
	for (int i = 0; i < ROIcount; i++)
	{
		int indx2 = ROIPixel[i].x + ROIPixel[i].y * iWidthColor;

		if (pCSPoints[indx2].Z != -1 * numeric_limits<float>::infinity())
		//if (_fpclass(ROICameraSP[i].Z) == 256)
		//if (pCSPoints[index].Z <= 1 && pCSPoints[index].Z >= -1)
		{
			ROICameraSP[indx1].X = pCSPoints[indx2].X;
			ROICameraSP[indx1].Y = pCSPoints[indx2].Y;
			ROICameraSP[indx1].Z = pCSPoints[indx2].Z;

			//cout << ROICameraSP[i].X << " " << ROICameraSP[i].Y << " " << ROICameraSP[i].Z << endl;
			//cout << indx1 << " " << ROICameraSP[indx1].Z << endl;
			//cout << _fpclass(ROICameraSP[i].Z) << endl;

			ROICenterCameraS.x += ROICameraSP[indx1].X;
			ROICenterCameraS.y += ROICameraSP[indx1].Y;
			ROICenterCameraS.z += ROICameraSP[indx1].Z;

			indx1++;

		}
	}

	//if (IS_KEY_F1_UP)
	//{
	//	ROICameraSPStorage();
	//}

	if (ROIDepthCount > 0)
	{
		ROICenterCameraS.x = ROICenterCameraS.x / ROIDepthCount;
		ROICenterCameraS.y = ROICenterCameraS.y / ROIDepthCount;
		ROICenterCameraS.z = ROICenterCameraS.z / ROIDepthCount;

	}
	else if (ROIDepthCount == 0)
	{
		ROICenterCameraS.x = 0;
		ROICenterCameraS.y = 0;
		ROICenterCameraS.z = 0;
	}

}

void ROICameraSPStorage()
{
	//How mant points do you want to record
	const int StorageSize = 5000;

	if (ROICameraSP_Storage == nullptr)
	{
		ROICameraSP_Storage = new CameraSpacePoint[StorageSize];
	}

	if (IS_KEY_F1_UP && ROIDepthCount > 0)
	{
		if (ROIStorageCount + ROIDepthCount < StorageSize)
		{
			cout << endl << ROIDepthCount << endl;

			for (int i = 0; i < ROIDepthCount; i++)
			{
				ROICameraSP_Storage[i + ROIStorageCount] = ROICameraSP[i];

				ROICameraSP_Storage[i + ROIStorageCount].Z *= -1;
				
				//cout << ROICameraSP[i].Z << endl;
				//cout << ROICameraSP_Storage[i + ROIStorageCount].Z << endl;
				
				//cout << ROICameraSP_Storage[i + ROIStorageCount].X << " " << ROICameraSP_Storage[i + ROIStorageCount].Y << " " << ROICameraSP_Storage[i + ROIStorageCount].Z << endl;
			}

			ROIStorageCount += ROIDepthCount;

			//cout << endl << "ROIStorageCount: " << ROIStorageCount << endl;

		}
		else
		{
			cout << endl << "The array ROICameraSP_Storage is full, press F2 to clear" << endl;
		}
	}

}



/*--------------------------------------------------*/
/*--------------OpenHaptics HL Function-------------*/
/*--------------------------------------------------*/
void initHL()
{
	HDErrorInfo error;

	hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		fprintf(stderr, "Press any key to exit");
		getchar();
		exit(-1);
	}

	hHLRC = hlCreateContext(hHD);
	hlMakeCurrent(hHLRC);

	hlDisable(HL_USE_GL_MODELVIEW);

	spring = hlGenEffects(1);
}



/*--------------------------------------------------------*/
/*----------------Path Generation Function----------------*/
/*--------------------------------------------------------*/
void PathGenProjToPlane(CameraSpacePoint* Data, int DataNum, double* PlaneCoeff, CameraSpacePoint* Result)
{
	//int Order = sizeof(PlaneCoeff) / sizeof(double);

	for (int i = 0; i < DataNum; i++)
	{
		double tScal = -(PlaneCoeff[0] * Data[i].X + PlaneCoeff[1] * Data[i].Y + PlaneCoeff[2] * Data[i].Z + PlaneCoeff[3]) / (pow(PlaneCoeff[0], 2) + pow(PlaneCoeff[1], 2) + pow(PlaneCoeff[2], 2));

		Result[i].X = Data[i].X + tScal * PlaneCoeff[0];
		Result[i].Y = Data[i].Y + tScal * PlaneCoeff[1];
		Result[i].Z = Data[i].Z + tScal * PlaneCoeff[2];

		//cout << endl << Result[i].X << " " << Result[i].Y << " " << Result[i].Z << endl;
	}

}

void PathGenCoordWorldToCubic(CameraSpacePoint* Data, int DataNum, CameraSpacePoint* Result)
{
	//GLdouble m[16] = { 0 };
	//m[0] = m[5] = m[10] = m[15] = 1;

	for (int i = 0; i < DataNum; i++)
	{
		//cout << endl << Data[i].X << " " << Data[i].Y << " " << Data[i].Z << endl;
		GLfloat m[16] = { 0 };
		m[0] = m[5] = m[10] = m[15] = 1;

		m[12] = Data[i].X;
		m[13] = Data[i].Y;
		m[14] = Data[i].Z;

		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//glRotated(-45, 0, 1, 0);
		glTranslatef(-CubicPosi.x, -CubicPosi.y, -CubicPosi.z - CubicUnitT);

		glMultMatrixf(m);

		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		glPopMatrix();

		Result[i].X = m[12];
		Result[i].Y = m[13];
		Result[i].Z = m[14];

		//cout << endl << Result[i].X << " " << Result[i].Y << " " << Result[i].Z << endl;

	}

	//cout << endl << m[0] << " " << m[4] << " " << m[8] << " " << m[12] << endl
	//	           << m[1] << " " << m[5] << " " << m[9] << " " << m[13] << endl
	//			   << m[2] << " " << m[6] << " " << m[10] << " " << m[14] << endl
	//			   << m[3] << " " << m[7] << " " << m[11] << " " << m[15] << endl;

}

void PathGenCoordCubicToWorld(CameraSpacePoint* Data, int DataNum, CameraSpacePoint* Result)
{
	GLfloat m[16] = { 0 };
	m[0] = m[5] = m[10] = m[15] = 1;

	for (int i = 0; i < DataNum; i++)
	{
		m[12] = Data[i].X;
		m[13] = Data[i].Y;
		m[14] = Data[i].Z;


		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		
		glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z + CubicUnitT);
		//glRotated(45, 0, 1, 0);

		glMultMatrixf(m);

		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		glPopMatrix();

		Result[i].X = m[12];
		Result[i].Y = m[13];
		Result[i].Z = m[14];

		cout << endl << Result[i].X << " " << Result[i].Y << " " <<Result[i].Z << endl;

	}
}

void PathGenCalcMean(CameraSpacePoint* Data, int DataNum, double* Result)
{
	//Result = new double[3];
	Result[0] = Result[1] = Result[2] = 0;
	for (int i = 0; i < DataNum; i++)
	{
		Result[0] += Data[i].X;
		Result[1] += Data[i].Y;
		Result[2] += Data[i].Z;
	}

	Result[0] /= DataNum;
	Result[1] /= DataNum;
	Result[2] /= DataNum;

	cout << endl << Result[0] << " " << Result[1] << endl;
}


void PathGenRegression(CameraSpacePoint* Data, int DataNum, double* Result)
{
	double CalcMean[3] = { 0, 0, 0 };
	PathGenCalcMean(Data, DataNum, CalcMean);

	double paraNum = 0, paraDen = 0;

	for (int i = 0; i < DataNum; i++)
	{
		paraNum += (Data[i].X - CalcMean[0])*(Data[i].Y - CalcMean[1]);
		paraDen += pow(Data[i].X - CalcMean[0], 2);
	}
	
	cout << endl << paraNum << " " << paraDen << endl;

	Result[1] = paraNum / paraDen;
	Result[2] = CalcMean[1] - Result[1] * CalcMean[0];
}

/*------------------------------------*/
/*--------------main loop-------------*/
/*------------------------------------*/
int main(int argc, char** argv)
{
	KineticInit();
	GLInit();
	initHL();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(iWidthColor, iHeightColor);
	glutCreateWindow("Perspective Projection");
	glutFullScreen();
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(KineticUpdate);
	
	glutTimerFunc(1000, timer, 0);
	glutTimerFunc(125, timer, 1);

	BuildPopupMenu();

	glutMainLoop();


	return 0;
} 