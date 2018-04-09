#pragma region Initialization
/*--------------Standard Library-------------*/
#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>
//#include <thread>
 /*--------------OpenCV-------------*/
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <cv.h>

 /*--------------Kinect-------------*/
#include <Kinect.h>
#include <windows.h>   
 /*--------------M232-------------*/
#include "dllmain.h"
#using <System.dll>
using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace System::Threading;
using namespace std;
using namespace cv;

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif
#pragma region Kinect Initial
/*--------------------------------------*/
/*--------------Kinect v2---------------*/
/*--------------------------------------*/
//Initial (Sensor)
IKinectSensor* pSensor = nullptr;
ICoordinateMapper* Mapper = nullptr;
IDepthFrameReader* pFrameReaderDepth = nullptr;
IColorFrameReader* pFrameReaderColor = nullptr; 
 //Depth Map
int iWidthDepth = 0;
int iHeightDepth = 0;
UINT depthPointCount = 0;
UINT16* pBufferDepth = nullptr;   //Depth map origin format
UINT16 uDepthMin = 0, uDepthMax = 0;
 //Color Map
int iWidthColor = 0;
int iHeightColor = 0;
UINT colorPointCount = 0;
UINT uBufferSizeColor = 0;
BYTE* pBufferColor = nullptr;   //Color map origin format (RGBA)
#pragma endregion Kinect Initial
/*--------------------------------------*/
/*----------------OpenCV----------------*/
/*--------------------------------------*/
Mat mDepthImg;                  //Depth map UINT16 深度图像
Mat mImg8bit;                   //Depth map CV_8U 深度图像
Mat mColorImg;                  //Color map OpenCV format (BGRA) 彩色图像
 //Initial
Rect2i ROI_rect;
Point ROI_p1, ROI_p2;			//start point and end point of ROI 感兴趣区域的起始点和终止点
bool ROI_S1 = false;			//flag of mouse event
bool ROI_S2 = false;
Mat ROI;
void onMouseROI(int event, int x, int y, int flags, void* param);
void FindROI(void);
//Tracking
int ROIcount = 0;
Point2i ROICenterColorS_Old, ROICenterColorS_New;				   //center of tracking object of ROI in Color
Point2i* ROIPixel = nullptr;
CameraSpacePoint* ROICameraSP = nullptr;
void MoveROI(void);
/*-------------------------------------*/
/*-------------Cam Calibration----------*/
/*-------------------------------------*/
vector<Point2f> imageCorners_Machine;
vector<Point3f> objectCorners_Machine;
vector<vector<Point3f>> objectPoints_Machine;
vector<vector<Point2f>> imagePoints_Machine;
//why the size of machine is different from the size of chessboard
Size boardSize_Machine(9, 5);//点胶机要跑的点的大小
Mat cameraMatrix_Machine;
Mat distCoeffs_Machine;
vector<Point2f> imageCorners_ChessBoard;
vector<vector <Point2f> > imagePoints_ChessBoard;
vector<Point3f> objectCorners_ChessBoard;
vector<vector <Point3f> > objectPoints_ChessBoard;

Size boardSize_ChessBoard(9, 6);//这是矫正板的点的阵列的大小
Mat cameraMatrix_ChessBoard;
Mat distCoeffs_ChessBoard;

//cubesize_machine就是机器移动的点之间的距离，距离为2.5cm
const int cubeSize_Machine = 25;
Size imageSize;
/*-------------------------------------*/
/*----------------5-Axis Dispenser----------------*/
/*-------------------------------------*/
bool MtHome(void);
void MtReflashWait(void);
void MtInit(void);
void MtMove(void);
bool IS_MT_MOVE = false;
bool IsMtMoving = false;
/*----------------what is the Rs232Motiondata?----------------*/
Rs232MotionData* md = new struct Rs232MotionData;
#pragma endregion Initialization


void MtReflashWait()//这个应该是让机器停留在某个位置一段时间
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

//let the 5-axis dispenser go to it's initial position
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
	MtCmd("mt_set_leave_home_speed 2,2,2,2,50,2");
	//MtCmd("mt_go_home 50,50,10,50,2,2,1,255"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 順序x, 順序y, 順序z, 順序u, 255 = u不回home)
	//MtCmd("mt_go_home 50,50,10,10,2,2,1,3"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 順序x, 順序y, 順序z, 順序u, 255 = u不回home)
	MtCmd("mt_home 50,50,10,50,50,20,2,2,1,2,2,255"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 速度v, 速度w, 順序x, 順序y, 順序z, 順序u, 顺序v, 顺序w, 255 = u不回home)
	Sleep(1000);
	do{
		MtReflashWait();
		if (MtFlag(MotionStatus::emg_signal))
		{
			MtCmd("mt_abort_home");
			return false;
		}
	} while (MtFlag(MotionStatus::home_x) || MtFlag(MotionStatus::home_y) || MtFlag(MotionStatus::home_z) || MtFlag(MotionStatus::home_u) || MtFlag(MotionStatus::home_v)); //home_x 回到原點的過程 = true

	MtCmd("mt_home_finish");
	MtCmd("mt_speed1 30");
	MtCmd("mt_soft_limit 0,-2,400"); //Aixs no., Left limit, Right limit
	MtCmd("mt_soft_limit 1,-2,400");
	MtCmd("mt_soft_limit 2,-2,100");
	MtCmd("mt_soft_limit 3,0,360");
	MtCmd("mt_soft_limit 4,-70,70");

	MtCmd("mt_check_ot 1,1,1,1");	//enable limit 
	MtCmd("mt_out 11,1"); //door switch
	MtCmd("mt_m_acc 150");  //door switch
	MtCmd("mt_v_acc 80");  //door switch
	return true;
}

void MtInit(void)
{
	/*
	 * #1 在这个里面，是为了拿到点胶机链接的COMPORT
	 * 可以改进的部分：让机器自动遍历不同的COMPort寻找到机器链接的哪一个
	 */
	//what is the string^
	array<System::String^>^ serialPorts = nullptr;
	try//可能发生错误的地方
	{
		// Get a list of serial port names.
		serialPorts = SerialPort::GetPortNames();
	}
	catch (Win32Exception^ ex)//以外的处理方法
	{
		Console::WriteLine(ex->Message);
	}
 	Console::WriteLine("The following serial ports were found:");
 	// Display each port name to the console.
	for each(System::String^ port in serialPorts)
	{
		Console::WriteLine(port);
	}
 	
 	char ptr[5] = "COM6";
	//cout << "Connect I/O = " << MtConnect(ptr) << endl;
 	long IS_CONECT = 0;
 	//刚开机的时候第一次连线都会失败，因为发现机器会读出来两个port，COM1和COM6，而COM1是不能链接成功的，所以第一次链接都会失败，不过不知道为什么有COM1
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
	bool IS_HOME = MtHome();
 	MtReflash(md);
	//cout << "md->x : " << md->x << endl;
	//cout << "md->y : " << md->y << endl;
	Sleep(100);
 	for (int i = 0; i < 2; i++)
	{
		MtCmd("mt_out 12,1"); //12是出膠 9是閃燈
		Sleep(100);
		MtCmd("mt_out 12,0");
		Sleep(100);
	}
 	//Clear image corner image
	imageCorners_Machine.clear();
	//这里是让机器移动到X,Z的位置
 	for (int i = 0; i < boardSize_Machine.height; i++)
	{
		for (int j = 0; j < boardSize_Machine.width; j++)
		{
			char mybuffx[50], mybuffz[50];
			char commandx[60] = "mt_m_x ",  commandz[60] = "mt_m_z ";
			sprintf(mybuffx, "%i", cubeSize_Machine * j);
			sprintf(mybuffz, "%i", cubeSize_Machine * i);
 			
			//strcat进行string的合并
 			strcat(commandx, mybuffx);
			strcat(commandz, mybuffz);
 			MtCmd(commandx);
			MtCmd(commandz); 
 			
			do
			{
			    //在这里MtReflash是控制点胶机再次移动，是让点胶机达到X,Z的标准后停一下
				MtReflash(md);
 			} while (md->x != cubeSize_Machine * j || md->z != cubeSize_Machine * i);
 			//在这里表示在移动到点之后休息的时间，单位毫秒，在每个点休息1.5秒
			sleep(1500);
			//ROICenterColorS_old表示的是追踪点的位置
 			while (ROICenterColorS_Old.x > 1920 || ROICenterColorS_Old.y > 1080)
			{
				FindROI();
			}
			//这里拿到的是像素坐标，UV坐标
			imageCorners_Machine.push_back(Point2f(ROICenterColorS_Old.x, ROICenterColorS_Old.y));
 		}
	}
 	//Clear object point
    //objectCorners_machine就是理想的跑出来的红色点的坐标值，而imageCorners_machine就是追踪点的像素坐标
	objectCorners_Machine.clear();//
 	//Fill object point
	// The corners are at 3D location (X,Y,Z)= (i,j,0)
	for (int i = 0; i<boardSize_Machine.height; i++)
	{
		for (int j = 0; j<boardSize_Machine.width; j++)
		{
			//这里把机器坐标存进来，但是存进来的是X,Y的值
			objectCorners_Machine.push_back(Point3f(j * cubeSize_Machine * 0.001, i * cubeSize_Machine*0.001, 0.0f));
		}
	}
 	//Fill image and object point in bigger array (vector)
	if (imageCorners_Machine.size() == boardSize_Machine.area())
	{
		//输出t的倒数第五步
		imagePoints_Machine.push_back(imageCorners_Machine);
		objectPoints_Machine.push_back(objectCorners_Machine);
	}
 }

void onMouseROI(int event, int x, int y, int flags, void* param)
{
	int thickness = 2;
 	if (event == CV_EVENT_LBUTTONDOWN)
	{
		//current mouse's position （x，y）  
		ROI_rect.x = x;
		ROI_rect.y = y;
 		ROI_S1 = true;
		ROI_S2 = false;
 		//cout << "CV_EVENT_LBUTTONDOWN: " << ROI_rect.x << ", " << ROI_rect.y << endl;
	}
	else if (ROI_S1 && event == CV_EVENT_MOUSEMOVE)
	{
		ROI_S2 = true;
 		ROI_p1 = Point(ROI_rect.x, ROI_rect.y);
		ROI_p2 = Point(x, y);
 		ROI = mColorImg.clone();
		rectangle(ROI, ROI_p1, ROI_p2, Scalar(255, 255, 0), 2);
		cv::imshow("Color Map", ROI);
 	}
	else if (ROI_S1 && event == CV_EVENT_LBUTTONUP)
	{
		ROI_S1 = false;
 		ROI_rect.height = y - ROI_rect.y;
		ROI_rect.width = x - ROI_rect.x;
 		ROI_p2 = Point(x, y);
	}
}

void FindROI()
{
	//namedWindow("ROI");
	//namedWindow("YCrCb");
 	/*--------------Find ROI-------------*/
	Mat ROI_Image = mColorImg.colRange(ROI_p1.x, ROI_p2.x + 1).rowRange(ROI_p1.y, ROI_p2.y + 1).clone();
	//imshow("ROI", ROI_Image);
 	/*--------------BGR to YCrCb-------------*/
	Mat ROI_YCrCb;
	cvtColor(ROI_Image, ROI_YCrCb, /*CV_BGR2HSV*/CV_BGR2YCrCb);
	//imshow("YCrCb", ROI_YCrCb);
 	/*-------------- Color Detection and Tracking-------------*/
	int rows = ROI_YCrCb.rows;
	int cols = ROI_YCrCb.cols;
	ROIcount = 0;
	//ROIDepthCount = 0;
 	ROICenterColorS_New.x = 0;
	ROICenterColorS_New.y = 0;
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
 	//imshow("ROI", ROI_Image);
	imshow("Color Map", ROI);
 	if (ROIcount > 0)
	{
		//输出t的倒数第七步
		ROICenterColorS_New.x = static_cast<int>(ROICenterColorS_New.x / ROIcount);//强制类型转换
		ROICenterColorS_New.y = static_cast<int>(ROICenterColorS_New.y / ROIcount);
 		ROICenterColorS_Old = ROICenterColorS_New;
	}
	else if (ROIcount == 0)
	{
		ROICenterColorS_Old.x = ROICenterColorS_New.x = 0;
		ROICenterColorS_Old.y = ROICenterColorS_New.y = 0;
	}
 	ROI_Image.release();
	ROI_YCrCb.release();
	//ROI.release();
	//cout << "ROI dimenssion" << ROI_Image.cols << " " << ROI_Image.rows << endl;
}

void PrintMat(Mat H)
{
	cout << "H = " << endl << " " << H << endl << endl;
}
int main()
{
 	MtInit();
#pragma region KinectStreamOpen
	// 1. Sensor related code
	cout << "Try to get default sensor" << endl;
	{
		if (GetDefaultKinectSensor(&pSensor) != S_OK)
		{
			cerr << "Get Sensor failed" << endl;
			return -1;
		}
 		cout << "Try to open sensor" << endl;
		if (pSensor->Open() != S_OK)
		{
			cerr << "Can't open sensor" << endl;
			return -1;
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
			return -1;
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
 			pBufferColor = new BYTE[4 * colorPointCount];
			//pCSPoints = new CameraSpacePoint[colorPointCount];
 			imageSize.height = iHeightColor;
			imageSize.width = iWidthColor;
		}
		pFrameDescription->Release();
		pFrameDescription = nullptr;
 		// get frame reader
		cout << "Try to get color frame reader" << endl;
		if (pFrameSource->OpenReader(&pFrameReaderColor) != S_OK)
		{
			cerr << "Can't get color frame reader" << endl;
			return -1;
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
			return -1;
		}
		if (pSensor->get_DepthFrameSource(&pFrameSource) == S_OK)
		{
			pFrameSource->get_DepthMinReliableDistance(&uDepthMin);
			pFrameSource->get_DepthMaxReliableDistance(&uDepthMax);
		}
 		// Get frame description
		cout << "get depth frame description" << endl;
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
			return -1;
		}
		// release Frame source
		cout << "Release frame source" << endl;
		pFrameSource->Release();
		pFrameSource = nullptr;
	}
#pragma endregion KinectStreamOpen
 	mDepthImg = cv::Mat::zeros(iHeightDepth, iWidthDepth, CV_16UC1);
	mImg8bit = cv::Mat::zeros(iHeightDepth, iWidthDepth, CV_8UC1);
	mColorImg = cv::Mat::zeros(iHeightColor, iWidthColor, CV_8UC4);
 	int key;
	int file_idx = 0;
	char filename[256];
	char fileidx[10];
 	while (true)
	{
 #pragma region KinectStreamUpdate
		/*--------------Read color data-------------*/
		IColorFrame* pFrameColor = nullptr;
		if (pFrameReaderColor->AcquireLatestFrame(&pFrameColor) == S_OK)
		{
			pFrameColor->CopyConvertedFrameDataToArray(uBufferSizeColor, pBufferColor, ColorImageFormat_Rgba);
			pFrameColor->CopyConvertedFrameDataToArray(uBufferSizeColor, mColorImg.data, ColorImageFormat_Bgra);
 			pFrameColor->Release();
			pFrameColor = nullptr;
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
#pragma endregion KinectStreamUpdate
 		namedWindow("Depth Map");
		namedWindow("Color Map");
 		mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);
		imshow("Depth Map", mImg8bit);
 		cvSetMouseCallback("Color Map", onMouseROI, NULL);
#pragma region FlashScreen
		//如果都是false的话就说明鼠标无活动
		if (ROI_S1 == false && ROI_S2 == false)
		{
			// Initial State
			ROI = mColorImg.clone();
			imshow("Color Map", ROI);
		}
		else if (ROI_S2 == true)
		{
			// CV_EVENT_LBUTTONUP：ROI選取完畢
			if (ROI_S1 == false)
			{
				int thickness = 2;
 				ROI = mColorImg.clone();
				rectangle(ROI, ROI_p1, ROI_p2, Scalar(255, 255, 0), thickness);
 				for (int i = 0; i < imageCorners_Machine.size(); i++)
				{
					circle(ROI, imageCorners_Machine.at(i), 1.5, Scalar(0, 0, 255));//这里是为了让机器移动到一个点的时候给他画一个小圆圈
				}
				imshow("Color Map", ROI);
 				FindROI();
			}
			//CV_EVENT_MOUSEMOVE：只选择了左上角的点 
			else
			{
				imshow("Color Map", ROI);
			}
 		}
#pragma endregion  FlashScreen
		//just get the photo of all the chessboard images
		key = cvWaitKey(5);
 		if (key == VK_ESCAPE)
		{
			break;
		}
		else if (key == 's' || key == 'S') //按键S可以存储现在的照片
		{
			Mat fourChannel;
			cvtColor(mColorImg, fourChannel, CV_BGR2GRAY);
 			sprintf(fileidx, "%03d", file_idx++);		
			strcpy(filename, "SAVE_IMG");
			strcat(filename, fileidx);
			strcat(filename, ".bmp");
 			imwrite(filename, fourChannel);
			printf("Image file %s saved.\n", filename);
 		}
		else if (key == 'q' || key == 'Q')//按键Q,开始让机器跑动
		{
			if (ROICenterColorS_Old.x != 0 && ROICenterColorS_Old.y != 0)
			{
				Thread^ thread1 = gcnew::Thread(gcnew::ThreadStart(MtMove));
				thread1->Name = "thread1";
 				thread1->Start();
			}
		}
		else if (key == 'r' || key == 'R')
		{
			file_idx = 0;
		}
		else if (key == 'c' || key == 'C')//按键C使用保存的图片生成后面的内容
		{
			destroyWindow("Depth Map");
			destroyWindow("Color Map");
 			Mat img;
			Mat img2;
 			Mat map1, map2;
 			int loop = 1;
			int key;
			int file_idx = 0;
			int file_idx2 = 0;
			char filename[256];
			char fileidx[10];
 			int i;
 			// The corners are at 3D location (X,Y,Z)= (i,j,0)因为矫正板是一个面，所以Z=0
			for (int i = 0; i<boardSize_ChessBoard.height; i++)
			{
				for (int j = 0; j<boardSize_ChessBoard.width; j++)
				{					
					//根据矫正板的角点数目存取一个阵列，被存取进入objectCorners_ChessBoard
					objectCorners_ChessBoard.push_back(Point3f(0.025*i, 0.025*j, 0.0f));
				}
			}
 			//Find corners of Chess board and fill the array
			while (loop)
			{
				// find file: sprintf把格式化的数据写入字符串中(buffer, format, argument)
				sprintf(fileidx, "%03d", file_idx++);				
				strcpy(filename, "SAVE_IMG");
				strcat(filename, fileidx);
				strcat(filename, ".bmp");
 				printf("IMG = %s\n", filename);
 				if (ifstream(filename))
				{				
 					img = imread(filename, 0);
 					cvNamedWindow("File");
					imshow("File", img);
 					cvWaitKey(400);
 					cout << img.channels() << endl;
					cout << img.depth() << endl;
					cout << img.type() << endl;
					cout << (int)img.at<uchar>(100,100) << endl;
 					if (img.empty())
						loop = 0;
					else
					{
						bool found = cv::findChessboardCorners(
							img, //输入图，必须为8位元的灰阶或者彩色图像
							boardSize_ChessBoard,  //棋盘的尺寸
							imageCorners_ChessBoard //输出角点
							);
						//找到显示为true，找不到显示为false
 						cout << imageCorners_ChessBoard.size() << endl;
 						if (found)
						{
							//OpenCV找精确角点
							cornerSubPix(
								img, //输入图
								imageCorners_ChessBoard, //输入角点
								cv::Size(11, 11), //搜索范围
								cv::Size(-1, -1),
								cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, //迭代搜索的终止条件
								30,      // max number of iterations 迭代的最大次数
								0.1));   // min accuracy 
						}
						else
						{
							cout << endl << "No corner was found" << endl;
							continue;
						}
						//棋盘的尺寸和抓到的角点的尺寸相同
 						if (imageCorners_ChessBoard.size() == boardSize_ChessBoard.area())
						{
							imagePoints_ChessBoard.push_back(imageCorners_ChessBoard);//抓到的图像的角点被放入imagePoints_ChessBoard
							objectPoints_ChessBoard.push_back(objectCorners_ChessBoard);//objectCorners_ChessBoard就是棋盘格应该的值
						}
						img.copyTo(img2);
						cvtColor(img2, img2, CV_GRAY2BGR);
						drawChessboardCorners(img2, boardSize_ChessBoard, imageCorners_ChessBoard, found);
 						sprintf(fileidx, "%03d", file_idx2++);
						strcpy(filename, "Corner_IMG");
						strcat(filename, fileidx);
						strcat(filename, ".bmp");
 						imwrite(filename, img2);
						printf("Image file %s saved.\n", filename);
 						imshow("image", img2);
 						key = cvWaitKey(400);
					}
				}
				else
				{
					cout << endl << "File not exist" << endl;
					break;
				}
 			}
 			//Calibration of chessboard
			vector<Mat> rvecsC, tvecsC;
			/*机器跑出来的点和抓到的图像上的点进行calibrate，呼叫calibrateCamera()得到相機矩陣cameraMatrix
			以及畸變矩陣distCoeffs，這兩個矩陣會在後續校正時使用*/
			//第一个calibrate是为了得到相机的内部参数矩阵
 			calibrateCamera(objectPoints_ChessBoard, // the 3D points 校正坐标系统的位置：三维坐标 根据矫正板的点的数量的多少放置的角点相对于机器的坐标
				imagePoints_ChessBoard, // the image points 校正点的投影：二维坐标 矫正板抓取到的角点在空间中相对于像素坐标的实际坐标
				imageSize,   // image size 影像的尺寸：图像大小
				cameraMatrix_ChessBoard,// output camera matrix 输出3*3浮点数的相机矩阵：相机内参数矩阵
				distCoeffs_ChessBoard,  // output distortion matrix 输出的畸变参数：投影矩阵
				rvecsC, //rotate 旋转
				tvecsC,// translation 平移
				0); //标记opencv提供几种参数
			// set options
			//误差计算 https://blog.csdn.net/u011574296/article/details/73823569
 			//这个是为了能够得到像素坐标转换到机器坐标的矩阵
			vector<Mat> rvecsM, tvecsM;
 			calibrateCamera(objectPoints_Machine, // the 3D points 点胶机理想的跑出红色点的值相对于机器坐标系的坐标
				imagePoints_Machine, // the image points 点胶机实际跑出来的红色点的值相对于像素坐标系的坐标
				imageSize,   // image size
				cameraMatrix_Machine,// output camera matrix
				distCoeffs_Machine,  // output distortion matrix
				rvecsM, tvecsM,// Rs, Ts
				0);       

			// set options
			//Fill all corners into corresponding array to increase accuracy
			vector<Point2f> imageCorners_H;
			vector<Point3f> objectCorners_H;
			for (int num = 0; num < objectPoints_Machine.size(); num++)
			{
				for (int arr_num = 0; arr_num < objectPoints_Machine.at(num).size(); arr_num++)
				{
					//输出t的倒数第四步
					imageCorners_H.push_back(imagePoints_Machine.at(num).at(arr_num));//点胶机点在像素坐标系坐标
					objectCorners_H.push_back(objectPoints_Machine.at(num).at(arr_num));//点胶机点在机器坐标系坐标,(0,0,0)
				}
			}

			//其中objectCorners_H为0，0.25这样的点，以第一个点为0，0，0，改变的是X,Y的值
			//其中imageCorners_H在图像坐标系UV坐标系，是像素的坐标，findHomography(src,dst)
			Mat H = findHomography(objectCorners_H, imageCorners_H);
			//从机器坐标系到像素坐标系的单应性矩阵
			
			PrintMat(H);
			//cameraMatrix_ChessBoard Kinect 相机的内参数矩阵
			Mat Rt = cameraMatrix_ChessBoard.inv()*H;

 			Mat RR = Mat::zeros(3, 3, CV_32F);
			Rt.copyTo(RR);
			//col影像的宽
			RR.col(0) = RR.col(0) / norm(RR.col(0));//r1 
			RR.col(1) = RR.col(1) / norm(RR.col(1));//r2 
			RR.col(0).cross(RR.col(1)).copyTo(RR.col(2));// 


 			SVD de_Rt(RR);

			//SVD分解矩阵成
			//t()转置矩阵
			Mat Out_Rt = de_Rt.vt.t()*de_Rt.u.t();
 			
			ofstream fs2;
			fs2.open("Coord.txt");
			//输出t的倒数第一步
			fs2 << "t" << endl << Rt.col(2) << endl << endl;
			fs2 << "R" << endl << Out_Rt << endl << endl;
			fs2 << "R.inv()" << endl << Out_Rt.inv() << endl;
			fs2.close();
 			ofstream fs;
			fs.open("test_Machine.txt");
			fs << "intrinsic" << cameraMatrix_Machine << endl << endl;
			fs << "dist-coeff" << distCoeffs_Machine << endl << endl;
 			fs << "fx = " << cameraMatrix_Machine.at<double>(0) << endl;
			fs << "fy = " << cameraMatrix_Machine.at<double>(4) << endl;
			fs << "cx = " << cameraMatrix_Machine.at<double>(2) << endl;
			fs << "cy = " << cameraMatrix_Machine.at<double>(5) << endl << endl;
 			fs << "k1 = " << cameraMatrix_Machine.at<double>(0) << endl;
			fs << "k2 = " << cameraMatrix_Machine.at<double>(1) << endl;
			fs << "p1 = " << cameraMatrix_Machine.at<double>(2) << endl;
			fs << "p2 = " << cameraMatrix_Machine.at<double>(3) << endl;
			fs << "k3 = " << cameraMatrix_Machine.at<double>(4) << endl;
 			fs.close();
 			ofstream fs3;
			fs3.open("test.txt");
			fs3 << "intrinsic" << cameraMatrix_ChessBoard << endl << endl;
			fs3 << "dist-coeff" << distCoeffs_ChessBoard << endl << endl;
 			fs3 << "fx = " << cameraMatrix_ChessBoard.at<double>(0) << endl;
			fs3 << "fy = " << cameraMatrix_ChessBoard.at<double>(4) << endl;
			fs3 << "cx = " << cameraMatrix_ChessBoard.at<double>(2) << endl;
			fs3 << "cy = " << cameraMatrix_ChessBoard.at<double>(5) << endl << endl;
 			fs3 << "k1 = " << cameraMatrix_ChessBoard.at<double>(0) << endl;
			fs3 << "k2 = " << cameraMatrix_ChessBoard.at<double>(1) << endl;
			fs3 << "p1 = " << cameraMatrix_ChessBoard.at<double>(2) << endl;
			fs3 << "p2 = " << cameraMatrix_ChessBoard.at<double>(3) << endl;
			fs3 << "k3 = " << cameraMatrix_ChessBoard.at<double>(4) << endl;
 			fs3.close();
 		}
 	}
 	// 3b. release frame reader
	pFrameReaderColor->Release();
	pFrameReaderColor = nullptr;
 	pFrameReaderDepth->Release();
	pFrameReaderDepth = nullptr;
 	// 1c. Close Sensor

	pSensor->Close();
 	// 1d. Release Sensor
	pSensor->Release();
	pSensor = nullptr;
	MtHome();
	return 0;
}