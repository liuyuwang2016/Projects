/*--------------Standard Library-------------*/
#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>

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
GLuint textureid = 0;                      //BG texture

int button = -1;//按下左鍵或右鍵

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
int FPS = 0;
int FPS_RS = 0;
BYTE* pBufferColor_RGB = nullptr;
GLfloat fAspect;
void GLInit(void);                         //OpenGL Initial
void SpecialKeys(int key, int x, int y);   //OpenGL Key Func

void Keyboard(unsigned char key, int x, int y);
float g_fps(void(*func)(void), int n_frame);
bool Finish_Without_Update = FALSE;

void timer(int value);                     //OpenGL Time Func
void RenderScene(void);                    //OpenGL Render Func
bool BG_IS_OPEN = TRUE;                    //Flag for BG
void SceneWithBackground(void);            //Background Func
void SceneWithoutBackground(void);
int DRAWING_TYPE = 0;                      //Flag for drawing type: 0 = none, 1 = mesh, 2 = point cloud
void DrawPointCloud(void);                 //Drawing Type
void DrawMeshes(void);


//Draw Cubic
const float CubicUnitW = 0.1f * 1.5f;
const float CubicUnitH = 0.05f * 1.5f;
const float CubicUnitT = 0.02f;
Point3f CubicPosi;
Point3f CubicRota;
bool ROI_IS_COLLIDE = FALSE;   //FLAG of if prob insert into the virtual cube
bool ROI_IS_FRONT = FALSE;
bool Cubic_IS_BLEND = FALSE;
void DrawCubic(void);        //Draw virtula cube Func

float M_Cubic[16] = { 0 };

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

//glPrint
DWORD fdwCharSet = CHINESEBIG5_CHARSET;   // 字碼: 大五碼
char szFace[] = "Courier New";            // 字型: Courier New
int nHeight = 16;                         // 字體大小 : 16 pixel

//記錄 字元 的 字型圖像 是否已被載入, 如果 某字元 的 字型圖像 已被載入, 則不需再載入
GLuint base = 0;               // 字型圖像 的 display lists, 初始成 0
//視窗系統的東西, 我們要透過它才能向 視窗系統 取得 某字元的字型圖像
bool loaded[65536] = { 0 };    // 定義 65536 個 bool, 全都初始成 0
HFONT hFont;                   // font handle
int glPrintf(const char *format, ...);


/*----------------------------------------------*/
/*----------------OpenCV Declare----------------*/
/*----------------------------------------------*/
//For tracking
Rect2i ROI_rect;
Point ROI_p1, ROI_p2;   //start point and end point of ROI
Mat ROI;
bool ROI_S1 = FALSE;    //FLAG of mouse event
bool ROI_S2 = FALSE;    //FLAG of mouse event


enum {
	SELECT_LINE1 = 1,
	SELECT_LINE2,
};
unsigned int INIT_STEP = SELECT_LINE1;

//For Line
Rect2i LR_rect[2];
Point LR_p1[2], LR_p2[2];   //start point and end point of ROI
bool LR_S1[2] = { FALSE };    //FLAG of mouse event
bool LR_S2[2] = { FALSE };    //FLAG of mouse event

bool IS_LR_SELECT[2] = { FALSE };

void onMouseROI(int event, int x, int y, int flags, void* param);   //Mouse callback Func

//DataType: 0 = unsigned chgar, 1 = float
void InputValue(Mat M, int DataType, int Row, int Col, int Chan, float Data);
void OutputValue(Mat M, int Row, int Col, int Chan, uchar* Data);
void OutputValue(Mat M, int Row, int Col, int Chan, float* Data);

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



/*-----------------------------------------------------------------*/
/*----------------Plane of Regression (POR) Declare----------------*/
/*-----------------------------------------------------------------*/
//Rect2i POR_rect;
//Point POR_p1, POR_p2;   //start point and end point of ROI
//
//bool POR_S1 = FALSE;    //FLAG of mouse event
//bool POR_S2 = FALSE;    //FLAG of mouse event
//bool IS_POR = FALSE;
//
//Mat POR_Image;
//int PORDepthCount = 0;
//CameraSpacePoint* PORCameraSP = nullptr;
//Point3f PORCenterCameraS;
//Point3f PORNormal;
//void CameraSpacePOR(void);
//void FindPlaneOfRegression(void);

const int MAX_DEPTH_COUNT_POR = 30000;
CameraSpacePoint PORCameraSP[MAX_DEPTH_COUNT_POR];
Point3f PORCenterCameraS;
int PORDepthCount = 0;
double PORNormal[4] = { 0 };

void CameraSpacePOR(void);

//Result = Regression Coeff = [a, b, c, d]; ax + by + cz + d = 0;
void FindPlaneOfRegression(CameraSpacePoint* Data, int DataNum, double* Result);
void RotationAboutArbitraryAxes(float theta, float* axis, CameraSpacePoint* Data, int Size);
void RotationAboutArbitraryAxes(float theta, float* axis, float* M);
void PlaceObject(void);

/*-----------------------------------------------------------------*/
/*------------------Linear Regression (LR) Declare-----------------*/
/*-----------------------------------------------------------------*/

const int MAX_PIXEL_COUNT_LR = 2000;
const int MAX_DEPTH_COUNT_LR = 1000;

int LRPixelCount[2] = { 0 };
Point2i LRPixel[2][MAX_PIXEL_COUNT_LR] = { 0 };
void FindLR(void);

CameraSpacePoint LRCameraSP[2][MAX_DEPTH_COUNT_LR];
int LRDepthCount[2] = { 0 };
void CameraSpaceLR(void);

double LRNormal[2][4] = { 0 };

//Result = Regression Coeff = [type, a, b]; y = ax + b
//type = 1 = yx, type = 2 = zx
double RegrL1[3] = { 2, 0, 0 };
double RegrL2[3] = { 2, 0, 0 };

CameraSpacePoint Intersect;



/*---------------------------------------------------------------*/
/*----------------Region Of Interest(ROI) Declare----------------*/
/*---------------------------------------------------------------*/
////Initial
//Rect2i ROI_rect;
//Point ROI_p1, ROI_p2;   //start point and end point of ROI
//Mat ROI;
//Mat ROI_Image;
//Mat ROI_YCrCb;
//bool ROI_S1 = FALSE;    //FLAG of mouse event
//bool ROI_S2 = FALSE;    //FLAG of mouse event
//void onMouseROI(int event, int x, int y, int flags, void* param);   //Mouse callback Func
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
//How mant points do you want to record
const int StorageSize = 5000;
int ROIStorageCount = 0;

Point2i Start, End;
bool IS_REC_START = FALSE;                        //FLAG of star point and end point of data
bool IS_REC_END = FALSE;

bool IS_KEY_F1_UP = FALSE;                        //FLAG of data collecting, see SpecialKeys()
bool IS_KEY_F3_UP = FALSE;
bool IS_KEY_F6_UP = FALSE;
void ROICameraSPStorage(void);                    //Storage ROI Func



/*-------------------------------------------------------*/
/*----------------Path Generation Declare----------------*/
/*-------------------------------------------------------*/
CameraSpacePoint* ROICameraSP_Projection = nullptr;
CameraSpacePoint* ROICameraSP_AtWorldCoordOrigin = nullptr;
CameraSpacePoint* ROICameraSP_Regression = nullptr;
bool IS_PathGenProjToPlane = FALSE;       //If raw data is projected to plane
bool IS_PathGenRegression = FALSE;
bool IS_PathGenProcessing = FALSE;

//PlaneCoeff = [a, b, c, d]; ax + by + cz +d = 0
void PathGenProjToPlane(CameraSpacePoint* Data, int DataNum, double* PlaneCoeff, CameraSpacePoint* Result);

//TransM = [Rx, Ry, Rz, Tx, Ty, Tz] in order
void PathGenCoordWorldToCubic(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result);

void PathGenCoordCubicToWorld(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result);

void PathGenCalcMean(CameraSpacePoint* Data, int DataNum, double* Result);

//Result = Regression Coeff = [type, a, b]; y = ax + b
//type = 1 = yx, type = 2 = zx
void PathGenRegression(CameraSpacePoint* Data, int DataNum, double* Result);



/*----------------------------------------------*/
/*--------------OpenHaptics Declare-------------*/
/*----------------------------------------------*/
static HHD hHD = HD_INVALID_HANDLE;
static HHLRC hHLRC = 0;
HLuint spring;
float gGain = 0.8f;
float gMagnitude = 1.0f;

bool IS_HL_FORCE_APPLY = FALSE;
bool IS_HL_FORCE_GEN = FALSE;
void initHL(void);





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

	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidthColor, iHeightColor, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);

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
			IS_PathGenProcessing = TRUE;

			cout << endl << "Start collecting data..." << endl;

		}

	}
	//Press F2 to clear
	else if (key == GLUT_KEY_F2)
	{
		if (ROICameraSP_Storage != nullptr)
		{
			IS_KEY_F1_UP = FALSE;
			
			delete[] ROICameraSP_Storage;
			ROICameraSP_Storage = nullptr;

			ROIStorageCount = 0;

			//IS_KEY_F3_UP = FALSE;
			IS_PathGenProjToPlane = FALSE;
			IS_PathGenRegression = FALSE;
			IS_PathGenProcessing = FALSE;

			IS_REC_END = FALSE;
			IS_REC_START = FALSE;


			cout << endl << "Clear ROICameraSP_Storage..." << endl;
		}
		//else if (LRPixel_Select != nullptr)
		//{
		//	IS_LR_RECORDING = FALSE;
		//	
		//	delete[] LRPixel_Select;
		//	LRPixel_Select = nullptr;
		//
		//	LRStorageCount = 0;
		//
		//
		//	cout << endl << "Clear LRPixel_Select..." << endl;
		//}
	}
	//Press F3 to calculate
	else if (key == GLUT_KEY_F3)
	{
		if (IS_KEY_F3_UP)
		{
			cout << endl << "Calculating Please Wait..." << endl;
		}
		else if (!IS_KEY_F3_UP && ROIStorageCount > 0)
		{
			IS_KEY_F3_UP = TRUE;
			cout << endl << "Start Calculating..." << endl;
		}
		else
		{
			cout << endl << "Press F1 to collect data before calculating..." << endl;
		}
	}
	//Press F4 to enable or disable transparent mode
	else if (key == GLUT_KEY_F4)
	{
		if (Cubic_IS_BLEND)
		{
			Cubic_IS_BLEND = FALSE;
			cout << endl << "Disable Transparent Mode..." << endl;
		}
		else
		{
			Cubic_IS_BLEND = TRUE;
			cout << endl << "Enable Transparent Mode..." << endl;
			
		}
	}
	//Press F5 to apply force
	else if (key == GLUT_KEY_F5)
	{
		if (IS_HL_FORCE_APPLY)
		{
			IS_HL_FORCE_APPLY = FALSE;
			cout << endl << "Disable force feedback..." << endl;
		}
		else
		{
			IS_HL_FORCE_APPLY = TRUE;
			cout << endl << "Enable force feedback..." << endl;

		}
	}
	//Press F6 to recoard data in txt file
	else if (key == GLUT_KEY_F6)
	{
		if (!IS_KEY_F6_UP)
		{
			IS_KEY_F6_UP = TRUE;
		}

	}

	/*-------------Rotation------------*/
	//else if (key == GLUT_KEY_DOWN && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.x == 360)
	//	{
	//		CubicRota.x = 0;
	//	}
	//	else
	//	{
	//		CubicRota.x += 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.x : " << CubicRota.x << endl;
	//}
	//else if (key == GLUT_KEY_UP && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.x == -360)
	//	{
	//		CubicRota.x = 0;
	//	}
	//	else
	//	{
	//		CubicRota.x -= 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.x : " << CubicRota.x << endl;
	//}
	//else if (key == GLUT_KEY_RIGHT && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.y == 360)
	//	{
	//		CubicRota.y = 0;
	//	}
	//	else
	//	{
	//		CubicRota.y += 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.y : " << CubicRota.y << endl;
	//}
	//else if (key == GLUT_KEY_LEFT && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.y == -360)
	//	{
	//		CubicRota.y = 0;
	//	}
	//	else
	//	{
	//		CubicRota.y -= 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.y : " << CubicRota.y << endl;
	//}
	//else if (key == GLUT_KEY_F9 && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.z == 360)
	//	{
	//		CubicRota.z = 0;
	//	}
	//	else
	//	{
	//		CubicRota.z += 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.z : " << CubicRota.z << endl;
	//}
	//else if (key == GLUT_KEY_F10 && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.z == -360)
	//	{
	//		CubicRota.z = 0;
	//	}
	//	else
	//	{
	//		CubicRota.z -= 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.z : " << CubicRota.z << endl;
	//}

	/*-------------Translation------------*/
	//else if (key == GLUT_KEY_DOWN && !IS_PathGenProcessing)
	//{
	//	CubicPosi.y += 0.01f;
	//
	//	cout << endl << "CubicPosi.y : " << CubicPosi.y << endl;
	//}
	//else if (key == GLUT_KEY_UP && !IS_PathGenProcessing)
	//{
	//	CubicPosi.y -= 0.01f;
	//
	//	cout << endl << "CubicPosi.y : " << CubicPosi.y << endl;
	//}
	//
	//else if (key == GLUT_KEY_RIGHT && !IS_PathGenProcessing)
	//{
	//	CubicPosi.x += 0.01f;
	//
	//	cout << endl << "CubicPosi.x : " << CubicPosi.x << endl;
	//}
	//else if (key == GLUT_KEY_LEFT && !IS_PathGenProcessing)
	//{
	//	CubicPosi.x -= 0.01f;
	//
	//	cout << endl << "CubicPosi.x : " << CubicPosi.x << endl;
	//}
	//
	//else if (key == GLUT_KEY_F9 && !IS_PathGenProcessing)
	//{
	//	CubicPosi.z += 0.01f;
	//
	//	cout << endl << "CubicPosi.z : " << CubicPosi.z << endl;
	//}
	//else if (key == GLUT_KEY_F10 && !IS_PathGenProcessing)
	//{
	//	CubicPosi.z -= 0.01f;
	//
	//	cout << endl << "CubicPosi.z : " << CubicPosi.z << endl;
	//}

	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'F':
	case 'f':
		Finish_Without_Update = TRUE;
		printf("%f fps\n", g_fps(RenderScene, 100));
		//cout << "F" << endl;
		//cout << "FPS_RS = " << FPS_RS << endl;
		Finish_Without_Update = FALSE;
		break;

	//case 'A':
	//case 'a':
	//	if (IS_LR_RECORDING)
	//	{
	//		IS_LR_RECORDING = FALSE;
	//	}
	//	else
	//	{
	//		IS_LR_RECORDING = TRUE;
	//	}
	//	break;
	}
}

float g_fps(void(*func)(void), int n_frame)
{
	clock_t start, finish;
	int i;
	float fps;

	printf("Performing benchmark, please wait");
	start = clock();
	for (i = 0; i < n_frame; i++)
	{
		func();
	}
	printf("done\n");
	finish = clock();

	fps = float(n_frame) / (finish - start)*CLOCKS_PER_SEC;
	return fps;
}

void timer(int value)
{
	switch (value)
	{
	//OpenGL refresh timmer
	case 0:
		printf("FPS = %d\n", FPS);
		FPS = 0;
		FPS_RS = 0;
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
	glOrtho(0, iWidthColor, 0, iHeightColor, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	 
	glLoadIdentity();

	//Draw backgroung through RGBA image from Kinect
	
	glRasterPos2f(0.0f, 0.0f);
	//glDrawPixels(iWidthColor, iHeightColor, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);
	
	glEnable(GL_TEXTURE_2D);

	//Without painting BG white the BG will turn orange and glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) did not work. Don't know why
	glColor3ub(255, 255, 255);
	// tell opengl to use the generated texture name
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidthColor, iHeightColor, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPushMatrix();

	// make a rectangle
	glBegin(GL_QUADS);

	// top left
	glTexCoord2i(0, 0);
	glVertex2i(0, iHeightColor);

	// top right
	glTexCoord2i(1, 0);
	glVertex2i(iWidthColor, iHeightColor);

	// bottom right
	glTexCoord2i(1, 1);
	glVertex2i(iWidthColor, 0);

	// bottom left
	glTexCoord2i(0, 1);
	glVertex2i(0, 0);

	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);



	static const double kFovY = 54.5;
	static const double kPI = 3.1415926535897932384626433832795;
	double nearDist, farDist, aspect;

	nearDist = 0.01f / tan((kFovY / 2.0) * kPI / 180.0);
	farDist = 20000;
	aspect = (double)iWidthColor / iHeightColor;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(kFovY, aspect, nearDist, farDist);
	//gluPerspective(54.5f, (double)iWidthColor / (double)iHeightColor, 0.01f, 20000.0f);

	glEnable(GL_DEPTH_TEST);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	glTranslatef(0.055f, -0.02f, 0.0f);
}

void SceneWithoutBackground()
{
	static const double kFovY = 54.5;
	static const double kPI = 3.1415926535897932384626433832795;
	double nearDist, farDist, aspect;

	nearDist = 0.01f / tan((kFovY / 2.0) * kPI / 180.0);
	farDist = 20000;
	aspect = (double)iWidthColor / iHeightColor;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(kFovY, aspect, nearDist, farDist);
	//gluPerspective(54.5f, (double)iWidthColor / (double)iHeightColor, 0.001f, 20000.0f);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	glTranslatef(0.055f, -0.02f, 0.0f);
}

void DrawPointCloud()
{

	/**************/
	//待改善：
	//畫一部分就好，不用畫全部
	/*************/

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
	/**************/
	//待改善：
	//畫一部分就好，不用畫全部
	/*************/

	if (BG_IS_OPEN)
	{
		glPushMatrix();
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_TRIANGLE_STRIP);
		
		//glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

		for (int i = 0; i < colorPointCount; i+=3)
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
			else
			{
				i += iWidthColor;
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

		for (int i = 0; i < colorPointCount; i+=3)
		{
			glColor3ub(pBufferColor[4 * i], pBufferColor[4 * i + 1], pBufferColor[4 * i + 2]);

			if (i % iWidthColor < (iWidthColor - 1) && i < iWidthColor*(iHeightColor - 1))
			{
				glVertex3f(pCSPoints[i].X, pCSPoints[i].Y, -pCSPoints[i].Z);
				glVertex3f(pCSPoints[i + iWidthColor].X, pCSPoints[i + iWidthColor].Y, -pCSPoints[i + iWidthColor].Z);
				glVertex3f(pCSPoints[i + 1].X, pCSPoints[i + 1].Y, -pCSPoints[i + 1].Z);
				glVertex3f(pCSPoints[i + iWidthColor + 1].X, pCSPoints[i + iWidthColor + 1].Y, -pCSPoints[i + iWidthColor + 1].Z);

			}
			else
			{
				i += iWidthColor;
			}
		}
		glEnd();
		glPopMatrix();
	}

}

void DrawCubic()
{
	float theta = acos(-1 / sqrt(pow(1, 2) + pow(RegrL1[1], 2)));

	glPushMatrix();
	//glLoadIdentity;
	//glMatrixMode(GL_MODELVIEW_MATRIX);
	//glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);
	//
	//glRotatef(CubicRota.y, 0, 1, 0);
	//glRotatef(CubicRota.z, 0, 0, 1);
	//glRotatef(CubicRota.x, 1, 0, 0);

	glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);

	glMultMatrixf(M_Cubic);

	glRotatef(-theta*180.0f / 3.14159265358979323846 + 180, 0, 1, 0);
	glTranslatef(-CubicUnitW, -CubicUnitH, CubicUnitT);  //6
	
	glRotatef(180, 0, 1, 0);
	glRotatef(180, 0, 0, 1);
	//glRotatef(90, 1, 0, 0);

	//glEnable(GL_TEXTURE_2D);
	//
	//glGenTextures(1, &textureid);


	//glBindTexture(GL_TEXTURE_2D, textureid);
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, iWidthColor, iHeightColor, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindTexture(GL_TEXTURE_2D, textureid);

	glBegin(GL_QUADS);        // Draw The Cube Using quads

	//Front
	//glTexCoord2i(0, 0);
	glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
	//glTexCoord2i(1, 0);
	glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
	//glTexCoord2i(1, 1);
	glVertex3f( CubicUnitW, -CubicUnitH,  CubicUnitT);    // 3
	//glTexCoord2i(0, 1);
	glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // 4


	//Back
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
	glColor3ub(0, 255, 0);
	glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
	glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
	glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
	glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
	//Down
	glColor3ub(255, 255, 0);
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

	//glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
	//glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
	//glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
	//glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
	//glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5

	//glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
	//glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // 4
	//glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5

	//glVertex3f(CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
	//glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8

	
	glVertex3f(-CubicUnitW,  CubicUnitH,  CubicUnitT);    // 1
	glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
	glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
	glVertex3f( CubicUnitW,  CubicUnitH,  CubicUnitT);    // 2
	
	glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
	glVertex3f( CubicUnitW,  CubicUnitH, -CubicUnitT);    // 6
	glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
	glVertex3f(-CubicUnitW,  CubicUnitH, -CubicUnitT);    // 5
	glVertex3f(-CubicUnitW, -CubicUnitH,  CubicUnitT);    // 4

	glVertex3f(-CubicUnitW, -CubicUnitH, -CubicUnitT);    // 8
	glVertex3f( CubicUnitW, -CubicUnitH, -CubicUnitT);    // 7
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


}

void DrawPath()
{
	const float T_MIN = -0.19;
	const float T_MAX =  0.19;
	const float T_PRECISION = 0.001;
	
	glPushMatrix();
		
	glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);
	glRotatef(CubicRota.y, 0, 1, 0);
	glRotatef(CubicRota.z, 0, 0, 1);
	glRotatef(CubicRota.x, 1, 0, 0);
	glTranslatef(0, 0, CubicUnitT + 0.0001f);


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

	glPopMatrix();


}

int glPrintf(const char *format, ...)
{
	//DWORD fdwCharSet = CHINESEBIG5_CHARSET;   // 字碼: 大五碼
	//char szFace[] = "Courier New";            // 字型: Courier New
	//int nHeight = 16;                         // 字體大小 : 16 pixel


	char buffer[65536];
	va_list args;                         // 定義 argument list 
	va_start(args, format);               // 取得本函數的 argument list 
	vsprintf_s(buffer, format, args);     // 把 argument list 交給 vsprintf() 處理, 並把字串存到 buffer
	va_end(args);

	////記錄 字元 的 字型圖像 是否已被載入, 如果 某字元 的 字型圖像 已被載入, 則不需再載入
	//static GLuint base = 0;               // 字型圖像 的 display lists, 初始成 0
	////視窗系統的東西, 我們要透過它才能向 視窗系統 取得 某字元的字型圖像
	//static bool loaded[65536] = { 0 };    // 定義 65536 個 bool, 全都初始成 0
	//static HFONT hFont;                   // font handle

	static int x0 = 0, y0 = 0;

	if (base == 0)                        // 如果 base 還沒有被創造, 才執行以下的
	{
		base = glGenLists(65536);
		hFont = CreateFont(nHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, fdwCharSet,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			FF_DONTCARE | DEFAULT_PITCH, szFace);
	}

	float p0[4], p1[4], c0[2][4] = { { 0, 0, 0, 1 }, { 1, 1, 1, 1 } };
	int i, j, len, offset[2] = { 1, 0 };
	wchar_t *wstr;
	GLint viewport[4];
	HDC hdc = 0;

	glGetIntegerv(GL_VIEWPORT, viewport);                  // 取得 view-port, 設定 projection matrix 時需要的
	glGetFloatv(GL_CURRENT_RASTER_POSITION, p0);           // 取得 raster position, 換新行時, 用它來計算座標的

	glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);   // push attributes, 為還原設定作準備
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);                           // 自訂 projection matrix
	glPushMatrix();                                        // 為還原 projection matrix 作準備
	glLoadIdentity();
	gluOrtho2D(0, viewport[2], 0, viewport[3]);

	glMatrixMode(GL_MODELVIEW);                             // 自訂 model-view matrix
	glPushMatrix();                                         // 為還原 model-view matrix 作準備
	glLoadIdentity();

	if (/*strcmp(buffer, ">>glut") == 0*/memcmp(buffer, ">>glut", 6) == 0)                      // 如果來源字串是 ">>glut", 就把秀文字的起始位置設到左上角.
	{

		sscanf_s(buffer, ">>glut(%i,%i)", &x0, &y0);
		glTranslatef(x0, -y0, 0);

		glRasterPos2f(4, viewport[3] - nHeight);
		//glRasterPos2f(50.0f, 20.0f);
		//cout << endl << "4 " << viewport[3] - nHeight;

	}
	else if (strcmp(buffer, ">>free") == 0)                 // 如果來源字串是 ">>free", 就把預載字型資料刪除. (範例沒有使用它的)
	{
		glDeleteLists(base, 65536); base = 0;
		memset(loaded, 0, 65536 * sizeof(bool));
		DeleteObject(hFont);
	}
	else
	{
		glRasterPos2f(p0[0], p0[1]);                                             // (這句是沒實質作用的, 不過, 要先呼叫它, 下一句 glGetFloatv() 才能正確執行, 奇哉, 怪哉. )
		glGetFloatv(GL_CURRENT_RASTER_COLOR, c0[1]);                             // 最得 glColor() 所述的顏色
		len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, 0, 0);     // 計算轉換成大五碼後, 字串的長度
		wstr = (wchar_t*)malloc(len*sizeof(wchar_t));                            // 配置記憶體去儲存 大五碼字串
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wstr, len);      // 把 buffer 轉換成 大五碼字串
		for (j = 0; j < 2; j++)                                                  // 秀兩次字串
		{
			glColor4fv(c0[j]);                                     // 設定 字串顏色
			//glColor3b(255, 0, 0);
			glRasterPos2f(p0[0] + offset[j], p0[1] - offset[j]);   // 設定 字串起始位置
			for (i = 0; i<len - 1; i++)
			{
				if (wstr[i] == '\n')                                        // 如果是 '\n', 就換新行
				{
					glGetFloatv(GL_CURRENT_RASTER_POSITION, (float*)&p1);   // 取得現在的位置
					//glRasterPos2f(4 + offset[j], p1[1] - (nHeight + 2));    // 設定新行的起始位置
					glRasterPos2f(4 + x0 + offset[j], p1[1] - (nHeight + 2));
				}
				else
				{
					if (!loaded[wstr[i]])    // 如果字元未被載入
					{
						if (hdc == 0)
						{
							hdc = wglGetCurrentDC();    // 取得 device context 的 handle (視窗系統的東西)
							SelectObject(hdc, hFont);   // 選取 font handle (視窗系統的東西)
						}
						wglUseFontBitmapsW(hdc, wstr[i], 1, base + wstr[i]);    // 載入字元的字型圖像
						loaded[wstr[i]] = TRUE;                                 // 標示這字元已被載入
					}
					glCallList(base + wstr[i]);   // 繪畫字元的 display list
				}
			}
		}
		free(wstr);    // 把配置了的記憶體歸還系統
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();

	return 0;
}


void RenderScene()
{
	//PORDepthCount = 6;
	//PORCameraSP = new CameraSpacePoint[6];
	//PORCameraSP[0].X = 0.5;
	//PORCameraSP[0].Y = 0.2;
	//PORCameraSP[0].Z = 0.4;
	//
	//PORCameraSP[1].X = 0.2;
	//PORCameraSP[1].Y = 0.8;
	//PORCameraSP[1].Z = 0.3;
	//
	//PORCameraSP[2].X = 0.1;
	//PORCameraSP[2].Y = 0.6;
	//PORCameraSP[2].Z = 0.7;
	//
	//PORCameraSP[3].X = 0.2;
	//PORCameraSP[3].Y = 0.9;
	//PORCameraSP[3].Z = 0.5;
	//
	//PORCameraSP[4].X = 0.4;
	//PORCameraSP[4].Y = 0.3;
	//PORCameraSP[4].Z = 0.2;
	//
	//PORCameraSP[5].X = 0.1;
	//PORCameraSP[5].Y = 0.2;
	//PORCameraSP[5].Z = 0.4;
	//
	//FindPlaneOfRegression();

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
		CameraSpacePoint* ROICameraSP_TouchDetec = new CameraSpacePoint[ROIDepthCount];
		GLfloat TransM[6] = { -CubicRota.x, -CubicRota.y, -CubicRota.z, -CubicPosi.x, -CubicPosi.y, -CubicPosi.z };

		PathGenCoordWorldToCubic(ROICameraSP, ROIDepthCount, TransM, ROICameraSP_TouchDetec);

		glPushMatrix();
		glPointSize(2.0f);
		glBegin(GL_POINTS);
			for (int i = 0; i < ROIDepthCount; i++)
			{
				glColor3ub(0, 255, 0);
				glVertex3f(ROICameraSP[i].X, ROICameraSP[i].Y, ROICameraSP[i].Z);
				
				GLfloat pX = ROICameraSP_TouchDetec[i].X;
				GLfloat pY = ROICameraSP_TouchDetec[i].Y;
				GLfloat pZ = ROICameraSP_TouchDetec[i].Z;
				
				/*--------------Touching Detected-------------*/
				if (abs(pZ) < CubicUnitT)
				{
					if (abs(pY) < CubicUnitH)
					{
						if (abs(pX) < CubicUnitW)
						{
							ROI_IS_COLLIDE = TRUE;
						}
					}
				}
				else if (pZ  > CubicUnitT)
				{
					ROI_IS_FRONT = TRUE;
				}

			}

		glEnd();

		if (!ROI_IS_FRONT)
		{
			glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x - 60, ROICenterColorS_Old.y - 35);
			glPrintf("(%.4f, %.4f, %.4f)", ROICenterCameraS.x, ROICenterCameraS.y, ROICenterCameraS.z);
		}

		glPopMatrix();

		delete[] ROICameraSP_TouchDetec;

	}

	/*--------------LR Mapping-------------*/

	glPushMatrix();
	for (int target = 0; target < 2; target++)
	{
		if (LRDepthCount[target] != 0)
		{
			
			glPointSize(2.0f);
			glBegin(GL_POINTS);

			for (int i = 0; i < LRDepthCount[target]; i++)
			{
				glColor3ub(255, 0, 0);
				glVertex3f(LRCameraSP[target][i].X, LRCameraSP[target][i].Y, LRCameraSP[target][i].Z);
			}

			glEnd();

		}

		//cout << target << ", " << LRDepthCount[target] << endl;
	}
	glPopMatrix();


	//PlaneCoeff = [a, b, c, d]; ax + by + cz + d = 0
	//PathGenProjToPlane(CameraSpacePoint* Data, int DataNum, double* PlaneCoeff, CameraSpacePoint* Result)


	/*--------------POR-------------*/
	if (PORDepthCount != 0)
	{

		//glPushMatrix();
		//glPointSize(2.0f);
		//glBegin(GL_POINTS);
		//for (int i = 0; i < PORDepthCount; i++)
		//{
		//	glColor3ub(0, 255, 255);
		//
		//	//cout << PORCameraSP[i].X << " " << PORCameraSP[i].Y << " " << PORCameraSP[i].Z << endl;
		//
		//	glVertex3f(PORCameraSP[i].X, PORCameraSP[i].Y, PORCameraSP[i].Z);
		//}
		//glEnd();
		//
		//glLineWidth(2.5);
		//glBegin(GL_LINES);
		//
		//glColor3ub(255, 0, 0);
		//
		//glVertex3f(PORCameraSP[5000].X, PORCameraSP[5000].Y, PORCameraSP[5000].Z);
		//glVertex3f(PORCameraSP[5000].X - PORNormal[0] * 0.01, PORCameraSP[5000].Y - PORNormal[1] * 0.01, PORCameraSP[5000].Z - PORNormal[2] * 0.01);
		//	
		//	
		//glEnd();



		//if (RegrL1[1] != 0)
		//{
		//	glPushMatrix();
		//	glLineWidth(2.5);
		//	glBegin(GL_LINES);
		//	for (float i = 0.0f; i < 1.0f; i += 0.1)
		//	{
		//		glColor3ub(0, 255, 255);
		//		glVertex3f(LRCameraSP[0][0].X + i, LRCameraSP[0][0].Y, RegrL1[1] * (LRCameraSP[0][0].X + i) + RegrL1[2]);
		//	}
		//	glEnd();
		//}
		//
		//if (RegrL2[1] != 0)
		//{
		//	glPushMatrix();
		//	glLineWidth(2.5);
		//	glBegin(GL_LINES);
		//	for (float i = -0.5f; i < 0.0f; i += 0.1)
		//	{
		//		glColor3ub(0, 255, 255);
		//		glVertex3f(LRCameraSP[1][0].X + i, LRCameraSP[1][0].Y, RegrL2[1] * (LRCameraSP[1][0].X + i) + RegrL2[2]);
		//	}
		//	glEnd();
		//}


		


		//if (LRNormal[1][0] != 0)
		//{
		//	glLineWidth(5);
		//	glBegin(GL_LINES);
		//
		//	glColor3ub(0, 255, 0);
		//
		//	glVertex3f(LRCameraSP[0][10].X, LRCameraSP[0][10].Y, LRCameraSP[0][10].Z);
		//	glVertex3f(LRCameraSP[0][10].X - LRNormal[0][0] * 0.01, LRCameraSP[0][10].Y - LRNormal[0][1] * 0.01, LRCameraSP[0][10].Z - LRNormal[0][2] * 0.01);
		//
		//
		//	glEnd();
		//
		//	glLineWidth(5);
		//	glBegin(GL_LINES);
		//
		//	glColor3ub(0, 0, 255);
		//
		//	glVertex3f(LRCameraSP[1][10].X, LRCameraSP[1][10].Y, LRCameraSP[1][10].Z);
		//	glVertex3f(LRCameraSP[1][10].X - LRNormal[1][0] * 0.01, LRCameraSP[1][10].Y - LRNormal[1][1] * 0.01, LRCameraSP[1][10].Z - LRNormal[1][2] * 0.01);
		//
		//
		//	glEnd();
		//
		//}


		//Mat Normal = Mat::zeros(3, 1, CV_32FC1);
		//Mat A = Mat::zeros(3, 1, CV_32FC1);
		//Mat B = Mat::zeros(3, 1, CV_32FC1);

		//Normal.at<float>(0, 0) = PORNormal[0];
		//Normal.at<float>(1, 0) = PORNormal[1];
		//Normal.at<float>(2, 0) = PORNormal[2];

		//A.at<float>(0, 0) = LRCameraSP[0][100].X - LRCameraSP[0][200].X;
		//A.at<float>(1, 0) = LRCameraSP[0][100].X - LRCameraSP[0][200].X;
		//A.at<float>(2, 0) = LRCameraSP[0][100].X - LRCameraSP[0][200].X;

		//B.at<float>(0, 0) = Normal.cross(A).at<float>(0, 0);
		//B.at<float>(1, 0) = Normal.cross(A).at<float>(1, 0);
		//B.at<float>(2, 0) = Normal.cross(A).at<float>(2, 0);

		//glLineWidth(2.5);
		//glBegin(GL_LINES);


		//glColor3ub(255, 0, 0);
		//glVertex3f(


		//glEnd();


		glPopMatrix();

		glPushMatrix();
		glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
		glColor3ub(35, 128, 53);
		glutSolidSphere(0.001, 100, 100);
		glPopMatrix();


	}
		//cout << PORCenterCameraS.x << " " << PORCenterCameraS.y << " " << PORCenterCameraS.z << endl;
	//
	//	glPushMatrix();
	//	
	//	//glTranslatef(PORCenterCameraS.x, PORCenterCameraS.y, PORCenterCameraS.z);
	//	//glScaled(0.001f, 0.001f, 0.001f);
	//	glLineWidth(2.5);
	//	glBegin(GL_LINES);
	//
	//	glColor3ub(255, 0, 0);
	//
	//	glVertex3f(PORCenterCameraS.x, PORCenterCameraS.y, PORCenterCameraS.z);
	//	glVertex3f(PORCenterCameraS.x - PORNormal.x*0.1, PORCenterCameraS.y - PORNormal.y*0.1, PORCenterCameraS.z - PORNormal.z*0.1);
	//	
	//	
	//	glEnd();
	//	glPopMatrix();
	//
	//}




	/*--------------ROI Storage-------------*/
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
			if (i == 0 && !IS_REC_START)
			{
				Start.x = ROICenterColorS_Old.x;
				Start.y = ROICenterColorS_Old.y;
				
				IS_REC_START = TRUE;
			}
			else if (i == ROIStorageCount - 1 && !IS_KEY_F1_UP && !IS_REC_END)
			{
				End.x = ROICenterColorS_Old.x;
				End.y = ROICenterColorS_Old.y;

				IS_REC_END = TRUE;
			}

			GLfloat pX = ROICameraSP_Storage[i].X;
			GLfloat pY = ROICameraSP_Storage[i].Y;
			GLfloat pZ = ROICameraSP_Storage[i].Z;
			glVertex3f(pX, pY, pZ);

		}

		glEnd();
		glPopMatrix();
	}

	/*--------------ROI Storage in .txt file-------------*/
	if (IS_KEY_F6_UP && ROIStorageCount != 0)
	{
		fstream file;
		file.open("coord.txt", ios::out);      //開啟檔案

		if (!file)     //檢查檔案是否成功開啟

		{

			cerr << "Can't open file!\n";

			exit(1);     //在不正常情形下，中斷程式的執行

		}


		cout << endl << "Writing to file, please wait" << endl;

		for (int i = 0; i < ROIStorageCount; i++)    
		{
			if (i == 0)
			{
				file << "Total count : " << ROIStorageCount << endl;
			}

			file <<  setw(5) << i + 1 << " : " << fixed << setprecision(4) << ROICameraSP_Storage[i].X << "   " << ROICameraSP_Storage[i].Y << "   " << ROICameraSP_Storage[i].Z << "   " << "\n";

		}      //將資料輸出至檔案

		IS_KEY_F6_UP = FALSE;

		cout << endl << "End writing to file" << endl;
	}



	/*--------------IS_PathGenProjToPlane-------------*/
	if (IS_KEY_F3_UP && ROIStorageCount > 0)
	{	

		if (ROICameraSP_AtWorldCoordOrigin != nullptr)
		{
			delete[] ROICameraSP_AtWorldCoordOrigin;
			ROICameraSP_AtWorldCoordOrigin = nullptr;
		}

		ROICameraSP_AtWorldCoordOrigin = new CameraSpacePoint[ROIStorageCount];
		

		if (ROICameraSP_Projection != nullptr)
		{
			delete[] ROICameraSP_Projection;
			ROICameraSP_Projection = nullptr;
		}

		ROICameraSP_Projection = new CameraSpacePoint[ROIStorageCount];
		

		GLfloat TransM1[6] = { -CubicRota.x, -CubicRota.y, -CubicRota.z, -CubicPosi.x, -CubicPosi.y, -CubicPosi.z };
		GLfloat TransM2[6] = { CubicRota.x, CubicRota.y, CubicRota.z, CubicPosi.x, CubicPosi.y, CubicPosi.z };
		double Coeff[4] = { 0, 0, 1, -CubicUnitT };

		PathGenCoordWorldToCubic(ROICameraSP_Storage, ROIStorageCount, TransM1, ROICameraSP_AtWorldCoordOrigin);
		PathGenProjToPlane(ROICameraSP_AtWorldCoordOrigin, ROIStorageCount, Coeff, ROICameraSP_Projection);
		PathGenCoordCubicToWorld(ROICameraSP_Projection, ROIStorageCount, TransM2, ROICameraSP_Storage);


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

	}


	/*--------------IS_PathGenRegression-------------*/

	if (IS_PathGenProjToPlane && ROIStorageCount > 0)
	{
		if (ROICameraSP_Regression != nullptr)
		{
			delete[] ROICameraSP_Regression;
			ROICameraSP_Regression = nullptr;
		}
	
		ROICameraSP_Regression = new CameraSpacePoint[ROIStorageCount];
	
		//Result = Regression Coeff = [type, a, b]; y = ax + b
		//type = 1 = yx, type = 2 = zx
		double RegresCoeff[3] = { 1, 0, 0 };
		
		//[Rx, Ry, Rz, Tx, Ty, Tz] in order
		GLfloat TransM3[6] = { 0, 0, 0, 0, 0, -CubicUnitT };
	

		PathGenCoordWorldToCubic(ROICameraSP_AtWorldCoordOrigin, ROIStorageCount, TransM3, ROICameraSP_Regression);
		PathGenRegression(ROICameraSP_Regression, ROIStorageCount, RegresCoeff);
		

		//畫出regression的結果，應該要用原始資料去找起點跟終點，但是好麻煩所以硬代lol
		//但是應該可以在bubble sort那邊算出來
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
	
		glPushMatrix();
		glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);
	
		glRotatef(CubicRota.y, 0, 1, 0);
		glRotatef(CubicRota.z, 0, 0, 1);
		glRotatef(CubicRota.x, 1, 0, 0);
		glTranslatef(0, 0, CubicUnitT);
	
		glColor3ub(0, 255, 0);
	
		glBegin(GL_LINE_STRIP);
		for (double t = T_MIN; t <= T_MAX; t += T_PRECISION)
		{
			glVertex2d(t, RegresCoeff[1] * t + RegresCoeff[2]);
		}
		glEnd();
	
		glPopMatrix();
	
		IS_PathGenRegression = TRUE;
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
	
	if (Cubic_IS_BLEND)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	if (ROI_IS_COLLIDE)
	{
		glColor4f(1.0f, 0.0f, 0.0f, 0.6f);
		//glColor3f(1.0f, 0.0f, 0.0f);
	}
	else
	{
		glColor4f(0.0f, 0.0f, 0.7f, 0.6f);
		//glColor3f(0.0f, 0.0f, 1.0f);
	}

	DrawCubic();


	if (Cubic_IS_BLEND)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

	}

	glPopMatrix();


	/*--------------show start and ent point-------------*/
	glPushMatrix();

	if (ROIStorageCount != 0)
	{
		if (BG_IS_OPEN)
		{
			if (IS_REC_START)
			{
				glColor3ub(255, 255, 0);
				glPrintf(">>glut(%i, %i)", Start.x, Start.y);
				glPrintf("Start");

			}
			if (IS_REC_END && !IS_KEY_F1_UP)
			{
				glPrintf(">>glut(%i, %i)", End.x, End.y);
				glPrintf("End");
			}
		}
		else
		{
			if (IS_REC_START)
			{
				glColor3ub(255, 255, 0);
				glPrintf(">>glut(%i, %i)", Start.x - 70, Start.y - 35);
				glPrintf("Start");

			}
			if (IS_REC_END && !IS_KEY_F1_UP)
			{
				glPrintf(">>glut(%i, %i)", End.x - 70, End.y - 35);
				glPrintf("End");
			}
		}

	}

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

	//DrawPath();

	glPopMatrix();



	if (ROI_IS_FRONT)
	{
		glPushMatrix();

		glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x - 60, ROICenterColorS_Old.y - 35);
		glColor3ub(0, 255, 0);
		glPrintf("(%.4f, %.4f, %.4f)", ROICenterCameraS.x, ROICenterCameraS.y, ROICenterCameraS.z);
		glPopMatrix();

	}


	/*-----------------------------------------------------*/
	/*--------------OpenHaptics Force Generate-------------*/
	/*-----------------------------------------------------*/
	if (IS_HL_FORCE_APPLY)
	{
		hlBeginFrame();

		HDdouble EETRans[16];
		HDdouble anchor[3];

		hlGetDoublev(HL_PROXY_TRANSFORM, EETRans);
		hlGetDoublev(HL_PROXY_POSITION, anchor);

		//cout << EETRans[0] << " " << EETRans[4] << " " << EETRans[8] << " " << EETRans[12] << endl;
		//cout << EETRans[1] << " " << EETRans[5] << " " << EETRans[9] << " " << EETRans[13] << endl;
		//cout << EETRans[2] << " " << EETRans[6] << " " << EETRans[10] << " " << EETRans[14] << endl;
		//cout << EETRans[3] << " " << EETRans[7] << " " << EETRans[11] << " " << EETRans[15] << endl << endl;

		//cout << anchor[0] << " " << anchor[1] << " " << anchor[2] << " " << anchor[3] << endl << endl;

		if (ROI_IS_COLLIDE == TRUE)
		{
			if (IS_HL_FORCE_GEN == FALSE)
			{
				HDdouble anchor[3];

				spring = hlGenEffects(1);
				hlGetDoublev(HL_PROXY_POSITION, anchor);

				hlEffectd(HL_EFFECT_PROPERTY_GAIN, gGain);
				hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
				hlEffectdv(HL_EFFECT_PROPERTY_POSITION, anchor);
				hlStartEffect(HL_EFFECT_SPRING/*HL_EFFECT_VISCOUS*/, spring);

				IS_HL_FORCE_GEN = TRUE;
			}

		}
		else
		{
			if (IS_HL_FORCE_GEN)
			{
				if (gMagnitude > 0)
				{
					gMagnitude -= 0.3;
					hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
					hlUpdateEffect(spring);
				}
				else
				{
					gMagnitude = 0.8f;

					hlStopEffect(spring);
					IS_HL_FORCE_GEN = FALSE;
				}
			}


		}

		hlEndFrame();

	}
	else
	{
		if (IS_HL_FORCE_GEN)
		{
			hlBeginFrame();

			if (gMagnitude > 0)
			{
				gMagnitude -= 0.3;
				hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
				hlUpdateEffect(spring);
			}
			else
			{
				gMagnitude = 0.8f;

				hlStopEffect(spring);
				IS_HL_FORCE_GEN = FALSE;
			}

			hlEndFrame();
		}
	}


	/*--------------Draw Solid Tea Pot-------------*/
	//glPushMatrix();
	//glTranslatef(PORCenterCameraS.x, PORCenterCameraS.y, PORCenterCameraS.z);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glutSolidSphere(0.01, 100, 100);
	//glPopMatrix();

	ROI_IS_COLLIDE = FALSE;
	ROI_IS_FRONT = FALSE;

	//FPS_RS++;
	//glutSwapBuffers();
	//glFinish();

	if (Finish_Without_Update)
		glFinish();
	else
		glutSwapBuffers();

	FPS_RS++;
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

	 CubicPosi.x =  0.0f;
	 CubicPosi.y = -0.11f;
	 CubicPosi.z = -0.68f;

	 CubicRota.x = -86.0f;
	 CubicRota.y = 72.0f;
	 CubicRota.z = 19.0f;
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
	Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);	
	
	//blur(mDepthImg, mDepthImg, Size(3, 3));
	//GaussianBlur(mDepthImg, mDepthImg, Size(3, 3), 0, 0);
	//pBufferDepth = reinterpret_cast<UINT16*>(mDepthImg.data);
	//Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);

	/*--------------Call Window Function-------------*/
	ShowImage();
}

void ShowImage()
{

	namedWindow("Color Map");

	/*--------------Depth Image-------------*/
	//namedWindow("Depth Map");
	//// Convert from 16bit to 8bit
	//mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);
	//cv::imshow("Depth Map", mImg8bit);

	/*--------------Set Mouse Callback Function and Find ROI-------------*/
	cvSetMouseCallback("Color Map", onMouseROI, NULL);

	//if (ROI.data != NULL)
	//{
	//	ROI.release();
	//}

	mColorImg.copyTo(ROI);

	if (ROI_S2 == TRUE && ROI_S1 == FALSE)
	{
		int thickness = 2;
		rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), thickness);
		FindROI();
	}

	if (LR_S2[0] == TRUE && LR_S1[0] == FALSE)
	{
		int thickness = 2;
		rectangle(ROI, LR_p1[0], LR_p2[0], Scalar(232, 117, 0), thickness);

		stringstream ss;
		ss << 1; //把int型態變數寫入到stringstream

		string text2;
		ss >> text2;

		String text1 = "Line " + text2;

		putText(ROI, text1, Point(LR_p1[0].x, LR_p1[0].y - 5), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(232, 117, 0));

		FindLR();

	}

	if (LR_S2[1] == TRUE && LR_S1[1] == FALSE)
	{
		int thickness = 2;
		rectangle(ROI, LR_p1[1], LR_p2[1], Scalar(232, 117, 0), thickness);

		stringstream ss;
		ss << 2; //把int型態變數寫入到stringstream

		string text2;
		ss >> text2;

		String text1 = "Line " + text2;

		putText(ROI, text1, Point(LR_p1[1].x, LR_p1[1].y - 5), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(232, 117, 0));

		FindLR();

	}

	//if (LRPixelCount[0] > 0)
	//{
	//	for (int i = 0; i < LRPixelCount[0]; i++)
	//	{
	//		int Px = LRPixel_Select_L1[i].x;
	//		int Py = LRPixel_Select_L1[i].y;
	//		circle(ROI, Point(Px, Py), 10, Scalar(0, 0, 255), -1);
	//
	//	}
	//}
	//if (LRPixelCount[1] > 0)
	//{
	//	for (int i = 0; i < LRPixelCount[1]; i++)
	//	{
	//		int Px = LRPixel_Select_L2[i].x;
	//		int Py = LRPixel_Select_L2[i].y;
	//		circle(ROI, Point(Px, Py), 10, Scalar(0, 0, 255), -1);
	//		
	//	}
	//}

	cv::imshow("Color Map", ROI);


}

void onMouseROI(int event, int x, int y, int flags, void* param)
{
	int thickness = 2;

	//Push
	if (event == CV_EVENT_LBUTTONDOWN)
	{ 
		ROI_rect.x = x;
		ROI_rect.y = y;

		ROI_S1 = TRUE;
		ROI_S2 = FALSE;

	}
	else if (event == CV_EVENT_RBUTTONDOWN)
	{
		int LINE_FLAG = -1;

		switch (INIT_STEP)
		{
		case SELECT_LINE1:

			LINE_FLAG = 0;
			//if (LRPixel_Select_L1 != nullptr)
			//{
			//	delete [] LRPixel_Select_L1;
			//	LRPixel_Select_L1 = nullptr;
			//	LRPixelCount[0] = 0;
			//}
			//LRPixel_Select_L1 = new Point2i[LR_StorageSize];
			break;

		case SELECT_LINE2:

			LINE_FLAG = 1;
			//if (LRPixel_Select_L2 != nullptr)
			//{
			//	delete[] LRPixel_Select_L2;
			//	LRPixel_Select_L2 = nullptr;
			//	LRPixelCount[1] = 0;
			//}
			//LRPixel_Select_L2 = new Point2i[LR_StorageSize];
			break;
		}

		LR_rect[LINE_FLAG].x = x;
		LR_rect[LINE_FLAG].y = y;

		LR_S1[LINE_FLAG] = TRUE;
		LR_S2[LINE_FLAG] = FALSE;

		IS_LR_SELECT[LINE_FLAG] = FALSE;


	}

	//Release
	else if (event == CV_EVENT_LBUTTONUP)
	{
		ROI_S1 = FALSE;

		ROI_rect.height = y - ROI_rect.y;
		ROI_rect.width = x - ROI_rect.x;

		ROI_p2 = Point(x, y);

	}
	else if (event == CV_EVENT_RBUTTONUP)
	{
		int LINE_FLAG = -1;

		switch (INIT_STEP)
		{
		case SELECT_LINE1:

			LINE_FLAG = 0;
			INIT_STEP = SELECT_LINE2;
			break;

		case SELECT_LINE2:

			LINE_FLAG = 1;
			INIT_STEP = SELECT_LINE1;

			break;
		}

		LR_S1[LINE_FLAG] = FALSE;

		LR_rect[LINE_FLAG].height = y - LR_rect[LINE_FLAG].y;
		LR_rect[LINE_FLAG].width = x - LR_rect[LINE_FLAG].x;

		LR_p2[LINE_FLAG] = Point(x, y);

		IS_LR_SELECT[LINE_FLAG] = TRUE;

		//if (IS_LR_SELECT[0] + IS_LR_SELECT[1] == 2)
		//{
		//	CameraSpacePOR();
		//}

		//cout << endl << INIT_STEP << endl;
	}

	//if (ROI.data != NULL)
	//{
	//	ROI.release();
	//}

	mColorImg.copyTo(ROI);

	//Drag
	if (flags == CV_EVENT_FLAG_LBUTTON)
	{
		ROI_S2 = TRUE;

		ROI_p1 = Point(ROI_rect.x, ROI_rect.y);
		ROI_p2 = Point(x, y);

		rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), 2);
		cv::imshow("Color Map", ROI);

	}
	else if (flags == CV_EVENT_FLAG_RBUTTON)
	{
		int LINE_FLAG = -1;

		switch (INIT_STEP)
		{
		case SELECT_LINE1:

			LINE_FLAG = 0;
			//if (LRPixelCount[0] < LR_StorageSize)
			//{
			//	LRPixel_Select_L1[LRPixelCount[0]].x = x;
			//	LRPixel_Select_L1[LRPixelCount[0]].y = y;
			//
			//	//很慢的方式，但是改Mat裡的RGB值更慢
			//	circle(ROI, Point(LRPixel_Select_L1[LRPixelCount[0]].x, LRPixel_Select_L1[LRPixelCount[0]].y), 10, Scalar(0, 0, 255), -1);
			//	LRPixelCount[0]++;
			//	cout << LRPixelCount[0] << endl;
			//}
			break;

		case SELECT_LINE2:

			LINE_FLAG = 1;
			//if (LRPixelCount[1] < LR_StorageSize)
			//{
			//	LRPixel_Select_L2[LRPixelCount[1]].x = x;
			//	LRPixel_Select_L2[LRPixelCount[1]].y = y;
			//
			//	//很慢的方式，但是改Mat裡的RGB值更慢
			//	circle(ROI, Point(LRPixel_Select_L2[LRPixelCount[1]].x, LRPixel_Select_L2[LRPixelCount[1]].y), 10, Scalar(0, 0, 255), -1);
			//	LRPixelCount[1]++;
			//}
			break;
		}

		//CvFont* font;
		//cvInitFont(font, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f);

		stringstream ss;
		ss << LINE_FLAG + 1; //把int型態變數寫入到stringstream

		string text2;
		ss >> text2;

		String text1 = "Line " + text2;

		putText(ROI, text1, LR_p1[LINE_FLAG], CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(232, 117, 0));

		LR_S2[LINE_FLAG] = TRUE;

		LR_p1[LINE_FLAG] = Point(LR_rect[LINE_FLAG].x, LR_rect[LINE_FLAG].y);
		LR_p2[LINE_FLAG] = Point(x, y);

		rectangle(ROI, LR_p1[LINE_FLAG], LR_p2[LINE_FLAG], Scalar(232, 117, 0), 2);
		cv::imshow("Color Map", ROI);
	}
}


/*---------------------------------------*/
/*--------------ROI Function-------------*/
/*---------------------------------------*/
void FindROI()
{
	Mat ROI_Image;

	/*--------------Find ROI-------------*/
	if (ROI_p2.x > ROI_p1.x && ROI_p2.y > ROI_p1.y)
	{
		ROI_Image = mColorImg.colRange(ROI_p1.x, ROI_p2.x + 1).rowRange(ROI_p1.y, ROI_p2.y + 1).clone();
	}
	else
	{
		OutputDebugString("你按了左鍵。拉框請從左上到右下，不要亂搞\n");
		return;
	}


	/*--------------BGR to YCrCb-------------*/
	Mat ROI_YCrCb;
	cvtColor(ROI_Image, ROI_YCrCb, CV_BGR2YCrCb);

	/*-------------- Color Detection and Tracking-------------*/
	int channels = ROI_YCrCb.channels();
	int rows = ROI_YCrCb.rows;
	int cols = ROI_YCrCb.cols;
	int steps = ROI_YCrCb.step;

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
			
			if (ROIcount > 999)
			{
				//cout << endl << "--------Array Out of range--------" << endl;
				OutputDebugString("--------Array Out of range---------//-------Array Out of range--------//--------Array Out of range--------\n");
				exit(1);
			}
			
			uchar IsRed = 0;
			OutputValue(ROI_YCrCb, i, j, 1, &IsRed);

			//threshold = 150 for fluorescent pink
			//threshold = 170 for red
			if (IsRed > 160)
			{

				InputValue(ROI, 0, i + ROI_p1.y, j + ROI_p1.x, 0, 255);
				InputValue(ROI, 0, i + ROI_p1.y, j + ROI_p1.x, 1, 0);
				InputValue(ROI, 0, i + ROI_p1.y, j + ROI_p1.x, 2, 0);

				ROIPixel[ROIcount].x = j + ROI_p1.x;
				ROIPixel[ROIcount].y = i + ROI_p1.y;


				ROICenterColorS_New.x += ROIPixel[ROIcount].x;
				ROICenterColorS_New.y += ROIPixel[ROIcount].y;

				ROIcount++;
			}
		}
	}

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


	CameraSpacePoint* Temp = new CameraSpacePoint[ROIDepthCount];
	
	
	int indx1 = 0;
	for (int i = 0; i < ROIcount; i++)
	{
		int indx2 = ROIPixel[i].x + ROIPixel[i].y * iWidthColor;
	
		if (pCSPoints[indx2].Z != -1 * numeric_limits<float>::infinity())
		{
			Temp[indx1].X = pCSPoints[indx2].X;
			Temp[indx1].Y = pCSPoints[indx2].Y;
			Temp[indx1].Z = -pCSPoints[indx2].Z;


			//cout << Temp[indx1].X << " " << Temp[indx1].Y << " " << Temp[indx1].Z << endl;

			indx1++;
	
		}
	}

	/*------------------------------------------------------------------*/
	//Bubble sort
	for (int i = ROIDepthCount - 1; i > 0; --i)
	{
		for (int j = 0; j < i; ++j)
		{
			if (Temp[j].Z < Temp[j + 1].Z)
			{
				CameraSpacePoint temp;

				temp.X = Temp[j].X;
				temp.Y = Temp[j].Y;
				temp.Z = Temp[j].Z;

				Temp[j].X = Temp[j + 1].X;
				Temp[j].Y = Temp[j + 1].Y;
				Temp[j].Z = Temp[j + 1].Z;

				Temp[j + 1].X = temp.X;
				Temp[j + 1].Y = temp.Y;
				Temp[j + 1].Z = temp.Z;

			}

		}
	}

	//Settint threshold and filtering
	int IndexLim = 0;

	for (int i = 0; i < ROIDepthCount; i++)
	{
		if (Temp[i].Z < Temp[0].Z*1.02)
		{
			IndexLim = i;
			break;
		}
	}

	//Mismatch did not occur
	if (IndexLim == 0)
	{			
		ROICameraSP = new CameraSpacePoint[ROIDepthCount];
		for (int i = 0; i < ROIDepthCount; i++)
		{
			ROICameraSP[i].X = Temp[i].X;
			ROICameraSP[i].Y = Temp[i].Y;
			ROICameraSP[i].Z = Temp[i].Z;

			ROICenterCameraS.x += ROICameraSP[i].X;
			ROICenterCameraS.y += ROICameraSP[i].Y;
			ROICenterCameraS.z += ROICameraSP[i].Z;
		}

	}
	//Mismatch occur
	else
	{
		ROICameraSP = new CameraSpacePoint[IndexLim];
		for (int i = 0; i < IndexLim; i++)
		{
			ROICameraSP[i].X = Temp[i].X;
			ROICameraSP[i].Y = Temp[i].Y;
			ROICameraSP[i].Z = Temp[i].Z;

			ROICenterCameraS.x += ROICameraSP[i].X;
			ROICenterCameraS.y += ROICameraSP[i].Y;
			ROICenterCameraS.z += ROICameraSP[i].Z;
		}
		
		ROIDepthCount = IndexLim;

	}

	delete [] Temp;

	/*------------------------------------------------------------------*/



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
	if (ROICameraSP_Storage == nullptr)
	{
		ROICameraSP_Storage = new CameraSpacePoint[StorageSize];
	}

	

	if (IS_KEY_F1_UP && ROIDepthCount > 0)
	{

		if (ROIStorageCount + ROIDepthCount < StorageSize)
		{

			for (int i = 0; i < ROIDepthCount; i++)
			{

				ROICameraSP_Storage[i + ROIStorageCount] = ROICameraSP[i];
			}

			ROIStorageCount += ROIDepthCount;

		}
		else
		{
			cout << endl << "The array ROICameraSP_Storage is full, press F2 to clear" << endl;
		}
	}

}



/*-----------------------------------------------------------------*/
/*------------------Linear Regression Function-----------------*/
/*-----------------------------------------------------------------*/
void FindLR()
{
	int total = IS_LR_SELECT[0] + IS_LR_SELECT[1];
	int SET_iter = 0;
	int SET_target = -1;

	switch (total)
	{
		//L1, L2 其中一個好了
	case 1:
		SET_target = 0 * IS_LR_SELECT[0] + 1 * IS_LR_SELECT[1];
		SET_iter = 1;
		break;

		//L1, L2 兩個好了
	case 2:
		SET_target = 0;
		SET_iter = 2;
		break;
	}

	int iter = 0;
	int target = SET_target;


	while (iter < SET_iter)
	{

		/*--------------Find ROI-------------*/
		Mat LR_Image;

		if (LR_p2[target].x > LR_p1[target].x && LR_p2[target].y > LR_p1[target].y)
		{
			LR_Image = mColorImg.colRange(LR_p1[target].x, LR_p2[target].x + 1).rowRange(LR_p1[target].y, LR_p2[target].y + 1).clone();
		}
		else
		{
			OutputDebugString("你按了右鍵。拉框請從左上到右下，不要亂搞\n");
			return;
		}

		/*--------------BGR to YCrCb-------------*/
		Mat LR_YCrCb;
		cvtColor(LR_Image, LR_YCrCb, CV_BGR2YCrCb);

		/*-------------- Color Detection and Tracking-------------*/
		int channels = LR_YCrCb.channels();
		int rows = LR_YCrCb.rows;
		int cols = LR_YCrCb.cols;
		int steps = LR_YCrCb.step;
		
		LRPixelCount[target] = 0;
		LRDepthCount[target] = 0;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{

				if (LRPixelCount[target] > MAX_PIXEL_COUNT_LR)
				{
					OutputDebugString("--------Array Out of range---------//-------Array Out of range--------//--------Array Out of range--------\n");
					exit(1);
				}

				uchar IsBlue = 0;

				OutputValue(LR_YCrCb, i, j, 2, &IsBlue);

				//threshold < 145
				if (IsBlue > 130)
				{



					InputValue(ROI, 0, i + LR_p1[target].y, j + LR_p1[target].x, 0, 255);
					InputValue(ROI, 0, i + LR_p1[target].y, j + LR_p1[target].x, 1, 0);
					InputValue(ROI, 0, i + LR_p1[target].y, j + LR_p1[target].x, 2, 0);

					LRPixel[target][LRPixelCount[target]].x = j + LR_p1[target].x;
					LRPixel[target][LRPixelCount[target]].y = i + LR_p1[target].y;

					LRPixelCount[target]++;

				}
			}
		}


		iter += 1;
		target += 1;
	}

	if (LRPixelCount[0] > 0 || LRPixelCount[1] > 0)
	{
		CameraSpaceLR();
	}

}

void CameraSpaceLR()
{
	int total = IS_LR_SELECT[0] + IS_LR_SELECT[1];
	int SET_iter = 0;
	int SET_target = -1;

	switch (total)
	{
		//L1, L2 其中一個好了
	case 1:
		SET_target = 0 * IS_LR_SELECT[0] + 1 * IS_LR_SELECT[1];
		SET_iter = 1;
		break;

		//L1, L2 兩個好了
	case 2:
		SET_target = 0;
		SET_iter = 2;
		break;
	}

	int iter = 0;
	int target = SET_target;


	while (iter < SET_iter)
	{
		LRDepthCount[target] = 0;

		for (int i = 0; i < LRPixelCount[target]; i++)
		{
			int index1 = LRPixel[target][i].x + LRPixel[target][i].y * iWidthColor;
			if (pCSPoints[index1].Z != -1 * numeric_limits<float>::infinity() && LRDepthCount[target] < MAX_DEPTH_COUNT_LR)
			{
				LRCameraSP[target][LRDepthCount[target]].X = pCSPoints[index1].X;
				LRCameraSP[target][LRDepthCount[target]].Y = pCSPoints[index1].Y;
				LRCameraSP[target][LRDepthCount[target]].Z = -pCSPoints[index1].Z;

				LRDepthCount[target]++;

			}
		}

		//cout << LRDepthCount[target] << endl;

		////Bubble sort
		//for (int i = LRDepthCount[target] - 1; i > 0; --i)
		//{
		//	for (int j = 0; j < i; ++j)
		//	{
		//		if (LRCameraSP[target][j].Z < LRCameraSP[target][j + 1].Z)
		//		{
		//			CameraSpacePoint temp;
		//
		//			temp.X = LRCameraSP[target][j].X;
		//			temp.Y = LRCameraSP[target][j].Y;
		//			temp.Z = LRCameraSP[target][j].Z;
		//
		//			LRCameraSP[target][j].X = LRCameraSP[target][j + 1].X;
		//			LRCameraSP[target][j].Y = LRCameraSP[target][j + 1].Y;
		//			LRCameraSP[target][j].Z = LRCameraSP[target][j + 1].Z;
		//
		//			LRCameraSP[target][j + 1].X = temp.X;
		//			LRCameraSP[target][j + 1].Y = temp.Y;
		//			LRCameraSP[target][j + 1].Z = temp.Z;
		//		}
		//	}
		//}
		//
		////Settint threshold and filtering
		//int IndexLim = 0;
		//
		//for (int i = 0; i < LRDepthCount[target]; i++)
		//{
		//	if (LRCameraSP[target][i].Z < LRCameraSP[target][0].Z*1.5)
		//	{
		//		IndexLim = i;
		//		break;
		//	}
		//}
		//
		////Mismatch occur
		//if (IndexLim != 0)
		//{
		//	LRDepthCount[target] = IndexLim;
		//
		//	cout << "Yeeee" << endl;
		//}

		//cout << target << ", " << LRDepthCount[target] << endl;

		iter += 1;
		target += 1;
	}

	if (IS_LR_SELECT[1] == TRUE)
	{
		CameraSpacePOR();
	}


}




/*----------------------------------------------------*/
/*--------------Plane of Regression (POR)-------------*/
/*----------------------------------------------------*/
void CameraSpacePOR(void)
{
	int total = IS_LR_SELECT[0] + IS_LR_SELECT[1];
	
	if (total == 2)
	{
		PORDepthCount = 0;

		for (int target = 0; target < 2; target++)
		{
			Mat LR_Image;

			if (LR_p2[target].x > LR_p1[target].x && LR_p2[target].y > LR_p1[target].y)
			{
				LR_Image = mColorImg.colRange(LR_p1[target].x, LR_p2[target].x + 1).rowRange(LR_p1[target].y, LR_p2[target].y + 1).clone();
			}
			else
			{
				OutputDebugString("你按了右鍵。拉框請從左上到右下，不要亂搞\n");
				return;
			}

			int one_iter_count = 0;
			for (int i = 0; i < LR_Image.rows; i++)
			{
				for (int j = 0; j < LR_Image.cols; j++)
				{

					int index1 = (j + LR_p1[target].x) + (i + LR_p1[target].y) * iWidthColor;

					if (pCSPoints[index1].Z != -1 * numeric_limits<float>::infinity() && one_iter_count < MAX_DEPTH_COUNT_POR / 2)
					{
						PORCameraSP[PORDepthCount].X = pCSPoints[index1].X;
						PORCameraSP[PORDepthCount].Y = pCSPoints[index1].Y;
						PORCameraSP[PORDepthCount].Z = -pCSPoints[index1].Z;

						PORCenterCameraS.x += PORCameraSP[PORDepthCount].X;
						PORCenterCameraS.y += PORCameraSP[PORDepthCount].Y;
						PORCenterCameraS.z += PORCameraSP[PORDepthCount].Z;
						

						//cout << PORCameraSP[PORDepthCount].Z << endl;

						PORDepthCount++;

						one_iter_count++;
						
					}

				}
			}

			//cout << PORDepthCount << endl;

		}


		if (PORDepthCount > 0)
		{
			PORCenterCameraS.x = PORCenterCameraS.x / PORDepthCount;
			PORCenterCameraS.y = PORCenterCameraS.y / PORDepthCount;
			PORCenterCameraS.z = PORCenterCameraS.z / PORDepthCount;

			PlaceObject();

		}
		else if (PORDepthCount == 0)
		{
			PORCenterCameraS.x = 0;
			PORCenterCameraS.y = 0;
			PORCenterCameraS.z = 0;
		}


		//for (int i = PORDepthCount - 1; i > 0; --i)
		//{
		//	for (int j = 0; j < i; ++j)
		//	{
		//		if (PORCameraSP[j].Z < PORCameraSP[j + 1].Z)
		//		{
		//			CameraSpacePoint temp;
		//
		//			temp.X = PORCameraSP[j].X;
		//			temp.Y = PORCameraSP[j].Y;
		//			temp.Z = PORCameraSP[j].Z;
		//
		//			PORCameraSP[j].X = PORCameraSP[j + 1].X;
		//			PORCameraSP[j].Y = PORCameraSP[j + 1].Y;
		//			PORCameraSP[j].Z = PORCameraSP[j + 1].Z;
		//
		//			PORCameraSP[j + 1].X = temp.X;
		//			PORCameraSP[j + 1].Y = temp.Y;
		//			PORCameraSP[j + 1].Z = temp.Z;
		//		}
		//	}
		//}
		//
		////Settint threshold and filtering
		//int IndexLim = 0;
		//
		//for (int i = 0; i < PORDepthCount; i++)
		//{
		//	if (PORCameraSP[i].Z < PORCameraSP[0].Z*1.3)
		//	{
		//		IndexLim = i;
		//		break;
		//	}
		//}
		//
		////Mismatch occur
		//if (IndexLim != 0)
		//{
		//	PORDepthCount = IndexLim;
		//
		//	cout << "Yeeee" << endl;
		//}
	}

	


}
void PlaceObject()
{
	if (PORDepthCount > 0 && LRDepthCount[1] > 0)
	{
		FindPlaneOfRegression(PORCameraSP, PORDepthCount, PORNormal);

		PathGenProjToPlane(LRCameraSP[0], LRDepthCount[0], PORNormal, LRCameraSP[0]);
		PathGenProjToPlane(LRCameraSP[1], LRDepthCount[1], PORNormal, LRCameraSP[1]);

		//dot(Y-axis-GL, PORNormal)
		float theta = acos(PORNormal[1] / sqrt(pow(PORNormal[0], 2) + pow(PORNormal[1], 2) + pow(PORNormal[2], 2)));

		//cross(Y-axis-GL, PORNormal)
		float ArbRotAxis[3] = { -PORNormal[2], 0, PORNormal[0] };

		RotationAboutArbitraryAxes(theta, ArbRotAxis, LRCameraSP[0], LRDepthCount[0]);
		RotationAboutArbitraryAxes(theta, ArbRotAxis, LRCameraSP[1], LRDepthCount[1]);

		//for (int i = 0; i < LRDepthCount[0]; i++)
		//{
		//	cout << LRCameraSP[0][i].Y << endl;
		//}

		//double RegrL1[3] = { 2, 0, 0 };
		PathGenRegression(LRCameraSP[0], LRDepthCount[0], RegrL1);

		//double RegrL2[3] = { 2, 0, 0 };
		PathGenRegression(LRCameraSP[1], LRDepthCount[1], RegrL2);



		Intersect.X = (RegrL1[2] - RegrL2[2]) / (RegrL2[1] - RegrL1[1]);
		Intersect.Z = RegrL1[1] * Intersect.X + RegrL1[2];
		Intersect.Y = /*-(PORNormal[0] * Intersect.X + PORNormal[2] * Intersect.Z + PORNormal[3]) / PORNormal[1]*/PORCenterCameraS.y;



		RotationAboutArbitraryAxes(-theta, ArbRotAxis, &Intersect, 1);

		RotationAboutArbitraryAxes(-theta, ArbRotAxis, LRCameraSP[0], LRDepthCount[0]);
		RotationAboutArbitraryAxes(-theta, ArbRotAxis, LRCameraSP[1], LRDepthCount[1]);

		RotationAboutArbitraryAxes(theta, ArbRotAxis, M_Cubic);

		//M_Cubic[0] = 0;
		//M_Cubic[1] = 0;
		//M_Cubic[2] = 1;
		//M_Cubic[3] = 0;

		//M_Cubic[4] = 0;
		//M_Cubic[5] = 1;
		//M_Cubic[6] = 0;
		//M_Cubic[7] = 0;

		//M_Cubic[8] = -1;
		//M_Cubic[9] = 0;
		//M_Cubic[10] = 0;
		//M_Cubic[11] = 0;

		//M_Cubic[12] = 0;
		//M_Cubic[13] = 0;
		//M_Cubic[14] = 0;
		//M_Cubic[15] = 1;

		//PathGenProjToPlane(LRCameraSP[0], LRDepthCount[0], PORNormal, LRCameraSP[0]);
		//PathGenProjToPlane(LRCameraSP[1], LRDepthCount[1], PORNormal, LRCameraSP[1]);
		//
		//Mat X_axis = Mat::zeros(3, 1, CV_32FC1);
		//Mat Y_axis = Mat::zeros(3, 1, CV_32FC1);
		//Mat Z_axis = Mat::zeros(3, 1, CV_32FC1);
		//
		//Y_axis.at<float>(0, 0) = PORNormal[0];
		//Y_axis.at<float>(1, 0) = PORNormal[1];
		//Y_axis.at<float>(2, 0) = PORNormal[2];
		//
		//
		//Z_axis.at<float>(0, 0) = LRCameraSP[0][100].X - LRCameraSP[0][200].X;
		//Z_axis.at<float>(1, 0) = LRCameraSP[0][100].Y - LRCameraSP[0][200].Y;
		//Z_axis.at<float>(2, 0) = LRCameraSP[0][100].Z - LRCameraSP[0][200].Z;
		//
		//X_axis.at<float>(0, 0) = (Y_axis.cross(Z_axis)).at<float>(0, 0);
		//X_axis.at<float>(1, 0) = (Y_axis.cross(Z_axis)).at<float>(1, 0);
		//X_axis.at<float>(2, 0) = (Y_axis.cross(Z_axis)).at<float>(2, 0);
		//
		//
		//X_axis = X_axis.mul(1.0f / norm(X_axis));
		//Y_axis = Y_axis.mul(1.0f / norm(Y_axis));
		//Z_axis = Z_axis.mul(1.0f / norm(Z_axis));
		//
		//float thetaZ = acos(Y_axis.at<float>(2, 0) / sqrt(1 - pow(Z_axis.at<float>(2, 0), 2)));
		//float thetaX = acos(Z_axis.at<float>(2, 0));
		//float thetaZ2 = -acos(Z_axis.at<float>(1, 0) / sqrt(1 - pow(Z_axis.at<float>(2, 0), 2)));
		//
		//
		//GLfloat m[16] = { 0 };
		//m[0] = m[5] = m[10] = m[15] = 1;
		//
		//glMatrixMode(GL_MODELVIEW);
		//glPushMatrix();
		//glLoadIdentity();
		//
		//glRotatef(thetaZ * 180 / 3.141592653589793, 0, 0, 1);
		//glRotatef(thetaX * 180 / 3.141592653589793, 1, 0, 0);
		//glRotatef(thetaZ2 * 180 / 3.141592653589793, 0, 0, 1);
		//
		//
		//glMultMatrixf(m);
		//
		//glGetFloatv(GL_MODELVIEW_MATRIX, m);
		//glPopMatrix();
		//
		//m[0] *= (-1);
		//m [1] *= (-1);
		//m[4] *= (-1);
		//m[5] *= (-1);
		//m[8] *= (-1);
		//m[9] *= (-1);
		//
		//
		//Mat MM = Mat::zeros(4, 4, CV_32FC1);
		//float* srcData_MM = (float*)MM.data;
		//for (int i = 0; i < 4; i++)
		//{
		//	*(srcData_MM + 4 * i + 0) = m[4 * i + 0];
		//	*(srcData_MM + 4 * i + 1) = m[4 * i + 1];
		//	*(srcData_MM + 4 * i + 2) = m[4 * i + 2];
		//	*(srcData_MM + 4 * i + 3) = m[4 * i + 3];
		//}
		//
		//
		//for (int target = 0; target < 2; target++)
		//{
		//
		//
		//	Mat LinePoint = Mat::ones(4, LRDepthCount[target], CV_32FC1);
		//
		//	float* srcData_LinePoint = (float*)LinePoint.data;
		//
		//	for (int i = 0; i < LRDepthCount[target]; i++)
		//	{
		//		*(srcData_LinePoint + 4 * i + 0) = LRCameraSP[target][i].X;
		//		*(srcData_LinePoint + 4 * i + 1) = LRCameraSP[target][i].Y;
		//		*(srcData_LinePoint + 4 * i + 2) = LRCameraSP[target][i].Z;
		//
		//	}
		//
		//	LinePoint = MM.inv()*LinePoint;
		//	cout << LinePoint.at<float>(1, 0) << " " << LinePoint.at<float>(1, 100)/* << " " << LinePoint.at<float>(350, 2)*/ << endl;
		//}



		//fstream file;
		//file.open("Data.txt", ios::out);      //開啟檔案
		//
		//if (!file)     //檢查檔案是否成功開啟
		//
		//{
		//	cerr << "Can't open file!\n";
		//
		//	exit(1);     //在不正常情形下，中斷程式的執行
		//}
		//
		//file << "RegrL1:" << endl;
		//file << RegrL1[1] << " " << RegrL1[2] << endl;
		//
		//file << "RegrL2:" << endl;
		//file << RegrL2[1] << " " << RegrL2[2] << endl;

		//file << "Intersect" << endl;
		//file << Intersect.X << " " << Intersect.Y << " " << Intersect.Z << endl;

		//file.close();





		//cout << LRCameraSP[0][0].X << " " << LRCameraSP[0][0].Y << " " << LRCameraSP[0][0].Z << endl;



		//PathGenProjToPlane(LRCameraSP[0], LRDepthCount[0], PORNormal, LRCameraSP[0]);

		//cout << LRCameraSP[0][0].X << " " << LRCameraSP[0][0].Y << " " << LRCameraSP[0][0].Z << endl;

		//PathGenProjToPlane(LRCameraSP[1], LRDepthCount[0], PORNormal, LRCameraSP[1]);


		//TransM = [Rx, Ry, Rz, Tx, Ty, Tz] in order
		//void PathGenCoordWorldToCubic(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result);

	}

}

void InputValue(Mat M, int DataType, int Row, int Col, int Chan, float Data)
{
	int Steps = M.cols*M.channels();
	int Channels = M.channels();

	if (DataType == 0)
	{
		uchar* srcData = M.data;

		*(srcData + Row*Steps + Col*Channels + Chan) = Data;
	}
	else if (DataType == 1)
	{
		float* srcData= (float*)M.data;

		*(srcData + Row*Steps + Col*Channels + Chan) = Data;
	}
}
void OutputValue(Mat M, int Row, int Col, int Chan, uchar* Data)
{
	int Steps = M.cols*M.channels();
	int Channels = M.channels();

	uchar* srcData = M.data;

	*Data = *(srcData + Row*Steps + Col*Channels + Chan);
}
void OutputValue(Mat M, int Row, int Col, int Chan, float* Data)
{
	int Steps = M.cols*M.channels();
	int Channels = M.channels();

	float* srcData = (float*)M.data;

	*Data = *(srcData + Row*Steps + Col*Channels + Chan);
}

void RotationAboutArbitraryAxes(float theta, float* axis, CameraSpacePoint* Data, int Size)
{
	float a = axis[0], b = axis[1], c = axis[2], d = sqrt(pow(axis[1], 2) + pow(axis[2], 2)), l = pow(axis[0], 2) + pow(axis[1], 2) + pow(axis[2], 2);

	Mat Tran_1_TR = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_2_Rot_x = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_3_Rot_y = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_4_Rot_z = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_5_Rot_y_inv = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_6_Rot_x_inv = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_7_TR_inv = Mat::eye(4, 4, CV_32FC1);

	Mat Vertices = Mat::ones(Size, 4, CV_32FC1);

	InputValue(Tran_1_TR, 1, 3, 0, 0, -PORCenterCameraS.x);
	InputValue(Tran_1_TR, 1, 3, 1, 0, -PORCenterCameraS.y);
	InputValue(Tran_1_TR, 1, 3, 2, 0, -PORCenterCameraS.z);

	InputValue(Tran_2_Rot_x, 1, 1, 1, 0, c / d);
	InputValue(Tran_2_Rot_x, 1, 1, 2, 0, b / d);
	InputValue(Tran_2_Rot_x, 1, 2, 1, 0, -b / d);
	InputValue(Tran_2_Rot_x, 1, 2, 2, 0, c / d);

	InputValue(Tran_3_Rot_y, 1, 0, 0, 0, d / l);
	InputValue(Tran_3_Rot_y, 1, 0, 2, 0, a / l);
	InputValue(Tran_3_Rot_y, 1, 2, 0, 0, -a / l);
	InputValue(Tran_3_Rot_y, 1, 2, 2, 0, d / l);

	InputValue(Tran_4_Rot_z, 1, 0, 0, 0, cos(theta));
	InputValue(Tran_4_Rot_z, 1, 0, 1, 0, sin(theta));
	InputValue(Tran_4_Rot_z, 1, 1, 0, 0, -sin(theta));
	InputValue(Tran_4_Rot_z, 1, 1, 1, 0, cos(theta));

	InputValue(Tran_5_Rot_y_inv, 1, 0, 0, 0, d / l);
	InputValue(Tran_5_Rot_y_inv, 1, 0, 2, 0, -a / l);
	InputValue(Tran_5_Rot_y_inv, 1, 2, 0, 0, a / l);
	InputValue(Tran_5_Rot_y_inv, 1, 2, 2, 0, d / l);

	InputValue(Tran_6_Rot_x_inv, 1, 1, 1, 0, c / d);
	InputValue(Tran_6_Rot_x_inv, 1, 1, 2, 0, -b / d);
	InputValue(Tran_6_Rot_x_inv, 1, 2, 1, 0, b / d);
	InputValue(Tran_6_Rot_x_inv, 1, 2, 2, 0, c / d);

	InputValue(Tran_7_TR_inv, 1, 3, 0, 0, PORCenterCameraS.x);
	InputValue(Tran_7_TR_inv, 1, 3, 1, 0, PORCenterCameraS.y);
	InputValue(Tran_7_TR_inv, 1, 3, 2, 0, PORCenterCameraS.z);

	

	for (int i = 0; i < Size; i++)
	{
		InputValue(Vertices, 1, i, 0, 0, Data[i].X);
		InputValue(Vertices, 1, i, 1, 0, Data[i].Y);
		InputValue(Vertices, 1, i, 2, 0, Data[i].Z);

	}

	Mat temp = Vertices*Tran_1_TR*Tran_2_Rot_x*Tran_3_Rot_y*Tran_4_Rot_z*Tran_5_Rot_y_inv*Tran_6_Rot_x_inv*Tran_7_TR_inv;

	for (int i = 0; i < Size; i++)
	{
		OutputValue(temp, i, 0, 0, &Data[i].X);
		OutputValue(temp, i, 1, 0, &Data[i].Y);
		OutputValue(temp, i, 2, 0, &Data[i].Z);
		
		//cout << Data[i].X << " " << Data[i].Y << " " << Data[i].Z << endl;
	}

	

	//Mat temp2 = Tran_1_TR*Tran_2_Rot_x*Tran_3_Rot_y*Tran_4_Rot_z*Tran_5_Rot_y_inv*Tran_6_Rot_x_inv*Tran_7_TR_inv;
	//fstream file;
	//file.open("Data.txt", ios::out);      //開啟檔案
	//if (!file)     //檢查檔案是否成功開啟
	//
	//{
	//	cerr << "Can't open file!\n";
	//	exit(1);     //在不正常情形下，中斷程式的執行
	//}
	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		file << temp2.at<float>(i, j) << endl;
	//	}
	//}
	//file.close();


}

void RotationAboutArbitraryAxes(float theta, float* axis, float* M)
{
	float a = axis[0], b = axis[1], c = axis[2], d = sqrt(pow(axis[1], 2) + pow(axis[2], 2)), l = pow(axis[0], 2) + pow(axis[1], 2) + pow(axis[2], 2);

	Mat Tran_1_TR = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_2_Rot_x = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_3_Rot_y = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_4_Rot_z = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_5_Rot_y_inv = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_6_Rot_x_inv = Mat::eye(4, 4, CV_32FC1);
	Mat Tran_7_TR_inv = Mat::eye(4, 4, CV_32FC1);


	InputValue(Tran_1_TR, 1, 3, 0, 0, -PORCenterCameraS.x);
	InputValue(Tran_1_TR, 1, 3, 1, 0, -PORCenterCameraS.y);
	InputValue(Tran_1_TR, 1, 3, 2, 0, -PORCenterCameraS.z);

	InputValue(Tran_2_Rot_x, 1, 1, 1, 0, c / d);
	InputValue(Tran_2_Rot_x, 1, 1, 2, 0, b / d);
	InputValue(Tran_2_Rot_x, 1, 2, 1, 0, -b / d);
	InputValue(Tran_2_Rot_x, 1, 2, 2, 0, c / d);

	InputValue(Tran_3_Rot_y, 1, 0, 0, 0, d / l);
	InputValue(Tran_3_Rot_y, 1, 0, 2, 0, a / l);
	InputValue(Tran_3_Rot_y, 1, 2, 0, 0, -a / l);
	InputValue(Tran_3_Rot_y, 1, 2, 2, 0, d / l);

	InputValue(Tran_4_Rot_z, 1, 0, 0, 0, cos(theta));
	InputValue(Tran_4_Rot_z, 1, 0, 1, 0, sin(theta));
	InputValue(Tran_4_Rot_z, 1, 1, 0, 0, -sin(theta));
	InputValue(Tran_4_Rot_z, 1, 1, 1, 0, cos(theta));

	InputValue(Tran_5_Rot_y_inv, 1, 0, 0, 0, d / l);
	InputValue(Tran_5_Rot_y_inv, 1, 0, 2, 0, -a / l);
	InputValue(Tran_5_Rot_y_inv, 1, 2, 0, 0, a / l);
	InputValue(Tran_5_Rot_y_inv, 1, 2, 2, 0, d / l);

	InputValue(Tran_6_Rot_x_inv, 1, 1, 1, 0, c / d);
	InputValue(Tran_6_Rot_x_inv, 1, 1, 2, 0, -b / d);
	InputValue(Tran_6_Rot_x_inv, 1, 2, 1, 0, b / d);
	InputValue(Tran_6_Rot_x_inv, 1, 2, 2, 0, c / d);

	InputValue(Tran_7_TR_inv, 1, 3, 0, 0, PORCenterCameraS.x);
	InputValue(Tran_7_TR_inv, 1, 3, 1, 0, PORCenterCameraS.y);
	InputValue(Tran_7_TR_inv, 1, 3, 2, 0, PORCenterCameraS.z);




	//Mat temp = Mat::eye(4, 4, CV_32FC1); 
	
	//temp = (Tran_1_TR*Tran_2_Rot_x*Tran_3_Rot_y*Tran_4_Rot_z*Tran_5_Rot_y_inv*Tran_6_Rot_x_inv*Tran_7_TR_inv)/*.t()*/;
	Mat temp = (Tran_1_TR*Tran_2_Rot_x*Tran_3_Rot_y*Tran_4_Rot_z*Tran_5_Rot_y_inv*Tran_6_Rot_x_inv*Tran_7_TR_inv).t();


	//   fstream file;
	//file.open("Data.txt", ios::out);      //開啟檔案
	//
	//if (!file)     //檢查檔案是否成功開啟
	//
	//{
	//	cerr << "Can't open file!\n";
	//
	//	exit(1);     //在不正常情形下，中斷程式的執行
	//}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			OutputValue(temp, i, j, 0, (M + i * 4 + j));

			//M[i * 4 + j] = temp.at<float>(i, j);

			//float meoww[1] = { 0 };

			//OutputValue(temp.t(), i, j, 1, meoww);

			//file << meoww[0] << endl;
		}
	}


	//file.close();

}

void FindPlaneOfRegression(CameraSpacePoint* Data, int DataNum, double* Result)
{
	//Mat LSQ_33 = Mat::zeros(3, 3, CV_32F);
	//Mat LSQ_31 = Mat::zeros(3, 1, CV_32F);
	//Mat PORCoeff = Mat::zeros(3, 1, CV_32F);
	//
	//for (int i = 0; i < PORDepthCount; i++)
	//{
	//	LSQ_33.at<float>(0, 0) += PORCameraSP[i].X * PORCameraSP[i].X;
	//	LSQ_33.at<float>(0, 1) += PORCameraSP[i].X * PORCameraSP[i].Y;
	//	LSQ_33.at<float>(0, 2) += PORCameraSP[i].X;
	//
	//	LSQ_33.at<float>(1, 1) += PORCameraSP[i].Y * PORCameraSP[i].Y;
	//	LSQ_33.at<float>(1, 2) += PORCameraSP[i].Y;
	//
	//	LSQ_31.at<float>(0, 0) += PORCameraSP[i].X * PORCameraSP[i].Z;
	//	LSQ_31.at<float>(1, 0) += PORCameraSP[i].Y * PORCameraSP[i].Z;
	//	LSQ_31.at<float>(2, 0) += PORCameraSP[i].Z;
	//
	//}
	//
	//LSQ_33.at<float>(2, 2) = PORDepthCount;
	//
	//LSQ_33.at<float>(1, 0) = LSQ_33.at<float>(0, 1);
	//
	//LSQ_33.at<float>(2, 0) = LSQ_33.at<float>(0, 2);
	//LSQ_33.at<float>(2, 1) = LSQ_33.at<float>(1, 2);
	//
	//
	//PORCoeff = LSQ_33.inv()* LSQ_31;
	//
	//float a = PORCoeff.at<float>(0, 0), b = PORCoeff.at<float>(1, 0);
	//
	//cout << PORCoeff.at<float>(0, 0) << " " << PORCoeff.at<float>(1, 0) << " " << PORCoeff.at<float>(2, 0) << endl;
	//
	//PORNormal.x = 1 / sqrt(a*a + b*b + 1) * a;
	//PORNormal.y = 1 / sqrt(a*a + b*b + 1) * b;
	//PORNormal.z = 1 / sqrt(a*a + b*b + 1) * -1;
	//
	//cout << PORNormal.x << " " << PORNormal.y << " " << PORNormal.z << endl;
	//
	//IS_POR = TRUE;


	/**************/
	//待改善：
	//要加上維度變數
	//和 PathGenRegression() 整合
	//使用InputValue()
	/*************/

	Mat XX = Mat::ones(DataNum, 3, CV_32FC1);
	Mat YY = Mat::zeros(DataNum, 1, CV_32FC1);
	Mat BB = Mat::zeros(3, 1, CV_32FC1);


	int nRows = XX.rows; 

	float* srcData_XX = (float*)XX.data;
	float* srcData_YY = (float*)YY.data;

	for (int i = 0; i < nRows; i++)
	{
		*(srcData_XX + 3 * i + 1) = Data[i].X;
		*(srcData_XX + 3 * i + 2) = Data[i].Y;

		*(srcData_YY + i) = Data[i].Z;

	}

	BB = (XX.t()*XX).inv()*XX.t()*YY;

	float* srcData_BB = (float*)BB.data;

	Result[0] = *(srcData_BB + 1);
	Result[1] = *(srcData_BB + 2);
	Result[2] = -1.0f;
	Result[3] = *(srcData_BB + 0);


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

	//spring = hlGenEffects(1);
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

void PathGenCoordWorldToCubic(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result)
{

	for (int i = 0; i < DataNum; i++)
	{
		//cout << endl << Data[i].X << " " << Data[i].Y << " " << Data[i].Z << endl;
		GLfloat m[16] = { 0 };
		
		m[0] = m[5] = m[10] = m[15] = 1;

		m[12] = Data[i].X;
		m[13] = Data[i].Y;
		m[14] = Data[i].Z;

		glMatrixMode(GL_MODELVIEW);
		
		glPushMatrix();
		
		glLoadIdentity();

		glRotatef(TransM[0], 1, 0, 0);
		glRotatef(TransM[2], 0, 0, 1);
		glRotatef(TransM[1], 0, 1, 0);

		glTranslatef(TransM[3], TransM[4], TransM[5]);

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

void PathGenCoordCubicToWorld(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result)
{


	for (int i = 0; i < DataNum; i++)
	{	
		GLfloat m[16] = { 0 };
	
		m[0] = m[5] = m[10] = m[15] = 1;
		
		m[12] = Data[i].X;
		m[13] = Data[i].Y;
		m[14] = Data[i].Z;


		glMatrixMode(GL_MODELVIEW);
		
		glPushMatrix();
		
		glLoadIdentity();

		glTranslatef(TransM[3], TransM[4], TransM[5]);

		glRotatef(TransM[1], 0, 1, 0);
		glRotatef(TransM[2], 0, 0, 1);
		glRotatef(TransM[0], 1, 0, 0);
		
		glMultMatrixf(m);

		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		glPopMatrix();

		Result[i].X = m[12];
		Result[i].Y = m[13];
		Result[i].Z = m[14];

		//cout << endl << Result[i].X << " " << Result[i].Y << " " <<Result[i].Z << endl;

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

	//cout << endl << Result[0] << " " << Result[1] << endl;
}

void PathGenRegression(CameraSpacePoint* Data, int DataNum, double* Result)
{
	double CalcMean[3] = { 0, 0, 0 };
	PathGenCalcMean(Data, DataNum, CalcMean);

	double paraNum = 0, paraDen = 0;

	if (Result[0] == 1)
	{
		for (int i = 0; i < DataNum; i++)
		{
			paraNum += (Data[i].X - CalcMean[0])*(Data[i].Y - CalcMean[1]);
			paraDen += pow(Data[i].X - CalcMean[0], 2);
		}
	
		//cout << endl << paraNum << " " << paraDen << endl;

		Result[1] = paraNum / paraDen;
		Result[2] = CalcMean[1] - Result[1] * CalcMean[0];

		//cout << endl << Result[1] << " " << Result[2] << " " << endl;
	}
	else if (Result[0] == 2)
	{
		for (int i = 0; i < DataNum; i++)
		{
			paraNum += (Data[i].X - CalcMean[0])*(Data[i].Z - CalcMean[2]);
			paraDen += pow(Data[i].X - CalcMean[0], 2);
		}

		//cout << endl << paraNum << " " << paraDen << endl;

		Result[1] = paraNum / paraDen;
		Result[2] = CalcMean[2] - Result[1] * CalcMean[0];

		//cout << endl << Result[1] << " " << Result[2] << " " << endl;
	}

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
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(KineticUpdate);
	
	glutTimerFunc(1000, timer, 0);
	glutTimerFunc(125, timer, 1);

	BuildPopupMenu();


	//typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	//PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	//wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	//wglSwapIntervalEXT(1);

	glutMainLoop();


	return 0;
}  