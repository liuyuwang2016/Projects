/*************************************************************************
> File Name: main.cpp
> Author:yanxj
> Mail: cn_yanxj@foxmail.com
> Created Time: 2017��11��06�� ����һ 20ʱ59��23��
************************************************************************/
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;

bool selectObject = false; //���ڱ���Ƿ���ѡȡĿ��
int trackObject = 0;    //1��ʾ��׷�ٶ���0��ʾ��׷�ٶ��� -1 ��ʾ׷�ٶ�����δ����Camshift ���������

cv::Rect selection;     //�������ѡ�������

cv::Mat image;          //���ڻ����ȡ������Ƶ֡

//OpenCV ����ע������ٵ���������Ϊ��
//void onMouse(int event, int x,int y,int flags,void *param)
//���е��ĸ�����flagΪevent�µĸ���״̬��param ���û�����Ĳ��������Ƕ�����Ҫʹ��
//�ʲ���д������
void onMouse(int event, int x, int y, int, void*)
{
	static cv::Point origin;
	if (selectObject)
	{
		//ȷ�����ѡ����������Ͻ������Լ�����ĳ��Ϳ�
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = abs(x - origin.x);
		selection.height = abs(y - origin.y);

		// & �������cv::Rect����
		// ��ʾ�������򽻼�����ҪĿ����Ϊ�˴��������ѡ������ʱ�Ƴ�������
		selection &= cv::Rect(0, 0, image.cols, image.rows);
	}

	switch (event){
		//����������������
	case CV_EVENT_LBUTTONDOWN:
		origin = cv::Point(x, y);
		selection = cv::Rect(x, y, 0, 0);
		selectObject = true;
		break;
		//������������̧��
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1; //׷�ٵ�Ŀ�껹δ����Camshift���������
		break;
	}
}


int main(int argc, const char** argv)
{
	//����һ����Ƶ��׽����
	//OpenCV �ṩ��һ��VideoCapture ������������
	//���ļ���ȡ��Ƶ��������ͷ��ȡ����ͷ�Ĳ��죬������
	//��������Ϊ�ļ�·��ʱ������ļ���ȡ��Ƶ���������캯��
	//����Ϊ�豸���ʱ���ڼ�������ͷ��ͨ��ֻ��һ������ͷʱΪ0��
	//�������ͷ����ȡ��Ƶ��
	cv::VideoCapture video("video.ogv");//��ȡ�ļ�
	//cv::VideoCapture video(0);//ʹ������ͷ

	cv::namedWindow("CamShift at ");

	//1.ע������¼��Ļص�����������������ʱ�û��ṩ���ص������ģ�Ҳ���ǻص�����������param����
	cv::setMouseCallback("CamShift at ", onMouse, 0);

	//�������������OpenCV�е�Mat����
	//OpenCV����ؼ���Mat�࣬Mat��Matrix������
	//����д��OpenCV������������ͼ�ĸ���þ���������
	//�����ع��ɵ�ͼ��
	cv::Mat frame;  //��������video��Ƶ���е�ͼ��֡
	cv::Mat hsv, hue, mask, hist, backproj;
	cv::Rect trackWindow;   //׷�ٵ��Ĵ���
	int hsize = 16;             //����ֱ��ͼ���ر�������
	float hranges[] = { 0, 180 };  //����ֱ��ͼ���ر�������
	const float* phranges = hranges; //����ֱ��ͼ���ر�������


	//����Ƶ���ж�ȡͼ��
	while (true){

		//��video�е�����д��frame��
		//����>>���������OpenCV���ص�
		video >> frame;

		//��û��֡���Լ�����ȡʱ���˳�ѭ��
		if (frame.empty()) break;

		//��frame �е�ͼ��д��ȫ�ֱ���image��Ϊ����camshift�Ļ���
		frame.copyTo(image);

		//ת����HSV�ռ�
		cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

		//����Ŀ��ʱ��ʼ����
		if (trackObject)
		{

			//ֻ��������ֵΪH��0��180,S��30��256��V��10��256֮��Ĳ��֣����˵��������ֲ����Ƹ�mask
			cv::inRange(hsv, cv::Scalar(0, 30, 10), cv::Scalar(180, 256, 256), mask);
			//�������佫hsvͼ���е�H ͨ���������
			int ch[] = { 0, 0 };
			hue.create(hsv.size(), hsv.depth());
			cv::mixChannels(&hsv, 1, &hue, 1, ch, 1);

			//�����Ҫ׷�ٵ����廹û�н���������ȡ�����ѡ���Ŀ���е�ͼ��������ȡ
			if (trackObject < 0){

				//����H ͨ����maskͼ���ROI
				cv::Mat roi(hue, selection), maskroi(mask, selection);
				//����ROI���������ֱ��ͼ
				calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
				//��ֱ��ͼ��һ
				normalize(hist, hist, 0, 255, CV_MINMAX);

				//����׷�ٴ���
				trackWindow = selection;

				//���׷�ٵ�Ŀ���Ѿ������ֱ��ͼ����
				trackObject = 1;
			}
			//��ֱ��ͼ���з���ͶӰ
			calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
			//ȡ��������
			backproj &= mask;
			//����Camshift �㷨�ӿ�
			cv::RotatedRect trackBox = CamShift(backproj, trackWindow, cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));
			//����׷�������С�����
			if (trackWindow.area() <= 1){
				int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
				trackWindow = cv::Rect(trackWindow.x - r, trackWindow.y - r, trackWindow.x + r, trackWindow.y + r)& cv::Rect(0, 0, cols, rows);
			}
			//����׷������
			ellipse(image, trackBox, cv::Scalar(0, 0, 255), 3, CV_AA);
		}

		//�������ѡ��׷��Ŀ�꣬�򻭳�ѡ���
		if (selectObject && selection.width > 0 && selection.height > 0)
		{
			cv::Mat roi(image, selection);
			bitwise_not(roi, roi);   //��ѡ�������ͼ��ɫ
		}


		//��ʾ��ǰ֡
		imshow("CamShift at yanxj", image);

		//¼��Ƶ֡��Ϊ15,�ȴ�1000/15��֤��Ƶ��������
		//waitKey(int delay) ��OpenCV �ṩ��һ���ȴ�������
		//�����е��������ʱ������delay�����ʱ�����ȴ���������
		int Key = cv::waitKey(1000 / 15.0);

		//������ΪESCʱ���˳�ѭ��
		if (Key == 27) break;
	}

	//�ͷ���������ڴ�
	cv::destroyAllWindows();
	video.release();
	return 0;
}

