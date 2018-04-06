/************************************************************************/
/*
Description:	�����������˲�Ŀ�����
Author:			Yang Xian
Email:			yang_xian521@163.com
Version:		2011-11-2
History:
*/
/************************************************************************/
#include <iostream>	// for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion

#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace cv;
using namespace std;

// ������Щ�����Խ��Ӱ��ܴ󣬶���Ҳ�������Ƶ���ݣ���Խ���кܴ��Ӱ��
const int PARTICLE_NUM = 25;	// ���Ӹ���
// ���ӷ�����������
const double A1 = 2.0;
const double A2 = -1.0;
const double B0 = 1.0;
// ��˹�����sigma����
const double SIGMA_X = 1.0;
const double SIGMA_Y = 0.5;
const double SIGMA_SCALE = 0.001;

// ���ӽṹ��
typedef struct particle {
	double x;			// ��ǰx����
	double y;			// ��ǰy����
	double scale;		// ���ڱ���ϵ��
	double xPre;			// x����Ԥ��λ��
	double yPre;			// y����Ԥ��λ��
	double scalePre;		// ����Ԥ�����ϵ��
	double xOri;			// ԭʼx����
	double yOri;			// ԭʼy����
	// 	int width;			// ԭʼ������
	// 	int height;			// ԭʼ����߶�
	Rect rect;			// ԭʼ�����С
	MatND hist;			// �������������ֱ��ͼ
	double weight;		// �����ӵ�Ȩ��
} PARTICLE;

Mat hsv;	// hsvɫ�ʿռ������ͼ��
Mat roiImage;	// Ŀ������
MatND roiHist;	// Ŀ������ֱ��ͼ
Mat img;	// �����Ŀ��ͼ��
PARTICLE particles[PARTICLE_NUM];	// ����

int nFrameNum = 0;

bool bSelectObject = false;	// ����ѡ���־
bool bTracking = false;		// ��ʼ���ٱ�־
Point origin;	// ��갴��ʱ�ĵ�λ��
Rect selection;// ����Ȥ�������С

// ֱ��ͼ��ز�����������ѡȡҲ��Խ��Ӱ��޴�
// Quantize the hue to 30 levels
// and the saturation to 32 levels
// value to 10 levels
int hbins = 180, sbins = 256, vbin = 10;
int histSize[] = { hbins, sbins, vbin };
// hue varies from 0 to 179, see cvtColor
float hranges[] = { 0, 180 };
// saturation varies from 0 (black-gray-white) to 255 (pure spectrum color)
float sranges[] = { 0, 256 };
// value varies from 0 (black-gray-white) to 255 (pure spectrum color)
float vranges[] = { 0, 256 };
const float* ranges[] = { hranges, sranges, vranges };
// we compute the histogram from the 0-th and 1-st channels
int channels[] = { 0, 1, 2 };

// �����Ӧ�������õ�ѡ������򣬱�����selection
void onMouse(int event, int x, int y, int, void*)
{
	if (bSelectObject)
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);

		selection &= Rect(0, 0, img.cols, img.rows);
	}

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		bSelectObject = true;
		bTracking = false;
		break;
	case CV_EVENT_LBUTTONUP:
		bSelectObject = false;
		bTracking = true;
		nFrameNum = 0;
		break;
	}
}

// ���������㷨������
int particle_cmp(const void* p1, const void* p2)
{
	PARTICLE* _p1 = (PARTICLE*)p1;
	PARTICLE* _p2 = (PARTICLE*)p2;

	if (_p1->weight < _p2->weight)
		return 1;	//����Ȩ�ؽ�������
	if (_p1->weight > _p2->weight)
		return -1;
	return 0;
}

int main(int argc, char *argv[])
{
	int delay = 10;	// ���Ʋ����ٶ�
	char c;	// ��ֵ

	VideoCapture captRefrnc("1.avi"/*"soccer.avi"*/);	// ��Ƶ�ļ�

	if (!captRefrnc.isOpened())
	{
		return -1;
	}

	// Windows
	// 	const char* WIN_SRC = "Source";
	const char* WIN_RESULT = "Result";
	// 	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE );
	namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);
	// �����Ӧ����
	setMouseCallback(WIN_RESULT, onMouse, 0);

	Mat frame;	//��Ƶ��ÿһ֡ͼ��

	bool paused = false;
	PARTICLE * pParticles = particles;
	//	PARTICLE * pParticles = new PARTICLE[sizeof(PARTICLE) * PARTICLE_NUM];

	while (true) //Show the image captured in the window and repeat
	{
		if (!paused)
		{
			captRefrnc >> frame;
			if (frame.empty())
				break;
		}

		frame.copyTo(img);	// �������Ĳ������Ƕ�src��

		// ѡ��Ŀ�����и���
		if (bTracking == true)
		{
			if (!paused)
			{
				nFrameNum++;
				cvtColor(img, hsv, CV_BGR2HSV);
				Mat roiImage(hsv, selection);	// Ŀ������

				if (nFrameNum == 1)	//ѡ��Ŀ���ĵ�һ֡��Ҫ��ʼ��
				{
					// step 1: ��ȡĿ����������
					calcHist(&roiImage, 1, channels, Mat(), roiHist, 3, histSize, ranges);
					normalize(roiHist, roiHist);	// ��һ��L2

					// step 2: ��ʼ��particle
					pParticles = particles;
					for (int i = 0; i < PARTICLE_NUM; i++)
					{
						pParticles->x = selection.x + 0.5 * selection.width;
						pParticles->y = selection.y + 0.5 * selection.height;
						pParticles->xPre = pParticles->x;
						pParticles->yPre = pParticles->y;
						pParticles->xOri = pParticles->x;
						pParticles->yOri = pParticles->y;
						pParticles->rect = selection;
						pParticles->scale = 1.0;
						pParticles->scalePre = 1.0;
						pParticles->hist = roiHist;
						pParticles->weight = 0;
						pParticles++;
					}
				}
				else
				{
					pParticles = particles;
					RNG rng;
					for (int i = 0; i < PARTICLE_NUM; i++)
					{
						// step 3: ��particle��transition
						double x, y, s;

						pParticles->xPre = pParticles->x;
						pParticles->yPre = pParticles->y;
						pParticles->scalePre = pParticles->scale;

						x = A1 * (pParticles->x - pParticles->xOri) + A2 * (pParticles->xPre - pParticles->xOri) +
							B0 * rng.gaussian(SIGMA_X) + pParticles->xOri;
						pParticles->x = std::max(0.0, std::min(x, img.cols - 1.0));


						y = A1 * (pParticles->y - pParticles->yOri) + A2 * (pParticles->yPre - pParticles->yOri) +
							B0 * rng.gaussian(SIGMA_Y) + pParticles->yOri;
						pParticles->y = std::max(0.0, std::min(y, img.rows - 1.0));

						s = A1 * (pParticles->scale - 1.0) + A2 * (pParticles->scalePre - 1.0) +
							B0 * rng.gaussian(SIGMA_SCALE) + 1.0;
						pParticles->scale = std::max(0.1, std::min(s, 3.0));
						// rect�����д���֤
						pParticles->rect.x = std::max(0, std::min(cvRound(pParticles->x - 0.5 * pParticles->rect.width * pParticles->scale), img.cols - 1));		// 0 <= x <= img.rows-1
						pParticles->rect.y = std::max(0, std::min(cvRound(pParticles->y - 0.5 * pParticles->rect.height * pParticles->scale), img.rows - 1));	// 0 <= y <= img.cols-1
						pParticles->rect.width = std::min(cvRound(pParticles->rect.width * pParticles->scale), img.cols - pParticles->rect.x);
						pParticles->rect.height = std::min(cvRound(pParticles->rect.height * pParticles->scale), img.rows - pParticles->rect.y);
						// Ori�������ı�

						// step 4: ��particle���������ֱ��ͼ
						Mat imgParticle(img, pParticles->rect);
						calcHist(&imgParticle, 1, channels, Mat(), pParticles->hist, 3, histSize, ranges);
						normalize(pParticles->hist, pParticles->hist);	// ��һ��L2

						// step 5: �����ıȶ�,����particleȨ��
						pParticles->weight = compareHist(roiHist, pParticles->hist, CV_COMP_INTERSECT);

						pParticles++;
					}

					// step 6: ��һ������Ȩ��
					double sum = 0.0;
					int i;

					pParticles = particles;
					for (i = 0; i < PARTICLE_NUM; i++)
					{
						sum += pParticles->weight;
						pParticles++;
					}
					pParticles = particles;
					for (i = 0; i < PARTICLE_NUM; i++)
					{
						pParticles->weight /= sum;
						pParticles++;
					}

					// step 7: resample�������ӵ�Ȩ�صĺ�����ʷֲ����²���
					pParticles = particles;
					// 					PARTICLE* newParticles = new PARTICLE[sizeof(PARTICLE) * PARTICLE_NUM];
					PARTICLE newParticles[PARTICLE_NUM];
					int np, k = 0;

					qsort(pParticles, PARTICLE_NUM, sizeof(PARTICLE), &particle_cmp);
					for (int i = 0; i < PARTICLE_NUM; i++)
					{
						np = cvRound(particles[i].weight * PARTICLE_NUM);
						for (int j = 0; j < np; j++)
						{
							newParticles[k++] = particles[i];
							if (k == PARTICLE_NUM)
								goto EXITOUT;
						}
					}
					while (k < PARTICLE_NUM)
					{
						newParticles[k++] = particles[0];
					}

				EXITOUT:
					for (int i = 0; i < PARTICLE_NUM; i++)
					{
						particles[i] = newParticles[i];
					}

				}// end else

				qsort(pParticles, PARTICLE_NUM, sizeof(PARTICLE), &particle_cmp);

				// step 8: �������ӵ���������Ϊ���ٽ��
				Rect_<double> rectTrackingTemp(0.0, 0.0, 0.0, 0.0);
				pParticles = particles;
				for (int i = 0; i < PARTICLE_NUM; i++)
				{
					rectTrackingTemp.x += pParticles->rect.x * pParticles->weight;
					rectTrackingTemp.y += pParticles->rect.y * pParticles->weight;
					rectTrackingTemp.width += pParticles->rect.width * pParticles->weight;
					rectTrackingTemp.height += pParticles->rect.height * pParticles->weight;
					pParticles++;
				}
				Rect rectTracking(rectTrackingTemp);	// ���ٽ��

				// ��ʾ�����ӵ��˶�
				for (int i = 0; i < PARTICLE_NUM; i++)
				{
					rectangle(img, particles[i].rect, Scalar(255, 0, 0));
				}
				// ��ʾ���ٽ��
				rectangle(img, rectTracking, Scalar(0, 0, 255), 3);

			}
		}// end Tracking

		// 		imshow(WIN_SRC, frame);
		imshow(WIN_RESULT, img);

		c = (char)waitKey(delay);
		if (c == 27)
			break;
		switch (c)
		{
		case 'p':
			paused = !paused;
			break;
		default:
			;
		}
	}// end while
}