#ifndef SCENE_H
#define SCENE_H
/*--------------Standard-------------*/
#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>
#include <stdio.h>
/*--------------OpenCV-------------*/
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cv.h>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/video/tracking.hpp>
/*--------------Kinect-------------*/
#include <Kinect.h>
#include "src/RealScene.h"
/*--------------OpenGL-------------*/
#include <windows.h>   
#include <gl/Gl.h>
#include <gl/glu.h>
#pragma once
#include <GL/freeglut.h>

#include "src/OpenGLCamera.h"
//#include <gl/glut.h>
//将库文件链接到文件中
#pragma comment(lib,"opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment(lib,"GLU32.LIB")
#pragma comment(lib,"GLUT32.LIB")
/*-------------LoadOBJ-------------*/
#include "glm.h"
/*------------PortAduio-------------*/
#include "src/portaudio.h"
/*----------输出最大最小值-----------*/
#include <algorithm>
///*--------使用工作线程控制移动--------*/
//#include "osapi/Thread.h"
/*----------矩阵运算库Eigen------------*/
#include <Eigen/Dense>

using namespace std;
using namespace cv;
void showHelpText();
#pragma region Shader
GLint iLocPosition;
GLint iLocColor;
GLint iLocMVP;

int current_x, current_y;
vector<string> filenames;//.obj模型列表
int cur_idx = 0;//表示现在哪一个模型应该被渲染
bool use_wire_mode = false;
bool operation_translate = false;
bool operation_scaling = false;
bool operation_rotation = false;

#pragma endregion Shader
#pragma region Eigen
Eigen::MatrixXd TransMFormKinect2Machine(4, 4);
#pragma endregion Eigen
#pragma region Global_variable

#pragma region OpenCV&ROI Initial
/*---------------------------------*/
/*--------------OpenCV-------------*/
/*---------------------------------*/
/*Global variable of 3d line and plane */
bool g_dDrawingBox = FALSE;
bool ROI_S1 = FALSE;
bool ROI_S2 = FALSE;
int Head, Tail;
Rect ROI_rect = Rect(-1, -1, 0, 0);
Point ROI_p1, ROI_p2;
Mat ROI;

Mat g_srcImage, g_dstImage, g_grayImage, g_maskImage;
Mat dst;
Mat mColorImg;							//Color map OpenCV format (BGRA)
Mat mDepthImg;							//Depth map UINT16
Mat dstDepthImg;
Mat mImg8bit;							//Depth map CV_8U
//Mat g_dstImage, g_midImage;
Mat ROI_Image;
Mat map_x, map_y;
//Mat g_dstImage, g_grayImage, g_maskImage;
//int g_nthreshold = 100;

int g_nFillMode = 1;//漫水填充的模式
int g_nLowDifference = 20, g_nUpDifference = 20;//负差最大值、正差最大值
int g_nConnectivity = 4;//表示floodFill函数标识符低八位的连通值
int g_bIsColor = true;//是否为彩色图的标识符布尔值
bool g_bUseMask = false;//是否显示掩膜窗口的布尔值
int g_nNewMaskVal = 255;//Find ROI Func

//Tracking
int ROIcount = 0;
int PlanePixelcount = 0;
Point2i ROICenterColorS_Old, ROICenterColorS_New;   //center of tracking object of ROI in Color Space
Point2i* ROIPixel = nullptr;
Point2i* PlanePixel = nullptr;
Point2i* LineROIPixel = nullptr;

CameraSpacePoint* ROICameraSP = nullptr;
CameraSpacePoint* PlaneSP = nullptr;
//Mapping
int ROIDepthCount = 0;

Point3f ROICenterCameraS;

//Storage
CameraSpacePoint* ROICameraSP_Storage = nullptr;					//Probe tip on GL Coordinate
CameraSpacePoint* ROICameraSP_Proj_Storage = nullptr;				//Probe tip projection on GL Coordinate
CameraSpacePoint* ROICameraSP_MachineCoord_Storage = nullptr;			//Probe tip on Machine Coordinate
CameraSpacePoint* ROICameraSP_MachineCoord_Proj_Storage = nullptr;		//Probe tip projection on Machine Coordinate
const int StorageSize = 5000;						//How many points do you want to record
int ROIStorageCount = 0;							//Total # of points stored
bool IS_KEY_F1_UP = FALSE;							//FLAG of data collecting, see SpecialKeys()

CameraSpacePoint* ROICameraSP_TouchDetec = nullptr;
CameraSpacePoint* ROICameraSP_MechCoord = nullptr;
CameraSpacePoint* ROICameraSP_Proj_MechCoord = nullptr;
CameraSpacePoint* LineSP_MachCoord = nullptr;
CameraSpacePoint* PlaneSP_MachCoord = nullptr;

void onMouseROI(int event, int x, int y, int flags, void* param);
static void onMouse(int event, int x, int y, int, void*);
//DataType: 0 = unsigned char, 1 = float 这里是读取mat的数据
void InputValue(Mat M, int DataType, int Row, int Col, int Chan, float Data);
void OutputValue(Mat M, int Row, int Col, int Chan, uchar* Data);
void OutputValue(Mat M, int Row, int Col, int Chan, float* Data);
void FindROI(void);
void CameraShift(void);
void Draw3DLine(void);
void Draw3DPlane(void);
bool ifGetPlane = false;
void colorExtract(Mat& inputImage, Mat& outputImage);
vector<MatND> getHSVHist(Mat& src);
static void onMouse3D(int event, int x, int y, int, void*);
void ShowImage(void);					//Display Image Func
void MoveROI(void);
void BubbleSort(void);
void CameraSpaceROI(void);                         //Mapping ROI Func
void ROICameraSPStorage(void);						//Storage ROI Func
void ROITrans(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result);
void ROITrans(float* Data, int DataNum, GLfloat* TransM, float* Result);
void DrawRectangle(cv::Mat& img, cv::Rect box);

struct result {
	double max;
	double min;
	int head;
	int tail;
};

result* Get_X_Min_Max(CameraSpacePoint* point, int start, int end);
result* Get_Y_Min_Max(CameraSpacePoint* point, int start, int end);
int line_Res_X_head, line_Res_X_tail;
int plane_Res_X_head, plane_Res_X_tail;
int plane_Res_Y_head, plane_Res_Y_tail;
/*-------------CamShift------------*/
bool backprojMode = false;//表示是否要进入反向投影模式，true则表示要进入反向投影模式
bool selectObject = false;//表是否在选中要跟踪的初始目标，true表示正在用鼠标选择要跟踪的目标
int trackObject = 0;//跟踪目标的数目
bool showHist = true;//是否显示HUE分量直方图
Rect selection;//用于保存鼠标选择的矩形框
Rect loadPicture;
Point2i origin;
Rect trackWindow;
int vmin = 10, vmax = 256, smin = 30;
int hsize = 16;
float hranges[] = { 0, 180 };//hranges在后面的计算直方图函数中要用到
const float* phranges = hranges;

struct MyEllipse
{
	float center_x;
	float center_y;
	Point2i tip;
	float u_angle;//第四个轴向的角度，理论上角度在-7-~70之间，在后面要记得控制
};

MyEllipse ellipse_ROI;
#pragma endregion OpenCV&ROI Initial

#pragma region Kinect Initial
/*---------------------------------*/
/*--------------Kinect-------------*/
/*---------------------------------*/

color3uc *img;
//Initial (Sensor)
IKinectSensor* pSensor = nullptr;
ICoordinateMapper* Mapper = nullptr;
IDepthFrameReader* pFrameReaderDepth = nullptr;
IColorFrameReader* pFrameReaderColor = nullptr;
int KinectInit(void);					//Kinect Initial
void KinectUpdate(void);				//Kinect Update Frame
//Depth Map--深度图像
int iWidthDepth = 0;
int iHeightDepth = 0;
int PlaneDepthCount = 0;
UINT depthPointCount = 0;
UINT16* pBufferDepth = nullptr;			//Depth map origin format
UINT16 uDepthMin = 0, uDepthMax = 0;

//Color Map--彩色图像
int iWidthColor = 0;
int iHeightColor = 0;
UINT colorPointCount = 0;
UINT uBufferSizeColor = 0;
BYTE* pBufferColor = nullptr;			//Color map origin format (RGBA)


//Map to camera space point
CameraSpacePoint* pCSPoints = nullptr;

#pragma endregion Kinect Initial

/*必须放在后面初始化的*/
Mat hsv, hue, mask, hist, histimg = Mat::zeros(iWidthColor, iHeightColor, CV_8UC3), backproj;

#pragma region OpenGL&DrawCubic Initial
/*---------------------------------*/
/*--------------OpenGL-------------*/
/*---------------------------------*/
//Basic OpenGL Function
GLfloat m[16];
SimpleCamera g_Camera;
unsigned int*	pIndex = nullptr;
unsigned int	uTriangles = 0;
void GLInit(void);
void SpecialKeys(int key, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void timer(int value);					//OpenGL Time Func
void DrawProbeTip(void);
void RenderScene(void);					//OpenGL Render Func
bool BG_IS_OPEN = TRUE;					//Flag for BG
void SceneWithBackground(void);			//Background Func
void SceneWithoutBackground(void);
void DrawPointCloud(void);				//Drawing Type
void DrawMeshes(void);
void PrintMatrix(GLfloat* m);
/*--------------------------------------*/
float dev_theta = 5 * CV_PI / 180; //为什么dev_theta的值刚好是这么多
CameraSpacePoint* DeviaDueToY = nullptr;
//To synchronize machine and .obj movement
int mtmove_step = 0;
/*-------------------------------------*/
int FPS = 0;
GLuint textureid;						//BG texture
//Calculating FPS w/o waiting monitor to display
int FPS_RS = 0;
bool Finish_Without_Update = FALSE;
float g_fps(void(*func)(void), int n_frame);
//Draw Cubic
Point3f CubicRota;						//Manually rotate the .obj (seldom use)
Point3f ObjPosi;						//.obj translation
Point3f TipPosi;						//Prob tip translation
bool ROI_IS_COLLIDE = FALSE;			//FLAG of if prob insert into the virtual cube
bool Cubic_IS_BLEND = FALSE;
bool CUBIC_MOVE = FALSE;				//FLAG to translate the .obj while plate moves
bool ARFunc_IS_ON = TRUE;				//FLAG to show AR Function or nor

float M_Cubic[16] = {					//Machine Coordinate rotation
	-0.9999192410372007, -0.01136610825316975, -0.005685330840486585, 0,
	-0.012679650367934, 0.9224754399968302, 0.3858474945742184, 0,
	0.0008589936760605928, 0.3858884219380412, -0.9225450601157825, 0,
	0, 0, 0, 1 };
float M_Cubic_inv[16] = {				//Machine Coordinate rotation inversion
	-0.9999192410372008, -0.01267965036793402, 0.0008589936760605181, 0,
	-0.0113661082531697, 0.9224754399968301, 0.3858884219380413, 0,
	-0.005685330840486647, 0.3858474945742184, -0.9225450601157825, 0,
	0, 0, 0, 1 };

CameraSpacePoint IntersectPoint;				//Machine Coordinate translation

void DrawCubic(void);
//Creat Pop Up menu
enum {
	MENU_WTBG_NONE = 1,
	MENU_WTBG_MESH,
	MENU_3D_LINE,
	MENU_3D_PLANE,
	MENU_WTBG_POINTCLOUD,
	MENU_WOBG_MESH,
	MENU_WOBG_POINTCLOUD,
	MENU_STORAGETYPE1,
	MENU_STORAGETYPE2,
	MENU_COLOR_TRACK,

};
int menu_value = 0;
int submenuID_BG;
int submenuID_WTBG_DrawType;
int submenuID_WOBG_DrawType;
int submenuID_StorageType;
int submenuID_ROIType;

int DRAWING_TYPE = 0;                      //Flag for drawing type: 0 = none, 1 = mesh, 2 = point cloud
int STORAGE_TYPE = 0;                      //Flag for storage type: 0 = prob tip pt, 1 = projection pt
int ROI_TYPE = 0;
void BuildPopupMenu(void);
void menuCB(int menu_value);
/*what does this region mean? */
//glPrint
DWORD fdwCharSet = CHINESEBIG5_CHARSET;		// Encoding code: Big Five
char szFace[] = "Courier New";				// Script: Courier New
int nHeight = 16;							// Character Size: 16 pixel
//记录字符的字型图像是否已被加载, 如果某字符的字型图像已被加载, 则不需再载入
GLuint base = 0;							// 字型图像 的 display lists, 初始成 0
//窗口系统的东西, 我们要透过它才能向 窗口系统 取得 某字符的字型图像
bool loaded[65536] = { 0 };					// 定义 65536 个 bool, 全都初始成 0
HFONT hFont;								// font handle
int glPrintf(const char *format, ...);

#pragma endregion OpenGL&DrawCubic Initial

#pragma region LoadOBJ Initial
/*---------------------------------*/
/*-------------LoadOBJ-------------*/
/*---------------------------------*/
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
//void traverseColorModel(model &m);
void traverseColorModel();
void loadConfigFile(void);
void setShaders(void);
void showShaderCompileStatus(GLuint shader, GLint *shaderCompiled);
#pragma endregion LoadOBJ Initial

#pragma region AR Function Initial
CameraSpacePoint* ARFunc_ROICSP_Proj = nullptr;		//Projection point of the probe tip
double ARFuncNormal1[4] = { 0 };						//Top plane planner function of the .OBJ file
double ARFuncNormal2[4] = { 0 };
float ARFunc_ROICSP_Proj_Dist = 0;					//Distance between probe tip and its projection point
void ARFunc_FindProj(void);	//Find projection point
//Check if the point is inside triangle
bool ARFunc_InsideTriCheck(CameraSpacePoint* Point, CameraSpacePoint* TriVertex0, CameraSpacePoint* TriVertex1, CameraSpacePoint* TriVertex2);
float Dot(float* Vector1, float* Vector2);			//Dot product
//|a b|
//|c d|
float Determinant(float a, float b, float c, float d);
bool IS_ARFunc_InsideTriCheck = FALSE;
bool IS_AR_PLANE_FUNC_FIND = FALSE;
#pragma endregion AR Function Initial

#pragma region PortAudio Initial
/*---------------------------------*/
/*------------PortAduio------------*/
/*---------------------------------*/
#define NUM_SECONDS   (10)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#define TABLE_SIZE   (200)
/*What is the part?*/
class Sine
{
public:
	Sine() : stream(0), left_phase(0), right_phase(0)
	{
		/* initialise sinusoidal wavetable */
		for (int i = 0; i<TABLE_SIZE; i++)
		{
			sine[i] = (float)sin(((double)i / (double)TABLE_SIZE) * CV_PI * 2.);
		}
		sprintf_s(message, "No Message");
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
	* This routine is called by PortAudio when playback is done.
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
/*What's this part?*/
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
#pragma endregion PortAudio Initial
#pragma endregion Global_variable
#endif