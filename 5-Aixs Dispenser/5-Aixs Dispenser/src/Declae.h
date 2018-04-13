//http://blog.csdn.net/arag2009/article/details/78393052 opencv+opengl

#include "src/Scene.h"
#include "src/M232.h"
#include "osapi/Thread.h"

#pragma region Function

#pragma region OpenCV&ROI Function 
//将传入的点的坐标从Kinect的坐标转换到机器的坐标
void ROITrans(CameraSpacePoint* Data, int DataNum, GLfloat* TransM, CameraSpacePoint* Result)
{
	for (int i = 0; i < DataNum; i++)
	{
		GLfloat m[16] = { 0 };
		m[3] = m[7] = m[11] = m[15] = 1;
		m[12] = Data[i].X;
		m[13] = Data[i].Y;
		m[14] = Data[i].Z;
		
		glMatrixMode(GL_MODELVIEW);
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

//模型换了之后没影响
void ROITrans(float* Data, int DataNum, GLfloat* TransM, float* Result)
{
	for (int i = 0; i < DataNum; i++)
	{
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
/*對於opencv裡面的Mat數據進行重新讀取//模型换了之后没影响*/
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
/*對於opencv中mat類進行輸出//模型换了之后没影响*/
void OutputValue(Mat M, int Row, int Col, int Chan, uchar* Data)
{
	int Steps = M.cols*M.channels();
	int Channels = M.channels();
	uchar *srcData = M.data;
	*Data = *(srcData + Row*Steps + Col*Channels + Chan);
}

void OutputValue(Mat M, int Row, int Col, int Chan, float* Data)
{
	int Steps = M.cols*M.channels();
	int Channels = M.channels();
	float* srcData = (float*)M.data;
	*Data = *(srcData + Row*Steps + Col*Channels + Chan);
}
#pragma endregion OpenCV&ROI Function

#pragma region ROI Function
//模型换了之后没影响
/*---------------------------------------*/
/*--------------ROI Function-------------*/
/*---------------------------------------*/
void onMouseROI(int event, int x, int y, int flags, void* param)
{
	int thickness = 2;
	//Push
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		ROI_rect.x = x;
		ROI_rect.y = y;
		ROI_S1 = TRUE;
		ROI_S2 = FALSE;
		break;
	case EVENT_LBUTTONUP:
		ROI_S1 = FALSE;
		ROI_rect.height = y - ROI_rect.y;
		ROI_rect.width = x - ROI_rect.x;
		ROI_p2 = Point(x, y);
		break;
	}
	//Drag
	if (flags == CV_EVENT_FLAG_LBUTTON)
	{
		namedWindow("ROI Map", WINDOW_AUTOSIZE);
		ROI_S2 = TRUE;
		ROI_p1 = Point(ROI_rect.x, ROI_rect.y);
		ROI_p2 = Point(x, y);
		rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), 2);
		cv::imshow("ROI Map", ROI);
	}
}

void FindROI()
{
	/*--------------Find ROI-------------*/
	if (ROI_p2.x > ROI_p1.x && ROI_p2.y > ROI_p1.y)
	{
		/*get the ROI region that you choose by mouse*/
		ROI_Image = mColorImg.colRange(ROI_p1.x, ROI_p2.x + 1).rowRange(ROI_p1.y, ROI_p2.y + 1).clone();
	}
	else
	{
		OutputDebugString("You push the left button, please add ROI from left-top to right-down, don't mess up with that\n");
		return;
	}
	//namedWindow("ROI");
	//imshow("ROI", ROI_Image);
	/*--------------BGR to YCrCb-------------*/
	Mat ROI_YCrCb;
	cvtColor(ROI_Image, ROI_YCrCb, CV_BGR2YCrCb);
	/*-------------after BGR to YCrCb, the image was storaged in ROI_YCrCb-------------*/
	//namedWindow("YCrCb");
	//imshow("YCrCb", ROI_YCrCb);
	/*-------------- Color Detection and Tracking-------------*/
	int channels = ROI_YCrCb.channels();
	int rows = ROI_YCrCb.rows;
	int cols = ROI_YCrCb.cols;
	int steps = ROI_YCrCb.step;
	ROIcount = 0;
	ROIDepthCount = 0;
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
			if (ROIcount > 999)
			{
				//cout << endl << "--------Array Out of range--------" << endl;
				OutputDebugString("--------Array Out of range---------//-------Array Out of range--------//--------Array Out of range--------\n");
				exit(1);
			}
			uchar IsRed = 0;
			/*input & output value, OpenCV matrix input & output value function*/
			OutputValue(ROI_YCrCb, i, j, 1, &IsRed);

			//threshold = 150 for fluorescent pink
			//threshold = 170 for red
			if (IsRed > 155)
			{
				ROI_Image.at<Vec4b>(i, j)[0] = 255;
				ROI_Image.at<Vec4b>(i, j)[1] = 0;
				ROI_Image.at<Vec4b>(i, j)[2] = 0;

				InputValue(ROI, 0, i + ROI_p1.y, j + ROI_p1.x, 0, 255);
				InputValue(ROI, 0, i + ROI_p1.y, j + ROI_p1.x, 1, 0);
				InputValue(ROI, 0, i + ROI_p1.y, j + ROI_p1.x, 2, 0);

				ROIPixel[ROIcount].x = j + ROI_p1.x;
				ROIPixel[ROIcount].y = i + ROI_p1.y;

				ROICenterColorS_New.x += ROIPixel[ROIcount].x;
				ROICenterColorS_New.y += ROIPixel[ROIcount].y;
				ROIcount++;
			}
		}
	}
	//imshow("ROI", ROI_Image);
	/*If we got the color we want, do this*/
	if (ROIcount > 3)
	{
		/*get the mean value */
		//static_cast把后面的类型转换为int。
		ROICenterColorS_New.x = static_cast<int>(ROICenterColorS_New.x / ROIcount);
		ROICenterColorS_New.y = static_cast<int>(ROICenterColorS_New.y / ROIcount);
		/*call mapper function*/
		CameraSpaceROI();
		/*tracking ROI, let the probe tip always in the center of ROI*/
		MoveROI();
	}
	else if (ROIcount <= 3)
	{
		ROICenterColorS_Old.x = ROICenterColorS_New.x = 0;
		ROICenterColorS_Old.y = ROICenterColorS_New.y = 0;
		Draw3DLine();
	}
}

void CameraSpaceROI()
{
	if (ROICameraSP != nullptr && PlaneSP != nullptr)
	{
		delete[] ROICameraSP;
		delete[] PlaneSP;
		PlaneSP = nullptr;
		ROICameraSP = nullptr;
	}
	ROIDepthCount = 0;

	ROICenterCameraS.x = 0;
	ROICenterCameraS.y = 0;
	ROICenterCameraS.z = 0;

	//跑一个loop，看一下 depth count 有几个
	for (int i = 0; i < ROIcount; i++)
	{
		//在之前对ROI区域的像素做处理，在这里拿到每个像素的index，然后筛选出没有Z值的像素
		int index1 = ROIPixel[i].x + ROIPixel[i].y * iWidthColor;
		//numeric_limits<float>::infinity()无穷大
		if (pCSPoints[index1].Z != -1 * numeric_limits<float>::infinity())
		{
			ROIDepthCount++;
		}
	}
	CameraSpacePoint* Temp = new CameraSpacePoint[ROIDepthCount];
	int indx1 = 0;
	for (int i = 0; i < ROIcount; i++)
	{
		int indx2 = ROIPixel[i].x + ROIPixel[i].y * iWidthColor;
		if (pCSPoints[indx2].Z != -1 * numeric_limits<float>::infinity())
		{
			//kinect坐标到opengl坐标，所以之后的坐标中Z相反，所以将Z设置为负
			Temp[indx1].X = pCSPoints[indx2].X;
			Temp[indx1].Y = pCSPoints[indx2].Y;
			Temp[indx1].Z = -pCSPoints[indx2].Z;
			indx1++;
		}
	}
	/*---------------------------------Bubble sort---------------------------------*/
	for (int i = ROIDepthCount - 1; i > 0; --i)
	{
		for (int j = 0; j < i; ++j)
		{
			if (Temp[j].Z < Temp[j + 1].Z)
			{
				CameraSpacePoint temp;
				temp.X = Temp[j].X;
				temp.Y = Temp[j].Y;
				temp.Z = Temp[j].Z;

				Temp[j].X = Temp[j + 1].X;
				Temp[j].Y = Temp[j + 1].Y;
				Temp[j].Z = Temp[j + 1].Z;

				Temp[j + 1].X = temp.X;
				Temp[j + 1].Y = temp.Y;
				Temp[j + 1].Z = temp.Z;
			}
		}
	}
	//Set int threshold and filtering
	int IndexLim = 0;
	for (int i = 0; i < ROIDepthCount; i++)
	{
		if (Temp[i].Z < Temp[0].Z*1.02)
		{
			IndexLim = i;
			break;
		}
	}
	//Mismatch did not occur
	if (IndexLim == 0)
	{
		ROICameraSP = new CameraSpacePoint[ROIDepthCount];
		for (int i = 0; i < ROIDepthCount; i++)
		{
			//ROICameraSP的坐标是相对于Kinect的坐标
			ROICameraSP[i].X = Temp[i].X;
			ROICameraSP[i].Y = Temp[i].Y;
			ROICameraSP[i].Z = Temp[i].Z;

			ROICenterCameraS.x += ROICameraSP[i].X;
			ROICenterCameraS.y += ROICameraSP[i].Y;
			ROICenterCameraS.z += ROICameraSP[i].Z;
		}
	}
	//Mismatch occur
	else
	{
		ROICameraSP = new CameraSpacePoint[IndexLim];
		for (int i = 0; i < IndexLim; i++)
		{
			ROICameraSP[i].X = Temp[i].X;
			ROICameraSP[i].Y = Temp[i].Y;
			ROICameraSP[i].Z = Temp[i].Z;

			ROICenterCameraS.x += ROICameraSP[i].X;
			ROICenterCameraS.y += ROICameraSP[i].Y;
			ROICenterCameraS.z += ROICameraSP[i].Z;
		}
		ROIDepthCount = IndexLim;
	}
	delete[] Temp;
	/*------------------------------------------------------------------*/
	if (ROIDepthCount > 0)
	{
		ROICenterCameraS.x = ROICenterCameraS.x / ROIDepthCount;
		ROICenterCameraS.y = ROICenterCameraS.y / ROIDepthCount;
		ROICenterCameraS.z = ROICenterCameraS.z / ROIDepthCount;
	}
	else if (ROIDepthCount == 0)
	{
		ROICenterCameraS.x = 0;
		ROICenterCameraS.y = 0;
		ROICenterCameraS.z = 0;
	}
}

void MoveROI()
{
	if (ROICenterColorS_Old.x == 0 && ROICenterColorS_Old.y == 0)
	{
		ROICenterColorS_Old.x = ROICenterColorS_New.x;
		ROICenterColorS_Old.y = ROICenterColorS_New.y;
	}
	else if (ROICenterColorS_Old.x != 0 && ROICenterColorS_New.y != 0)
	{
		Vec2i Dir;

		Point2i center;

		center.x = (ROI_p1.x + ROI_p2.x) / 2;
		center.y = (ROI_p1.y + ROI_p2.y) / 2;

		Dir.val[0] = ROICenterColorS_New.x - center.x;
		Dir.val[1] = ROICenterColorS_New.y - center.y;

		ROI_p1.x = ROI_p1.x + Dir.val[0];
		ROI_p1.y = ROI_p1.y + Dir.val[1];

		ROI_p2.x = ROI_p2.x + Dir.val[0];
		ROI_p2.y = ROI_p2.y + Dir.val[1];

		ROICenterColorS_Old.x = ROICenterColorS_New.x;
		ROICenterColorS_Old.y = ROICenterColorS_New.y;

		ROICenterColorS_New.x = 0;
		ROICenterColorS_New.y = 0;
	}
}

/*we need to storage the probe tip's coordinate position in the machine coordinate system and the opengl coordinate system*/
void ROICameraSPStorage()
{
	if (ROICameraSP_Storage == nullptr)
	{
		ROICameraSP_Storage = new CameraSpacePoint[StorageSize];
		ROICameraSP_Proj_Storage = new CameraSpacePoint[StorageSize];
		ROICameraSP_MachineCoord_Storage = new CameraSpacePoint[StorageSize];
		ROICameraSP_MachineCoord_Proj_Storage = new CameraSpacePoint[StorageSize];
		ROIStorageCount = 0;
	}
	if (!ROI_IS_COLLIDE)
	{
		if (ROIStorageCount + 1 < StorageSize)
		{
			CameraSpacePoint Center;
			//center的坐标还是Kinect的坐标系
			Center.X = ROICenterCameraS.x;
			Center.Y = ROICenterCameraS.y;
			Center.Z = ROICenterCameraS.z;

			//因为要把笔尖点相对于opengl的坐标以及其相对于机器的坐标都存下来，才能同时指导机器的移动以及图像化
			ROICameraSP_Storage[ROIStorageCount] = Center;
			ROICameraSP_Proj_Storage[ROIStorageCount] = *ARFunc_ROICSP_Proj/*Center*/;

			ROICameraSP_MachineCoord_Storage[ROIStorageCount] = *ROICameraSP_MechCoord;
			ROICameraSP_MachineCoord_Proj_Storage[ROIStorageCount] = *ROICameraSP_Proj_MechCoord;

			ROIStorageCount += 1;
		}
		else
		{
			cout << endl << "The array ROICameraSP_Storage is full, press F2 to clear" << endl;
		}
	}
	else
	{
		cout << endl << "Collide!! Can not set path here!!!!" << endl;
	}
}

#pragma endregion ROI Function

#pragma region OpenGL Function
//模型换了之后没影响
void GLInit()
{
	Intersect.X = 0.3221273277841458;
	Intersect.Y = -0.1384631684450974;
	Intersect.Z = -1;

	ObjPosi.x = -0.311;
	ObjPosi.y = 0.195;
	ObjPosi.z = 0.267;

	TipPosi.x = 0;
	TipPosi.y = -0.12;
	TipPosi.z = -0.035;
	
	DeviaDueToY = new CameraSpacePoint[1];
	DeviaDueToY->X = DeviaDueToY->Y = DeviaDueToY->Z = 0;
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
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidthColor, iHeightColor, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case VK_ESCAPE:
		glutExit();
	//case 'F':
	//case 'f':
	//	/*Use CPU to get the FPS of the machine*/
	//	Finish_Without_Update = TRUE;
	//	printf("%f fps\n", g_fps(RenderScene, 100));
	//	cout << "FPS_RS = " << FPS_RS << endl;
	//	Finish_Without_Update = FALSE;
	//	break;
		/*A move */
	case 'A':
	case 'a':
		MtReflash(md);
		MtMove();
		CUBIC_MOVE = TRUE;
		ARFunc_IS_ON = FALSE;
		break;
	case 'S':
	case 's':
		ROICameraSPStorage();
		break;
	case 'B':
	case 'b':
		MtReflash(md);
		//long MtEmpty();
		MtHome();
		break;
	case 'L':
	case 'l':
		MtReflash(md);
		Mt_Line_Move();
		break;
	case 'R': //控制X+++
	case 'r': 
		md->x += Mt_speed;
		Mt_x = md->x;
		Mt_XMove(Mt_x);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->x != Mt_x);
		break;
	case 'F'://控制X--
	case 'f':
		md->x -= Mt_speed;
		Mt_x = md->x;
		Mt_XMove(Mt_x);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->x != Mt_x);
		break;
	case 'T'://控制Y++
	case 't':
		md->y += Mt_speed;
		Mt_y = md->y;
		Mt_YMove(Mt_y);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->y != Mt_y);
		break;
	case 'G'://控制Y--
	case 'g':
		md->y -= Mt_speed;
		Mt_y = md->y;
		Mt_YMove(Mt_y);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->y != Mt_y);
		break;
	case 'Y'://控制Z++
	case 'y':
		md->z += Mt_speed;
		Mt_z = md->z;
		Mt_ZMove(Mt_z);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->z != Mt_z);
		break;
	case 'H'://控制Z--
	case 'h':
		md->z -= Mt_speed;
		Mt_z = md->z;
		Mt_ZMove(Mt_z);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->z != Mt_z);
		break;
	case 'U'://控制U++
	case 'u':
		md->u += Mt_speed;
		Mt_u = md->u;
		Mt_UMove(Mt_u);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->u != Mt_u);
		break;
	case 'J'://控制U--
	case 'j':
		md->u -= Mt_speed;
		Mt_u = md->u;
		Mt_UMove(Mt_u);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->u != Mt_u);
		break;
	case 'I'://控制V++
	case 'i':
		md->v += Mt_speed;
		Mt_v = md->v;
		Mt_VMove(Mt_v);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->v != Mt_v);
		break;
	case 'K'://控制V--
	case 'k':
		md->v -= Mt_speed;
		Mt_v = md->v;
		Mt_VMove(Mt_v);
		do
		{
			MtReflash(md);
			//cout << "md->v : " << md->v << endl;
		} while (md->v != Mt_v);
		break;
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
	case 0:
		//printf("FPS = %d\n", FPS);
		FPS = 0;
		FPS_RS = 0;
		glutTimerFunc(1000, timer, 0);
		break;
	}
}

/*建立鼠标右键的小菜单*/
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

	submenuID_StorageType = glutCreateMenu(menuCB);
	glutAddMenuEntry("ROI Center Point", MENU_STORAGETYPE1);
	glutAddMenuEntry("Projection Point", MENU_STORAGETYPE2);

	submenuID_ROIType = glutCreateMenu(menuCB);
	glutAddMenuEntry("Color Tracking", MENU_COLOR_TRACK);
	glutAddMenuEntry("3D Line Detection", MENU_3D_LINE);
	glutAddMenuEntry("3D Plane Detection", MENU_3D_PLANE);

	//1st layer
	glutCreateMenu(menuCB);
	glutAddSubMenu("Background type", submenuID_BG);
	glutAddSubMenu("Storage type", submenuID_StorageType);
	glutAddSubMenu("ROI type", submenuID_ROIType);
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
	case MENU_STORAGETYPE1:
		STORAGE_TYPE = 0;
		break;
	case MENU_STORAGETYPE2:
		STORAGE_TYPE = 1;
		break;
	case MENU_COLOR_TRACK:
		BG_IS_OPEN = TRUE;
		ROI_TYPE = 0;
		break;
	case MENU_3D_LINE:
		BG_IS_OPEN = TRUE;
		ROI_TYPE = 1;
		break;
	case MENU_3D_PLANE:
		BG_IS_OPEN = TRUE;
		ROI_TYPE = 2;
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
	//Draw Background through RGBA image from Kinect
	glRasterPos2f(0.0f, 0.0f);
	//glDrawPixels(iWidthColor, iHeightColor, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);
	glEnable(GL_TEXTURE_2D);
	//如果不设置的话背景会变成红色，不知道原因
	glColor3ub(255, 255, 255);
	// tell OpenGL to use the generated texture name
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidthColor, iHeightColor, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBufferColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//画背景
	bool MirrorFunction = FALSE;
	if (!MirrorFunction)
	{
		glPushMatrix();
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex2i(0, iHeightColor);
		glTexCoord2i(1, 0);
		glVertex2i(iWidthColor, iHeightColor);
		glTexCoord2i(1, 1);
		glVertex2i(iWidthColor, 0);
		glTexCoord2i(0, 1);
		glVertex2i(0, 0);
		glEnd();
		glPopMatrix();
	}
	else{
		glPushMatrix();
		glBegin(GL_QUADS);
		glTexCoord2i(1, 0);
		glVertex2i(0, iHeightColor);
		glTexCoord2i(0, 0);
		glVertex2i(iWidthColor, iHeightColor);
		glTexCoord2i(0, 0);
		glVertex2i(iWidthColor, 0);
		glTexCoord2i(1, 1);
		glVertex2i(0, 0);
		glEnd();
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
	static const double kFovY = 53.3;

	double nearDist, farDist, aspect;
	nearDist = 0.01f / tan((kFovY / 2.0) * CV_PI / 180.0);
	farDist = 50;
	aspect = (double)iWidthColor / iHeightColor;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(kFovY, aspect, nearDist, farDist);
	glEnable(GL_DEPTH_TEST);
 	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	glTranslatef(0.055f, -0.015f, 0.0f);
}

void SceneWithoutBackground()
{
	static const double kFovY = 53.3;
	double nearDist, farDist, aspect;
	nearDist = 0.01f / tan((kFovY / 2.0) * CV_PI / 180.0);
	farDist = 50;
	aspect = (double)iWidthColor / iHeightColor;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(kFovY, aspect, nearDist, farDist);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	glTranslatef(0.055f, -0.015f, 0.0f);
}

void DrawPointCloud()
{
	/**************/
	/**************/
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
	/*for (int y = 0; y < iHeightColor; ++y)
	{
		for (int x = 0; x < iWidthColor; ++x)
		{
			int idx = x + y * iWidthColor;
			const CameraSpacePoint& rPt = pCSPoints[idx];
			if (rPt.Z > 0)
			{
				glColor4ubv((const GLubyte*)(&pBufferColor[4 * idx]));
				glVertex3f(rPt.X, rPt.Y, rPt.Z);
			}
		}
	}*/
	glEnd();
	glPopMatrix();
}

static void onMouse3D(int event, int x, int y, int, void*)
{
	// 若鼠标左键没有按下，便返回
	if (event != EVENT_LBUTTONDOWN)
		return;

	//-------------------【<1>调用floodFill函数之前的参数准备部分】---------------
	Point seed = Point(x, y);
	int LowDifference = g_nFillMode == 0 ? 0 : g_nLowDifference;//空范围的漫水填充，此值设为0，否则设为全局的g_nLowDifference
	int UpDifference = g_nFillMode == 0 ? 0 : g_nUpDifference;//空范围的漫水填充，此值设为0，否则设为全局的g_nUpDifference

	//标识符的0~7位为g_nConnectivity，8~15位为g_nNewMaskVal左移8位的值，16~23位为CV_FLOODFILL_FIXED_RANGE或者0。
	int flags = g_nConnectivity + (g_nNewMaskVal << 8) + (g_nFillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);

	//随机生成bgr值
	int b = 255;//全部填充蓝色
	int g = 0;
	int r = 0;
	Rect ccomp;//定义重绘区域的最小边界矩形区域

	Scalar newVal = g_bIsColor ? Scalar(b, g, r) : Scalar(r*0.299 + g*0.587 + b*0.114);//在重绘区域像素的新值，若是彩色图模式，取Scalar(b, g, r)；若是灰度图模式，取Scalar(r*0.299 + g*0.587 + b*0.114)

	Mat dst = g_bIsColor ? g_dstImage : g_grayImage;//目标图的赋值
	int area;

	//--------------------【<2>正式调用floodFill函数】-----------------------------
	if (g_bUseMask)
	{
		threshold(g_maskImage, g_maskImage, 1, 128, THRESH_BINARY);
		area = floodFill(dst, g_maskImage, seed, newVal, &ccomp, Scalar(LowDifference, LowDifference, LowDifference),
			Scalar(UpDifference, UpDifference, UpDifference), flags);
		imshow("mask", g_maskImage);
	}
	else
	{
		area = floodFill(dst, seed, newVal, &ccomp, Scalar(LowDifference, LowDifference, LowDifference),
			Scalar(UpDifference, UpDifference, UpDifference), flags);
	}
	imshow("2D Extraction", dst);
	imwrite("planeExtract.jpg", dst);
	cout << area << " 个像素被重绘\n";
}

void Draw3DLine()
{
	/*--------------Draw Line Detection--------------*/
	/*Mat srcImage = ROI_Image.clone();
	Mat midImage, dstImage;

	Canny(srcImage, midImage, 50, 200, 3);
	cvtColor(midImage, dstImage, COLOR_GRAY2BGR);

	vector<Vec4i> lines;
	RNG rng(12345);
	HoughLinesP(midImage, lines, 1, CV_PI / 180, 80, 50, 10);
	Mat drawing = Mat::zeros(dstImage.size(), CV_8UC3);
	for (size_t i = 0; i < lines.size(); i++)
	{
	Vec4i l = lines[i];
	line(drawing, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 1, LINE_AA);
	}
	imshow("ROI to extract", srcImage);
	imshow("2D hough line", drawing);
	waitKey(0);*/
	/*--------------Draw Contours Detection--------------*/
	Mat srcImage = ROI_Image.clone();
	Mat dstImage;
	cvtColor(srcImage, dstImage, CV_BGR2GRAY);
	Canny(dstImage, dstImage, 100, 200, 3);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);
	findContours(dstImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing = ROI_Image.clone();
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			//cout << "Point(x,y)=" <<  contours[i][j] << endl;
		}
	}
	//imshow("2D Contour", drawing);
	//waitKey(0);
	imwrite("drawing.jpg", drawing);
	//在这里不能够直接读取drawing的值，因为drawing是视频而不是图片，而floodfill只能读取图片，而且是RGB图片
	g_srcImage = imread("drawing.jpg", 1);
	g_srcImage.copyTo(g_dstImage);
	cvtColor(g_srcImage, g_grayImage, COLOR_BGR2GRAY);
	g_maskImage.create(g_srcImage.rows + 2, g_srcImage.cols + 2, CV_8UC1);

	namedWindow("2D Extraction", WINDOW_AUTOSIZE);
	createTrackbar("负差最大值", "效果图", &g_nLowDifference, 255, 0);
	createTrackbar("正差最大值", "效果图", &g_nUpDifference, 255, 0);

	setMouseCallback("2D Extraction", onMouse3D, 0);
	while (!ifGetPlane)
	{
		imshow("2D Extraction", g_bIsColor ? g_dstImage : g_grayImage);
		//获取键盘按键
		int c = waitKey(0);
		//判断ESC是否按下，若按下便退出
		if ((c & 255) == 27)
		{
			cout << "程序退出...........\n";
			break;
		}
		//根据按键的不同，进行各种操作
		switch ((char)c)
		{
			//如果键盘“1”被按下，效果图在在灰度图，彩色图之间互换
		case '1':
			if (g_bIsColor)//若原来为彩色，转为灰度图，并且将掩膜mask所有元素设置为0
			{
				cout << "键盘“1”被按下，切换彩色/灰度模式，当前操作为将【彩色模式】切换为【灰度模式】\n";
				cvtColor(g_srcImage, g_grayImage, COLOR_BGR2GRAY);
				g_maskImage = Scalar::all(0);	//将mask所有元素设置为0
				g_bIsColor = false;	//将标识符置为false，表示当前图像不为彩色，而是灰度
			}
			else//若原来为灰度图，便将原来的彩图image0再次拷贝给image，并且将掩膜mask所有元素设置为0
			{
				cout << "键盘“1”被按下，切换彩色/灰度模式，当前操作为将【彩色模式】切换为【灰度模式】\n";
				g_srcImage.copyTo(g_dstImage);
				g_maskImage = Scalar::all(0);
				g_bIsColor = true;//将标识符置为true，表示当前图像模式为彩色
			}
			break;
			//如果键盘按键“2”被按下，显示/隐藏掩膜窗口
		case '2':
			if (g_bUseMask)
			{
				destroyWindow("mask");
				g_bUseMask = false;
			}
			else
			{
				namedWindow("mask", 0);
				g_maskImage = Scalar::all(0);
				imshow("mask", g_maskImage);
				g_bUseMask = true;
			}
			break;
		case '3':
			cout << "按键“3”被按下，恢复原始图像\n";
			g_srcImage.copyTo(g_dstImage);
			cvtColor(g_dstImage, g_grayImage, COLOR_BGR2GRAY);
			g_maskImage = Scalar::all(0);
			break;
		case '4':
			cout << "按键“4”被按下，使用空范围的漫水填充\n";
			g_nFillMode = 0;
			break;
		case '5':
			cout << "按键“5”被按下，使用渐变、固定范围的漫水填充\n";
			g_nFillMode = 1;
			break;
		case '6':
			cout << "按键“6”被按下，使用渐变、浮动范围的漫水填充\n";
			g_nFillMode = 2;
			break;
		case '7':
			cout << "按键“7”被按下，操作标志符的低八位使用4位的连接模式\n";
			g_nConnectivity = 4;
			break;
		case '8':
			cout << "按键“8”被按下，操作标志符的低八位使用8位的连接模式\n";
			g_nConnectivity = 8;
			break;
		case '9':
			cout << "按键“9”被按下，进行三维平面的绘制并且导入三维模型\n";
			destroyWindow("2D Extraction");
			Draw3DPlane();
			break;
		}
	}

	//waitKey(0);
}

//为了分离蓝色的function
vector<MatND> getHSVHist(Mat &src)
{
	//输入图片得是三通道彩色图片
	assert(!src.empty() && src.channels() == 3);

	//rgb转hsv图像
	Mat hsv;
	cvtColor(src, hsv, CV_BGR2HSV);

	//h的范围是0~180，所以选取30个bin
	//s和v的范围都是0~255，那就选择51个bin
	int hbins = 30;
	int sbins = 51;
	int vbins = 51;

	int hHistSize[] = { hbins };
	int sHistSize[] = { sbins };
	int vHistSize[] = { vbins };

	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 255 };
	float vranges[] = { 0, 255 };
	const float* hRanges[] = { hranges };
	const float* sRanges[] = { sranges };
	const float* vRanges[] = { vranges };
	vector<MatND> hist;

	int hChannels[] = { 0 };
	int sChannels[] = { 1 };
	int vChannels[] = { 2 };

	MatND hHist, sHist, vHist;
	calcHist(&hsv, 1, hChannels, Mat(), hHist, 1, hHistSize, hRanges);
	calcHist(&hsv, 1, sChannels, Mat(), sHist, 1, sHistSize, sRanges);
	calcHist(&hsv, 1, vChannels, Mat(), vHist, 1, vHistSize, vRanges);

	hist.push_back(hHist);
	hist.push_back(sHist);
	hist.push_back(vHist);

	normalize(hist[0], hist[0], 0, 1, NORM_MINMAX, -1, Mat());
	normalize(hist[1], hist[1], 0, 1, NORM_MINMAX, -1, Mat());
	normalize(hist[2], hist[2], 0, 1, NORM_MINMAX, -1, Mat());

	int i;
	int start = -1, end = -1;
	for (i = 0; i < 30; i++)
	{
		float value = hist[0].at<float>(i);
		if (value  > 0)
		{
			if (start == -1)
			{
				start = i;
				end = i;
			}
			else
				end = i;
			std::cout << "H Value" << i << ": " << value << endl;
		}
		else
		{
			if (start != -1)
				std::cout << "H:" << start * 6 << "~" << (end + 1) * 6 - 1 << endl;
			start = end = -1;
		}
	}
	if (start != -1)
		std::cout << "H:" << start * 5 << "~" << (end + 1) * 5 - 1 << endl;

	start = -1, end = -1;
	for (i = 0; i < 51; i++)
	{
		float value = hist[1].at<float>(i);
		if (value  > 0)
		{
			if (start == -1)
			{
				start = i;
				end = i;
			}
			else
				end = i;
			std::cout << "S Value" << i << ": " << value << endl;
		}
		else
		{
			if (start != -1)
				std::cout << "S:" << start * 5 << "~" << (end + 1) * 5 - 1 << endl;
			start = end = -1;
		}
	}
	if (start != -1)
		std::cout << "S:" << start * 5 << "~" << (end + 1) * 5 - 1 << endl;

	start = -1, end = -1;
	for (i = 0; i < 51; i++)
	{
		float value = hist[2].at<float>(i);
		if (value  > 0)
		{
			if (start == -1)
			{
				start = i;
				end = i;
			}
			else
				end = i;
			std::cout << "V Value" << i << ": " << value << endl;
		}
		else
		{
			if (start != -1)
				std::cout << "V:" << start * 5 << "~" << (end + 1) * 5 - 1 << endl;
			start = end = -1;
		}
	}
	if (start != -1)
		std::cout << "V:" << start * 5 << "~" << (end + 1) * 5 - 1 << endl;

	return hist;
}

//从截取出来的图案中分离出所有的蓝色的部分
void filteredBlue(const Mat &inputImage, Mat &resultGray, Mat &resultColor){
	Mat hsvImage;
	cvtColor(inputImage, hsvImage, CV_BGR2HSV);
	resultGray = Mat(hsvImage.rows, hsvImage.cols, CV_8U, cv::Scalar(255));
	resultColor = Mat(hsvImage.rows, hsvImage.cols, CV_8UC3, cv::Scalar(255, 255, 255));
	double H = 0.0, S = 0.0, V = 0.0;
	for (int i = 0; i < hsvImage.rows; i++)
	{
		for (int j = 0; j < hsvImage.cols; j++)
		{
			H = hsvImage.at<Vec3b>(i, j)[0];
			S = hsvImage.at<Vec3b>(i, j)[1];
			V = hsvImage.at<Vec3b>(i, j)[2];

			if ((V >= 250 && V < 255))
			{
				if ((H >= 120 && H < 125))
				{
					resultGray.at<uchar>(i, j) = 0;
					resultColor.at<Vec3b>(i, j)[0] = inputImage.at<Vec3b>(i, j)[0];
					resultColor.at<Vec3b>(i, j)[1] = inputImage.at<Vec3b>(i, j)[1];
					resultColor.at<Vec3b>(i, j)[2] = inputImage.at<Vec3b>(i, j)[2];
				}
			}
		}
	}
}

void Draw3DPlane()
{
	//颜色转换       http://blog.csdn.net/jianjian1992/article/details/51274834
	//OpenCV像素操作 https://www.kancloud.cn/digest/usingopencv/145307
	Mat d_srcImage = imread("planeExtract.jpg", 1);
	Mat d_dstImage;
	Mat color_gray;

	//颜色直方图获取颜色的H,S,V的值
	/*Mat color_src = imread("bluePlane.jpg", 1);
	getHSVHist(color_src);*/

	filteredBlue(d_srcImage, color_gray, d_dstImage);

	imwrite("2d_plane_recognition.jpg", d_dstImage);

	int rows = d_dstImage.rows;
	int cols = d_dstImage.cols;

	PlanePixel = new Point2i[100000];

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int b = d_dstImage.at<Vec3b>(i, j)[0];
			int g = d_dstImage.at<Vec3b>(i, j)[1];
			int r = d_dstImage.at<Vec3b>(i, j)[2];

			if (b >= 250 && g <= 5 && r <= 5)
			{
				PlanePixel[PlanePixelcount].x = j + ROI_p1.x;
				PlanePixel[PlanePixelcount].y = i + ROI_p1.y;
				PlanePixelcount++;
			}
		}
	}
	//cout << "PlanePixelcount的值 = " << PlanePixelcount << endl;
	for (int i = 0; i < PlanePixelcount; i++)
	{	
		int index3 = PlanePixel[i].x + PlanePixel[i].y * iWidthColor;
		//cout << "PlanePixel[i].x = " << PlanePixel[i].x << endl;
		if (pCSPoints[index3].Z != -1 * numeric_limits<float>::infinity())
		{		
			PlaneDepthCount++;
		}
	}

	int indexP = 0;
	PlaneSP = new CameraSpacePoint[PlaneDepthCount];
	
	for (int i = 0; i < PlanePixelcount; i++)
	{
		int index4 = PlanePixel[i].x + PlanePixel[i].y * iWidthColor;
		//cout << "PlanePixel[i].x = " << PlanePixel[i].x << endl;
		if (pCSPoints[index4].Z != -1 * numeric_limits<float>::infinity())
		{
			PlaneSP[indexP].X = pCSPoints[index4].X;
			PlaneSP[indexP].Y = pCSPoints[index4].Y;
			PlaneSP[indexP].Z = -pCSPoints[index4].Z;
			indexP++;
		}
	}
	cout << "选择出来的平面中有意义的点（有x,y,z值）有 " << PlaneDepthCount << "个\n";
	delete[] PlanePixel;
	ifGetPlane = true;
}

void DrawMeshes()
{
	/**************/
	/*************/
	if (!BG_IS_OPEN)
	{
		glPushMatrix();
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i < colorPointCount; i += 3)//
		{

			glColor3ub(pBufferColor[4 * i], pBufferColor[4 * i + 1], pBufferColor[4 * i + 2]);
			if (i < iWidthColor*(iHeightColor - 1))
			{
				glVertex3f(pCSPoints[i].X, pCSPoints[i].Y, -pCSPoints[i].Z);
				glVertex3f(pCSPoints[i + iWidthColor].X, pCSPoints[i + iWidthColor].Y, -pCSPoints[i + iWidthColor].Z);
			}
			else
			{
				i += iWidthColor;
			}
		}
		glEnd();
		//glDisable(GL_BLEND);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i < colorPointCount; i += 3)
		{
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
		glEnd();
		glPopMatrix();
	}
}

int glPrintf(const char *format, ...)
{
	char buffer[65536];
	va_list args;                         //define argument list
	va_start(args, format);               //get this function's argument list
	vsprintf_s(buffer, format, args);     //give argument list to vsprintf(), and save string into buffer
	va_end(args);
	static int x0 = 0, y0 = 0;
	if (base == 0)                        //if we don't have base, execute this part
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

	glGetIntegerv(GL_VIEWPORT, viewport);                  // get view-port, set projection matrix needed
	glGetFloatv(GL_CURRENT_RASTER_POSITION, p0);           // get raster position, use this to calculate the coordinate when changing line
	glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);   // push attributes, be prepared for initialization
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);                           // setting: projection matrix
	glPushMatrix();                                        // be prepared for projection matrix 
	glLoadIdentity();
	gluOrtho2D(0, viewport[2], 0, viewport[3]);
	glMatrixMode(GL_MODELVIEW);                             // custom setting model-view matrix
	glPushMatrix();                                         // be prepared for model-view matrix 
	glLoadIdentity();
	if (memcmp(buffer, ">>glut", 6) == 0)                      // if the input string is ">>glut", set the start point of article in the left-top point.
	{
		sscanf_s(buffer, ">>glut(%i,%i)", &x0, &y0);
		glTranslatef(x0, -y0, 0);
		glRasterPos2f(4, viewport[3] - nHeight);
		//glRasterPos2f(50.0f, 20.0f);
		//cout << endl << "4 " << viewport[3] - nHeight;
	}
	else if (strcmp(buffer, ">>free") == 0)                 // if the input string is ">>free", delete the string (The sample did not use others）
	{
		glDeleteLists(base, 65536); base = 0;
		memset(loaded, 0, 65536 * sizeof(bool));
		DeleteObject(hFont);
	}
	else
	{
		glRasterPos2f(p0[0], p0[1]);                                             // (这句是没实质作用的, 不过, 要先呼叫它, 下一句 glGetFloatv() 才能正确执行 )
		glGetFloatv(GL_CURRENT_RASTER_COLOR, c0[1]);                             // 最得 glColor() 所述的颜色
		len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, 0, 0);     // 计算转换成大五码后, 字符串的长度
		wstr = (wchar_t*)malloc(len*sizeof(wchar_t));                            // 配置内存去储存 大五码字符串
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, wstr, len);      // 把 buffer 转换成 大五码字符串
		for (j = 0; j < 2; j++)                                                  // 秀两次字符串
		{
			glColor4fv(c0[j]);                                     //设定 字符串颜色
			//glColor3b(255, 0, 0);
			glRasterPos2f(p0[0] + offset[j], p0[1] - offset[j]);   //设定 字符串起始位置
			for (i = 0; i < len - 1; i++)
			{
				if (wstr[i] == '\n')                                        // 如果是 '\n', 就换新行
				{
					glGetFloatv(GL_CURRENT_RASTER_POSITION, (float*)&p1);   //取得现在的位置
					//glRasterPos2f(4 + offset[j], p1[1] - (nHeight + 2));    //设定新行的起始位置
					glRasterPos2f(4 + x0 + offset[j], p1[1] - (nHeight + 2));
				}
				else
				{
					if (!loaded[wstr[i]])    // 如果字符未被加载
					{
						if (hdc == 0)
						{
							hdc = wglGetCurrentDC();    // 取得 device context 的 handle (窗口系统的东西)
							SelectObject(hdc, hFont);   // 选取 font handle (窗口系统的东西)
						}
						wglUseFontBitmapsW(hdc, wstr[i], 1, base + wstr[i]);    // 加载字符的字型图像
						loaded[wstr[i]] = TRUE;                                 // 标示这字符已被加载
					}
					glCallList(base + wstr[i]);   //  绘画字符的 display list
				}
			}
		}
		free(wstr);    // 把配置了的内存归还系统
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
	return 0;
}
#pragma endregion OpenGL Function

#pragma region Mt232 Function
void MtMove(void)
{
	MtHome(); //保证在移动的时候一定已经重新回到原点
	MtReflash(md);
	cout << "md->x : " << md->x << endl;
	cout << "md->y : " << md->y << endl;
	cout << "md->z : " << md->z << endl;
	Sleep(100);
	//开始的时候调用Machine的声音
	for (int i = 0; i < 5; i++)
	{
		MtCmd("mt_out 12,1");
		Sleep(100);
		MtCmd("mt_out 12,0");
		Sleep(100);
	}

	for (int i = 0; i < ROIStorageCount; i++)
	{
		char mybuffx[50], mybuffy[50], mybuffz[50], mybuffu[50], mybuffv[50];
		char commandx[60] = "mt_m_x ", commandy[60] = "mt_m_y ", commandz[60] = "mt_m_z ", commandu[60] = "mt_m_u ", commandv[60] = "mt_m_v ";
		switch (STORAGE_TYPE)
		{
		case 0://在这里Z值为负值，不知道原因，需要进一步勘测
			sprintf(mybuffx, "%f", ROICameraSP_MachineCoord_Storage[i].X * 1000 - 81);
			sprintf(mybuffy, "%f", ROICameraSP_MachineCoord_Storage[i].Y * 1000 - 69);//在这里调整Z的值的时候，一定要在MtCheck里面也对于存下来的点的值进行调整
			sprintf(mybuffz, "%f", ROICameraSP_MachineCoord_Storage[i].Z * 1000 + 170/*- (ROICameraSP_MachineCoord_Storage[i].Y * 1000 + 40)* tan(dev_theta)*/);
			break;
		case 1://在这里Z值为负值，不知道原因，需要进一步勘测
			sprintf(mybuffx, "%f", ROICameraSP_MachineCoord_Proj_Storage[i].X * 1000 - 81);
			sprintf(mybuffy, "%f", ROICameraSP_MachineCoord_Proj_Storage[i].Y * 1000 - 69);
			sprintf(mybuffz, "%f", ROICameraSP_MachineCoord_Proj_Storage[i].Z * 1000 + 170 /*- ROICameraSP_MachineCoord_Proj_Storage[i].Y * 1000 * tan(dev_theta)*/);
			break;

		}
		strcat(commandx, mybuffx);
		strcat(commandy, mybuffy);
		strcat(commandz, mybuffz);

		MtCmd(commandx);
		MtCmd(commandy);
		MtCmd(commandz);

	}

	Thread^ thread1 = gcnew Thread(gcnew ThreadStart(MtCheck));
	thread1->Name = "thread1";
	thread1->Start();
}

void Mt_Line_Move()
{
	MtHome(); //保证在移动的时候一定已经重新回到原点
	MtReflash(md);
	cout << "md->x : " << md->x << endl;
	cout << "md->y : " << md->y << endl;
	cout << "md->z : " << md->z << endl;
	Sleep(100);
	//开始的时候调用Machine的声音
	for (int i = 0; i < 5; i++)
	{
		MtCmd("mt_out 12,1");
		Sleep(100);
		MtCmd("mt_out 12,0");
		Sleep(100);
	}
	if (LineSP_MachCoord != nullptr)
	{
		delete[] LineSP_MachCoord;
	}
	LineSP_MachCoord = new CameraSpacePoint[2];
	LineSP_MachCoord[0] = PlaneSP_MachCoord[Head];
	LineSP_MachCoord[1] = PlaneSP_MachCoord[Tail];

	for (int i = 0; i <= 1; i++)
	{
		char mybuffx[50], mybuffy[50], mybuffz[50], mybuffu[50], mybuffv[50];
		char commandx[60] = "mt_m_x ", commandy[60] = "mt_m_y ", commandz[60] = "mt_m_z ", commandu[60] = "mt_m_u ", commandv[60] = "mt_m_v ";
		float value = 0;
		switch (0)
		{
		case 0://在这里Z值为负值，不知道原因，需要进一步勘测
			sprintf(mybuffx, "%f", LineSP_MachCoord[i].X * 1000 - 81);
			sprintf(mybuffy, "%f", LineSP_MachCoord[i].Y * 1000 - 69);//在这里调整Z的值的时候，一定要在MtCheck里面也对于存下来的点的值进行调整
			sprintf(mybuffz, "%f", LineSP_MachCoord[i].Z * 1000 + 170/*- (ROICameraSP_MachineCoord_Storage[i].Y * 1000 + 40)* tan(dev_theta)*/);
			value = LineSP_MachCoord[i].Z * 1000 + 170;
			break;
		}
		strcat(commandx, mybuffx);
		strcat(commandy, mybuffy);
		strcat(commandz, mybuffz);

		MtCmd(commandx);
		MtCmd(commandy);
		MtCmd(commandz);
		do
		{
			MtReflash(md);
			//cout << value << " " << md->z << endl;
		} while (abs(md->z - value) > 0.01);
	}
}

void Mt_XMove(float mt_x)
{
	MtReflash(md);
	MtCmd("mt_speed 50");
	char mybuffx[60];
	char commandx[60] = "mt_m_x ";
	sprintf(mybuffx, "%f", mt_x);
	strcat(commandx, mybuffx);
	MtCmd(commandx);
	Sleep(1);	
}

void Mt_YMove(float mt_y)
{
	MtReflash(md);
	MtCmd("mt_speed 50");
	char mybuffy[60];
	char commandy[60] = "mt_m_y ";
	sprintf(mybuffy, "%f", mt_y);
	strcat(commandy, mybuffy);
	MtCmd(commandy);
	Sleep(1);
}

void Mt_ZMove(float mt_z)
{
	MtReflash(md);
	MtCmd("mt_speed 50");
	char mybuffz[60];
	char commandz[60] = "mt_m_z ";
	sprintf(mybuffz, "%f", mt_z);
	strcat(commandz, mybuffz);
	MtCmd(commandz);
	Sleep(1);
}


void Mt_UMove(float mt_u)
{
	MtReflash(md);
	MtCmd("mt_speed 50");
	char mybuffu[60];
	char commandu[60] = "mt_m_u ";
	sprintf(mybuffu, "%f", mt_u);
	strcat(commandu, mybuffu);
	MtCmd(commandu);
	Sleep(1);
}

void Mt_VMove(float mt_v)
{
	MtReflash(md);
	MtCmd("mt_speed 50");
	char mybuffv[60];
	char commandv[60] = "mt_m_v ";
	sprintf(mybuffv, "%f", mt_v);
	strcat(commandv, mybuffv);
	MtCmd(commandv);
	Sleep(1);
}

void MtCheck(void)
{
	for (mtmove_step = 0; mtmove_step < ROIStorageCount; mtmove_step++)
	{
		float value = 0;
		switch (STORAGE_TYPE)
		{
		case 0:
			//这里是怎么搞得？为什么需要用Z的值减去Y的值乘以一个tan值:因为Kinect 看物体的时候会有一个角度，这里的tan值是为了能够消除角度带来的误差:mtmove_step使用开控制谁先谁后
			value = ROICameraSP_MachineCoord_Storage[mtmove_step].Z * 1000  +170/*- ROICameraSP_MachineCoord_Storage[mtmove_step].Y * 1000 * tan(dev_theta)*/;
			break;
		case 1:
			value = ROICameraSP_MachineCoord_Proj_Storage[mtmove_step].Z * 1000 /*- ROICameraSP_MachineCoord_Proj_Storage[mtmove_step].Y * 1000 * tan(dev_theta)*/;
			break;
		}
		do
		{
			MtReflash(md);
			//cout << value << " " << md->z << endl;
		} while (abs(md->z - value) > 0.01);
	}
	//mtmove_step等于
	mtmove_step = ROIStorageCount - 1;
}
#pragma endregion Mt232 Function

#pragma region PortAudio Function
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
	fprintf(stderr, "An error occurred while using the PortAudio stream\n");
	fprintf(stderr, "Error number: %d\n", paInit.result());
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(paInit.result()));
	return 0;
}
#pragma endregion PortAudio Function

#pragma region Path Generation Function
/*--------------------------------------------------------*/
/*----------------Path Generation Function----------------*/
/*--------------------------------------------------------*/
void PathGenProjToPlane(CameraSpacePoint* Data, int DataNum, double* PlaneCoeff, CameraSpacePoint* Result)
{
	for (int i = 0; i < DataNum; i++)
	{
		double tScal = -(PlaneCoeff[0] * Data[i].X + PlaneCoeff[1] * Data[i].Y + PlaneCoeff[2] * Data[i].Z + PlaneCoeff[3]) / (pow(PlaneCoeff[0], 2) + pow(PlaneCoeff[1], 2) + pow(PlaneCoeff[2], 2));
		Result[i].X = Data[i].X + tScal * PlaneCoeff[0];
		Result[i].Y = Data[i].Y + tScal * PlaneCoeff[1];
		Result[i].Z = Data[i].Z + tScal * PlaneCoeff[2];
	}
}
#pragma endregion Path Generation Function

#pragma region AR Function
/*---------------------------------------------------*/
/*----------------AR Function Declare----------------*/
/*---------------------------------------------------*/
float Dot(float* Vector1, float* Vector2)
{
	return Vector1[0] * Vector2[0] + Vector1[1] * Vector2[1] + Vector1[2] * Vector2[2];
}

float Determinant(float a, float b, float c, float d)
{
	return a*d - b*c;
}

//需要修改的
void ARFunc_FindProj()//换了模型之后有影响
{
	/*改成OBJ頂面名面方程式只要找一次就好*/
	/*-----------------------------------*/
	CameraSpacePoint* CubeTop1 = new CameraSpacePoint[4];
	float scale = 1;
#pragma region NewWorkepiece
	//input .obj file's top surface's 4 points
	
	CubeTop1[0].X = OBJ->vertices[3 * 12 + 0] * scale; //0
	CubeTop1[0].Y = OBJ->vertices[3 * 12 + 1] * scale; //0
	CubeTop1[0].Z = OBJ->vertices[3 * 12 + 2] * scale; //12

	CubeTop1[1].X = OBJ->vertices[3 * 13 + 0] * scale; //190
	CubeTop1[1].Y = OBJ->vertices[3 * 13 + 1] * scale; //0
	CubeTop1[1].Z = OBJ->vertices[3 * 13 + 2] * scale; //12

	CubeTop1[2].X = OBJ->vertices[3 * 22 + 0] * scale; //0
	CubeTop1[2].Y = OBJ->vertices[3 * 22 + 1] * scale; //240
	CubeTop1[2].Z = OBJ->vertices[3 * 22 + 2] * scale; //12

	CubeTop1[3].X = OBJ->vertices[3 * 18 + 0] * scale; //190
	CubeTop1[3].Y = OBJ->vertices[3 * 18 + 1] * scale; //240
	CubeTop1[3].Z = OBJ->vertices[3 * 18 + 2] * scale; //12
	
#pragma endregion NewWorkpiece

#pragma region OriginObj
	//CubeTop1[0].X = OBJ->vertices[3 * 1 + 0] * 0.001; //0.0000
	//CubeTop1[0].Y = OBJ->vertices[3 * 1 + 1] * 0.001; //70.0000
	//CubeTop1[0].Z = OBJ->vertices[3 * 1 + 2] * 0.001; //80.0000

	//CubeTop1[1].X = OBJ->vertices[3 * 32 + 0] * 0.001; //150.000
	//CubeTop1[1].Y = OBJ->vertices[3 * 32 + 1] * 0.001; //70.0000
	//CubeTop1[1].Z = OBJ->vertices[3 * 32 + 2] * 0.001; //80.0000

	//CubeTop1[2].X = OBJ->vertices[3 * 44 + 0] * 0.001; //150.000
	//CubeTop1[2].Y = OBJ->vertices[3 * 44 + 1] * 0.001; //70.000
	//CubeTop1[2].Z = OBJ->vertices[3 * 44 + 2] * 0.001; //0.000

	//CubeTop1[3].X = OBJ->vertices[3 * 4 + 0] * 0.001; //0.0000
	//CubeTop1[3].Y = OBJ->vertices[3 * 4 + 1] * 0.001; //70.000
	//CubeTop1[3].Z = OBJ->vertices[3 * 4 + 2] * 0.001; //0.00
#pragma endregion OriginObj

	//这个是用来寻找顶面的平面的法向量CubeNorm1;

	CameraSpacePoint* CubeNorm1 = new CameraSpacePoint[2];

	//测试得知OBJ->vertices[3 * 1 + 0]就是文件中的第一个vertices

	//The normal vector of one of the triangles in the plane
	//顶面其中一个三角片的法向量
	//平面5,6,7就是第10个三角形，第134个三角形的法向量是向上的
	CubeNorm1[0].X = OBJ->normals[OBJ->triangles[134].nindices[0] * 3 + 0];
	CubeNorm1[0].Y = OBJ->normals[OBJ->triangles[134].nindices[0] * 3 + 1];
	CubeNorm1[0].Z = OBJ->normals[OBJ->triangles[134].nindices[0] * 3 + 2];
	//起点与终点
	//start point & finish point
	CubeNorm1[1].X = 0;
	CubeNorm1[1].Y = 0;
	CubeNorm1[1].Z = 0;

	GLfloat TransM_AR[16] = { 0 };
	TransM_AR[0] = TransM_AR[5] = TransM_AR[10] = TransM_AR[15] = 1;
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(ObjPosi.x, ObjPosi.y, ObjPosi.z);
	glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
	
	glMultMatrixf(M_Cubic);
	glGetFloatv(GL_MODELVIEW_MATRIX, TransM_AR);
	glPopMatrix();

	ROITrans(CubeNorm1, 2, TransM_AR, CubeNorm1);
	ROITrans(CubeTop1, 4, TransM_AR, CubeTop1);

	//figure out the plane equation of top surface of .OBJfile
	//求解OBJ档案的顶面大平面的平面方程式
	ARFuncNormal1[0] = CubeNorm1[0].X - CubeNorm1[1].X;
	ARFuncNormal1[1] = CubeNorm1[0].Y - CubeNorm1[1].Y;
	ARFuncNormal1[2] = CubeNorm1[0].Z - CubeNorm1[1].Z;
	ARFuncNormal1[3] = -(ARFuncNormal1[0] * CubeTop1[0].X + ARFuncNormal1[1] * CubeTop1[0].Y + ARFuncNormal1[2] * CubeTop1[0].Z);
	//求解ROI中心点到OBJ档案顶面的投影
	//Figure out the ROI center point to .OBJ file's projection point
	CameraSpacePoint* ROICenter = new CameraSpacePoint;
	ARFunc_ROICSP_Proj = new CameraSpacePoint;
	
	ROICenter->X = ROICenterCameraS.x;
	ROICenter->Y = ROICenterCameraS.y;
	ROICenter->Z = ROICenterCameraS.z;

	PathGenProjToPlane(ROICenter, 1, ARFuncNormal1, ARFunc_ROICSP_Proj);

	//ARFunc_ROICSP_Proj->Y = -ARFunc_ROICSP_Proj->Y;
	GLMgroup* group = OBJ->groups;
	group = group->next;
	//cout << "group-> numtraingles = " << group->numtriangles << endl;
	//检查投影点是不是在区域内部
	for (int TriCount = 0; TriCount < group->numtriangles; TriCount++)
	{
		//TriID三角形数量
		int TriID = group->triangles[TriCount];
		double scale = 1;
		CameraSpacePoint TriVertex[3];
		//每个三角形的第一个顶点的X,Y,Z的值乘以scale:在这里的scale是因为需要防止单位不同，之前模型单位是米，系统单位为厘米，新模型单位为厘米，因此scale从0.001重新成为1
		TriVertex[0].X = OBJ->vertices[OBJ->triangles[TriID].vindices[0] * 3 + 0] * scale;
		TriVertex[0].Y = OBJ->vertices[OBJ->triangles[TriID].vindices[0] * 3 + 1] * scale;
		TriVertex[0].Z = OBJ->vertices[OBJ->triangles[TriID].vindices[0] * 3 + 2] * scale;

		TriVertex[1].X = OBJ->vertices[OBJ->triangles[TriID].vindices[1] * 3 + 0] * scale;
		TriVertex[1].Y = OBJ->vertices[OBJ->triangles[TriID].vindices[1] * 3 + 1] * scale;
		TriVertex[1].Z = OBJ->vertices[OBJ->triangles[TriID].vindices[1] * 3 + 2] * scale;

		TriVertex[2].X = OBJ->vertices[OBJ->triangles[TriID].vindices[2] * 3 + 0] * scale;
		TriVertex[2].Y = OBJ->vertices[OBJ->triangles[TriID].vindices[2] * 3 + 1] * scale;
		TriVertex[2].Z = OBJ->vertices[OBJ->triangles[TriID].vindices[2] * 3 + 2] * scale;

		ROITrans(TriVertex, 3, TransM_AR, TriVertex);
		//查看投影点是在模型里面还是外面
		if (ARFunc_InsideTriCheck(ARFunc_ROICSP_Proj, &TriVertex[0], &TriVertex[2], &TriVertex[1]))
		{
			IS_ARFunc_InsideTriCheck = TRUE;
			break;
		}
		else
		{
			IS_ARFunc_InsideTriCheck = FALSE;
		}
	}
}

/*Check out if the projection point is in the operation area查看是不是在区域内//换了模型之后有影响*/
bool ARFunc_InsideTriCheck(CameraSpacePoint* Point, CameraSpacePoint* TriVertex0, CameraSpacePoint* TriVertex1, CameraSpacePoint* TriVertex2)
{
	BOOL IS_INSIDE_TRI = FALSE;
	//Vector AP AB AC in order
	float Vector[3][3];

	Vector[0][0] = Point->X - TriVertex0->X;
	Vector[0][1] = Point->Y - TriVertex0->Y;
	Vector[0][2] = Point->Z - TriVertex0->Z;

	Vector[1][0] = TriVertex1->X - TriVertex0->X;
	Vector[1][1] = TriVertex1->Y - TriVertex0->Y;
	Vector[1][2] = TriVertex1->Z - TriVertex0->Z;

	Vector[2][0] = TriVertex2->X - TriVertex0->X;
	Vector[2][1] = TriVertex2->Y - TriVertex0->Y;
	Vector[2][2] = TriVertex2->Z - TriVertex0->Z;

	float delt = Determinant(Dot(Vector[2], Vector[2]), Dot(Vector[1], Vector[2]), Dot(Vector[1], Vector[2]), Dot(Vector[1], Vector[1]));
	float deltX = Determinant(Dot(Vector[0], Vector[2]), Dot(Vector[1], Vector[2]), Dot(Vector[0], Vector[1]), Dot(Vector[1], Vector[1]));
	float deltY = Determinant(Dot(Vector[2], Vector[2]), Dot(Vector[0], Vector[2]), Dot(Vector[1], Vector[2]), Dot(Vector[0], Vector[1]));

	float u = deltX / delt, v = deltY / delt;
	if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1)
	{
		IS_INSIDE_TRI = TRUE;
	}
	else
	{
		IS_INSIDE_TRI = FALSE;
	}
	return IS_INSIDE_TRI;
}
#pragma endregion AR Function

#pragma region DrawCubic Function
/*------------------------------------------*/
/*--------------OpenGL Function-------------*/
/*------------------------------------------*/
void DrawCubic()//换了模型之后有影响
{
	/*从opengl坐标转换到Kinect坐标
	 *Virtual object's translate and rotate function*/
	glPushMatrix();
	
	glTranslatef(ObjPosi.x, ObjPosi.y, ObjPosi.z);
	/*Interact: the center red point of three-Axis dispenser's
	 *0 translate value*/
	glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
	glRotatef(90, 1, 0, 0);
	glMultMatrixf(M_Cubic);
	
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	/*If the collide not happened, we draw the cubic with the original color 没有碰撞就画出原来的颜色*/
 	if (!ROI_IS_COLLIDE)
	{
		GLMgroup* group = OBJ->groups;
		int gCount = 0;
		while (group)
		{
			glColor3f(1, 1, 1);
			glVertexPointer(3, GL_FLOAT, 0, vertices[gCount]);//指定数组顶点
			
			glTexCoordPointer(2, GL_FLOAT, 0, vtextures[gCount]);//纹理映射坐标用法
			
			glBindTexture(GL_TEXTURE_2D, textures[gCount]);//一定不能放在glBegin和glEnd函数之中
			glDrawArrays(GL_TRIANGLES, 0, group->numtriangles * 3);//第三个为count点的绘制次数
			gCount++;
			group = group->next;
		}
	}
	else
	{
		GLMgroup* group = OBJ->groups;
		int gCount = 0;
		while (group)
		{
			/*---------------------------------------------------------------------*/
			/*Now, our method is to draw the color cubic and draw the wireframe of the cubic, need to have a better method*/
			/*----------------------如果有碰撞就画两次-----------------------------------------------*/
			glVertexPointer(3, GL_FLOAT, 0, vertices[gCount]);
			glColor3f(1, 0, 0);
			glDrawArrays(GL_TRIANGLES, 0, group->numtriangles * 3);
			glColor3f(0, 0, 0);
			glLineWidth(2.5);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, group->numtriangles * 3);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			gCount++;
			group = group->next;
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);// http://www.cnblogs.com/Clingingboy/archive/2010/10/16/1853304.html 顶点数组
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
}
#pragma endregion DrawCubic Function

#pragma region Kinect Function
/*------------------------------------------*/
/*--------------Kinect Function-------------*/
/*------------------------------------------*/
/*------------------------------------------ The first Step ------------------------------------------*/
int KinectInit()
{
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
			return -1;
		}
		// release Frame source
		cout << "Release frame source" << endl;
		pFrameSource->Release();
		pFrameSource = nullptr;
	}
	//pixelFiltering(depthImage);
	// 4. Coordinate Mapper
	if (pSensor->get_CoordinateMapper(&Mapper) != S_OK)
	{
		cerr << "get_CoordinateMapper failed" << endl;
		return -1;
	}
	mDepthImg = cv::Mat::zeros(iHeightDepth, iWidthDepth, CV_16UC1);
	mImg8bit = cv::Mat::zeros(iHeightDepth, iWidthDepth, CV_8UC1);
	mColorImg = cv::Mat::zeros(iHeightColor, iWidthColor, CV_8UC4);

#pragma region pixelfiltering
	/*
	* Another File
	*/
#pragma endregion pixelfiltering
}

/*------------------------------------------ The second Step ------------------------------------------*/
/*读color frame, depth frame,然后mapper,然后就show Image*/
void KinectUpdate()
{
	/*----------------------------Read color data---------------------------*/
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
	/*----------------------------Read depth data---------------------------*/
	IDepthFrame* pDFrameDepth = nullptr;
	if (pFrameReaderDepth->AcquireLatestFrame(&pDFrameDepth) == S_OK)
	{
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, pBufferDepth);
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, reinterpret_cast<UINT16*>(mDepthImg.data));
		pDFrameDepth->Release();
		pDFrameDepth = nullptr;
	}
#pragma region Remap ColorImg
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
#pragma endregion Remap ColorImg
	
	/*--------------Mapper Function (Point Cloud)-------------*/
	Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);

	/*--------------Call Window Function-------------*/
	/*show image can have a color frame to you*/
	ShowImage();
}

void ShowImage()
{
	//这里是用opencv读出来kinect的图像
	/*在这里就是读出 color map, 其中 depth 的被删掉了*/
	/*open a color map, the depth map was deleted */
	namedWindow("ROI Map", WINDOW_AUTOSIZE);
	//namedWindow("Depth Map", WINDOW_AUTOSIZE);
	//imshow("Depth Map", mDepthImg);
	/*--------------Set Mouse Callback Function and Find ROI-------------*/
	/*-----------Use green box to get ROI by mouse-----------*/
	cvSetMouseCallback("ROI Map", onMouseROI, NULL);//鼠标事件发生的时候被调用的函数指针
	mColorImg.copyTo(ROI);
	//// finish or not finish the ROI getting process
	if (ROI_S2 == TRUE && ROI_S1 == FALSE)
	{
		int thickness = 2;
		rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), thickness);
		/*when the code make sure that the ROI Rec has been done. Use FindROI to do color tracking*/
		FindROI();
	}
	//cout << "-----------------------ShowImage--------------------" << endl;
	//opencv需要编译 http://blog.csdn.net/github_28833431/article/details/48226291
	//不是opencv编译的问题，在这里如果关闭集显的话不能debug
	imshow("ROI Map", ROI);
}
#pragma endregion Kinect Function

#pragma region Load OBJ Function
/*--------------------------------------------*/
/*--------------Load Obj Function-------------*/
/*--------------------------------------------*/
void loadOBJModel()//换了模型之后有影响
{
	// read an obj model here
	if (OBJ != NULL) {
		free(OBJ);
	}
	//OBJ = glmReadOBJ("box33/456.obj");
	
	OBJ = glmReadOBJ("box33/WorkPiece/WorkPiece.obj");
	
	// traverse the color model
	traverseModel();
}

/*It's traverseModel because we need to read the data column by column*/
void traverseModel()
{
	GLMgroup* group = OBJ->groups;

	float maxx, maxy, maxz;
	float minx, miny, minz;
	float dx, dy, dz;
	//glm中，vertices3,4,5是第一个点
	maxx = minx = OBJ->vertices[3];
	maxy = miny = OBJ->vertices[4];
	maxz = minz = OBJ->vertices[5];

	//3,4,5的值为第一组v的值
	//找最大最小值：三个维度,遍历模型中顶点的数目
	for (unsigned int i = 2; i <= OBJ->numvertices; i++)
	{
		GLfloat vx, vy, vz;

		vx = OBJ->vertices[i * 3 + 0];
		vy = OBJ->vertices[i * 3 + 1];
		vz = OBJ->vertices[i * 3 + 2];

		if (vx > maxx)
			maxx = vx;
		if (vx < minx)
			minx = vx;
		if (vy > maxy)
			maxy = vy;
		if (vy < miny)
			miny = vy;
		if (vz > maxz)
			maxz = vz;
		if (vz < minz)
			minz = vz;
	}
	//printf("max\n%f %f, %f %f, %f %f\n", maxx, minx, maxy, miny, maxz, minz);
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;
	//找到模型的顶点中X,Y,Z的值最大的三个点和X,Y,Z值最小的三个点，从而算出模型的长宽高
	//printf("dx,dy,dz = %f %f %f\n", dx, dy, dz);
	//GLfloat normalizationScale = myMax(myMax(dx, dy), dz) / 2;
	//算出模型的长宽高之后，拿到模型中心点所在的位置
	OBJ->position[0] = (maxx + minx) / 2;
	OBJ->position[1] = (maxy + miny) / 2;
	OBJ->position[2] = (maxz + minz) / 2;
	
	int gCount = 0;
	while (group) {
		for (unsigned int i = 0; i < group->numtriangles; i++) {

			// triangle index
			int triangleID = group->triangles[i];

			// the index of each vertex vindices是三角形的顶点
			int indv1 = OBJ->triangles[triangleID].vindices[0];
			int indv2 = OBJ->triangles[triangleID].vindices[1];
			int indv3 = OBJ->triangles[triangleID].vindices[2];

			//在这里可以得到，见文件triangle.txt
			/*cout << "OBJ->triangles["<<triangleID<<"].vindices[0] = " << OBJ->triangles[triangleID].vindices[0] << endl;
			cout << "OBJ->triangles["<<triangleID<<"].vindices[1] = " << OBJ->triangles[triangleID].vindices[1] << endl;
			cout << "OBJ->triangles["<<triangleID<<"].vindices[2] = " << OBJ->triangles[triangleID].vindices[2] << endl;*/

			// vertices
			GLfloat vx, vy, vz;
			double scale = 0.001;
			//抓取的三角形的第一个顶点的数据X,Y,Z
			vx = OBJ->vertices[indv1 * 3];
			vy = OBJ->vertices[indv1 * 3 + 1];
			vz = OBJ->vertices[indv1 * 3 + 2];

			//读三角形三个点
			//printf("vertices1 %f %f %f\n", vx, vy, vz);
			//这里是把原来的 vertices 转移到新的二维数组中，原来的单位是米，我们转变后单位是毫米
			/* The model size's unit is mm but glm.h's size unit is m, so we use a scale to resize it\n*/
			//让真实的三角形的第一个顶点的数据等于模型中的数据
			vertices[gCount][i * 9 + 0] = vx * scale;
			vertices[gCount][i * 9 + 1] = vy * scale;
			vertices[gCount][i * 9 + 2] = vz * scale;

			//在这里可以得到，见文件vertices.txt
			/*cout << "vertices[" << gCount << "][" << i  << " * 9]+0 = " << vertices[gCount][i * 9 + 0] << endl;
			cout << "vertices[" << gCount << "][" << i << " * 9]+1 = " << vertices[gCount][i * 9 + 1] << endl;
			cout << "vertices[" << gCount << "][" << i  << " * 9]+2 = " << vertices[gCount][i * 9 + 2] << endl;*/
			//抓取的三角形的第二个顶点的数据
			vx = OBJ->vertices[indv2 * 3];
			vy = OBJ->vertices[indv2 * 3 + 1];
			vz = OBJ->vertices[indv2 * 3 + 2];

			vertices[gCount][i * 9 + 3] = vx * scale;
			vertices[gCount][i * 9 + 4] = vy * scale;
			vertices[gCount][i * 9 + 5] = vz * scale;
			//抓取的三角形的第三个顶点的数据
			vx = OBJ->vertices[indv3 * 3];
			vy = OBJ->vertices[indv3 * 3 + 1];
			vz = OBJ->vertices[indv3 * 3 + 2];

			vertices[gCount][i * 9 + 6] = vx * scale;
			vertices[gCount][i * 9 + 7] = vy * scale;
			vertices[gCount][i * 9 + 8] = vz * scale;

			//抓取的三角形的三个点的法向量
			int indn1 = OBJ->triangles[triangleID].nindices[0];
			int indn2 = OBJ->triangles[triangleID].nindices[1];
			int indn3 = OBJ->triangles[triangleID].nindices[2];

			// 三个点的法向量的X,Y,Z数据
			normals[gCount][i * 9 + 0] = OBJ->normals[indn1 * 3 + 0];
			normals[gCount][i * 9 + 1] = OBJ->normals[indn1 * 3 + 1];
			normals[gCount][i * 9 + 2] = OBJ->normals[indn1 * 3 + 2];

			normals[gCount][i * 9 + 3] = OBJ->normals[indn2 * 3 + 0];
			normals[gCount][i * 9 + 4] = OBJ->normals[indn2 * 3 + 1];
			normals[gCount][i * 9 + 5] = OBJ->normals[indn2 * 3 + 2];

			normals[gCount][i * 9 + 6] = OBJ->normals[indn3 * 3 + 0];
			normals[gCount][i * 9 + 7] = OBJ->normals[indn3 * 3 + 1];
			normals[gCount][i * 9 + 8] = OBJ->normals[indn3 * 3 + 2];

			//三角形的三个点的纹理坐标
			int indt1 = OBJ->triangles[triangleID].tindices[0];
			int indt2 = OBJ->triangles[triangleID].tindices[1];
			int indt3 = OBJ->triangles[triangleID].tindices[2];
			//三角形的三个点的纹理坐标的X,Y值
			vtextures[gCount][i * 6 + 0] = OBJ->texcoords[indt1 * 2 + 0];
			vtextures[gCount][i * 6 + 1] = OBJ->texcoords[indt1 * 2 + 1];

			vtextures[gCount][i * 6 + 2] = OBJ->texcoords[indt2 * 2 + 0];
			vtextures[gCount][i * 6 + 3] = OBJ->texcoords[indt2 * 2 + 1];

			vtextures[gCount][i * 6 + 4] = OBJ->texcoords[indt3 * 2 + 0];
			vtextures[gCount][i * 6 + 5] = OBJ->texcoords[indt3 * 2 + 1];
		}
		//指向下一组模型数据
		group = group->next;
		gCount++;
		//vertices;
	}
	cout << "gcount " << gCount << endl;
}

//换了模型之后有影响
void SetTexObj(char *filename, int ii)
{
	glBindTexture(GL_TEXTURE_2D, /*textures[ii]*/ii);
	IplImage *imageCV; // 影像的資料結構
	imageCV = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR); // 讀取影像的
	//cvShowImage("HelloWorld", imageCV);
	//cvWaitKey(0); // 停留視窗
	char *imageGL;
	imageGL = new char[3 * imageCV->height*imageCV->width];
	int step = imageCV->width * imageCV->nChannels;
	for (int i = 0; i < imageCV->height; i++)
	{
		for (int j = 0; j < imageCV->widthStep; j = j + 3)
		{
			imageGL[(imageCV->height - 1 - i)*step + j + 0] = imageCV->imageData[i*step + j + 2];
			imageGL[(imageCV->height - 1 - i)*step + j + 1] = imageCV->imageData[i*step + j + 1];
			imageGL[(imageCV->height - 1 - i)*step + j + 2] = imageCV->imageData[i*step + j + 0];
		}
	}

	//材质控制
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageCV->width, imageCV->height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageGL);
	
	delete[] imageGL;
	cvReleaseImage(&imageCV); // 釋放IplImage資料結構
}
//换了模型之后有影响
void Texture()
{
	//glEnable(GL_TEXTURE_2D);
	glGenTextures(7, textures);
	
	SetTexObj("box33/box5/m1.jpg", textures[0]);
	SetTexObj("box33/box5/m2.jpg", textures[1]);
	SetTexObj("box33/box5/m3.jpg", textures[2]);
	SetTexObj("box33/box5/m4.jpg", textures[3]);
	SetTexObj("box33/box5/m5.jpg", textures[4]);
	SetTexObj("box33/box5/m6.jpg", textures[5]);
	SetTexObj("box33/box5/m7.jpg", textures[6]);
}

#pragma endregion Load OBJ Function

#pragma endregion Function