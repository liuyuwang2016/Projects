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




/*----------------OpenGL----------------*/
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
int FPS = 0;
BYTE* pBufferColor_RGB = nullptr;
GLfloat fAspect;

void GLInit(void);
void SpecialKeys(int key, int x, int y);
void timer(int value);
void RenderScene(void);
void SceneWithBackground(void);             //RenderScene Background
void SceneWithoutBackground(void);          //RenderScene Background
void DrawPointCloud(void);                  //Drawing Type
void DrawMeshes(void);                      //Drawing Type



/*----------------Kinetic----------------*/
//Initial (Sensor)
IKinectSensor* pSensor = nullptr;
ICoordinateMapper* Mapper = nullptr;
IDepthFrameReader* pFrameReaderDepth = nullptr;
IColorFrameReader* pFrameReaderColor = nullptr;
void KineticInit(void);
void KineticUpdate(void);

//Depth Map
int iWidthDepth = 0;
int iHeightDepth = 0;
UINT depthPointCount = 0;
UINT16* pBufferDepth = nullptr;
UINT16 uDepthMin = 0, uDepthMax = 0;
Mat mDepthImg;
Mat mImg8bit;

//Color Map
int iWidthColor = 0;
int iHeightColor = 0;
UINT colorPointCount = 0;
UINT uBufferSizeColor = 0;
BYTE* pBufferColor = nullptr;
Mat mColorImg;

//Mapping
CameraSpacePoint* pCSPoints = nullptr;
void ShowImage(void);



/*-------------Region Of Interest(ROI)-------------*/
//Initial
Rect2i ROI_rect;
Point ROI_p1, ROI_p2;												//start point and end point of ROI
bool ROI_S1 = false;												//flag of mouse event
bool ROI_S2 = false;
Mat ROI;
void onMouseROI(int event, int x, int y, int flags, void* param);
void FindROI(void);

//Tracking
int ROIcount = 0;
Point2i ROICenterColorS_Old, ROICenterColorS_New;				   //center of tracking object of ROI in Color
//ColorSpacePoint* ROIColoSP = nullptr;							   //Tracking Object in Color Space
Point2i* ROIPixel = nullptr;
CameraSpacePoint* ROICameraSP = nullptr;
void MoveROI(void);

//Mapping
int ROIDepthCount = 0;
Point3f ROICenterCameraS;
void CameraSpaceROI(void);


/*--------------Draw Cubic-------------*/
float CubicUnit = 0.05f;
Point3f CubicPosi;
Point3f CubicPosiinHLCoord;


bool ROIIsCollide = FALSE;
void DrawCubic(void);




/*--------------OpenHaptics-------------*/
static HHD hHD = HD_INVALID_HANDLE;
static HHLRC hHLRC = 0;
void initHL(void);
HLuint spring;

////Shape ids for haptically rendered shapes
//HLuint gCubicShapeId = 0;
//
////Shape snap distances
//static double gCubicShapeSnapDistance = 0.0;
//
//Point3f HLOrigin;
//Point3f HDOrigin;
//
//HLdouble HLTipPosi[3] = { 0 };
//HDdouble HDTipPosi[3] = { 0 };
//
//#define CURSOR_SIZE_PIXELS 20
//static double gCursorScale = 0;
//static GLuint gCursorDisplayList = 0;
//
//void initHL(void);
//void drawSceneHaptics(void);
//void drawCursor(void);


/*--------------ARToolKit-------------*/
//#ifdef _WIN32
//char			*vconf = "Data\\WDM_camera_flipV.xml";
//#else
//char			*vconf = "";
//#endif
//int             xsize, ysize;
//char           *cparam_name = "Data/camera_para.dat";
//ARParam         cparam;
//
//int             thresh = 100;
//int             countAR = 0;
//
//char           *patt_name = "Data/patt.hiro";
//int             patt_id;
//double          patt_width = 80.0;
//double          patt_center[2] = { 0.0, 0.0 };
//double          patt_trans[3][4];
//
//int             marker_num = 0;
//
//static void   init(void);
//static void   cleanup(void);
//static void   draw(void);
//
//ARUint8* FrameFormateCVtoARTK(BYTE* pBufferColor);


/*--------------Change viewing volume and viewport.  Called when window is resized-------------*/
//void ChangeSize(int w, int h)
//{
//	GLfloat fAspect;
//
//	if (h == 0)
//		h = 1;
//
//	glViewport(0, 0, w, h);
//
//	fAspect = (GLfloat)w / (GLfloat)h;
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//
//	gluPerspective(53.0f, fAspect, 0.1f, 2000.0f);
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//
//
//}



void GLInit()
{
	// Light values and coordinates
	GLfloat  whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	GLfloat  sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat	 lightPos[] = { -50.f, 25.0f, 250.0f, 0.0f };

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Black blue background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


// Respond to arrow keys
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		yRot += 1.0f;

	if (key == GLUT_KEY_DOWN)
		yRot -= 1.0f;

	if (key == GLUT_KEY_LEFT)
		xRot += 1.0f;

	if (key == GLUT_KEY_RIGHT)
		xRot -= 1.0f;

	xRot = (GLfloat)((const int)xRot % 360);
	yRot = (GLfloat)((const int)yRot % 360);

	// Refresh the Window
	glutPostRedisplay();
}


void timer(int value)
{
	glutTimerFunc(1000, timer, 0);
	printf("FPS = %d\n", FPS);
	FPS = 0;
}


/*--------------RenderScene Background-------------*/
void SceneWithBackground()
{
	//double zNear = 0.5, zFar = 8.0,
	//	fx = 1060.707250708333, fy = 1058.608326305465,
	//	cx = 956.354471815484, cy = 518.9784429882449;

	//double Top = cy * zNear / fy;
	//double Bottom = (cy - iHeightColor) * zNear / fy;
	//double Left = -cx * zNear / fx;
	//double Right = (iWidthColor - cx) * zNear / fy;

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

	glDrawPixels(iWidthColor, iHeightColor, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor_RGB);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(54.5f, (double)iWidthColor / (double)iHeightColor, 0.01f, 20000.0f);
	//glFrustum(Left, Right, Bottom, Top, zNear, zFar);
	//glFrustum(-0.47056, 0.45884, -0.265019, 0.257899, 0.5, 8);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	glTranslatef(0.061f, 0.0f, 0.0f);
	//glTranslatef(0.0f, 0.0f, -0.05f);
}


void SceneWithoutBackground()
{
	//double zNear = 0.5, zFar = 8.0,
	//	fx = 1060.707250708333, fy = 1058.608326305465,
	//	cx = 956.354471815484, cy = 518.9784429882449;

	//double Top = cy * zNear / fy;
	//double Bottom = (cy - iHeightColor) * zNear / fy;
	//double Left = -cx * zNear / fx;
	//double Right = (iWidthColor - cx) * zNear / fy;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(54.5f, (double)iWidthColor / (double)iHeightColor, 0.001f, 20000.0f);
	//glFrustum(Left, Right, Bottom, Top, zNear, zFar);

	/*--------------OpenHaptics HL-------------*/
	//--------------------------------------------------------------------------------
	//GLdouble modelview[16];
	//GLdouble projection[16];
	//GLint viewport[4];
	//
	//glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	//glGetDoublev(GL_PROJECTION_MATRIX, projection);
	//glGetIntegerv(GL_VIEWPORT, viewport);
	//
	//hlMatrixMode(HL_TOUCHWORKSPACE);
	//hlLoadIdentity();
	//
	//// Fit haptic workspace to view volume.
	//hluFitWorkspace(projection);
	//
	//// Compute cursor scale.
	//gCursorScale = hluScreenToModelScale(modelview, projection, (HLint*)viewport);
	//gCursorScale *= CURSOR_SIZE_PIXELS;
	//--------------------------------------------------------------------------------

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);



}


/*--------------Draw Cubic-------------*/
void DrawCubic()
{

	/*--------------OpenHaptics HL-------------*/
	//CubicPosiinHLCoord.x = CubicPosi.x - HLOrigin.x;
	//CubicPosiinHLCoord.y = CubicPosi.y - HLOrigin.y;
	//CubicPosiinHLCoord.z = CubicPosi.z - HLOrigin.z;
	//
	//CubicPosiinHLCoord.x = CubicPosi.x - HDOrigin.x;
	//CubicPosiinHLCoord.y = CubicPosi.y - HDOrigin.y;
	//CubicPosiinHLCoord.z = CubicPosi.z - HDOrigin.z;

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

		//glTranslatef(CubicPosiinHLCoord.x, CubicPosiinHLCoord.y, CubicPosiinHLCoord.z);
		glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);


		glBegin(GL_QUADS);        // Draw The Cube Using quads
		//glColor3f(0.0f, 1.0f, 0.0f);    // Color Blue
		glVertex3f(CubicUnit, CubicUnit, -CubicUnit);    // Top Right Of The Quad (Top)
		glVertex3f(-CubicUnit, CubicUnit, -CubicUnit);    // Top Left Of The Quad (Top)
		glVertex3f(-CubicUnit, CubicUnit, CubicUnit);    // Bottom Left Of The Quad (Top)
		glVertex3f(CubicUnit, CubicUnit, CubicUnit);    // Bottom Right Of The Quad (Top)

		//glColor3f(0.5f, 0.5f, 0.0f);    // Color Orange
		glVertex3f(CubicUnit, -CubicUnit, CubicUnit);    // Top Right Of The Quad (Bottom)
		glVertex3f(-CubicUnit, -CubicUnit, CubicUnit);    // Top Left Of The Quad (Bottom)
		glVertex3f(-CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Left Of The Quad (Bottom)
		glVertex3f(CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Right Of The Quad (Bottom)

		//glColor3f(1.0f, 0.0f, 0.0f);    // Color Red    
		glVertex3f(CubicUnit, CubicUnit, CubicUnit);    // Top Right Of The Quad (Front)
		glVertex3f(-CubicUnit, CubicUnit, CubicUnit);    // Top Left Of The Quad (Front)
		glVertex3f(-CubicUnit, -CubicUnit, CubicUnit);    // Bottom Left Of The Quad (Front)
		glVertex3f(CubicUnit, -CubicUnit, CubicUnit);    // Bottom Right Of The Quad (Front)

		//glColor3f(1.0f, 1.0f, 0.0f);    // Color Yellow
		glVertex3f(CubicUnit, -CubicUnit, -CubicUnit);    // Top Right Of The Quad (Back)
		glVertex3f(-CubicUnit, -CubicUnit, -CubicUnit);    // Top Left Of The Quad (Back)
		glVertex3f(-CubicUnit, CubicUnit, -CubicUnit);    // Bottom Left Of The Quad (Back)
		glVertex3f(CubicUnit, CubicUnit, -CubicUnit);    // Bottom Right Of The Quad (Back)

		//glColor3f(0.0f, 0.0f, 1.0f);    // Color Blue
		glVertex3f(-CubicUnit, CubicUnit, CubicUnit);    // Top Right Of The Quad (Left)
		glVertex3f(-CubicUnit, CubicUnit, -CubicUnit);    // Top Left Of The Quad (Left)
		glVertex3f(-CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Left Of The Quad (Left)
		glVertex3f(-CubicUnit, -CubicUnit, CubicUnit);    // Bottom Right Of The Quad (Left)

		//glColor3f(1.0f, 0.0f, 1.0f);    // Color Violet
		glVertex3f(CubicUnit, CubicUnit, -CubicUnit);    // Top Right Of The Quad (Right)
		glVertex3f(CubicUnit, CubicUnit, CubicUnit);    // Top Left Of The Quad (Right)
		glVertex3f(CubicUnit, -CubicUnit, CubicUnit);    // Bottom Left Of The Quad (Right)
		glVertex3f(CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Right Of The Quad (Right)

		glEnd();


		glLineWidth(2.5);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINE_STRIP);

		glVertex3f(CubicUnit, CubicUnit, -CubicUnit);    // Top Right Of The Quad (Top)
		glVertex3f(-CubicUnit, CubicUnit, -CubicUnit);    // Top Left Of The Quad (Top)
		glVertex3f(-CubicUnit, CubicUnit, CubicUnit);    // Bottom Left Of The Quad (Top)
		glVertex3f(CubicUnit, CubicUnit, CubicUnit);    // Bottom Right Of The Quad (Top)

		glVertex3f(-CubicUnit, -CubicUnit, CubicUnit);    // Top Left Of The Quad (Bottom)
		glVertex3f(-CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Left Of The Quad (Bottom)
		glVertex3f(CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Right Of The Quad (Bottom)

		glVertex3f(CubicUnit, CubicUnit, CubicUnit);    // Top Right Of The Quad (Front)
		glVertex3f(-CubicUnit, CubicUnit, CubicUnit);    // Top Left Of The Quad (Front)
		glVertex3f(-CubicUnit, -CubicUnit, CubicUnit);    // Bottom Left Of The Quad (Front)
		glVertex3f(CubicUnit, -CubicUnit, CubicUnit);    // Bottom Right Of The Quad (Front)

		glVertex3f(CubicUnit, -CubicUnit, -CubicUnit);    // Top Right Of The Quad (Back)
		glVertex3f(-CubicUnit, -CubicUnit, -CubicUnit);    // Top Left Of The Quad (Back)
		glVertex3f(-CubicUnit, CubicUnit, -CubicUnit);    // Bottom Left Of The Quad (Back)
		glVertex3f(CubicUnit, CubicUnit, -CubicUnit);    // Bottom Right Of The Quad (Back)

		glVertex3f(-CubicUnit, CubicUnit, CubicUnit);    // Top Right Of The Quad (Left)
		glVertex3f(-CubicUnit, CubicUnit, -CubicUnit);    // Top Left Of The Quad (Left)
		glVertex3f(-CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Left Of The Quad (Left)
		glVertex3f(-CubicUnit, -CubicUnit, CubicUnit);    // Bottom Right Of The Quad (Left)

		glVertex3f(CubicUnit, CubicUnit, -CubicUnit);    // Top Right Of The Quad (Right)
		glVertex3f(CubicUnit, CubicUnit, CubicUnit);    // Top Left Of The Quad (Right)
		glVertex3f(CubicUnit, -CubicUnit, CubicUnit);    // Bottom Left Of The Quad (Right)
		glVertex3f(CubicUnit, -CubicUnit, -CubicUnit);    // Bottom Right Of The Quad (Right)

		glEnd();
		glPopMatrix();
		glEndList();
	}

}


/*--------------Point Cloud-------------*/
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


/*--------------Meshes-------------*/
void DrawMeshes()
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


// Called to draw scene
void RenderScene()
{
	//cout << endl << "------RenderScene-----" << endl;

	/*--------------RenderScene Background-------------*/
	//SceneWithBackground();
	SceneWithoutBackground();


	//GLdouble CamX = 1.5*sin(xRot*3.1415926 / 180);
	//GLdouble CamY = 1.5*sin(yRot*3.1415926 / 180);
	//GLdouble CamZ = 1.5*cos(xRot*3.1415926 / 180);
	//gluLookAt(CamX, CamY, CamZ, 0, 0, 0, 0, 1, 0);



	/*--------------Drawing Type-------------*/
	DrawPointCloud();
	//DrawMeshes();



	/*--------------Test Mapper Function-------------*/
	glPushMatrix();

	glPointSize(5.0f);
	glBegin(GL_POINTS);
		
		glColor3f(1.0f, 0.0f, 0.0f);
		for (int i = 0; i < 200; i++)
		{
			glVertex3f(pCSPoints[1280 * 300 + 500 + i].X + 0.05f, pCSPoints[1280 * 300 + 500 + i].Y + 0.05f, -pCSPoints[1280 * 300 + 500 + i].Z - 0.05f);
		}

	glEnd();
	glPopMatrix();

	
	
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
				if (abs(pX - CubicPosi.x) < CubicUnit)
				{
					if (abs(pY - CubicPosi.y) < CubicUnit)
					{
						if (abs(pZ - CubicPosi.z) < CubicUnit)
						{
							ROIIsCollide = TRUE;
						}
					}
				}

			}

		glEnd();
		glPopMatrix();
	}

	/*--------------Draw Cubic-------------*/
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);
	if (ROIIsCollide)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
	}
	else
	{
		glColor3f(0.0f, 0.0f, 1.0f);
	}
	
	DrawCubic();

	//ROIIsCollide = FALSE;
	glPopMatrix();

	/*--------------OpenHaptics Force Generate-------------*/
	//--------------------------------------------------------------------------------

	hlBeginFrame();
	if (ROIIsCollide == TRUE)
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

	ROIIsCollide = FALSE;
	//--------------------------------------------------------------------------------



	/*--------------OpenHaptics HL-------------*/
	////glTranslatef(HDOrigin.x, HDOrigin.y, HDOrigin.z);
	////glTranslatef(HDTipPosi[0], HDTipPosi[1], -HDTipPosi[2]);
	//
	//glPushMatrix();
	////glTranslatef(HLOrigin.x, HLOrigin.y, HLOrigin.z);
	//glTranslatef(HDOrigin.x, HDOrigin.y, HDOrigin.z);
	//drawSceneHaptics();
	//
	//glPopMatrix();
	//
	////drawCursor();
	//
	//
	//glPushMatrix();
	////glTranslatef(HLOrigin.x, HLOrigin.y, HLOrigin.z);
	//glTranslatef(HDOrigin.x, HDOrigin.y, HDOrigin.z);
	//if (ROIIsCollide)
	//{
	//	glColor3f(1.0f, 0.0f, 0.0f);
	//}
	//else
	//{
	//	glColor3f(0.0f, 0.0f, 1.0f);
	//}
	//glTranslatef(CubicPosiinHLCoord.x, CubicPosiinHLCoord.y, CubicPosiinHLCoord.z);
	//DrawCubic();
	//ROIIsCollide = FALSE;
	//glPopMatrix();
	//
	////drawCursor();
	//
	//glPushMatrix();
	//glTranslated(ROICenterCameraS.x, ROICenterCameraS.y, -ROICenterCameraS.z);
	//glColor3f(0.0f, 1.0f, 0.0f);
	//glutSolidSphere(0.01001, 50, 50);
	//glPopMatrix();
	//
	////cout << ROICenterCameraS.x << " " << ROICenterCameraS.y << " " << ROICenterCameraS.z << endl;
	

	

	/*--------------ARToolKit-------------*/
	//if (marker_num != 0)
	//{
	//	glPushMatrix();
	//
	//	double    gl_para[16];
	//	GLfloat   mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
	//	GLfloat   mat_flash[] = { 0.0, 0.0, 1.0, 1.0 };
	//	GLfloat   mat_flash_shiny[] = { 50.0 };
	//	GLfloat   light_position[] = { 100.0, -200.0, 200.0, 0.0 };
	//	GLfloat   ambi[] = { 0.1, 0.1, 0.1, 0.1 };
	//	GLfloat   lightZeroColor[] = { 0.9, 0.9, 0.9, 0.1 };
	//
	//	argDrawMode3D();
	//	argDraw3dCamera(0, 0);
	//	glClearDepth(1.0);
	//	glClear(GL_DEPTH_BUFFER_BIT);
	//	glEnable(GL_DEPTH_TEST);
	//	glDepthFunc(GL_LEQUAL);
	//
	//	/* load the camera transformation matrix */
	//	argConvGlpara(patt_trans, gl_para);
	//	glMatrixMode(GL_MODELVIEW);
	//	glLoadMatrixd(gl_para);
	//
	//	glEnable(GL_LIGHTING);
	//	glEnable(GL_LIGHT0);
	//	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
	//	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
	//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
	//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
	//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	//	glMatrixMode(GL_MODELVIEW);
	//	glTranslatef(0.0, 0.0, 25.0);
	//	glutSolidCube(50.0);
	//	glDisable(GL_LIGHTING);
	//
	//	glDisable(GL_DEPTH_TEST);
	//
	//	glPopMatrix();
	//
	//}

	/*--------------Draw Solid Tea Pot-------------*/
	//glPushMatrix();
	//glTranslatef(0.0f, 0.0f, -300.0f);
	//glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	//glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glutSolidTeapot(50.0f);
	//glPopMatrix();

	/*--------------Draw Solid Tea Sphere-------------*/
	//glPushMatrix();
	//glTranslatef(0.0f, 0.0f, -300.0f);
	//glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	//glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glutSolidSphere(50, 100, 100);
	//glPopMatrix();


	glutSwapBuffers();
}


void KineticInit()
{
	//cout << endl << "------KineticInit-----" << endl;

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
	 CubicPosi.y = -0.0647f;
	 CubicPosi.z = -0.5427f - 0.20f;

	 //CubicPosi.x = -0.0672f;
	 //CubicPosi.y = -0.1556f;
	 //CubicPosi.z = -0.6741;

	 //ROICenterCameraS.x = 0;
	 //ROICenterCameraS.y = 0;
	 //ROICenterCameraS.z = 0;
}


void KineticUpdate()
{
	//cout << endl << "------KineticUpdate-----" << endl;

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

	/*--------------Test Mapper Function (Point Cloud)-------------*/
	Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);
	


	/*--------------OpenGL Background Converting-------------*/
	//cout << "Update pBufferColor" << endl;
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

	/*--------------OpenHaptics HL-------------*/
	//hlGetDoublev(HL_PROXY_POSITION, HLTipPosi);
	////cout << HLTipPosi[0] << " " << HLTipPosi[1] << " " << HLTipPosi[2] << endl;
	//HLTipPosi[0] = HLTipPosi[0] / 1000;
	//HLTipPosi[1] = HLTipPosi[1] / 1000;
	//HLTipPosi[2] = HLTipPosi[2] / 1000;
	//
	//hdGetDoublev(HD_CURRENT_POSITION, HDTipPosi);
	//cout << HDTipPosi[0] << " " << HDTipPosi[1] << " " << HDTipPosi[2] << endl;
	//
	//HDTipPosi[0] = HDTipPosi[0] / 1000;
	//HDTipPosi[1] = HDTipPosi[1] / 1000;
	//HDTipPosi[2] = HDTipPosi[2] / 1000;
	

	//vector<CameraSpacePoint> depth2xyz(depthPointCount);
	//Mapper->MapDepthFrameToCameraSpace(depthPointCount, pBufferDepth, depthPointCount, &depth2xyz[0]);
	//CameraSpacePoint XX = depth2xyz[108544];//in meters

	/*--------------ARTool Kit-------------*/
	////ARTool Kit - Detect Marker
	//if (pBufferColor != nullptr)
	//{
	//	ARUint8         *dataPtr = nullptr;
	//	ARMarkerInfo    *marker_info = nullptr;
	//
	//
	//	//int             j, k;
	//
	//	///* grab a vide frame */
	//	//if ((dataPtr = (ARUint8 *)arVideoGetImage()) == NULL) {
	//	//	arUtilSleep(2);
	//	//	return;
	//	//}
	//	//if (countAR == 0) arUtilTimerReset();
	//	//countAR++;
	//
	//	//dataPtr = reinterpret_cast<ARUint8*>(pBufferColor);
	//	//if (countAR == 0) arUtilTimerReset();
	//	//countAR++;
	//
	//	//dataPtr;
	//
	//	//int a = arDetectMarker(dataPtr, thresh, &marker_info, &marker_num);
	//	//cout << a << endl;
	//	//cout << marker_num << endl;
	//
	//	//argDrawMode2D();
	//	//argDispImage(dataPtr, 0, 0);
	//
	//	///* detect the markers in the video frame */
	//	//if (arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0) {
	//	//	cleanup();
	//	//	exit(0);
	//	//}
	//	////cout << marker_num << endl;
	//	//arVideoCapNext();
	//
	//	///* check for object visibility */
	//	//k = -1;
	//	//for (j = 0; j < marker_num; j++) {
	//	//	if (patt_id == marker_info[j].id) {
	//	//		if (k == -1) k = j;
	//	//		else if (marker_info[k].cf < marker_info[j].cf) k = j;
	//	//	}
	//	//}
	//	//if (k == -1) {
	//	//	argSwapBuffers();
	//	//	return;
	//	//}
	//
	//	///* get the transformation between the marker and the real camera */
	//	//arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
	//
	//	////draw();
	//
	//	////argSwapBuffers();
	//
	//}

	//delete[] pCSPoints;
	//delete[] pBufferColor;
	//delete[] pBufferDepth;
	//delete[] ROIColoSP;
	//
	//pCSPoints = nullptr;
	//pBufferColor = nullptr;
	//pBufferDepth = nullptr;
	//ROIColoSP = nullptr;


}


void ShowImage()
{
	namedWindow("Depth Map");
	namedWindow("Color Map");

	/*--------------Depth Image-------------*/
	// Convert from 16bit to 8bit
	mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);

	//CameraSpacePoint* CSP = nullptr;
	//CSP = new CameraSpacePoint[1];
	//
	//CSP[0].X = pCSPoints[1280 * 500 + 640].X + 0.05f;
	//CSP[0].Y = pCSPoints[1280 * 500 + 640].Y + 0.05f;
	//CSP[0].Z = pCSPoints[1280 * 500 + 640].Z + 0.05f;
	//
	//ColorSpacePoint* ColoSP = nullptr;
	//ColoSP = new ColorSpacePoint[1];
	//
	//Mapper->MapCameraPointsToColorSpace((UINT)1, CSP, (UINT)1, ColoSP);
	//
	//cout << (int)ColoSP[0].X << " " << (int)ColoSP[0].Y << endl;

	/*--------------Test Mapper Function-------------*/
	CameraSpacePoint* CSP = nullptr;
	CSP = new CameraSpacePoint[200];

	for (int i = 0; i < 200; i++)
	{
		CSP[i].X = pCSPoints[1280 * 300 + 500 + i].X + 0.05f;
		CSP[i].Y = pCSPoints[1280 * 300 + 500 + i].Y + 0.05f;
		CSP[i].Z = pCSPoints[1280 * 300 + 500 + i].Z + 0.05f;

	}

	ColorSpacePoint* ColoSP = nullptr;
	ColoSP = new ColorSpacePoint[200];

	Mapper->MapCameraPointsToColorSpace((UINT)200, CSP, (UINT)200, ColoSP);


	for (int i = 0; i < 200; i++)
	{
		//cout << ColoSP[i].X << " " << ColoSP[i].Y  << endl;
		if (ColoSP[i].X != -1 * numeric_limits<float>::infinity())
		{
			//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[0] = 0;
			//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[1] = 0;
			//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[2] = 1;
			//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[3] = 0;

			circle(mColorImg, Point((int)ColoSP[i].X, (int)ColoSP[i].Y), 2, Scalar(0, 0, 255), -1);
		}

	}


	/*--------------Set Mouse Callback Function and Find ROI-------------*/
	cvSetMouseCallback("Color Map", onMouseROI, NULL);
	cv::imshow("Depth Map", mImg8bit);


	if (ROI_S1 == false && ROI_S2 == false)
	{
		// Initial State
		ROI = mColorImg.clone();
		cv::imshow("Color Map", ROI);
		//cvSetMouseCallback("Color Map", onMouseROI, NULL);
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
			//cvSetMouseCallback("Color Map", onMouseROI, NULL);

			FindROI();
		}
		//CV_EVENT_MOUSEMOVE：只選了左上角
		else
		{
			cv::imshow("Color Map", ROI);
			//cvSetMouseCallback("Color Map", onMouseROI, NULL);
		}

	}

	//cv::imshow("Color Map", mColorImg);


	//Mat a = Mat::zeros(1, 1, CV_8UC4);
	//Mat b;
	//a.at<cv::Vec4b>(0, 0)[2] = 255;
	//cvtColor(a, b, CV_BGR2HSV);
	//cout << (int)b.at<cv::Vec4b>(0, 0)[0] << " " << (int)b.at<cv::Vec4b>(0, 0)[1] << " " << (int)b.at<cv::Vec4b>(0, 0)[2] << endl;

	delete[] CSP;
	delete[] ColoSP;
	
	CSP = nullptr;
	ColoSP = nullptr;
}


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

		//cout << "CV_EVENT_LBUTTONDOWN: " << ROI_rect.x << ", " << ROI_rect.y << endl;
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

		//cout << "CV_EVENT_LBUTTONUP: " << x << ", " << y << endl;
		//cout << "ROI: " << ROI_rect.width << ", " << ROI_rect.height << endl;
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
	cvtColor(ROI_Image, ROI_YCrCb, /*CV_BGR2HSV*/CV_BGR2YCrCb);
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
			//cout << (float)ROI_YCrCb.at<Vec3b>(i, j)[0] << " " << (float)ROI_YCrCb.at<Vec3b>(i, j)[1] << " " << (float)ROI_YCrCb.at<Vec3b>(i, j)[2] << " " << endl;
			//cout << ROI_YCrCb.row(i).col(j) << endl << endl;

			int IsRed = (int)ROI_YCrCb.at<Vec3b>(i, j)[1];

			//threshold = 150 for fluorescent pink
			//threshold = 170 for red
			if (IsRed > 150)
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
	//cout << "ROI count" << ROIcount << endl;
	
	imshow("ROI", ROI_Image);
	imshow("Color Map", ROI);
	


	
	if (ROIcount > 0)
	{
		ROICenterColorS_New.x = static_cast<int>(ROICenterColorS_New.x / ROIcount);
		ROICenterColorS_New.y = static_cast<int>(ROICenterColorS_New.y / ROIcount);

		//cout << "Old: ( " << ROICenterColorS_Old.x << ", " << ROICenterColorS_Old.y << " )" << endl;
		//cout << "New: ( " << ROICenterColorS_New.x << ", " << ROICenterColorS_New.y << " )" << endl;

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

	//cout << "ROI dimenssion" << ROI_Image.cols << " " << ROI_Image.rows << endl;

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

	ROICameraSP = new CameraSpacePoint[ROIcount];

	ROICenterCameraS.x = 0;
	ROICenterCameraS.y = 0;
	ROICenterCameraS.z = 0;

	for (int i = 0; i < ROIcount; i++)
	{
		int index = ROIPixel[i].x + ROIPixel[i].y * iWidthColor;

		if (ROICameraSP[i].Z != -1 * numeric_limits<float>::infinity())
		{
			ROICameraSP[i].X = pCSPoints[index].X;
			ROICameraSP[i].Y = pCSPoints[index].Y;
			ROICameraSP[i].Z = pCSPoints[index].Z;

		//cout << i << ": ( " << ROICameraSP[i].X << ", " << ROICameraSP[i].Y << ", " << ROICameraSP[i].Z << " ) " << endl;


		//if (ROICameraSP[i].Z != -1 * numeric_limits<float>::infinity())
		//{
			//if (FirstNonZero == nullptr)
			//{
			//	FirstNonZero = new int[1];
			//	FirstNonZero[0] = i;
			//}
			//else
			//{
			//	float deltX = abs(ROICameraSP[i].X - ROICameraSP[FirstNonZero[0]].X);
			//	float deltY = abs(ROICameraSP[i].Y - ROICameraSP[FirstNonZero[0]].Y);
			//	float deltZ = abs(ROICameraSP[i].Z - ROICameraSP[FirstNonZero[0]].Z);

			//	if (deltZ < 0.005 && deltX < 0.002 && deltY < 0.002)
			//	{
			//		ROICenterCameraS.x += ROICameraSP[i].X;
			//		ROICenterCameraS.y += ROICameraSP[i].Y;
			//		ROICenterCameraS.z += ROICameraSP[i].Z;

			//		ROIDepthCount++;
			//	}
			//}

			ROICenterCameraS.x += ROICameraSP[i].X;
			ROICenterCameraS.y += ROICameraSP[i].Y;
			ROICenterCameraS.z += ROICameraSP[i].Z;

			ROIDepthCount++;


		}
	}

	if (ROIDepthCount > 0)
	{
		ROICenterCameraS.x = ROICenterCameraS.x / ROIDepthCount;
		ROICenterCameraS.y = ROICenterCameraS.y / ROIDepthCount;
		ROICenterCameraS.z = ROICenterCameraS.z / ROIDepthCount;

		//cout << "( " << ROICenterCameraS.x << ", " << ROICenterCameraS.y << ", " << ROICenterCameraS.z << " ) " << endl;
	}
	else if (ROIDepthCount == 0)
	{
		ROICenterCameraS.x = 0;
		ROICenterCameraS.y = 0;
		ROICenterCameraS.z = 0;
	}

	//cout << "ROI depth count" << ROIDepthCount << endl << endl;

}


/*--------------OpenHaptics HL-------------*/
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

	//hlBeginFrame();
	//hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.2);
	//hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.5);
	//hlEndFrame();

}
//void initHL()
//{
//	HDErrorInfo error;
//
//	hHD = hdInitDevice(HD_DEFAULT_DEVICE);
//	if (HD_DEVICE_ERROR(error = hdGetError()))
//	{
//		hduPrintError(stderr, &error, "Failed to initialize haptic device");
//		fprintf(stderr, "Press any key to exit");
//		getchar();
//		exit(-1);
//	}
//
//	hHLRC = hlCreateContext(hHD);
//	hlMakeCurrent(hHLRC);
//
//	// Enable optimization of the viewing parameters when rendering
//	// geometry for OpenHaptics.
//	//hlEnable(HL_HAPTIC_CAMERA_VIEW);
//
//	// Generate id's for the three shapes.
//	gCubicShapeId = hlGenShapes(1);;
//
//	// Compute the snap distances to use based on approximating how much
//	// force the user should need to exert to pull off (in Newtons).
//	const double kCubicShapeForce = 4.0;
//
//	HDdouble kStiffness;
//	hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &kStiffness);
//
//	// We can get a good approximation of the snap distance to use by
//	// solving the following simple force formula: 
//	// >  F = k * x  <
//	// F: Force in Newtons (N).
//	// k: Stiffness control coefficient (N/mm).
//	// x: Displacement (i.e. snap distance).
//	gCubicShapeSnapDistance = kCubicShapeForce / kStiffness;
//	//gHapticsStringShapeSnapDistance = kHapticsStringShapeForce / kStiffness;
//	//gLineShapeSnapDistance = kLineShapeForce / kStiffness;
//	//gSurfaceShapeSnapDistance = kSurfaceShapeForce / kStiffness;
//}
//
//void drawSceneHaptics()
//{
//	// Start haptic frame.  Must do this before rendering any haptic shapes.
//	hlBeginFrame();
//
//	// Cubic shape.
//
//	//cout << HLTipPosi[0] << " " << HLTipPosi[1] << " " << HLTipPosi[2] << endl;
//
//	HLOrigin.x = ROICenterCameraS.x - HLTipPosi[0];
//	HLOrigin.y = ROICenterCameraS.y - HLTipPosi[1];
//	HLOrigin.z = -ROICenterCameraS.z - HLTipPosi[2];
//
//	HDOrigin.x = ROICenterCameraS.x - HDTipPosi[0];
//	HDOrigin.y = ROICenterCameraS.y - HDTipPosi[1];
//	HDOrigin.z = -ROICenterCameraS.z - HDTipPosi[2];
//
//	glPushMatrix();
//	//glTranslated(HLOrigin.x, HLOrigin.y, HLOrigin.z);
//	glColor3f(1.0f, 0.0f, 0.0f);
//	glutSolidSphere(0.01001, 50, 50);
//	glPopMatrix();
//	
//	glPushMatrix();
//	glTranslatef(CubicPosiinHLCoord.x, CubicPosiinHLCoord.y, CubicPosiinHLCoord.z);
//	hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gCubicShapeId);
//	hlTouchModel(HL_CONSTRAINT);
//	hlTouchModelf(HL_SNAP_DISTANCE, gCubicShapeSnapDistance);
//	hlMaterialf(HL_FRONT, HL_STIFFNESS, 0.8);
//	hlMaterialf(HL_FRONT, HL_STATIC_FRICTION, 0.3);
//	hlMaterialf(HL_FRONT, HL_DYNAMIC_FRICTION, 0.5);
//	glColor3f(0.0f, 1.0f, 0.0f);
//	DrawCubic();
//	hlEndShape();
//	glPopMatrix();
//
//	// End the haptic frame.
//	hlEndFrame();
//}
//
//void drawCursor()
//{
//	static const double kCursorRadius = 0.0100;
//	static const double kCursorHeight = 0.0150;
//	static const int kCursorTess = 15;
//	HLdouble proxyxform[16];
//
//	GLUquadricObj *qobj = 0;
//
//	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
//	glPushMatrix();
//
//	if (!gCursorDisplayList)
//	{
//		gCursorDisplayList = glGenLists(1);
//		glNewList(gCursorDisplayList, GL_COMPILE);
//		qobj = gluNewQuadric();
//
//		gluCylinder(qobj, 0.0, kCursorRadius, kCursorHeight,
//			kCursorTess, kCursorTess);
//		glTranslated(0.0, 0.0, kCursorHeight);
//		gluCylinder(qobj, kCursorRadius, 0.0, kCursorHeight / 5.0,
//			kCursorTess, kCursorTess);
//
//		gluDeleteQuadric(qobj);
//		glEndList();
//	}
//
//	// Get the proxy transform in world coordinates.
//	//hlGetDoublev(HL_PROXY_TRANSFORM, proxyxform);
//	//glMultMatrixd(proxyxform);
//
//	//glTranslated(HLTipPosi[0], HLTipPosi[1], HLTipPosi[2]);
//	glTranslated(HDTipPosi[0], HDTipPosi[1], HDTipPosi[2]);
//	//glTranslated(0.1, 0.2, -0.5);
//	//cout << HLTipPosi[0] << " " << HLTipPosi[1] << " " << HLTipPosi[2] << endl;
//	//cout << HDTipPosi[0] << " " << HDTipPosi[1] << " " << HDTipPosi[2] << endl << endl;
//	
//	//cout << HLOrigin.x << " " << HLOrigin.y << " " << HLOrigin.z << endl << endl;
//	// Apply the local cursor scale factor.
//	//glScaled(gCursorScale, gCursorScale, gCursorScale);
//
//	glColor3f(1.0, 0.0, 1.0);
//
//	glCallList(gCursorDisplayList);
//
//	//glPushMatrix();
//	////glTranslated(HLOrigin.x, HLOrigin.y, HLOrigin.z);
//	//glColor3f(1.0f, 0.0f, 1.0f);
//	////glutSolidSphere(0.01001, 50, 50);
//	//glTranslated(0.0, 0.0, -0.05);
//	//glRotated(-90, 1, 0, 0);
//	////glutSolidCube(0.005);
//	//glutSolidCone(0.005, 0.001, 50, 50);
//	////hlGetDoublev(HL_PROXY_TRANSFORM, proxyxform);
//	////glMultMatrixd(proxyxform);
//	//glPopMatrix();
//
//	glPopMatrix();
//	glPopAttrib();
//}
//
//void updateWorkspace(void)
//{
//	GLdouble modelview[16];
//	GLdouble projection[16];
//	GLint viewport[4];
//
//	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
//	glGetDoublev(GL_PROJECTION_MATRIX, projection);
//	glGetIntegerv(GL_VIEWPORT, viewport);
//
//	hlMatrixMode(HL_TOUCHWORKSPACE);
//	hlLoadIdentity();
//
//	// Fit haptic workspace to view volume.
//	hluFitWorkspace(projection);
//
//	// Compute cursor scale.
//	gCursorScale = hluScreenToModelScale(modelview, projection, (HLint*)viewport);
//	gCursorScale *= CURSOR_SIZE_PIXELS;
//}
//
//void glutReshape(int width, int height)
//{
//	//static const double kPI = 3.1415926535897932384626433832795;
//	//static const double kFovY = 40;
//
//	//double nearDist, farDist, aspect;
//
//	////Make sure width & height dont go to zero when 
//	////window is minimized..
//	//if (width || height)
//	//{
//	//	glViewport(0, 0, width, height);
//
//	//	// Compute the viewing parameters based on a fixed fov and viewing
//	//	// a canonical box centered at the origin.
//	//	nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180.0);
//	//	farDist = nearDist + 2.0;
//	//	aspect = (double)width / height;
//
//	//	glMatrixMode(GL_PROJECTION);
//	//	glLoadIdentity();
//	//	gluPerspective(kFovY, aspect, nearDist, farDist);
//
//	//	// Place the camera down the Z axis looking at the origin.
//	//	glMatrixMode(GL_MODELVIEW);
//	//	glLoadIdentity();
//	//	gluLookAt(0, 0, nearDist + 1.0,
//	//		0, 0, 0,
//	//		0, 1, 0);
//
//		updateWorkspace();
//	//}
//}
//
//void exitHandler(void)
//{
//	// Deallocate the shape ids that were reserved in initHL.
//	hlDeleteShapes(gCubicShapeId, 1);
//
//	// Free up the haptic rendering context.
//	hlMakeCurrent(NULL);
//	if (hHLRC != NULL)
//	{
//		hlDeleteContext(hHLRC);
//	}
//
//	// Free up the haptic device.
//	if (hHD != HD_INVALID_HANDLE)
//	{
//		hdDisableDevice(hHD);
//	}
//}


/*--------------ARTool Kit-------------*/
//static void init(void)
//{
//	ARParam  wparam;
//
//	/* open the video path */
//	if (arVideoOpen(vconf) < 0) exit(0);
//	/* find the size of the window */
//	if (arVideoInqSize(&xsize, &ysize) < 0) exit(0);
//	printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);
//
//	/* set the initial camera parameters */
//	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
//		printf("Camera parameter load error !!\n");
//		exit(0);
//	}
//	arParamChangeSize(&wparam, xsize, ysize, &cparam);
//	arInitCparam(&cparam);
//	printf("*** Camera Parameter ***\n");
//	arParamDisp(&cparam);
//
//	if ((patt_id = arLoadPatt(patt_name)) < 0) {
//		printf("pattern load error !!\n");
//		exit(0);
//	}
//
//	/* open the graphics window */
//	argInit(&cparam, 1.0, 0, 0, 0, 0);
//}
//
//static void cleanup(void)
//{
//	arVideoCapStop();
//	arVideoClose();
//	argCleanup();
//}
//
//static void draw(void)
//{
//	double    gl_para[16];
//	GLfloat   mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
//	GLfloat   mat_flash[] = { 0.0, 0.0, 1.0, 1.0 };
//	GLfloat   mat_flash_shiny[] = { 50.0 };
//	GLfloat   light_position[] = { 100.0, -200.0, 200.0, 0.0 };
//	GLfloat   ambi[] = { 0.1, 0.1, 0.1, 0.1 };
//	GLfloat   lightZeroColor[] = { 0.9, 0.9, 0.9, 0.1 };
//
//	argDrawMode3D();
//	argDraw3dCamera(0, 0);
//	glClearDepth(1.0);
//	glClear(GL_DEPTH_BUFFER_BIT);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//
//	/* load the camera transformation matrix */
//	argConvGlpara(patt_trans, gl_para);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadMatrixd(gl_para);
//
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
//	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//	glMatrixMode(GL_MODELVIEW);
//	glTranslatef(0.0, 0.0, 25.0);
//	glutSolidCube(50.0);
//	glDisable(GL_LIGHTING);
//
//	glDisable(GL_DEPTH_TEST);
//}
//
//
//
//
//ARUint8* FrameFormateCVtoARTK(BYTE* pBufferColor)
//{
//	ARUint8* ARTKImage = new ARUint8[iWidthColor * iHeightColor * 4];
//
//	for (int y = 0; y < iHeightColor; y++)
//	{
//		for (int x = 0; x < iWidthColor; x++)
//		{
//			ARUint8 B, G, R, A;
//			int i = ((y * iWidthColor) + x) * 4; // Figure out index in array
//
//			//B = (ARUint8)CVImage.at<Vec4b>(iWidthColor, iHeightColor)[0];
//			//G = (ARUint8)CVImage.at<Vec4b>(iWidthColor, iHeightColor)[1];
//			//R = (ARUint8)CVImage.at<Vec4b>(iWidthColor, iHeightColor)[2];
//			//A = (ARUint8)CVImage.at<Vec4b>(iWidthColor, iHeightColor)[3];
//
//			B = (ARUint8)pBufferColor[i];
//			G = (ARUint8)pBufferColor[i + 1];
//			R = (ARUint8)pBufferColor[i + 2];
//			A = (ARUint8)pBufferColor[i + 3];
//
//			ARTKImage[i] = B;
//			ARTKImage[i + 1] = G;
//			ARTKImage[i + 2] = R;
//			ARTKImage[i + 3] = A;
//		}
//	}
//
//	return ARTKImage;
//
//}


int main(int argc, char** argv)
{
	KineticInit();
	GLInit();
	initHL();

	glutInit(&argc, argv);
	//init();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(iWidthColor, iHeightColor);
	glutCreateWindow("Perspective Projection");
	glutFullScreen();
	//glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	//glutReshapeFunc(glutReshape);
	glutIdleFunc(KineticUpdate);
	
	glutTimerFunc(1000, timer, 0);

	//atexit(exitHandler);

	glutMainLoop();


	return 0;
} 