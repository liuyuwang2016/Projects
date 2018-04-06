// Standard Library
#include <iostream>

// OpenCV Header
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// Kinect for Windows SDK Header
#include <Kinect.h>

#include <windows.h>   
#include <gl/Gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

#include <cv.h>

#include <math.h>

#pragma comment(lib,"opengl32.lib")  
#pragma comment(lib,"GLU32.LIB")  
#pragma comment(lib,"GLUT32.LIB")  

using namespace std;
using namespace cv;

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
int FPS = 0;

IKinectSensor* pSensor = nullptr;
ICoordinateMapper* Mapper = nullptr;
IDepthFrameReader* pFrameReaderDepth = nullptr;
IColorFrameReader* pFrameReaderColor = nullptr;

int iWidthDepth = 0;
int iHeightDepth = 0;
UINT depthPointCount = 0;
UINT16* pBufferDepth = nullptr;
UINT16 uDepthMin = 0, uDepthMax = 0;
cv::Mat mDepthImg;
cv::Mat mImg8bit;


int iWidthColor = 0;
int iHeightColor = 0;
UINT colorPointCount = 0;
UINT uBufferSizeColor = 0;
BYTE* pBufferColor = nullptr;
cv::Mat mColorImg;

CameraSpacePoint* pCSPoints = nullptr;

void ShowImage(void);


// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(int w, int h)
{
	GLfloat fAspect;

	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	//glutFullScreen();

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	fAspect = (GLfloat)w / (GLfloat)h;

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Produce the perspective projection
	gluPerspective(45.0f, fAspect, 0.01f, 2000.0f);
}


// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC(void)
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


// Called to draw scene
void RenderScene(void)
{
	//cout << endl << "------RenderScene-----" << endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	GLdouble CamX = 1.5*sin(xRot*3.1415926 / 180);
	GLdouble CamY = 1.5*sin(yRot*3.1415926 / 180);
	GLdouble CamZ = 1.5*cos(xRot*3.1415926 / 180);

	gluLookAt(CamX, CamY, CamZ, 0, 0, 0, 0, 1, 0);

	glPushMatrix();
	glPointSize(1.0f);
	glBegin(GL_POINTS);

	for (int i = 0; i < colorPointCount; i++)
		{
			glColor3ub(pBufferColor[4 * i], pBufferColor[4 * i + 1], pBufferColor[4 * i + 2]);
				GLfloat pX = pCSPoints[i].X;
				GLfloat pY = pCSPoints[i].Y ;
				GLfloat pZ = -pCSPoints[i].Z;  
				glVertex3f(pX, pY, pZ);
		}

	glEnd();
	glPopMatrix();



	glPushMatrix();

	glPointSize(5.0f);
	glBegin(GL_POINTS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(pCSPoints[1280 * 500 + 640].X + 0.05f, pCSPoints[1280 * 500 + 640].Y + 0.05f, -pCSPoints[1280 * 500 + 640].Z - 0.05f);
	glEnd();
	glPopMatrix();


	//glPushMatrix();
	//glTranslatef(0.0f, 0.0f, -300.0f);
	//glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	//glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glutSolidTeapot(50.0f);
	//glPopMatrix();



	//glPushMatrix();
	//glTranslatef(0.0f, 0.0f, -300.0f);
	//glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	//glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glutSolidSphere(50, 100, 100);
	//glPopMatrix();

	glutSwapBuffers();
}


void KineticInit(void)
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
}


void KineticUpdate(void)
{
	//cout << endl << "------KineticUpdate-----" << endl;

	// Read color data
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

	// Read depth data
	IDepthFrame* pDFrameDepth = nullptr;
	if (pFrameReaderDepth->AcquireLatestFrame(&pDFrameDepth) == S_OK)
	{
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, pBufferDepth);
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, reinterpret_cast<UINT16*>(mDepthImg.data));

		pDFrameDepth->Release();
		pDFrameDepth = nullptr;
	}


	Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);


	//vector<CameraSpacePoint> depth2xyz(depthPointCount);
	//Mapper->MapDepthFrameToCameraSpace(depthPointCount, pBufferDepth, depthPointCount, &depth2xyz[0]);
	//CameraSpacePoint XX = depth2xyz[108544];//in meters


	ShowImage();


}


void ShowImage()
{
	cv::namedWindow("Depth Map");
	cv::namedWindow("Color Map");

	// Convert from 16bit to 8bit
	mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);

	CameraSpacePoint* CSP = nullptr;
	CSP = new CameraSpacePoint[1];

	CSP[0].X = pCSPoints[1280 * 500 + 640].X + 0.05f;
	CSP[0].Y = pCSPoints[1280 * 500 + 640].Y + 0.05f;
	CSP[0].Z = pCSPoints[1280 * 500 + 640].Z + 0.05f;

	ColorSpacePoint* ColoSP = nullptr;
	ColoSP = new ColorSpacePoint[1];

	Mapper->MapCameraPointsToColorSpace((UINT)1, CSP, (UINT)1, ColoSP);
	
	cout << (int)ColoSP[0].X << " " << (int)ColoSP[0].Y << endl;

	if (ColoSP[0].X != -1 * numeric_limits<float>::infinity())
	{
		//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[0] = 0;
		//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[1] = 0;
		//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[2] = 1;
		//mColorImg.at<cv::Vec4b>((int)ColoSP[0].Y, (int)ColoSP[0].X)[3] = 0;

		circle(mColorImg, Point((int)ColoSP[0].X, (int)ColoSP[0].Y), 2, Scalar(0, 0, 255), -1);
		
	}




	cv::imshow("Depth Map", mImg8bit);
	cv::imshow("Color Map", mColorImg);
}



int main(int argc, char** argv)
{
	KineticInit();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(iWidthColor, iHeightColor);
	glutCreateWindow("Perspective Projection");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(KineticUpdate);
	SetupRC();
	glutTimerFunc(1000, timer, 0);
	glutMainLoop();


	return 0;
} 