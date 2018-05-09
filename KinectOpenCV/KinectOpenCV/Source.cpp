/*************************************************************************
> File Name: main.cpp
> Author:yanxj
> Mail: cn_yanxj@foxmail.com
> Created Time: 2017年11月06日 星期一 20时59分23秒
************************************************************************/
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;

bool selectObject = false; //用于标记是否有选取目标
int trackObject = 0;    //1表示由追踪对象，0表示无追踪对象 -1 表示追踪对象尚未计算Camshift 所需的属性

cv::Rect selection;     //保存鼠标选择的区域

cv::Mat image;          //用于缓存读取到的视频帧

//OpenCV 对所注册的鼠标毁掉函数定义为：
//void onMouse(int event, int x,int y,int flags,void *param)
//其中第四个参数flag为event下的附加状态，param 是用户传入的参数，我们都不需要使用
//故不填写参数名
void onMouse(int event, int x, int y, int, void*)
{
	static cv::Point origin;
	if (selectObject)
	{
		//确定鼠标选定区域的左上角坐标以及区域的长和宽
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = abs(x - origin.x);
		selection.height = abs(y - origin.y);

		// & 运算符被cv::Rect重载
		// 表示两个区域交集，主要目的是为了处理当鼠标在选择区域时移除画面外
		selection &= cv::Rect(0, 0, image.cols, image.rows);
	}

	switch (event){
		//处理鼠标左键被按下
	case CV_EVENT_LBUTTONDOWN:
		origin = cv::Point(x, y);
		selection = cv::Rect(x, y, 0, 0);
		selectObject = true;
		break;
		//处理鼠标左键被抬起
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1; //追踪的目标还未计算Camshift所需的属性
		break;
	}
}


int main(int argc, const char** argv)
{
	//创建一个视频捕捉对象
	//OpenCV 提供了一个VideoCapture 对象，它屏蔽了
	//从文件读取视频流从摄像头读取摄像头的差异，当构造
	//函数参数为文件路径时，会从文件读取视频流，当构造函数
	//参数为设备编号时（第几个摄像头，通常只有一个摄像头时为0）
	//会从摄像头处读取视频流
	cv::VideoCapture video("video.ogv");//读取文件
	//cv::VideoCapture video(0);//使用摄像头

	cv::namedWindow("CamShift at ");

	//1.注册鼠标事件的回调函数，第三个参数时用户提供给回调函数的，也就是回调函数中最后的param参数
	cv::setMouseCallback("CamShift at ", onMouse, 0);

	//捕获画面的容器，OpenCV中的Mat对象
	//OpenCV中最关键的Mat类，Mat是Matrix（矩阵）
	//的缩写，OpenCV中延续了像素图的概念，用矩阵来描述
	//由像素构成的图像
	cv::Mat frame;  //接受来自video视频流中的图像帧
	cv::Mat hsv, hue, mask, hist, backproj;
	cv::Rect trackWindow;   //追踪到的窗口
	int hsize = 16;             //计算直方图所必备的内容
	float hranges[] = { 0, 180 };  //计算直方图所必备的内容
	const float* phranges = hranges; //计算直方图所必备的内容


	//从视频流中读取图像
	while (true){

		//将video中的内容写道frame中
		//这里>>运算符经过OpenCV重载的
		video >> frame;

		//当没有帧可以继续读取时，退出循环
		if (frame.empty()) break;

		//将frame 中的图像写入全局变量image作为进行camshift的缓存
		frame.copyTo(image);

		//转换到HSV空间
		cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

		//当有目标时开始处理
		if (trackObject)
		{

			//只处理像素值为H：0～180,S：30～256，V：10～256之间的部分，过滤掉其他部分并复制给mask
			cv::inRange(hsv, cv::Scalar(0, 30, 10), cv::Scalar(180, 256, 256), mask);
			//下面三句将hsv图像中的H 通道分离出来
			int ch[] = { 0, 0 };
			hue.create(hsv.size(), hsv.depth());
			cv::mixChannels(&hsv, 1, &hue, 1, ch, 1);

			//如果需要追踪的物体还没有进行属性提取，则对选择的目标中的图像属性提取
			if (trackObject < 0){

				//设置H 通道和mask图像的ROI
				cv::Mat roi(hue, selection), maskroi(mask, selection);
				//计算ROI所在区域的直方图
				calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
				//将直方图归一
				normalize(hist, hist, 0, 255, CV_MINMAX);

				//设置追踪窗口
				trackWindow = selection;

				//标记追踪的目标已经计算过直方图属性
				trackObject = 1;
			}
			//将直方图进行反向投影
			calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
			//取公共部分
			backproj &= mask;
			//调用Camshift 算法接口
			cv::RotatedRect trackBox = CamShift(backproj, trackWindow, cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));
			//处理追踪面积过小的情况
			if (trackWindow.area() <= 1){
				int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
				trackWindow = cv::Rect(trackWindow.x - r, trackWindow.y - r, trackWindow.x + r, trackWindow.y + r)& cv::Rect(0, 0, cols, rows);
			}
			//绘制追踪区域
			ellipse(image, trackBox, cv::Scalar(0, 0, 255), 3, CV_AA);
		}

		//如果正在选择追踪目标，则画出选择框
		if (selectObject && selection.width > 0 && selection.height > 0)
		{
			cv::Mat roi(image, selection);
			bitwise_not(roi, roi);   //对选择的区域图像反色
		}


		//显示当前帧
		imshow("CamShift at yanxj", image);

		//录视频帧率为15,等待1000/15保证视频播放流畅
		//waitKey(int delay) 是OpenCV 提供的一个等待函数，
		//当运行到这个函数时会阻塞delay毫秒的时间来等待键盘输入
		int Key = cv::waitKey(1000 / 15.0);

		//当按键为ESC时，退出循环
		if (Key == 27) break;
	}

	//释放申请相关内存
	cv::destroyAllWindows();
	video.release();
	return 0;
}

