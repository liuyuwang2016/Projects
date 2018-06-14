#ifndef _FUNCTION_H_
#define _FUNCTION_H_
#include "Declae.h"
/*-----------Bullet Physics-----------*/

#pragma region loadConfigFile
void loadConfigFile()
{
	ifstream fin;
	string line;
	fin.open("config.txt", ios::in);
	if (fin.is_open())
	{
		while (getline(fin, line))
		{
			filenames.push_back(line);
		}
		fin.close();
	}
	else
	{
		cout << "Unable to open the config file!" << endl;
	}
	for (int i = 0; i < filenames.size(); i++)
		printf("%s\n", filenames[i].c_str());
}
#pragma endregion loadConfigFile

//void showShaderCompileStatus(GLuint shader, GLint *shaderCompiled)
//{
//	glGetShaderiv(shader, GL_COMPILE_STATUS, shaderCompiled);
//	if (GL_FALSE == (*shaderCompiled))
//	{
//		GLint maxLength = 0;
//		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
//
//		// The maxLength includes the NULL character.
//		GLchar *errorLog = (GLchar*)malloc(sizeof(GLchar)* maxLength);
//		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
//		fprintf(stderr, "%s", errorLog);
//
//		glDeleteShader(shader);
//		free(errorLog);
//	}
//}

#pragma region Init
void init()
{
	KinectInit();
	GLInit();         //OpenGL
	InitPortAudio();  //audio
	MtInit();         //dispenser machine
}
#pragma endregion Init

#pragma region SpecialKeys
/*Rotate and translate in special key can move virtual objects by hand */
void SpecialKeys(int key, int x, int y)
{
	//需要的时候才会调用一次
	float fRotateScale = 0.005f;
	//Press F1 to store pt
	if (key == GLUT_KEY_F1 && !ROI_IS_COLLIDE && ARFunc_InsideTriCheck)
	{
		ROICameraSPStorage();
	}
	//Press F4 to enable or disable transparent mode 按F4转换透明度
	else if (key == GLUT_KEY_F4)
	{
		if (Cubic_IS_BLEND)
		{
			Cubic_IS_BLEND = FALSE;
			cout << endl << "Disable Transparent Mode..." << endl;
		}
		else
		{
			Cubic_IS_BLEND = TRUE;
			cout << endl << "Enable Transparent Mode..." << endl;
		}
	}//F2删除存储的点
	else if (key == GLUT_KEY_F2)
	{
		if (ROICameraSP_Storage != nullptr)
		{
			delete[] ROICameraSP_Storage;
		}
		if (ROICameraSP_Proj_Storage != nullptr)
		{
			delete[] ROICameraSP_Proj_Storage;
		}
		if (ROICameraSP_MachineCoord_Storage != nullptr)
		{
			delete[] ROICameraSP_MachineCoord_Storage;
		}
		if (ROICameraSP_MachineCoord_Proj_Storage != nullptr)
		{
			delete[] ROICameraSP_MachineCoord_Proj_Storage;
		}
		ROIStorageCount = 0;
	}

#pragma region rotation
	/*-------------Rotation------------*/
	//else if (key == GLUT_KEY_DOWN && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.x == 360)
	//	{
	//		CubicRota.x = 0;
	//	}
	//	else
	//	{
	//		CubicRota.x += 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.x : " << CubicRota.x << endl;
	//}
	//else if (key == GLUT_KEY_UP && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.x == -360)
	//	{
	//		CubicRota.x = 0;
	//	}
	//	else
	//	{
	//		CubicRota.x -= 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.x : " << CubicRota.x << endl;
	//}
	//else if (key == GLUT_KEY_RIGHT && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.y == 360)
	//	{
	//		CubicRota.y = 0;
	//	}
	//	else
	//	{
	//		CubicRota.y += 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.y : " << CubicRota.y << endl;
	//}
	//else if (key == GLUT_KEY_LEFT && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.y == -360)
	//	{
	//		CubicRota.y = 0;
	//	}
	//	else
	//	{
	//		CubicRota.y -= 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.y : " << CubicRota.y << endl;
	//}
	//else if (key == GLUT_KEY_F9 && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.z == 360)
	//	{
	//		CubicRota.z = 0;
	//	}
	//	else
	//	{
	//		CubicRota.z += 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.z : " << CubicRota.z << endl;
	//}
	//else if (key == GLUT_KEY_F10 && !IS_PathGenProcessing)
	//{
	//	if (CubicRota.z == -360)
	//	{
	//		CubicRota.z = 0;
	//	}
	//	else
	//	{
	//		CubicRota.z -= 1.0f;
	//	}
	//
	//	cout << endl << "CubicRota.z : " << CubicRota.z << endl;
	//}
#pragma endregion rotation

	/*-------------Translation------------*/
	else if (key == GLUT_KEY_DOWN && ARFunc_IS_ON)
	{
		ObjPosi.y += 0.001f;
		cout << endl << "ObjPosi.y : " << ObjPosi.y << endl;
	}
	else if (key == GLUT_KEY_UP && ARFunc_IS_ON)
	{
		ObjPosi.y -= 0.001f;
		cout << endl << "ObjPosi.y : " << ObjPosi.y << endl;
	}
	else if (key == GLUT_KEY_RIGHT && ARFunc_IS_ON)
	{
		ObjPosi.x += 0.001f;
		cout << endl << "ObjPosi.x : " << ObjPosi.x << endl;
	}
	else if (key == GLUT_KEY_LEFT && ARFunc_IS_ON)
	{
		ObjPosi.x -= 0.001f;
		cout << endl << "ObjPosi.x : " << ObjPosi.x << endl;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		g_Camera.RotateUp(-fRotateScale);
		cout << endl << "Camera RotateUp minus = " << -fRotateScale << endl;
	}
	else if (key == GLUT_KEY_UP)
	{
		g_Camera.RotateUp(fRotateScale);
		cout << endl << "Camera RotateUp plus = " << fRotateScale << endl;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		g_Camera.RotateSide(fRotateScale);
		cout << endl << "Camera RotateUSide plus = " << fRotateScale << endl;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		g_Camera.RotateSide(-fRotateScale);
		cout << endl << "Camera RotateUSide minus = " << -fRotateScale << endl;
	}
	else if (key == GLUT_KEY_F9)
	{
		ObjPosi.z += 0.001f;
		cout << endl << "ObjPosi.z : " << ObjPosi.z << endl;
	}
	else if (key == GLUT_KEY_F10)
	{
		ObjPosi.z -= 0.001f;
		cout << endl << "ObjPosi.z : " << ObjPosi.z << endl;
	}
	glutPostRedisplay();
}
#pragma endregion SpecialKeys

#pragma region RenderScene
void RenderSceneBackground()
{
	/*--------------RenderScene Background & Drawing Type-------------*/
	if (BG_IS_OPEN)
	{
		SceneWithBackground();
		switch (DRAWING_TYPE) {
		case 0:
			break;
		case 1:
			DrawMeshes();
			break;
		case 2:
			DrawPointCloud();
			break;
		case 3:
			Draw3DLine();
			break;
		case 4:
			Draw3DPlane();
			break;

		}
	}
	else
	{
		SceneWithoutBackground();
		switch (DRAWING_TYPE) {
		case 0:
			cout << endl << "Error in drawing type" << endl;
			break;
		case 1:
			DrawMeshes();
			break;
		case 2:
			DrawPointCloud();
			break;
		}
	}
}

void DrawProbeTip()
{
	if (ROIDepthCount != 0)
	{
		glPushMatrix();
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (int i = 0; i < ROIDepthCount; i++)
		{
			glColor3ub(0, 255, 0);
			/*draw probe tip's depth points*/
			glVertex3f(ROICameraSP[i].X, ROICameraSP[i].Y, ROICameraSP[i].Z);
		}
		//cout << "抓取到的像素点有 " << ROIDepthCount << "个~" << endl;
		//在这里计算了抓取到的笔尖点的位置到相机中心位置的距离，单位米。
		/*
		* 此处进行了颜色追踪的误差测定和范围测定，具体见颜色追踪误差测定和范围文档
		*/
		glEnd();
		glPopMatrix();
		
		float sum_X = 0, sum_Y = 0, sum_Z = 0;
		int realCount = 0;
		for (int i = 0; i < ROIDepthCount; i++)
		{
			if (ROICameraSP[i].X != 0 && ROICameraSP[i].Y != 0 && ROICameraSP[i].Z != 0)
			{
				sum_X += ROICameraSP[i].X * 1000;
				sum_Y += ROICameraSP[i].Y * 1000;
				sum_Z += ROICameraSP[i].Z * 1000;
				realCount++;
			}
		}

		sum_X = sum_X / realCount;
		sum_Y = sum_Y / realCount;
		sum_Z = sum_Z / realCount;

		tipModel.x = sum_X;
		tipModel.y = sum_Y;
		tipModel.z = sum_Z;

		tipModel.distance = sqrt(abs(sum_X)*abs(sum_X) + abs(sum_Y)*abs(sum_Y) + abs(sum_Z)*abs(sum_Z));
		
		
		//在这里是把x,y,z存到position.txt这个文件里面去
		//ofstream out("position.txt");
		//if (out.is_open())
		//{
		//	out << sum_X << endl;
		//	out << sum_Y << endl;
		//	out << sum_Z << endl;
		//	out.close();
		//}
	}
	//这里是测试Kinect自身数据的稳定程度，求X,Y,Z值的差距

	//ofstream pos;
	//pos.open("position.txt",ios::app);
	//pos << "X = " << tipModel.x << endl;
	//pos << "Y = " << tipModel.y << endl;
	//pos << "Z = " << tipModel.z << endl;
	//pos << "Distance = " << tipModel.distance << endl;
}
/*
 * #1 刚开始不成功的原因是在这里的时候抓取平面的方程式在kinectUpdate中，但是画的在renderscene当中，因此需要先把点用其他的存下来
 * #2 在这里画点成功，但是frame的FPS只有3，所以要想办法把画很多点的任务缩小成为画一个平面，应该可以减小运算量。2018-3-15
 * #3 在这里抓取到了平面或者线，因为针对平面和线有不同的后续处理方式，怎样识别这样的不同进行后续的处理？平面的话应该只需要放一次，用于放置虚拟模型，
 *    但是线的话抓取到之后需要直接用于指导点胶机的移动。这个地方需要思考
 */

void DrawPlaneInScene()
{
	if (1000 > PlaneDepthCount > 0 && PlaneSP != nullptr)
	{
		glPushMatrix();
		glLineWidth(5.0f);
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		    glVertex3f(PlaneSP[line_Res_X_head].X, PlaneSP[line_Res_X_head].Y, PlaneSP[line_Res_X_head].Z);
		    glVertex3f(PlaneSP[line_Res_X_tail].X, PlaneSP[line_Res_X_tail].Y, PlaneSP[line_Res_X_tail].Z);
		glEnd();
		glPopMatrix();
	}
	else if (PlaneDepthCount >= 1000 && PlaneSP != nullptr)
	{
		glPushMatrix();
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		glVertex3f(PlaneSP[plane_Res_X_head].X, PlaneSP[plane_Res_X_head].Y, PlaneSP[plane_Res_X_head].Z);
		glColor3f(255, 255, 0);//这个点成功
		glVertex3f(PlaneSP[plane_Res_X_tail].X, PlaneSP[plane_Res_X_tail].Y, PlaneSP[plane_Res_X_tail].Z);
		glColor3f(0, 255, 255);
		glVertex3f(PlaneSP[plane_Res_X_head].X, PlaneSP[plane_Res_Y_head].Y, PlaneSP[plane_Res_Y_head].Z);
		glColor3f(0, 0, 255);
		glVertex3f(PlaneSP[plane_Res_X_tail].X, PlaneSP[plane_Res_Y_tail].Y, PlaneSP[plane_Res_Y_tail].Z);
		glEnd();
		glPopMatrix();
	}
}

void RANSAC(CameraSpacePoint* spacePoints, int Iter)
{
	//http://blog.csdn.net/fengdingha/article/details/22285147
	//RANSAC code
}
/*
*
* 需要改进的部分：碰撞侦测
*
*
*/
void CollisionDetection()
{
	/*--------------------碰撞侦测-------------------*/
	/*--------------Collision Detection-------------*/
	if (ROIDepthCount != 0 && ARFunc_IS_ON)
	{
		GLfloat TransM[16] = { 0 };
		TransM[0] = TransM[5] = TransM[10] = TransM[15] = 1;
		glPushMatrix();
		glLoadIdentity();
		//移动到原点,荧幕中心就是原点
		glMultMatrixf(M_Cubic_inv);
		//把obj模型的坐标移动到opengl坐标原点
		glTranslatef(-IntersectPoint.X, -IntersectPoint.Y, -IntersectPoint.Z);
		glTranslatef(-ObjPosi.x, -ObjPosi.y, -ObjPosi.z);
		//http://blog.csdn.net/lyx2007825/article/details/8792475
		//http://blog.csdn.net/hippig/article/details/7894034
		glGetFloatv(GL_MODELVIEW_MATRIX, TransM);
		glPopMatrix();

		if (ROICameraSP_TouchDetec != nullptr)
		{
			delete[] ROICameraSP_TouchDetec;
		}
		ROICameraSP_TouchDetec = new CameraSpacePoint[ROIDepthCount];
		//把抓到的点从Kinect的坐标系上变换到obj模型的坐标系上
		ROITrans(ROICameraSP, ROIDepthCount, TransM, ROICameraSP_TouchDetec);

		int CollideCount = 0;
		int CollideCount_Circle = 0;

		for (int ROICount = 0; ROICount < ROIDepthCount; ROICount++)
		{
			GLMgroup* group = OBJ->groups;
			int gCount = 0;
			//遍历所有的group里面的三角形
			while (group)
			{
				for (int TriCount = 0; TriCount < group->numtriangles; TriCount++)
				{
					//vertices[gCount][Tricount*9+0]指的是第gCount组的三角形的第tricount个顶点的X值,这个x值为纯粹的模型坐标系的值
					float vx = ROICameraSP_TouchDetec[ROICount].X - vertices[gCount][TriCount * 9 + 0];
					float vy = ROICameraSP_TouchDetec[ROICount].Y - vertices[gCount][TriCount * 9 + 1];
					float vz = ROICameraSP_TouchDetec[ROICount].Z - vertices[gCount][TriCount * 9 + 2];
					//这里为这个点的法向量的x,y,z的值
					float nx = normals[gCount][TriCount * 9 + 0];
					float ny = normals[gCount][TriCount * 9 + 1];
					float nz = normals[gCount][TriCount * 9 + 2];

					float Normalized_V = pow(pow(vx, 2) + pow(vy, 2) + pow(vz, 2), -0.5);
					float Normalized_N = pow(pow(nx, 2) + pow(ny, 2) + pow(nz, 2), -0.5);

					float dot = vx*nx*Normalized_V*Normalized_N + vy*ny*Normalized_V*Normalized_N + vz*nz*Normalized_V*Normalized_N;
					float Psi = acos(dot);

					if (Psi*180.0f / CV_PI > 90.0f)
					{
						switch (gCount)
						{
							//这里的 case3 的意思是 gCount 等于 3,也就是说原来代码中的第三个group是圆圈。
						case 3:
							/*the circle of the obj*/
							CollideCount_Circle++;
							break;
							//上面的所有case都不为真
						default:
							CollideCount++;//等于46
							break;
						}
					}
				}
				//gCount = 8
				gCount++;
				group = group->next;
			}

			if (CollideCount_Circle == 0)
			{
				ROI_IS_COLLIDE = FALSE;
			}
			//在这里被写死了，当collidecount达到这个数值的时候就说明确实碰撞了
			else if (CollideCount == OBJ->numtriangles - 76)
			{
				ROI_IS_COLLIDE = TRUE;
				break;
			}
			CollideCount = 0;
			CollideCount_Circle = 0;
		}
	}
}

void ARFuncPackage()
{
	/*--------------AR Function-------------*/
	/*find depth points and do not collide, call ARFunc_FindProj()，
	*如果没有发生碰撞，那么就执行AR程式*/
	if (ROIDepthCount > 0 && !ROI_IS_COLLIDE)
	{
		/*use this to get the projection point这个是用来找到投影点*/
		ARFunc_FindProj();
		//确定在区域内并且AR功能开启的时候往下执行
		if (IS_ARFunc_InsideTriCheck && ARFunc_IS_ON)
		{
			glPushMatrix();

			glPointSize(5.0f);
			glColor3ub(255, 0, 0);

			glBegin(GL_POINTS);
			glVertex3f(ARFunc_ROICSP_Proj->X, ARFunc_ROICSP_Proj->Y, ARFunc_ROICSP_Proj->Z);
			glEnd();

			glPopMatrix();
			/*calculate the distance between projection point and center point*/
			//unit in m
			ARFunc_ROICSP_Proj_Dist = pow(pow(ARFunc_ROICSP_Proj->X - ROICenterCameraS.x, 2) + pow(ARFunc_ROICSP_Proj->Y - ROICenterCameraS.y, 2) + pow(ARFunc_ROICSP_Proj->Z - ROICenterCameraS.z, 2), 0.5);
			float dist = ARFunc_ROICSP_Proj_Dist * 1000;
			const int low_range = 30, high_range = 100;
			int low_r = 1, high_r = 0, low_g = 0, high_g = 1;
			float Red = 0, Green = 0, Blue = 0;
			/*change the color of the line by the distance range*/
			if (dist > high_range)
			{
				Red = 0;
				Green = 1;
				Blue = 0;
			}
			else if (dist < low_range)
			{
				Red = 1;
				Green = 0;
				Blue = 0;
			}
			else
			{
				Red = (dist - low_range) / (high_range)* (high_r - low_r) + low_r;
				Green = (dist - (low_range + high_range)) / (-low_range) * (high_g - low_g) + low_g;
				Blue = -1.0f / pow((high_range - low_range) / 2, 2) * pow((dist - (low_range + high_range) / 2), 2) + 1.0f;
			}
			//
			/* draw line */
			glEnable(GL_LINE_STIPPLE);
			glPushMatrix();
			glColor3f(Red, Green, Blue);
			glLineWidth(2.5);
			glLineStipple(1, 0x1C47);
			glBegin(GL_LINES);
			glVertex3f(ARFunc_ROICSP_Proj->X, ARFunc_ROICSP_Proj->Y, ARFunc_ROICSP_Proj->Z);
			glVertex3f(ROICenterCameraS.x, ROICenterCameraS.y, ROICenterCameraS.z);
			glEnd();
			glPopMatrix();
			glDisable(GL_LINE_STIPPLE);
			if (IS_PORTAUDIO_INIT)
			{
				IS_PORTAUDIO_START = sine.start();
			}
		}
		else
		{
			if (IS_PORTAUDIO_START == 0)
			{
				sine.stop();
			}
		}
	}
	else
	{
		if (IS_PORTAUDIO_START == 0)
		{
			sine.stop();
		}
	}
}
//从opengl的坐标系转换到机器的坐标系
void CoordGLtoMachine()
{
	/*--------------Transfer Prob Tip from GL Coord. to Machine Coord.-------------*/
	if (ROIDepthCount > 0 && ROICameraSP != nullptr && !ROI_IS_COLLIDE)//在这里是颜色追踪抓取的点
	{
		GLfloat TransM_toMechCoord[16] = { 0 };
		TransM_toMechCoord[0] = TransM_toMechCoord[5] = TransM_toMechCoord[10] = TransM_toMechCoord[15] = 1;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();	
		//glTranslatef(0, 0, 0);
		//http://cuiqingcai.com/1658.html 旋转示例	
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
		glTranslatef(-IntersectPoint.X, -IntersectPoint.Y, -IntersectPoint.Z);
		glTranslatef(TipPosi.x, TipPosi.y, TipPosi.z);
		//glRotatef(90, 0, 0, 1);//旋转后Z值和X值与机器的坐标系相反		
		//glMultMatrixf(const GLfloat *m);//把m指定的16个值作为一个矩阵，与当前矩阵相乘，并把结果存储在当前矩阵中 
		//glMultMatrix 假设当前矩阵是C那么用矩阵M 调用glMultMatrix 对顶点v的变换就从原来的C*v变成C * M * v
		//http://blog.csdn.net/mathgeophysics/article/details/11434345
		glMultMatrixf(M_Cubic_inv);//opengl坐标转换到机器	
			
		glGetFloatv(GL_MODELVIEW_MATRIX, TransM_toMechCoord);
		glPopMatrix();
		if (ROICameraSP_MechCoord != nullptr)
		{
			delete[] ROICameraSP_MechCoord;
		}
		if (ROICameraSP_Proj_MechCoord != nullptr)
		{
			delete[] ROICameraSP_Proj_MechCoord;
		}
		ROICameraSP_MechCoord = new CameraSpacePoint[1];
		ROICameraSP_Proj_MechCoord = new CameraSpacePoint[1];
		/*normal GL coordinate在这里还是opengl的准确坐标系*/
		ROICameraSP_MechCoord->X = ROICenterCameraS.x;

		ROICameraSP_MechCoord->Y = ROICenterCameraS.y; 
		ROICameraSP_MechCoord->Z = ROICenterCameraS.z;
		/*After projection coordinate*/
		ROICameraSP_Proj_MechCoord->X = ARFunc_ROICSP_Proj->X;
		ROICameraSP_Proj_MechCoord->Y = ARFunc_ROICSP_Proj->Y;
		ROICameraSP_Proj_MechCoord->Z = ARFunc_ROICSP_Proj->Z;

		ROITrans(ROICameraSP_MechCoord, 1, TransM_toMechCoord, ROICameraSP_MechCoord);

		ROITrans(ROICameraSP_Proj_MechCoord, 1, TransM_toMechCoord, ROICameraSP_Proj_MechCoord);

		ROICameraSP_MechCoord->Y = -ROICameraSP_MechCoord->Y;
		
		ROICameraSP_Proj_MechCoord->Y = -ROICameraSP_Proj_MechCoord->Y;
		
	}
	else if (1000 > PlaneDepthCount > 0 && PlaneSP != nullptr && !ROI_IS_COLLIDE)//在这里是抓取到的线的点的排序和抓取
	{
		GLfloat TransGLtoMach[16] = { 0 };
		TransGLtoMach[0] = TransGLtoMach[5] = TransGLtoMach[10] = TransGLtoMach[15] = 1;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(TipPosi.x, TipPosi.y, TipPosi.z);
		glRotatef(-90, 1, 0, 0);
		glMultMatrixf(M_Cubic_inv);
		glTranslatef(-IntersectPoint.X, -IntersectPoint.Y, -IntersectPoint.Z);
		glGetFloatv(GL_MODELVIEW_MATRIX, TransGLtoMach);
		glPopMatrix();
		if (LineSP_MachCoord != nullptr)
		{
			delete[] LineSP_MachCoord;
		}

		LineSP_MachCoord = new CameraSpacePoint[PlaneDepthCount];
		
		for (int i = 0; i < PlaneDepthCount; i++)
		{
			LineSP_MachCoord[i].X = PlaneSP[i].X;
			LineSP_MachCoord[i].Y = PlaneSP[i].Y;
			LineSP_MachCoord[i].Z = PlaneSP[i].Z;

			GLfloat m[16] = { 0 };
			m[3] = m[7] = m[11] = m[15] = 1;
			m[12] = LineSP_MachCoord[i].X;
			m[13] = LineSP_MachCoord[i].Y;
			m[14] = LineSP_MachCoord[i].Z;

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(TransGLtoMach);
			glMultMatrixf(m);
			glGetFloatv(GL_MODELVIEW_MATRIX, m);
			glPopMatrix();

			LineSP_MachCoord[i].X = m[12] / m[15];
			LineSP_MachCoord[i].Y = m[13] / m[15];
			LineSP_MachCoord[i].Z = m[14] / m[15];
		}
		for (int i = 0; i < PlaneDepthCount; i++)
		{
			if (LineSP_MachCoord[i].X > 0.1)
			{
				LineSP_MachCoord[i].Y = -LineSP_MachCoord[i].Y;
			}
		}		
		result* Line_Res_X = Get_X_Min_Max(LineSP_MachCoord, 0, PlaneDepthCount - 1);
		line_Res_X_head = Line_Res_X->head;
		line_Res_X_tail = Line_Res_X->tail;
	}
	else if (PlaneDepthCount >= 1000 && PlaneSP != nullptr && !ROI_IS_COLLIDE)//在这里是抓取到的面的点的排序和抓取
	{
		GLfloat TransGLtoMach[16] = { 0 };
		TransGLtoMach[0] = TransGLtoMach[5] = TransGLtoMach[10] = TransGLtoMach[15] = 1;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(TipPosi.x, TipPosi.y, TipPosi.z);
		glRotatef(-90, 1, 0, 0);
		glMultMatrixf(M_Cubic_inv);
		glTranslatef(-IntersectPoint.X, -IntersectPoint.Y, -IntersectPoint.Z);
		glGetFloatv(GL_MODELVIEW_MATRIX, TransGLtoMach);
		glPopMatrix();
		if (PlaneSP_MachCoord != nullptr)
		{
			delete[] PlaneSP_MachCoord;
		}
		PlaneSP_MachCoord = new CameraSpacePoint[PlaneDepthCount];

		for (int i = 0; i < PlaneDepthCount; i++)
		{
			PlaneSP_MachCoord[i].X = PlaneSP[i].X;
			PlaneSP_MachCoord[i].Y = PlaneSP[i].Y;
			PlaneSP_MachCoord[i].Z = PlaneSP[i].Z;

			GLfloat m[16] = { 0 };
			m[3] = m[7] = m[11] = m[15] = 1;
			m[12] = PlaneSP_MachCoord[i].X;
			m[13] = PlaneSP_MachCoord[i].Y;
			m[14] = PlaneSP_MachCoord[i].Z;

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(TransGLtoMach);
			glMultMatrixf(m);
			glGetFloatv(GL_MODELVIEW_MATRIX, m);
			glPopMatrix();

			PlaneSP_MachCoord[i].X = m[12] / m[15];
			PlaneSP_MachCoord[i].Y = m[13] / m[15];
			PlaneSP_MachCoord[i].Z = m[14] / m[15];
		}
		for (int i = 0; i < PlaneDepthCount; i++)
		{
			if (PlaneSP_MachCoord[i].X > 0.1)
			{
				PlaneSP_MachCoord[i].Y = -PlaneSP_MachCoord[i].Y;
			}
		}
		result* Plane_Res_X = Get_X_Min_Max(PlaneSP_MachCoord, 0, PlaneDepthCount - 1);
		result* Plane_Res_Y = Get_Y_Min_Max(PlaneSP_MachCoord, 0, PlaneDepthCount - 1);
		plane_Res_X_head = Plane_Res_X->head;
		plane_Res_X_tail = Plane_Res_X->tail;
		plane_Res_Y_head = Plane_Res_Y->head;
		plane_Res_Y_tail = Plane_Res_Y->tail;
	}
}
//获取线段中的X最大值和最小值
/*
 * 一开始的bug在与进行比较的max和min使用的是int值，而实际上需要使用double值，修改后就可以成功打印出其中的最大最小值
 */
result* Get_X_Min_Max(CameraSpacePoint* point, int start, int end)
{
	int len = end - start + 1;

	if (end < start) {
		return NULL;
	}

	result* res = new result();

	double max, min;
	int headL, tailL;
	if (len % 2 == 0) {
		//偶数的 情况  
		res->min =
			((point[start].X) <= (point[start + 1].X)) ?
			(point[start].X) : (point[start + 1].X);
		res->max = 
			((point[start].X) >= (point[start + 1].X)) ?
			(point[start].X) : (point[start + 1].X);
		res->head =
			((point[start].X) <= (point[start + 1].X)) ?
			(start) : (start + 1);
		res->tail =
			((point[start].X) >= (point[start + 1].X)) ?
			(start) : (start + 1);
		start = start + 2; //如果是偶数，则需要让i从第三个元素开始  
	}
	else {
		//奇数的情况  
		res->min = point[start].X;
		res->max = point[start].X;		
		res->head = start;
		res->tail = start;
		start = start + 1; //如果是奇数，则需要让i从第二个元素开始  
	}

	while (start <= end) {
		min = ((point[start].X <= point[start + 1].X) ? (point[start].X) : (point[start + 1].X));
		headL = (point[start].X <= point[start + 1].X) ? (start) : (start + 1);
		max = ((point[start].X >= point[start + 1].X) ? (point[start].X) : (point[start + 1].X));
		tailL = (point[start].X >= point[start + 1].X) ? (start) : (start + 1);
		res->max = (res->max >= max) ? (res->max) : (max);
		res->min = (res->min <= min) ? (res->min) : (min);
		res->head = (res->min <= min) ? (res->head) : (headL);
		res->tail = (res->max <= max) ? (res->tail) : (tailL);

		start = start + 2;
	}

	return res;
}

result* Get_Y_Min_Max(CameraSpacePoint* point, int start, int end)
{
	int len = end - start + 1;

	if (end < start) {
		return NULL;
	}

	result* res = new result();

	double max, min;
	int headL, tailL;

	if (len % 2 == 0) {
		//偶数的情况  
		res->min =
			((point[start].Y) <= (point[start + 1].Y)) ?
			(point[start].Y) : (point[start + 1].Y);
		res->max =
			((point[start].Y) >= (point[start + 1].Y)) ?
			(point[start].Y) : (point[start + 1].Y);
		res->head =
			((point[start].Y) <= (point[start + 1].Y)) ?
			(start) : (start + 1);
		res->tail =
			((point[start].Y) >= (point[start + 1].Y)) ?
			(start) : (start + 1);
		start = start + 2; //如果是偶数，则需要让i从第三个元素开始  
	}
	else {
		//奇数的情况  
		res->min = point[start].Y;
		res->max = point[start].Y;
		res->head = start;
		res->tail = start;
		start = start + 1; //如果是奇数，则需要让i从第二个元素开始  
	}

	while (start <= end) {
		
		min = ((point[start].Y <= point[start + 1].Y) ? (point[start].Y) : (point[start + 1].Y));
		headL = (point[start].Y <= point[start + 1].Y) ? (start) : (start + 1);
		max = ((point[start].Y >= point[start + 1].Y) ? (point[start].Y) : (point[start + 1].Y));
		tailL = (point[start].Y >= point[start + 1].Y) ? (start) : (start + 1);

		res->max = (res->max >= max) ? (res->max) : (max);
		res->min = (res->min <= min) ? (res->min) : (min);

		res->head = (res->min <= min) ? (res->head) : (headL);
		res->tail = (res->max <= max) ? (res->tail) : (tailL);

		start = start + 2;
	}

	return res;
}
//用来打印出OpenGL 内部的Matrix便于查看
void PrintMatrix(GLfloat* m)
{
	cout << "---------------------------------------------------------" << endl;
	cout << "------------------打印出来的矩阵如下---------------------" << endl;
	cout << "---------------------------------------------------------" << endl;
	printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", m[0], m[4], m[8], m[12]);
	printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", m[1], m[5], m[9], m[13]);
	printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", m[2], m[6], m[10], m[14]);
	printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[15]:% 7.5f\n", m[3], m[7], m[11], m[15]); 
	cout << "---------------------------------------------------------" << endl;
	cout << "---------------------------------------------------------" << endl;
	cout << "---------------------------------------------------------" << endl;
}

void DrawStoragePoint()
{
	/* F1 button calls this */
	/*--------------Draw Storage Point-------------*/
	if (ROIStorageCount > 0)
	{
		glPushMatrix();
		glPointSize(5.0f);
		if (CUBIC_MOVE)
		{
			glTranslatef(DeviaDueToY->X, DeviaDueToY->Y, DeviaDueToY->Z);
		}
		glColor3ub(255, 0, 137);
		glBegin(GL_POINTS);
		GLfloat pX = 0, pY = 0, pZ = 0;
		switch (STORAGE_TYPE)
		{
		case 0:
			for (int i = 0; i < ROIStorageCount; i++)
			{
				pX = ROICameraSP_Storage[i].X;
				pY = ROICameraSP_Storage[i].Y;
				pZ = ROICameraSP_Storage[i].Z;
				glVertex3f(pX, pY, pZ);
			}
			break;
		case 1:
			for (int i = 0; i < ROIStorageCount; i++)
			{
				pX = ROICameraSP_Proj_Storage[i].X;
				pY = ROICameraSP_Proj_Storage[i].Y;
				pZ = ROICameraSP_Proj_Storage[i].Z;
				glVertex3f(pX, pY, pZ);
			}
			break;
		}
		glEnd();
		glPopMatrix();
	}
}

void MoveCubicInY_Axis()//换了模型之后有影响
{
	/*--------------cubic movement deviation--------------*/
	/*--------------Calculate deviation & Draw Cubic 方块也要跟着移动的距离-------------*/
	CameraSpacePoint* Y_Devia = new CameraSpacePoint[2];
	Y_Devia[0].X = Y_Devia[0].Y = Y_Devia[0].Z = 0;
	Y_Devia[1].X = Y_Devia[1].Y = Y_Devia[1].Z = 0;
	//存储的点大于0个并且检测到cubic需要移动：在按A的时候CUBIC_MOVE = TURE;
	if (ROIStorageCount > 0 && CUBIC_MOVE)
	{
		float dy = 0;
		switch (STORAGE_TYPE)
		{
			//根据储存方式的不同：投影点或者不是投影点来进行决策。给予不同的路径
		case 0:
			dy = ROICameraSP_MachineCoord_Storage[mtmove_step].Y;
			break;
		case 1:
			dy = ROICameraSP_MachineCoord_Proj_Storage[mtmove_step].Y;
			break;
		}
		GLfloat TransM_OBJ[16] = { 0 };
		TransM_OBJ[0] = TransM_OBJ[5] = TransM_OBJ[10] = TransM_OBJ[15] = 1;

		glPushMatrix();
		glLoadIdentity();
		glTranslatef(IntersectPoint.X, IntersectPoint.Y, IntersectPoint.Z);
		glMultMatrixf(M_Cubic);
		glRotatef(90, 1, 0, 0);
		glTranslatef(TipPosi.x, TipPosi.y, TipPosi.z);
		glGetFloatv(GL_MODELVIEW_MATRIX, TransM_OBJ);
		glPopMatrix();

		Y_Devia[1].Y = dy;

		ROITrans(Y_Devia, 2, TransM_OBJ, Y_Devia);

		DeviaDueToY->X = Y_Devia[1].X - Y_Devia[0].X;
		DeviaDueToY->Y = Y_Devia[1].Y - Y_Devia[0].Y + (Y_Devia[1].Z - Y_Devia[0].Z)*tan(-dev_theta);
		DeviaDueToY->Z = Y_Devia[1].Z - Y_Devia[0].Z;
	}

	glPushMatrix();
	if (Cubic_IS_BLEND)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	//直接让整个cubic跟着移动
	glTranslatef(DeviaDueToY->X, DeviaDueToY->Y, DeviaDueToY->Z);

	//DrawCubic();
	ARFunc_IS_ON = FALSE;
	if (Cubic_IS_BLEND)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
	glPopMatrix();
	//delete[] Y_Devia;
}

void ChangeLineColor()
{
	/*-----------------Change Color of projection line-------------*/
	if (ARFunc_IS_ON)
	{

		float dist = ARFunc_ROICSP_Proj_Dist * 1000;
		const int low_range = 30, high_range = 100;
		int low_r = 1, high_r = 0, low_g = 0, high_g = 1;
		float Red = 0, Green = 0, Blue = 0;
		if (dist > high_range)
		{
			Red = 0;
			Green = 1;
			Blue = 0;
		}
		else if (dist < low_range)
		{
			Red = 1;
			Green = 0;
			Blue = 0;
		}
		else
		{
			Red = (dist - low_range) / ((low_range + high_range) - low_range) * (high_r - low_r) + low_r;
			Green = (dist - (low_range + high_range)) / (high_range - (low_range + high_range)) * (high_g - low_g) + low_g;
			Blue = -1.0f / pow((high_range - low_range) / 2, 2) * pow((dist - (low_range + high_range) / 2), 2) + 1.0f;
		}
		/*--------------Show distance on Screen-------------*/
		glPushMatrix();
		glColor3f(Red, Green, Blue);
		if (IS_ARFunc_InsideTriCheck && !ROI_IS_COLLIDE)
		{
			glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x + 10, ROICenterColorS_Old.y + 15);
			glPrintf("%.2f mm ", ARFunc_ROICSP_Proj_Dist * 1000);
		}
		glPopMatrix();
	}
}
void Coordinate()
{
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);//红色的X轴
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);

	glColor3ub(0, 255, 0);//绿色的Y轴
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);

	glColor3ub(0, 0, 255);//蓝色的Z轴
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glEnd();
}
void RenderScene()
{

	RenderSceneBackground();
	DrawProbeTip();	
	CollisionDetection(); //碰撞侦测需要改进
	ARFuncPackage();
	CoordGLtoMachine();
	DrawPlaneInScene();
	DrawStoragePoint();
	MoveCubicInY_Axis();
	
	/*--------------Show Prob Tip Coordinate on Screen-------------*/
	glPushMatrix();
	glColor3ub(0, 255, 0);
	/*glPrintf is used to draw the probe tip's coordinate position*/
	if (ROICameraSP_MechCoord != nullptr)
	{
		glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x - 60, ROICenterColorS_Old.y - 35);
		glPrintf("(%.3f, %.3f, %.3f)", ROICameraSP_MechCoord[0].X , ROICameraSP_MechCoord[0].Y , ROICameraSP_MechCoord[0].Z );
	}
	glPopMatrix();

	ChangeLineColor();
	ROI_IS_COLLIDE = FALSE;
	//Coordinate();
	/*--------------Call Window Function-------------*/
	/*show image can have a color frame to you*/
	
}
#pragma endregion RenderScene

#endif