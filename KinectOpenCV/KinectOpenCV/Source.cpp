#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>
#include <Kinect.h>

using namespace cv;
using namespace std;

bool backprojMode = false;//��ʾ�Ƿ�Ҫ���뷴��ͶӰģʽ��true���ʾҪ���뷴��ͶӰģʽ
bool selectObject = false;//���Ƿ���ѡ��Ҫ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��Ҫ���ٵ�Ŀ��
int trackObject = 0;//����Ŀ�����Ŀ
bool showHist = true;//�Ƿ���ʾHUE����ֱ��ͼ
Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��
Rect selection;//���ڱ������ѡ��ľ��ο�
int vmin = 10, vmax = 256, smin = 30;
Mat image;
Mat map_x, map_y;

static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)//����������ʱ��������Ϊtrue����if����Ĵ�������ȷ����ѡ��ľ�������selection
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);//�������ϽǶ��������
		selection.width = std::abs(x - origin.x);//���ο�
		selection.height = std::abs(y - origin.y);//���θ�

		selection &= Rect(0, 0, image.cols, image.rows);//����ȷ����ѡ�ľ���������ͼƬ������
	}

	switch (event)//����¼�
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);//���հ���ȥʱ��ʼ����һ����������
		selectObject = true;
		break;
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1;
		break;
	}
}

static void help()//������ʾ�ĵ�
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
	float hranges[] = { 0, 180 };//hranges�ں���ļ���ֱ��ͼ������Ҫ�õ�
	const float* phranges = hranges;


	if (!mColorImg.data)//����ͷû�д��ĵ���ʾ
	{
		help();
		cout << "***Could not initialize capturing from Kinect v2 camera...***\n";
		return false;
	}

	namedWindow("Histogram", 0);
	namedWindow("CamShift Demo", 0);
	setMouseCallback("CamShift Demo", onMouse, 0);//�����Ӧ�¼�
	//createTrackbar("Vmin", "CamShift Demo", &vmin, 256, 0);//createTrackbar�����Ĺ������ڶ�Ӧ�Ĵ��ڴ�����������������Vmin,vmin��ʾ��������ֵ�����Ϊ256
	//createTrackbar("Vmax", "CamShift Demo", &vmax, 256, 0);//���һ������Ϊ0����û�е��û����϶�����Ӧ����
	//createTrackbar("Smin", "CamShift Demo", &smin, 256, 0);//vmin,vmax,smin��ʼֵ�ֱ�Ϊ10��256��30

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
				////�ɹ�remap����ӳ�佫Kinectץȡ�Ĳ�ɫͼ����ת
				//remap(mColorImg, mColorImg, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, cv::Scalar(0, 0, 0));
				mColorImg.copyTo(image);

				if (!paused)
				{
					cvtColor(image, hsv, COLOR_BGR2HSV);//��rgb����ͷ֡ת����hsv�ռ��

					if (trackObject)//trackObject��ʼ��Ϊ0�����߼��̵�c�����ҲΪ0������굥���ɿ���Ϊ-1
					{
						int _vmin = vmin, _vmax = vmax;
						//inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬�����Ƕ�ͨ����,mask����0ͨ������Сֵ��Ҳ����h����
						//����������hsv��3��ͨ�����Ƚ�h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)�����3��ͨ�����ڶ�Ӧ�ķ�Χ�ڣ���
						//mask��Ӧ���Ǹ����ֵȫΪ1(0xff)������Ϊ0(0x00).


						inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
							Scalar(180, 256, MAX(_vmin, _vmax)), mask);
						int ch[] = { 0, 0 };
						hue.create(hsv.size(), hsv.depth());//hue��ʼ��Ϊ��hsv��С���һ���ľ���ɫ���Ķ������ýǶȱ�ʾ�ģ�������֮�����120�ȣ���ɫ���180��
						mixChannels(&hsv, 1, &hue, 1, ch, 1);//��hsv��һ��ͨ��(Ҳ����ɫ��)�������Ƶ�hue�У�0��������

						if (trackObject < 0)//���ѡ�������ɿ��󣬸ú����ڲ��ֽ��丳ֵ1
						{
							//�˴��Ĺ��캯��roi�õ���Mat hue�ľ���ͷ����roi������ָ��ָ��hue����������ͬ�����ݣ�selectΪ�����Ȥ������
							Mat roi(hue, selection), maskroi(mask, selection);
							//calcHist()������һ������Ϊ����������У���2��������ʾ����ľ�����Ŀ����3��������ʾ��������ֱ��ͼά��ͨ�����б���4��������ʾ��ѡ�����뺯��
							//��5��������ʾ���ֱ��ͼ����6��������ʾֱ��ͼ��ά������7������Ϊÿһάֱ��ͼ����Ĵ�С����8������Ϊÿһάֱ��ͼbin�ı߽�
							calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);//��roi��0ͨ������ֱ��ͼ��ͨ��mask����hist�У�hsizeΪÿһάֱ��ͼ�Ĵ�С
							normalize(hist, hist, 0, 255, CV_MINMAX);//��hist����������鷶Χ��һ��������һ����0-255

							trackWindow = selection;
							trackObject = 1;//ֻҪ���ѡ�������ɿ�����û�а�������0��'c'����trackObjectһֱ����Ϊ1����˸�if����ֻ��ִ��һ�Σ���������ѡ���������


							histimg = Scalar::all(0);//�밴�¡�c������һ���ģ������all��0����ʾ���Ǳ���ȫ����0
							int binW = histimg.cols / hsize;//histing��һ��200*300�ľ���hsizeӦ����ÿһ��bin�Ŀ�ȣ�Ҳ����histing�����ֳܷ�����bin����

							Mat buf(1, hsize, CV_8UC3);//����һ�����嵥bin����
							for (int i = 0; i < hsize; i++)//saturate_case����Ϊ��һ����ʼ����׼ȷ�任����һ����ʼ����
								buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
							cvtColor(buf, buf, CV_HSV2BGR);//hsv��ת����bgr

							for (int i = 0; i < hsize; i++)
							{
								int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);//at����Ϊ����һ��ָ������Ԫ�صĲο�ֵ
								//��һ������ͼ���ϻ�һ���򵥳�ľ��Σ�ָ�����ϽǺ����½ǣ���������ɫ����С�����͵�
								rectangle(histimg, Point(i*binW, histimg.rows),
									Point((i + 1)*binW, histimg.rows - val),
									Scalar(buf.at<Vec3b>(i)), -1, 8);
							}
						}
						//����ֱ��ͼ�ķ���ͶӰ������hueͼ��0ͨ��ֱ��ͼhist�ķ���ͶӰ��������backproj��
						calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
						backproj &= mask;
						RotatedRect trackBox = CamShift(backproj, trackWindow,
							TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1)); //trackWindowΪ���ѡ�������TermCriteriaΪȷ��������ֹ��׼��

						if (trackWindow.area() <= 1)
						{
							int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
							trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
								trackWindow.x + r, trackWindow.y + r) &
								Rect(0, 0, cols, rows);
						}

						if (backprojMode)
							cvtColor(backproj, image, COLOR_GRAY2BGR);
						ellipse(image, trackBox, Scalar(0, 0, 255), 3, CV_AA);//���ٵ�ʱ������ԲΪ����Ŀ��
					}
				}
				else if (trackObject < 0)
					paused = false;

				if (selectObject && selection.width > 0 && selection.height > 0)
				{
					Mat roi(image, selection);
					bitwise_not(roi, roi);// bitwise_notΪ��ÿһ��bitλȡ��
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
		if (c == 27)//�˳���
			break;
		switch (c)
		{
		case 'b'://����ͶӰģ�ͽ���
			backprojMode = !backprojMode;
			break;
		case 'c'://�������Ŀ�����
			trackObject = 0;
			histimg = Scalar::all(0);
			break;
		case 'h'://��ʾֱ��ͼ����
			showHist = !showHist;
			if (!showHist)
				destroyWindow("Histogram");
			else
				namedWindow("Histogram", 1);
			break;
		case 'p'://��ͣ���ٽ���
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