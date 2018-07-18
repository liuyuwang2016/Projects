#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>
#include <Kinect.h>

using namespace cv;
using namespace std;

bool backprojMode = false;//表示是否要进入反向投影模式，true则表示要进入反向投影模式
bool selectObject = false;//表是否在选中要跟踪的初始目标，true表示正在用鼠标选择要跟踪的目标
int trackObject = 0;//跟踪目标的数目
bool showHist = true;//是否显示HUE分量直方图
Point origin;//用于保存鼠标选择第一次单击时点的位置
Rect selection;//用于保存鼠标选择的矩形框
int vmin = 10, vmax = 256, smin = 30;
Mat image;
Mat map_x, map_y;

static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)//鼠标左键按下时，则条件为true，在if里面的代码块就是确定所选择的矩形区域selection
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);//矩形左上角顶点的坐标
		selection.width = std::abs(x - origin.x);//矩形宽
		selection.height = std::abs(y - origin.y);//矩形高

		selection &= Rect(0, 0, image.cols, image.rows);//用于确保所选的矩形区域在图片区域内
	}

	switch (event)//鼠标事件
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);//鼠标刚按下去时初始化了一个矩形区域
		selectObject = true;
		break;
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1;
		break;
	}
}

static void help()//帮助提示文档
{
	cout << "\nThis is a demo that shows mean-shift based tracking\n"
		"You select a color objects such as your face and it tracks it.\n"
		"This reads from video camera (0 by default, or the camera number the user enters\n"
		"Usage: \n"
		"   ./camshiftdemo [camera number]\n";

	cout << "\n\nHot keys: \n"
		"\tESC - quit the program\n"
		"\tc - stop the tracking\n"
		"\tb - switch to/from backprojection view\n"
		"\th - show/hide object histogram\n"
		"\tp - pause video\n"
		"To initialize tracking, select the object with mouse\n";
}


int main(int argc, const char** argv)
{
	help();
	// 1a. Get default Sensor
	cout << "Try to get default sensor" << endl;
	IKinectSensor* pSensor = nullptr;
	if (GetDefaultKinectSensor(&pSensor) != S_OK)
	{
		cerr << "Get Sensor failed" << endl;
		return -1;
	}

	// 1b. Open sensor
	cout << "Try to open sensor" << endl;
	if (pSensor->Open() != S_OK)
	{
		cerr << "Can't open sensor" << endl;
		return -1;
	}

	// 2a. Get frame source
	cout << "Try to get color source" << endl;
	IColorFrameSource* pFrameSource = nullptr;
	if (pSensor->get_ColorFrameSource(&pFrameSource) != S_OK)
	{
		cerr << "Can't get color frame source" << endl;
		return -1;
	}

	// 2b. Get frame description
	cout << "get color frame description" << endl;
	int		iWidth = 0;
	int		iHeight = 0;
	IFrameDescription* pFrameDescription = nullptr;
	if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
	{
		pFrameDescription->get_Width(&iWidth);
		pFrameDescription->get_Height(&iHeight);
	}
	pFrameDescription->Release();
	pFrameDescription = nullptr;

	// 3a. get frame reader
	cout << "Try to get color frame reader" << endl;
	IColorFrameReader* pFrameReader = nullptr;
	if (pFrameSource->OpenReader(&pFrameReader) != S_OK)
	{
		cerr << "Can't get color frame reader" << endl;
		return -1;
	}

	// 2c. release Frame source
	cout << "Release frame source" << endl;
	pFrameSource->Release();
	pFrameSource = nullptr;

	// Prepare OpenCV data
	cv::Mat	mColorImg(iHeight, iWidth, CV_8UC4);
	UINT uBufferSize = iHeight * iWidth * 4 * sizeof(BYTE);

	Rect trackWindow;
	int hsize = 16;
	float hranges[] = { 0, 180 };//hranges在后面的计算直方图函数中要用到
	const float* phranges = hranges;


	if (!mColorImg.data)//摄像头没有打开文档提示
	{
		help();
		cout << "***Could not initialize capturing from Kinect v2 camera...***\n";
		return false;
	}

	namedWindow("Histogram", 0);
	namedWindow("CamShift Demo", 0);
	setMouseCallback("CamShift Demo", onMouse, 0);//鼠标响应事件
	//createTrackbar("Vmin", "CamShift Demo", &vmin, 256, 0);//createTrackbar函数的功能是在对应的窗口创建滑动条，滑动条Vmin,vmin表示滑动条的值，最大为256
	//createTrackbar("Vmax", "CamShift Demo", &vmax, 256, 0);//最后一个参数为0代表没有调用滑动拖动的响应函数
	//createTrackbar("Smin", "CamShift Demo", &smin, 256, 0);//vmin,vmax,smin初始值分别为10，256，30

	Mat hsv, hue, mask, hist, histimg = Mat::zeros(iWidth, iHeight, CV_8UC3), backproj;
	bool paused = false;

	while (true)
	{
		IColorFrame* pFrame = nullptr;
		if (pFrameReader->AcquireLatestFrame(&pFrame) == S_OK)
		{
			// 4c. Copy to OpenCV image
			if (pFrame->CopyConvertedFrameDataToArray(uBufferSize, mColorImg.data, ColorImageFormat_Bgra) == S_OK)
			{
				//map_x.create(mColorImg.size(), CV_32FC1);
				//map_y.create(mColorImg.size(), CV_32FC1);

				//for (int j = 0; j < mColorImg.rows; j++)
				//{
				//	for (int i = 0; i < mColorImg.cols; i++)
				//	{
				//		map_x.at<float>(j, i) = static_cast<float>(mColorImg.cols - i);
				//		map_y.at<float>(j, i) = static_cast<float>(j);
				//	}
				//}
				////成功remap。重映射将Kinect抓取的彩色图案反转
				//remap(mColorImg, mColorImg, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
				mColorImg.copyTo(image);

				if (!paused)
				{
					cvtColor(image, hsv, COLOR_BGR2HSV);//将rgb摄像头帧转化成hsv空间的

					if (trackObject)//trackObject初始化为0，或者键盘的c按完后也为0，当鼠标单击松开后为-1
					{
						int _vmin = vmin, _vmax = vmax;
						//inRange函数的功能是检查输入数组每个元素大小是否在2个给定数值之间，可以是多通道的,mask保存0通道的最小值，也就是h分量
						//这里利用了hsv的3个通道，比较h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)。如果3个通道都在对应的范围内，则
						//mask对应的那个点的值全为1(0xff)，否则为0(0x00).


						inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
							Scalar(180, 256, MAX(_vmin, _vmax)), mask);
						int ch[] = { 0, 0 };
						hue.create(hsv.size(), hsv.depth());//hue初始化为与hsv大小深度一样的矩阵，色调的度量是用角度表示的，红绿蓝之间相差120度，反色相差180度
						mixChannels(&hsv, 1, &hue, 1, ch, 1);//将hsv第一个通道(也就是色调)的数复制到hue中，0索引数组

						if (trackObject < 0)//鼠标选择区域松开后，该函数内部又将其赋值1
						{
							//此处的构造函数roi用的是Mat hue的矩阵头，且roi的数据指针指向hue，即共用相同的数据，select为其感兴趣的区域
							Mat roi(hue, selection), maskroi(mask, selection);
							//calcHist()函数第一个参数为输入矩阵序列，第2个参数表示输入的矩阵数目，第3个参数表示将被计算直方图维数通道的列表，第4个参数表示可选的掩码函数
							//第5个参数表示输出直方图，第6个参数表示直方图的维数，第7个参数为每一维直方图数组的大小，第8个参数为每一维直方图bin的边界
							calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);//将roi的0通道计算直方图并通过mask放入hist中，hsize为每一维直方图的大小
							normalize(hist, hist, 0, 255, CV_MINMAX);//将hist矩阵进行数组范围归一化，都归一化到0-255

							trackWindow = selection;
							trackObject = 1;//只要鼠标选完区域松开后，且没有按键盘清0键'c'，则trackObject一直保持为1，因此该if函数只能执行一次，除非重新选择跟踪区域


							histimg = Scalar::all(0);//与按下‘c’键是一样的，这里的all（0）表示的是标量全部清0
							int binW = histimg.cols / hsize;//histing是一个200*300的矩阵，hsize应该是每一个bin的宽度，也就是histing矩阵能分出几个bin出来

							Mat buf(1, hsize, CV_8UC3);//定义一个缓冲单bin矩阵
							for (int i = 0; i < hsize; i++)//saturate_case函数为从一个初始类型准确变换到另一个初始类型
								buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
							cvtColor(buf, buf, CV_HSV2BGR);//hsv又转换成bgr

							for (int i = 0; i < hsize; i++)
							{
								int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);//at函数为返回一个指定数组元素的参考值
								//在一幅输入图像上画一个简单抽的矩形，指定左上角和右下角，并定义颜色，大小，线型等
								rectangle(histimg, Point(i*binW, histimg.rows),
									Point((i + 1)*binW, histimg.rows - val),
									Scalar(buf.at<Vec3b>(i)), -1, 8);
							}
						}
						//计算直方图的反向投影，计算hue图像0通道直方图hist的反向投影，并让入backproj中
						calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
						backproj &= mask;
						RotatedRect trackBox = CamShift(backproj, trackWindow,
							TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1)); //trackWindow为鼠标选择的区域，TermCriteria为确定迭代终止的准则

						if (trackWindow.area() <= 1)
						{
							int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
							trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
								trackWindow.x + r, trackWindow.y + r) &
								Rect(0, 0, cols, rows);
						}

						if (backprojMode)
							cvtColor(backproj, image, COLOR_GRAY2BGR);
						ellipse(image, trackBox, Scalar(0, 0, 255), 3, CV_AA);//跟踪的时候以椭圆为代表目标
					}
				}
				else if (trackObject < 0)
					paused = false;

				if (selectObject && selection.width > 0 && selection.height > 0)
				{
					Mat roi(image, selection);
					bitwise_not(roi, roi);// bitwise_not为将每一个bit位取反
				}

				imshow("CamShift Demo", image);
				imshow("Histogram", histimg);
			}
			else
			{
				cerr << "Data copy error" << endl;
			}

			// 4e. release frame
			pFrame->Release();
		}

		char c = (char)waitKey(10);
		if (c == 27)//退出键
			break;
		switch (c)
		{
		case 'b'://反向投影模型交替
			backprojMode = !backprojMode;
			break;
		case 'c'://清零跟踪目标对象
			trackObject = 0;
			histimg = Scalar::all(0);
			break;
		case 'h'://显示直方图交替
			showHist = !showHist;
			if (!showHist)
				destroyWindow("Histogram");
			else
				namedWindow("Histogram", 1);
			break;
		case 'p'://暂停跟踪交替
			paused = !paused;
			break;
		default:
			;
		}
	}
	// 3b. release frame reader
	cout << "Release frame reader" << endl;
	pFrameReader->Release();
	pFrameReader = nullptr;

	// 1c. Close Sensor
	cout << "close sensor" << endl;
	pSensor->Close();

	// 1d. Release Sensor
	cout << "Release sensor" << endl;
	pSensor->Release();
	pSensor = nullptr;
	return 0;
}