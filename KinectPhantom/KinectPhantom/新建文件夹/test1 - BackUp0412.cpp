// Standard Library
#include <iostream>

// OpenCV Header
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// Kinect for Windows SDK Header
#include <Kinect.h>

#include <windows.h>   
#include <gl/Gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

#include <math.h>

#pragma comment(lib,"opengl32.lib")  
#pragma comment(lib,"GLU32.LIB")  
#pragma comment(lib,"GLUT32.LIB")  

using namespace std;

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(int w, int h)
{
	GLfloat fAspect;

	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	fAspect = (GLfloat)w / (GLfloat)h;

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Produce the perspective projection
	gluPerspective(60.0f, fAspect, 1.0, 400.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
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
		xRot -= 5.0f;

	if (key == GLUT_KEY_DOWN)
		xRot += 5.0f;

	if (key == GLUT_KEY_LEFT)
		yRot -= 5.0f;

	if (key == GLUT_KEY_RIGHT)
		yRot += 5.0f;

	xRot = (GLfloat)((const int)xRot % 360);
	yRot = (GLfloat)((const int)yRot % 360);

	// Refresh the Window
	glutPostRedisplay();
}


// Called to draw scene
void RenderScene(void)
{
	float fZ, bZ;

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	fZ = 100.0f;
	bZ = -100.0f;

	// Save the matrix state and do the rotations
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -300.0f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	// Set material color, Red
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidSphere(50, 100, 100);

	//glFrontFace(GL_CCW);		// Counter clock-wise polygons face out

	// Restore the matrix state
	glPopMatrix();

	// Buffer swap
	glutSwapBuffers();
}

void Kinetic(void)
{

}


IKinectSensor* pSensor = nullptr;
IDepthFrameSource* pFrameSourceDepth = nullptr;
IColorFrameSource* pFrameSourceColor = nullptr;
ICoordinateMapper* Mapper = nullptr;
IFrameDescription* pFrameDescriptionDepth = nullptr;
IFrameDescription* pFrameDescriptionColor = nullptr;
IDepthFrameReader* pFrameReaderDepth = nullptr;
IColorFrameReader* pFrameReaderColor = nullptr;
IDepthFrame* pFrameDepth = nullptr;
IColorFrame* pFrameColor = nullptr;


int main(int argc, char** argv)
{

	// 1a. Get default Sensor
	IKinectSensor* pSensor = nullptr;
	GetDefaultKinectSensor(&pSensor);

	// 1b. Open sensor
	pSensor->Open();

	// 2a. Get frame source
	IDepthFrameSource* pFrameSourceDepth = nullptr;
	pSensor->get_DepthFrameSource(&pFrameSourceDepth);

	IColorFrameSource* pFrameSourceColor = nullptr;
	pSensor->get_ColorFrameSource(&pFrameSourceColor);

	ICoordinateMapper* Mapper = nullptr;
	pSensor->get_CoordinateMapper(&Mapper);

	// 2b. Get frame description
	int        iWidthDepth = 0;
	int        iHeightDepth = 0;
	unsigned int depthPointCount = 0;
	IFrameDescription* pFrameDescriptionDepth = nullptr;
	pFrameSourceDepth->get_FrameDescription(&pFrameDescriptionDepth);
	pFrameDescriptionDepth->get_Width(&iWidthDepth);
	pFrameDescriptionDepth->get_Height(&iHeightDepth);
	pFrameDescriptionDepth->get_LengthInPixels(&depthPointCount);
	pFrameDescriptionDepth->Release();
	pFrameDescriptionDepth = nullptr;

	int        iWidthColor = 0;
	int        iHeightColor = 0;
	unsigned int colorPointCount = 0;
	IFrameDescription* pFrameDescriptionColor = nullptr;
	pFrameSourceColor->get_FrameDescription(&pFrameDescriptionColor);
	pFrameDescriptionColor->get_Width(&iWidthColor);
	pFrameDescriptionColor->get_Height(&iHeightColor);
	pFrameDescriptionColor->get_LengthInPixels(&colorPointCount);
	pFrameDescriptionColor->Release();
	pFrameDescriptionColor = nullptr;

	// 2c. get some dpeth only deta
	UINT16 uDepthMin = 0, uDepthMax = 0;
	pFrameSourceDepth->get_DepthMinReliableDistance(&uDepthMin);
	pFrameSourceDepth->get_DepthMaxReliableDistance(&uDepthMax);
	cout << "Reliable Distance: "
		<< uDepthMin << " ¡V " << uDepthMax << endl;

	// perpare OpenCV
	cv::Mat mDepthImg(iHeightDepth, iWidthDepth, CV_16UC1);
	cv::Mat mImg8bit(iHeightDepth, iWidthDepth, CV_8UC1);
	cv::namedWindow("Depth Map");

	cv::Mat mColorImg(iHeightColor, iWidthColor, CV_8UC4);
	cv::namedWindow("Color Map");

	// 3a. get frame reader
	IDepthFrameReader* pFrameReaderDepth = nullptr;
	pFrameSourceDepth->OpenReader(&pFrameReaderDepth);

	IColorFrameReader* pFrameReaderColor = nullptr;
	pFrameSourceColor->OpenReader(&pFrameReaderColor);

	// Enter main loop
	while (true)
	{
		// 4a. Get last frame Depth
		IDepthFrame* pFrameDepth = nullptr;
		if (pFrameReaderDepth->AcquireLatestFrame(&pFrameDepth) == S_OK)
		{
			// 4c. copy the depth map to image
			pFrameDepth->CopyFrameDataToArray(iWidthDepth * iHeightDepth,
				reinterpret_cast<UINT16*>(mDepthImg.data));


			//Read pixel depth 1
			int a = mDepthImg.at<unsigned short>(5000);
			//Read pixel depth 2
			UINT uBufferSizeDepth = 0;
			UINT16* pBufferDepth = nullptr;
			pFrameDepth->AccessUnderlyingBuffer(&uBufferSizeDepth, &pBufferDepth);


			//MapDepthFrameToColorSpace
			//CameraSpacePoint* depth2xyz = nullptr;
			vector<CameraSpacePoint> depth2xyz(depthPointCount);
			//CameraSpacePoint depth2xyz[217088];

			Mapper->MapDepthFrameToCameraSpace(depthPointCount, pBufferDepth, depthPointCount, /*depth2xyz*/&depth2xyz[0]);
			
			CameraSpacePoint XX = depth2xyz[108544];//in meters
			
			// 4d. convert from 16bit to 8bit
			mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);
			cv::imshow("Depth Map", mImg8bit);

			// 4e. release frame
			pFrameDepth->Release();
		}

		// 4a. Get last frame Color
		IColorFrame* pFrameColor = nullptr;
		if (pFrameReaderColor->AcquireLatestFrame(&pFrameColor) == S_OK)
		{
			/// read data
			UINT uBufferSize = iWidthColor * iHeightColor * 4 * sizeof(BYTE);
			pFrameColor->CopyConvertedFrameDataToArray(
				uBufferSize, mColorImg.data, ColorImageFormat_Bgra);

			cv::imshow("Color Map", mColorImg);

			// 4e. release frame
			pFrameColor->Release();
		}



		// 4f. check keyboard input
		if (cv::waitKey(30) == VK_ESCAPE){
			break;
		}
	}


	// 3b. release frame reader
	pFrameReaderDepth->Release();
	pFrameReaderDepth = nullptr;

	pFrameReaderColor->Release();
	pFrameReaderColor = nullptr;

	// 2d. release Frame source
	pFrameSourceDepth->Release();
	pFrameSourceDepth = nullptr;

	pFrameSourceColor->Release();
	pFrameSourceColor = nullptr;

	// 1c. Close Sensor
	pSensor->Close();

	// 1d. Release Sensor
	pSensor->Release();
	pSensor = nullptr;


	return 0;
} 