#pragma region Region_Library
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
/*--------------OpenHaptics-------------*/
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>
/*--------------LoadObj-------------*/
#include "GLM.h"
//#include "SOIL.h"
/*--------------PortAudio-------------*/
//#pragma comment(lib,"winmm.lib") 
//#include <mmsystem.h> 
#include "portaudio.h"
using namespace std;
using namespace cv;
/*--------------M232-------------*/
#include "dllmain.h"
#using <System.dll>
using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
#pragma endregion Region_Library
#pragma region Global_Variable
/*-------------------------------------*/
/*----------------Mt232----------------*/
/*-------------------------------------*/
bool MtHome(void);
void MtReflashWait(void);
void MtInit(void);
void MtMove(void);
bool IS_MT_MOVE = false;
Rs232MotionData* md = new struct Rs232MotionData;
/*----------------------------------------------*/
/*----------------OpenGL Declare----------------*/
/*----------------------------------------------*/
GLuint textureid;                      //BG texture
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
bool BG_IS_OPEN = TRUE/*FALSE*/;                    //Flag for BG
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
float M_Cubic_inv[16] = { 0 };
float M_Cubic2[16] = { 0 };
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
bool IS_PlaceObject = FALSE;
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
CameraSpacePoint* ROICameraSP_MechCoord_Storage = nullptr;
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
void ROITrans(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result);
void ROITrans(float* Data, int DataNum, GLfloat* TransM, float* Result);
CameraSpacePoint* ROICameraSP_TouchDetec = nullptr;
CameraSpacePoint* ROICameraSP_MechCoord = nullptr;
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
float theta2 = 0;
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
/*-----------------------------------------------*/
/*----------------LoadObj Declare----------------*/
/*-----------------------------------------------*/
GLMmodel* OBJ;
GLuint textures[6];
#define MAX_TEXTURE_NUM 50
#define MAXSIZE 200000
#define MAXGROUPSIZE 50
#define myMax(a,b) (((a)>(b))?(a):(b))
float vertices[MAXGROUPSIZE][MAXSIZE];
float normals[MAXGROUPSIZE][MAXSIZE];
float vtextures[MAXGROUPSIZE][MAXSIZE];
void loadOBJModel(void);
void SetTexObj(char *name, int i);
void Texture(void);
void traverseModel(void);
/*---------------------------------------------------*/
/*----------------AR Function Declare----------------*/
/*---------------------------------------------------*/
CameraSpacePoint* ARFunc_ROICSP_Proj = nullptr;
double ARFuncNormal[4] = { 0 };
void ARFunc_FindProj(void);
bool ARFunc_InsideTriCheck(CameraSpacePoint* Point, CameraSpacePoint* TriVertex0, CameraSpacePoint* TriVertex1, CameraSpacePoint* TriVertex2);
float Dot(float* Vector1, float* Vector2);
//|a b|
//|c d|
float Determinant(float a, float b, float c, float d);
float ARFunc_ROICSP_Proj_Dist = 0;
bool IS_ARFunc_InsideTriCheck = FALSE;
bool IS_AR_PLANE_FUNC_FIND = FALSE;
/*---------------------------------------------------*/
/*-----------------PortAudio Declare-----------------*/
/*---------------------------------------------------*/
#define NUM_SECONDS   (10)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)
#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#define TABLE_SIZE   (200)

class Sine
{
public:
	Sine() : stream(0), left_phase(0), right_phase(0)
	{
		/* initialise sinusoidal wavetable */
		for (int i = 0; i<TABLE_SIZE; i++)
		{
			sine[i] = (float)sin(((double)i / (double)TABLE_SIZE) * M_PI * 2.);
		}
		sprintf(message, "No Message");
	}
	bool open(PaDeviceIndex index)
	{
		PaStreamParameters outputParameters;
		outputParameters.device = index;
		if (outputParameters.device == paNoDevice) {
			return false;
		}
		const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
		if (pInfo != 0)
		{
			printf("Output device name: '%s'\r", pInfo->name);
		}
		outputParameters.channelCount = 2;       /* stereo output */
		outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
		outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;
		PaError err = Pa_OpenStream(
			&stream,
			NULL, /* no input */
			&outputParameters,
			SAMPLE_RATE,
			paFramesPerBufferUnspecified,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			&Sine::paCallback,
			this            /* Using 'this' for userData so we can cast to Sine* in paCallback method */
			);
		if (err != paNoError)
		{
			/* Failed to open stream to device !!! */
			return false;
		}
		err = Pa_SetStreamFinishedCallback(stream, &Sine::paStreamFinished);
		if (err != paNoError)
		{
			Pa_CloseStream(stream);
			stream = 0;
			return false;
		}
		return true;
	}
	bool close()
	{
		if (stream == 0)
			return false;
		PaError err = Pa_CloseStream(stream);
		stream = 0;
		return (err == paNoError);
	}
	bool start()
	{
		if (stream == 0)
			return false;
		PaError err = Pa_StartStream(stream);
		return (err == paNoError);
	}
	bool stop()
	{
		if (stream == 0)
			return false;
		PaError err = Pa_StopStream(stream);
		return (err == paNoError);
	}
private:
	/* The instance callback, where we have access to every method/variable in object of class Sine */
	int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags)
	{
		float *out = (float*)outputBuffer;
		unsigned long i;
		(void)timeInfo; /* Prevent unused variable warnings. */
		(void)statusFlags;
		(void)inputBuffer;
		int key = 0;
		float distant = ARFunc_ROICSP_Proj_Dist * 1000.0f;
		//if (ARFunc_ROICSP_Proj_Dist * 1000 > 100)
		//{
		//	key = 5;
		//}
		//else if (ARFunc_ROICSP_Proj_Dist * 1000 > 50)
		//{
		//	key = 9;
		//}
		//else if (ARFunc_ROICSP_Proj_Dist * 1000 > 10)
		//{
		//	key = 13;
		//}
		//else
		//{
		//	key = 15;
		//}
		const int low_range = 10, high_range = 100, low_tone = 3, high_tone = 15;
		if (distant > high_range)
		{
			key = low_tone;
		}
		else if (distant < low_range)
		{
			key = high_tone;
		}
		else
		{
			key = (distant - low_range) / (high_range - low_range) * (low_tone - high_tone) + high_tone;
		}
		for (i = 0; i<framesPerBuffer; i++)
		{
			*out++ = sine[left_phase];  /* left */
			*out++ = sine[right_phase];  /* right */
			//cout << *out << endl;
			//left_phase += 10;
			left_phase += key;
			if (left_phase >= TABLE_SIZE) left_phase -= TABLE_SIZE;
			//right_phase += 3; /* higher pitch so we can distinguish left and right. */
			right_phase += key;
			if (right_phase >= TABLE_SIZE) right_phase -= TABLE_SIZE;
			//cout << key << endl;
		}
		return paContinue;
	}
	/* This routine will be called by the PortAudio engine when audio is needed.
	** It may called at interrupt level on some machines so don't do anything
	** that could mess up the system like calling malloc() or free().
	*/
	static int paCallback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData)
	{
		/* Here we cast userData to Sine* type so we can call the instance method paCallbackMethod, we can do that since
		we called Pa_OpenStream with 'this' for userData */
		return ((Sine*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
			framesPerBuffer,
			timeInfo,
			statusFlags);
	}
	void paStreamFinishedMethod()
	{
		printf("Stream Completed: %s\n", message);
	}
	/*
	* This routine is called by portaudio when playback is done.
	*/
	static void paStreamFinished(void* userData)
	{
		return ((Sine*)userData)->paStreamFinishedMethod();
	}
	PaStream *stream;
	float sine[TABLE_SIZE];
	int left_phase;
	int right_phase;
	char message[20];
};

class ScopedPaHandler
{
public:
	ScopedPaHandler()
		: _result(Pa_Initialize())
	{
	}
	~ScopedPaHandler()
	{
		if (_result == paNoError)
		{
			Pa_Terminate();
		}
	}
	PaError result() const { return _result; }
private:
	PaError _result;
};
Sine sine;
ScopedPaHandler paInit;
bool InitPortAudio(void);
bool IS_PORTAUDIO_INIT = FALSE;
bool IS_PORTAUDIO_START = FALSE;
#pragma endregion Global_Variable

#pragma region Mt232
/*--------------------------------*/
/*--------------Mt232-------------*/
/*--------------------------------*/
void MtReflashWait()
{
	int i = mdr.wdt;
	int timeout = 0;
	while (i == mdr.wdt && timeout < 50)
	{
		MtReflash(md);
		timeout += 1;
		Sleep(1);
	}
}

bool MtHome()
{
	MtCmd("mt_emg 0");
	Sleep(100);
	MtCmd("mt_delay 20");
	MtCmd("mt_m_acc 20");
	MtCmd("mt_v_acc 20");
	MtCmd("mt_speed 30");
	MtCmd("mt_check_ot 0,0,0,0");
	MtCmd("mt_set_home_acc 50");
	MtCmd("mt_leave_home_speed 2,2,2,2");
	MtCmd("mt_go_home 50,50,10,50,2,2,1,255"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 順序x, 順序y, 順序z, 順序u, 255 = u不回home)
	Sleep(1000);
	do{
		MtReflashWait();
		if (MtFlag(MotionStatus::emg_signal))
		{
			MtCmd("mt_abort_home");
			return false;
		}
	} while (MtFlag(MotionStatus::home_x) || MtFlag(MotionStatus::home_y) || MtFlag(MotionStatus::home_z)); //home_x 回到原點的過程 = true
	MtCmd("mt_home_finish");
	MtCmd("mt_speed1 30");
	MtCmd("mt_soft_limit 0,-2,300"); //Axe no., Left limit, Right limit
	MtCmd("mt_soft_limit 1,-2,300");
	MtCmd("mt_soft_limit 2,-2,100");
	MtCmd("mt_soft_limit 3,-450,450");
	MtCmd("mt_check_ot 1,1,1,0"); //enable limit (by axe no)
	MtCmd("mt_out 11,1"); //door switch
	MtCmd("mt_m_acc 150");  //door switch
	MtCmd("mt_v_acc 80");  //door switch
	return true;
}

void MtInit(void)
{
	array<System::String^>^ serialPorts = nullptr;
	try
	{
		// Get a list of serial port names.
		serialPorts = SerialPort::GetPortNames();
	}
	catch (Win32Exception^ ex)
	{
		Console::WriteLine(ex->Message);
	}
	Console::WriteLine("The following serial ports were found:");
	// Display each port name to the console.
	for each(System::String^ port in serialPorts)
	{
		Console::WriteLine(port);
	}
	//cout << MtTestDebby(2, 3) << endl;
	char ptr[5] = "COM6";
	//cout << "Connect I/O = " << MtConnect(ptr) << endl;
	long IS_CONECT = 0;
	//剛開機時，第一次連線都會失敗不知為何
	do
	{
		IS_CONECT = MtConnect(ptr);
		cout << "Connect I/O = " << IS_CONECT << endl;
	} while (IS_CONECT != 1);
	Sleep(50);
	bool IS_HOME = MtHome();
}

void MtMove(void)
{
	//bool IS_HOME = MtHome();
	MtReflash(md);
	cout << "md->x : " << md->x << endl;
	cout << "md->y : " << md->y << endl;
	Sleep(100);
	for (int i = 0; i < 5; i++)
	{
		MtCmd("mt_out 12,1"); //12是出膠 9是閃燈
		Sleep(100);
		MtCmd("mt_out 12,0");
		Sleep(100);
	}
	//MtCmd("mt_speed 100");
	//MtCmd("mt_mr_x 30"); //mr = 相對座標; m = 絕對座標
	//MtCmd("mt_mr_y 30"); //mr = 相對座標; m = 絕對座標
	//Sleep(100);
	//ROICameraSP_MechCoord_Storage[0].X;
	for (int i = 0; i < ROIStorageCount; i++)
	{
		char mybuffx[50], mybuffy[50], mybuffz[50];
		char commandx[60] = "mt_m_x ", commandy[60] = "mt_m_y ", commandz[60] = "mt_m_z ";
		sprintf(mybuffx, "%f", ROICameraSP_MechCoord_Storage[i].X * 1000);
		sprintf(mybuffy, "%f", 320 - ROICameraSP_MechCoord_Storage[i].Y * 1000);
		sprintf(mybuffz, "%f", ROICameraSP_MechCoord_Storage[i].Z * 1000);
		strcat(commandx, mybuffx);
		strcat(commandy, mybuffy);
		strcat(commandz, mybuffz);
		//cout << mybuffx << endl
		//	<< mybuffy << endl
		//	<< mybuffz << endl << endl
		//
		//	<< commandx << endl
		//	<< commandy << endl
		//	<< commandz << endl << endl;
		MtCmd(commandx);
		MtCmd(commandy);
		MtCmd(commandz);
		Sleep(500);
	}
	//MtCmd("mt_m_x 145"); //mr = 相對座標; m = 絕對座標
	//MtCmd("mt_m_y 124"); //mr = 相對座標; m = 絕對座標
	//MtCmd("mt_m_z 50"); //mr = 相對座標; m = 絕對座標

	//int i = 0;
	//while (!MtFlag(MotionStatus::motion_idle))
	//{
	//	cout << i << endl;
	//	i++;
	//}
	//MtReflash(md);
	//cout << "md->x : " << md->x << endl;
	//cout << "md->y : " << md->y << endl;
}
#pragma endregion Mt232

#pragma region OpenGL
/*------------------------------------------*/
/*--------------OpenGL Function-------------*/
/*------------------------------------------*/
void GLInit()
{
	//glGenTextures(6, textures);
	//glBindTexture(GL_TEXTURE_2D, textureid);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidthColor, iHeightColor, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);
	CubicPosi.x = 0;
	CubicPosi.y = 0;
	CubicPosi.z = 0;

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
	glGenTextures(1, &textureid/*textures*/);
	//glGenTextures(6, textures);

	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidthColor, iHeightColor, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);
}

void SpecialKeys(int key, int x, int y)
{
	//Press F1 to start recoard path of the probe
	//Press again to end
	if (key == GLUT_KEY_F1)
	{
		ROICameraSPStorage();
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
			delete[] ROICameraSP_MechCoord_Storage;
			ROICameraSP_MechCoord_Storage = nullptr;
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
	else if (key == GLUT_KEY_DOWN && !IS_PathGenProcessing)
	{
		CubicPosi.y += 0.01f;
		cout << endl << "CubicPosi.y : " << CubicPosi.y << endl;
	}
	else if (key == GLUT_KEY_UP && !IS_PathGenProcessing)
	{
		CubicPosi.y -= 0.01f;
		cout << endl << "CubicPosi.y : " << CubicPosi.y << endl;
	}
	else if (key == GLUT_KEY_RIGHT && !IS_PathGenProcessing)
	{
		CubicPosi.x += 0.01f;
		cout << endl << "CubicPosi.x : " << CubicPosi.x << endl;
	}
	else if (key == GLUT_KEY_LEFT && !IS_PathGenProcessing)
	{
		CubicPosi.x -= 0.01f;
		cout << endl << "CubicPosi.x : " << CubicPosi.x << endl;
	}
	else if (key == GLUT_KEY_F9 && !IS_PathGenProcessing)
	{
		CubicPosi.z += 0.01f;
		cout << endl << "CubicPosi.z : " << CubicPosi.z << endl;
	}
	else if (key == GLUT_KEY_F10 && !IS_PathGenProcessing)
	{
		CubicPosi.z -= 0.01f;
		cout << endl << "CubicPosi.z : " << CubicPosi.z << endl;
	}
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
	case 'A':
	case 'a':
		MtMove();
		break;
	case 'S':
	case 's':
		ROICameraSPStorage();
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
		for (int i = 0; i < colorPointCount; i += 3)
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
		for (int i = 0; i < colorPointCount; i += 3)
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

void DrawPath()
{
	const float T_MIN = -0.19;
	const float T_MAX = 0.19;
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
#pragma endregion OpenGL

#pragma region ROI
/*---------------------------------------*/
/*--------------ROI Function-------------*/
/*---------------------------------------*/
void ROITrans(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result)
{
	for (int i = 0; i < DataNum; i++)
	{
		//cout << endl << Data[i].X << " " << Data[i].Y << " " << Data[i].Z << endl;
		GLfloat m[16] = { 0 };
		//m[0] = m[5] = m[10] = m[15] = 1;
		m[3] = m[7] = m[11] = m[15] = 1;
		m[12] = Data[i].X;
		m[13] = Data[i].Y;
		m[14] = Data[i].Z;
		//glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(TransM);
		glMultMatrixf(m);
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		glPopMatrix();
		Result[i].X = m[12] / m[15];
		Result[i].Y = m[13] / m[15];
		Result[i].Z = m[14] / m[15];
	}
}

void ROITrans(float* Data, int DataNum, GLfloat* TransM, float* Result)
{
	for (int i = 0; i < DataNum; i++)
	{
		//cout << endl << Data[i].X << " " << Data[i].Y << " " << Data[i].Z << endl;
		GLfloat m[16] = { 0 };
		m[0] = m[5] = m[10] = m[15] = 1;
		m[12] = Data[3 * i + 0];
		m[13] = Data[3 * i + 1];
		m[14] = Data[3 * i + 2];
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(TransM);
		glMultMatrixf(m);
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		glPopMatrix();
		Result[3 * i + 0] = m[12] / m[15];
		Result[3 * i + 1] = m[13] / m[15];
		Result[3 * i + 2] = m[14] / m[15];
	}
}
#pragma endregion ROI

#pragma region Plane_of_Regression
/*----------------------------------------------------*/
/*--------------Plane of Regression (POR)-------------*/
/*----------------------------------------------------*/
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
		float* srcData = (float*)M.data;
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
#pragma endregion Plane_of_Regression

#pragma region Path_generation
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
#pragma endregion Path_generation

#pragma region AR_Function
/*---------------------------------------------------*/
/*----------------AR Function Declare----------------*/
/*---------------------------------------------------*/
float Dot(float* Vector1, float* Vector2)
{
	//int Length1 = sizeof(Vector1) / sizeof(Vector1[0]);
	//int Length2 = sizeof(Vector2) / sizeof(Vector2[0]);
	return Vector1[0] * Vector2[0] + Vector1[1] * Vector2[1] + Vector1[2] * Vector2[2];
}

float Determinant(float a, float b, float c, float d)
{
	return a*d - b*c;
}
#pragma endregion AR_Function

/*------------------------------------------------------------*/
/*-----------------PortAudio Function Declare-----------------*/
/*------------------------------------------------------------*/
bool InitPortAudio()
{
	if (paInit.result() != paNoError) goto error;
	if (sine.open(Pa_GetDefaultOutputDevice()))
	{
		IS_PORTAUDIO_INIT = TRUE;
		return IS_PORTAUDIO_INIT;
	}
error:
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", paInit.result());
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit.result()));
	return 0;
}
