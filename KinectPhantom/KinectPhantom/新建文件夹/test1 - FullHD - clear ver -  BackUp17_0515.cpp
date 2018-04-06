#pragma region Region_Library
/*--------------Standard Library-------------*/
#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>

/*--------------OpenCV-------------*/
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cv.h>

/*--------------Kinect-------------*/
#include <Kinect.h>
#include <windows.h>   

/*--------------OpenGL-------------*/
#include <gl/Gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

#pragma comment(lib,"opengl32.lib")  
#pragma comment(lib,"GLU32.LIB")  
#pragma comment(lib,"GLUT32.LIB")


/*--------------ARTool Kit-------------*/
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

/*--------------OpenHaptics-------------*/
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>

/*--------------LoadObj-------------*/
#include "GLM.h"

/*--------------Beep Sound-------------*/
//#pragma comment(lib,"winmm.lib") 
//#include <mmsystem.h> 
#include "portaudio.h"

using namespace std;
using namespace cv;

#include "Declae.h"

/*--------------M232-------------*/
//#include "dllmain.h"

#using <System.dll>
using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;

#pragma endregion Region_Library


/*------------------------------------------*/
/*--------------OpenGL Function-------------*/
/*------------------------------------------*/
void DrawCubic()
{
	glPushMatrix();

	glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
	glMultMatrixf(M_Cubic);
	glRotatef(theta2, 0, 1, 0);


	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

	if (!ROI_IS_COLLIDE)
	{
		GLMgroup* group = OBJ->groups;
		int gCount = 0;
		while (group)
		{

			glColor3f(1, 1, 1);

			glVertexPointer(3, GL_FLOAT, 0, vertices[gCount]);
			glTexCoordPointer(2, GL_FLOAT, 0, vtextures[gCount]);


			glBindTexture(GL_TEXTURE_2D, textures[5 - gCount]);
			glDrawArrays(GL_TRIANGLES, 0, group->numtriangles * 3);

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
			/*想辦法連接端點就好，現階段之作法為畫一次fill 一次wireframe 很浪費資源*/
			/*---------------------------------------------------------------------*/

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



	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();


}

void RenderScene()
{
	/*--------------RenderScene Background & Drawing Type-------------*/
	if (BG_IS_OPEN)
	{
		SceneWithBackground();

		switch (DRAWING_TYPE){
		case 0:
			break;
		case 1:
			//cout << endl << "Error in drawing type" << endl;
			DrawMeshes();
			break;
		case 2:
			DrawPointCloud();
			break;
		}
	}
	else
	{
		SceneWithoutBackground();

		switch (DRAWING_TYPE){
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


	/*--------------ROI Mapping-------------*/
	if (ROIDepthCount != 0)
	{
		glPushMatrix();
		glPointSize(2.0f);
		glBegin(GL_POINTS);
			for (int i = 0; i < ROIDepthCount; i++)
			{
				glColor3ub(0, 255, 0);
				glVertex3f(ROICameraSP[i].X, ROICameraSP[i].Y, ROICameraSP[i].Z);

			}

		glEnd();

		glPopMatrix();

	}


	/*--------------LR Mapping-------------*/

	glPushMatrix();
	for (int target = 0; target < 2; target++)
	{
		if (LRDepthCount[target] != 0)
		{
			
			glPointSize(2.0f);
			glBegin(GL_POINTS);

			for (int i = 0; i < LRDepthCount[target]; i++)
			{
				glColor3ub(255, 0, 0);
				glVertex3f(LRCameraSP[target][i].X, LRCameraSP[target][i].Y, LRCameraSP[target][i].Z);
			}

			glEnd();

		}

	}
	glPopMatrix();


	/*--------------POR-------------*/
	if (PORDepthCount != 0)
	{

		glPushMatrix();
		glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
		glColor3ub(35, 128, 53);
		glutSolidSphere(0.001, 100, 100);
		glPopMatrix();


		if (ROIDepthCount != 0)
		{

			GLfloat TransM[16] = { 0 };
			TransM[0] = TransM[5] = TransM[10] = TransM[15] = 1;
			
			glPushMatrix();

			glLoadIdentity();
			glRotatef(-theta2, 0, 1, 0);
			glMultMatrixf(M_Cubic_inv);
			glTranslatef(-Intersect.X, -Intersect.Y, -Intersect.Z);
			//glMultMatrixf(TransM);
			glGetFloatv(GL_MODELVIEW_MATRIX, TransM);

			glPopMatrix();

			//CameraSpacePoint* ROICameraSP_TouchDetec = new CameraSpacePoint[ROIDepthCount];
			if (ROICameraSP_TouchDetec != nullptr)
			{
				delete[] ROICameraSP_TouchDetec;
			}
			
			ROICameraSP_TouchDetec = new CameraSpacePoint[ROIDepthCount];

			


			ROITrans(ROICameraSP, ROIDepthCount, TransM, ROICameraSP_TouchDetec);

			int CollideCount = 0;

			for (int ROICount = 0; ROICount < ROIDepthCount; ROICount++)
			{
				for (int TriCount = 0; TriCount < OBJ->numtriangles; TriCount++)
				{
					float vx = ROICameraSP_TouchDetec[ROICount].X - OBJ->vertices[OBJ->triangles[TriCount].vindices[0] * 3 + 0];
					float vy = ROICameraSP_TouchDetec[ROICount].Y - OBJ->vertices[OBJ->triangles[TriCount].vindices[0] * 3 + 1];
					float vz = ROICameraSP_TouchDetec[ROICount].Z - OBJ->vertices[OBJ->triangles[TriCount].vindices[0] * 3 + 2];

					float nx = OBJ->normals[OBJ->triangles[TriCount].nindices[0] * 3 + 0];
					float ny = OBJ->normals[OBJ->triangles[TriCount].nindices[0] * 3 + 1];
					float nz = OBJ->normals[OBJ->triangles[TriCount].nindices[0] * 3 + 2];

					float Normalized_V = pow(pow(vx, 2) + pow(vy, 2) + pow(vz, 2), -0.5);
					float Normalized_N = pow(pow(nx, 2) + pow(ny, 2) + pow(nz, 2), -0.5);

					float dot = vx*nx*Normalized_V*Normalized_N + vy*ny*Normalized_V*Normalized_N + vz*nz*Normalized_V*Normalized_N;
					float Psi = acos(dot);

					//cout << Psi*180.0f / 3.14159265358979323846 << endl;

					if (Psi*180.0f / 3.14159265358979323846 > 90.0f)
					{
						CollideCount++;
						//cout << TriCount << " " << CollideCount << endl;
					}

				}

				if (CollideCount == OBJ->numtriangles)
				{
					//cout <<  "Collide" << endl;
					ROI_IS_COLLIDE = TRUE;
					break;
				}
				
				CollideCount = 0;
				
			}

			/*****************************/

			//CameraSpacePoint* ROICameraSP_MechCoord = nullptr;

			  
		}

	}

	if (ARFunc_ROICSP_Proj != nullptr)
	{
		GLfloat TransM_toMechCoord[16] = { 0 };
		TransM_toMechCoord[0] = TransM_toMechCoord[5] = TransM_toMechCoord[10] = TransM_toMechCoord[15] = 1;

		glPushMatrix();

		glLoadIdentity();

		glTranslatef(0.14, 0, 0.2);
		glRotatef(-90, 1, 0, 0);

		glRotatef(-theta2, 0, 1, 0);
		glMultMatrixf(M_Cubic_inv);
		glTranslatef(-Intersect.X, -Intersect.Y, -Intersect.Z);

		//glMultMatrixf(TransM_toMechCoord);
		glGetFloatv(GL_MODELVIEW_MATRIX, TransM_toMechCoord);

		glPopMatrix();

		//CameraSpacePoint* ROICameraSP_TouchDetec = new CameraSpacePoint[ROIDepthCount];
		if (ROICameraSP_MechCoord != nullptr)
		{
			delete[] ROICameraSP_MechCoord;
		}

		ROICameraSP_MechCoord = new CameraSpacePoint[1];

		//ROICameraSP_MechCoord->X = ARFunc_ROICSP_Proj->X;
		//ROICameraSP_MechCoord->Y = ARFunc_ROICSP_Proj->Y;
		//ROICameraSP_MechCoord->Z = ARFunc_ROICSP_Proj->Z;

		ROICameraSP_MechCoord->X = ROICenterCameraS.x;
		ROICameraSP_MechCoord->Y = ROICenterCameraS.y;
		ROICameraSP_MechCoord->Z = ROICenterCameraS.z;


		ROITrans(ROICameraSP_MechCoord, 1, TransM_toMechCoord, ROICameraSP_MechCoord);
	}


	if (ROIStorageCount > 0)
	{
		//cout << ROIStorageCount << endl;

		glPushMatrix();
		glPointSize(5.0f);
		glBegin(GL_POINTS);

		//if (IS_PathGenProjToPlane || IS_PathGenRegression)
		//{
		//	glColor3ub(255, 255, 55);
		//}
		//else
		//{
		//	glColor3ub(255, 0, 137);
		//
		//}

		glColor3ub(255, 0, 137);
		for (int i = 0; i < ROIStorageCount; i++)
		{
			//if (i == 0 && !IS_REC_START)
			//{
			//	Start.x = ROICenterColorS_Old.x;
			//	Start.y = ROICenterColorS_Old.y;
			//	//Start.x = ROICameraSP_MechCoord[0].X;
			//	//Start.y = ROICameraSP_MechCoord[0].Y;

			//	IS_REC_START = TRUE;
			//}
			//else if (i == ROIStorageCount - 1 && !IS_KEY_F1_UP && !IS_REC_END)
			//{
			//	End.x = ROICenterColorS_Old.x;
			//	End.y = ROICenterColorS_Old.y;
			//	//End.x = ROICameraSP_MechCoord[0].X;
			//	//End.y = ROICameraSP_MechCoord[0].Y;

			//	IS_REC_END = TRUE;
			//}

			GLfloat pX = ROICameraSP_Storage[i].X;
			GLfloat pY = ROICameraSP_Storage[i].Y;
			GLfloat pZ = ROICameraSP_Storage[i].Z;
			glVertex3f(pX, pY, pZ);

		}

		glEnd();
		glPopMatrix();


	}



	/*--------------AR Function-------------*/
	if (IS_PlaceObject && ROIDepthCount > 0 && !ROI_IS_COLLIDE)
	{
		ARFunc_FindProj();

		if (IS_ARFunc_InsideTriCheck)
		{

			glPushMatrix();
			glPointSize(5.0f);

			glColor3ub(255, 0, 0);

			glBegin(GL_POINTS);

			glVertex3f(ARFunc_ROICSP_Proj->X, ARFunc_ROICSP_Proj->Y, ARFunc_ROICSP_Proj->Z);
		
			glEnd();
			glPopMatrix();

			//unit in km
			ARFunc_ROICSP_Proj_Dist = pow(pow(ARFunc_ROICSP_Proj->X - ROICenterCameraS.x, 2) + pow(ARFunc_ROICSP_Proj->Y - ROICenterCameraS.y, 2) + pow(ARFunc_ROICSP_Proj->Z - ROICenterCameraS.z, 2), 0.5);

			float dist = ARFunc_ROICSP_Proj_Dist * 1000;
			const int low_range = 30, high_range = 100;
			int low_r = 1, high_r = 0, low_g = 0, high_g = 1;

			float rr = 0, gg = 0, bb = 0;

			if (dist > high_range)
			{
				rr = 0;
				gg = 1;
				bb = 0;
			}
			//else if (dist < (low_range + high_range))
			//{
			//	rr = 0;
			//	gg = 0;
			//	bb = 255;
			//}
			else if (dist < low_range)
			{
				rr = 1;
				gg = 0;
				bb = 0;
			}
			else
			{
				rr = (dist - low_range) / ((low_range + high_range) - low_range) * (high_r - low_r) + low_r;
				gg = (dist - (low_range + high_range)) / (high_range - (low_range + high_range)) * (high_g - low_g) + low_g;

				//rr = (dist - low_range) / (high_range - low_range) * (high_r - low_r) + low_r;
				//gg = (dist - high_range) / (high_range - low_range) * (high_g - low_g) + low_g;

				bb = -1.0f / pow((high_range - low_range) / 2, 2) * pow((dist - (low_range + high_range) / 2), 2) + 1.0f;

				//rr *= 0.5;
				//gg *= 0.5;

			}
			//cout << rr << " " << gg << " " << bb << endl;

			glEnable(GL_LINE_STIPPLE);
			
			glPushMatrix();

			glColor3f(rr, gg, bb);
			//glColor3f(0.0f, 0.0f, 1.0f);
			
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



	/*****************************************************************************************************/
	/*****************************************************************************************************/
	/*****************************************************************************************************/

	/*--------------ROI Storage-------------*/
	//if (ROIStorageCount != 0)
	//{
	//	glPushMatrix();
	//	glPointSize(2.0f);
	//	glBegin(GL_POINTS);
	//	
	//	if (IS_PathGenProjToPlane || IS_PathGenRegression)
	//	{
	//		glColor3ub(255, 255, 55);
	//	}
	//	else
	//	{
	//		glColor3ub(255, 0, 137);
	//
	//	}
	//
	//	for (int i = 0; i < ROIStorageCount; i++)
	//	{
	//		if (i == 0 && !IS_REC_START)
	//		{
	//			Start.x = ROICenterColorS_Old.x;
	//			Start.y = ROICenterColorS_Old.y;
	//			
	//			IS_REC_START = TRUE;
	//		}
	//		else if (i == ROIStorageCount - 1 && !IS_KEY_F1_UP && !IS_REC_END)
	//		{
	//			End.x = ROICenterColorS_Old.x;
	//			End.y = ROICenterColorS_Old.y;
	//
	//			IS_REC_END = TRUE;
	//		}
	//
	//		GLfloat pX = ROICameraSP_Storage[i].X;
	//		GLfloat pY = ROICameraSP_Storage[i].Y;
	//		GLfloat pZ = ROICameraSP_Storage[i].Z;
	//		glVertex3f(pX, pY, pZ);
	//
	//	}
	//
	//	glEnd();
	//	glPopMatrix();
	//}

	/*--------------ROI Storage in .txt file-------------*/
	//if (IS_KEY_F6_UP && ROIStorageCount != 0)
	//{
	//	fstream file;
	//	file.open("coord.txt", ios::out);      //開啟檔案
	//
	//	if (!file)     //檢查檔案是否成功開啟
	//
	//	{
	//
	//		cerr << "Can't open file!\n";
	//
	//		exit(1);     //在不正常情形下，中斷程式的執行
	//
	//	}
	//
	//
	//	cout << endl << "Writing to file, please wait" << endl;
	//
	//	for (int i = 0; i < ROIStorageCount; i++)    
	//	{
	//		if (i == 0)
	//		{
	//			file << "Total count : " << ROIStorageCount << endl;
	//		}
	//
	//		file <<  setw(5) << i + 1 << " : " << fixed << setprecision(4) << ROICameraSP_Storage[i].X << "   " << ROICameraSP_Storage[i].Y << "   " << ROICameraSP_Storage[i].Z << "   " << "\n";
	//
	//	}      //將資料輸出至檔案
	//
	//	IS_KEY_F6_UP = FALSE;
	//
	//	cout << endl << "End writing to file" << endl;
	//}


	/*--------------IS_PathGenProjToPlane-------------*/
	//if (IS_KEY_F3_UP && ROIStorageCount > 0)
	//{	
	//
	//	if (ROICameraSP_AtWorldCoordOrigin != nullptr)
	//	{
	//		delete[] ROICameraSP_AtWorldCoordOrigin;
	//		ROICameraSP_AtWorldCoordOrigin = nullptr;
	//	}
	//
	//	ROICameraSP_AtWorldCoordOrigin = new CameraSpacePoint[ROIStorageCount];
	//	
	//
	//	if (ROICameraSP_Projection != nullptr)
	//	{
	//		delete[] ROICameraSP_Projection;
	//		ROICameraSP_Projection = nullptr;
	//	}
	//
	//	ROICameraSP_Projection = new CameraSpacePoint[ROIStorageCount];
	//	
//
	//	GLfloat TransM1[6] = { -CubicRota.x, -CubicRota.y, -CubicRota.z, -CubicPosi.x, -CubicPosi.y, -CubicPosi.z };
	//	GLfloat TransM2[6] = { CubicRota.x, CubicRota.y, CubicRota.z, CubicPosi.x, CubicPosi.y, CubicPosi.z };
	//	double Coeff[4] = { 0, 0, 1, -CubicUnitT };
//
	//	PathGenCoordWorldToCubic(ROICameraSP_Storage, ROIStorageCount, TransM1, ROICameraSP_AtWorldCoordOrigin);
	//	PathGenProjToPlane(ROICameraSP_AtWorldCoordOrigin, ROIStorageCount, Coeff, ROICameraSP_Projection);
	//	PathGenCoordCubicToWorld(ROICameraSP_Projection, ROIStorageCount, TransM2, ROICameraSP_Storage);
//
//
	//	glPushMatrix();
	//	glPointSize(2.0f);
	//	glBegin(GL_POINTS);
//
	//	for (int i = 0; i < ROIStorageCount; i++)
	//	{
	//		glColor3ub(255, 255, 55);
	//		GLfloat pX = ROICameraSP_Storage[i].X;
	//		GLfloat pY = ROICameraSP_Storage[i].Y;
	//		GLfloat pZ = ROICameraSP_Storage[i].Z;
//
	//		glVertex3f(pX, pY, pZ);
	//	}
//
	//	glEnd();
	//	glPopMatrix();
//
	//	IS_PathGenProjToPlane = TRUE;
	//	IS_KEY_F3_UP = FALSE;
//
	//}


	/*--------------IS_PathGenRegression-------------*/

	//if (IS_PathGenProjToPlane && ROIStorageCount > 0)
	//{
	//	if (ROICameraSP_Regression != nullptr)
	//	{
	//		delete[] ROICameraSP_Regression;
	//		ROICameraSP_Regression = nullptr;
	//	}
	//
	//	ROICameraSP_Regression = new CameraSpacePoint[ROIStorageCount];
	//
	//	//Result = Regression Coeff = [type, a, b]; y = ax + b
	//	//type = 1 = yx, type = 2 = zx
	//	double RegresCoeff[3] = { 1, 0, 0 };
	//	
	//	//[Rx, Ry, Rz, Tx, Ty, Tz] in order
	//	GLfloat TransM3[6] = { 0, 0, 0, 0, 0, -CubicUnitT };
	//
//
	//	PathGenCoordWorldToCubic(ROICameraSP_AtWorldCoordOrigin, ROIStorageCount, TransM3, ROICameraSP_Regression);
	//	PathGenRegression(ROICameraSP_Regression, ROIStorageCount, RegresCoeff);
	//	
//
	//	//畫出regression的結果，應該要用原始資料去找起點跟終點，但是好麻煩所以硬代lol
	//	//但是應該可以在bubble sort那邊算出來
	//	double T_MIN = 0;
	//	double T_MAX = 0;
	//	double T_PRECISION = 0;
	//
	//	for (int i = 0; i < ROIStorageCount; i++)
	//	{
	//		if (T_MIN > ROICameraSP_Regression[i].X)
	//		{
	//			T_MIN = ROICameraSP_Regression[i].X;
	//		}
	//		else if (T_MAX < ROICameraSP_Regression[i].X)
	//		{
	//			T_MAX = ROICameraSP_Regression[i].X;
	//		}
	//	}
	//
	//	T_PRECISION = (T_MAX - T_MIN) / 100;
	//
	//	glPushMatrix();
	//	glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);
	//
	//	glRotatef(CubicRota.y, 0, 1, 0);
	//	glRotatef(CubicRota.z, 0, 0, 1);
	//	glRotatef(CubicRota.x, 1, 0, 0);
	//	glTranslatef(0, 0, CubicUnitT);
	//
	//	glColor3ub(0, 255, 0);
	//
	//	glBegin(GL_LINE_STRIP);
	//	for (double t = T_MIN; t <= T_MAX; t += T_PRECISION)
	//	{
	//		glVertex2d(t, RegresCoeff[1] * t + RegresCoeff[2]);
	//	}
	//	glEnd();
	//
	//	glPopMatrix();
	//
	//	IS_PathGenRegression = TRUE;
	//}
	
	/*****************************************************************************************************/
	/*****************************************************************************************************/
	/*****************************************************************************************************/

	//GLfloat m[16] = { 0 };
	//m[0] = m[5] = m[10] = m[15] = 1;
	//m[12] = 4;
	//m[13] = 2;
	//m[14] = 5;
	//
	//glPushMatrix();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//
	//glTranslatef(0, 0, -10);
	//
	//glMultMatrixf(m);
	//
	//glGetFloatv(GL_MODELVIEW_MATRIX, m);
	//glPopMatrix();
	//	
	//cout << endl << m[0] << " " << m[4] << " " << m[8] << " " << m[12] << endl;
	//cout << m[1] << " " << m[5] << " " << m[9] << " " << m[13] << endl;
	//cout << m[2] << " " << m[6] << " " << m[10] << " " << m[14] << endl;
	//cout << m[3] << " " << m[7] << " " << m[11] << " " << m[15] << endl;



	/*--------------Draw Cubic-------------*/

	CameraSpacePoint* Y_Devia = new CameraSpacePoint[2];
	Y_Devia[0].X = Y_Devia[0].Y = Y_Devia[0].Z = 0;
	Y_Devia[1].X = Y_Devia[1].Y = Y_Devia[1].Z = 0;

	if (ROIStorageCount > 0)
	{
		float dy = 0;
		//for (int i = 0; i < ROIStorageCount; i++)
		//{
		//	if (i == 0)
		//	{
		//		dy = ROICameraSP_MechCoord_Storage[i].Y;
		//	}
		//	else
		//	{
		//		dy = ROICameraSP_MechCoord_Storage[i].Y - ROICameraSP_MechCoord_Storage[i - 1].Y;
		//	}


		//}

		dy =0.32 -  ROICameraSP_MechCoord_Storage[0].Y;

		GLfloat TransM_OBJ[16] = { 0 };
		TransM_OBJ[0] = TransM_OBJ[5] = TransM_OBJ[10] = TransM_OBJ[15] = 1;

		glPushMatrix();

		glLoadIdentity();

		glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
		glMultMatrixf(M_Cubic);
		glRotatef(theta2, 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		glTranslatef(-0.14, 0, -0.2);

		//glTranslatef(0.14, 0, 0.2);
		//glRotatef(-90, 1, 0, 0);
		//glRotatef(-theta2, 0, 1, 0);
		//glMultMatrixf(M_Cubic_inv);
		//glTranslatef(-Intersect.X, -Intersect.Y, -Intersect.Z);

		glGetFloatv(GL_MODELVIEW_MATRIX, TransM_OBJ);

		glPopMatrix();

		Y_Devia[1].Y = dy;

		ROITrans(Y_Devia, 2, TransM_OBJ, Y_Devia);

		//cout << Y_Devia->X << " " << Y_Devia->Y << " " << Y_Devia->Z << endl;
	}


	glPushMatrix();
	
	if (Cubic_IS_BLEND)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	if (ROI_IS_COLLIDE)
	{
		//glColor4f(1.0f, 0.0f, 0.0f, 0.6f);
		//glColor3f(1.0f, 0.0f, 0.0f);
	}
	else
	{
		//glColor4f(0.0f, 0.0f, 0.7f, 0.6f);
		//glColor3f(0.0f, 0.0f, 1.0f);
	}

	glTranslatef(Y_Devia[1].X - Y_Devia[0].X, Y_Devia[1].Y - Y_Devia[0].Y, Y_Devia[1].Z - Y_Devia[0].Z);
	//glTranslatef(CubicPosi.x, CubicPosi.y, CubicPosi.z);
	DrawCubic();


	if (Cubic_IS_BLEND)
	{
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

	}

	glPopMatrix();

	delete[] Y_Devia;


	glPushMatrix();
	glColor3ub(0, 255, 0);

	if (!ROI_IS_FRONT && ROICameraSP_MechCoord != nullptr)
	{
		glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x - 60, ROICenterColorS_Old.y - 35);
		//glPrintf("(%.4f, %.4f, %.4f)", ROICenterCameraS.x, ROICenterCameraS.y, ROICenterCameraS.z);
		glPrintf("(%.3f, %.3f, %.3f)", ROICameraSP_MechCoord[0].X, ROICameraSP_MechCoord[0].Y, ROICameraSP_MechCoord[0].Z);
	}
	glPopMatrix();

	float dist = ARFunc_ROICSP_Proj_Dist * 1000;
	const int low_range = 30, high_range = 100;
	int low_r = 1, high_r = 0, low_g = 0, high_g = 1;

	float rr = 0, gg = 0, bb = 0;

	if (dist > high_range)
	{
		rr = 0;
		gg = 1;
		bb = 0;
	}
	//else if (dist < (low_range + high_range))
	//{
	//	rr = 0;
	//	gg = 0;
	//	bb = 255;
	//}
	else if (dist < low_range)
	{
		rr = 1;
		gg = 0;
		bb = 0;
	}
	else
	{
		rr = (dist - low_range) / ((low_range + high_range) - low_range) * (high_r - low_r) + low_r;
		gg = (dist - (low_range + high_range)) / (high_range - (low_range + high_range)) * (high_g - low_g) + low_g;

		//rr = (dist - low_range) / (high_range - low_range) * (high_r - low_r) + low_r;
		//gg = (dist - high_range) / (high_range - low_range) * (high_g - low_g) + low_g;

		bb = -1.0f / pow((high_range - low_range) / 2, 2) * pow((dist - (low_range + high_range) / 2), 2) + 1.0f;

		//rr *= 0.5;
		//gg *= 0.5;

	}
	//cout << rr << " " << gg << " " << bb << endl;

	glPushMatrix();
	//glColor3ub(255, 0, 0);
	glColor3f(rr, gg, bb);
	if (IS_ARFunc_InsideTriCheck && !ROI_IS_COLLIDE)
	{
		glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x + 10, ROICenterColorS_Old.y + 15);
		glPrintf("%.2f mm ", ARFunc_ROICSP_Proj_Dist * 1000);
	}
	glPopMatrix();


	 
	/*--------------show start and ent point-------------*/
	//glPushMatrix();
	//
	//if (ROIStorageCount != 0)
	//{
	//	if (BG_IS_OPEN)
	//	{
	//		if (IS_REC_START)
	//		{
	//			glColor3ub(255, 255, 0);
	//			glPrintf(">>glut(%i, %i)", Start.x, Start.y);
	//			glPrintf("Start");
	//
	//		}
	//		if (IS_REC_END && !IS_KEY_F1_UP)
	//		{
	//			glPrintf(">>glut(%i, %i)", End.x, End.y);
	//			glPrintf("End");
	//		}
	//	}
	//	else
	//	{
	//		if (IS_REC_START)
	//		{
	//			glColor3ub(255, 255, 0);
	//			glPrintf(">>glut(%i, %i)", Start.x - 70, Start.y - 35);
	//			glPrintf("Start");
	//
	//		}
	//		if (IS_REC_END && !IS_KEY_F1_UP)
	//		{
	//			glPrintf(">>glut(%i, %i)", End.x - 70, End.y - 35);
	//			glPrintf("End");
	//		}
	//	}
	//
	//}
	//
	//glPopMatrix();


	/*--------------Draw Path-------------*/
	glPushMatrix();

	if (ROI_IS_COLLIDE)
	{
		glColor3ub(64, 128, 128);
	}
	else
	{
		glColor3ub(247, 80, 0);
	}

	//DrawPath();

	glPopMatrix();



	if (ROI_IS_FRONT && ROICameraSP_MechCoord != nullptr)
	{
		glPushMatrix();

		glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x - 60, ROICenterColorS_Old.y - 35);
		glColor3ub(0, 255, 0);
		//glPrintf("(%.4f, %.4f, %.4f)", ROICenterCameraS.x, ROICenterCameraS.y, ROICenterCameraS.z);
		glPrintf("(%.3f, %.3f, %.3f)", ROICameraSP_MechCoord[0].X, ROICameraSP_MechCoord[0].Y, ROICameraSP_MechCoord[0].Z);
		glPopMatrix();

	}


	/*-----------------------------------------------------*/
	/*--------------OpenHaptics Force Generate-------------*/
	/*-----------------------------------------------------*/
	if (IS_HL_FORCE_APPLY)
	{
		hlBeginFrame();

		HDdouble EETRans[16];
		HDdouble anchor[3];

		hlGetDoublev(HL_PROXY_TRANSFORM, EETRans);
		hlGetDoublev(HL_PROXY_POSITION, anchor);

		//cout << EETRans[0] << " " << EETRans[4] << " " << EETRans[8] << " " << EETRans[12] << endl;
		//cout << EETRans[1] << " " << EETRans[5] << " " << EETRans[9] << " " << EETRans[13] << endl;
		//cout << EETRans[2] << " " << EETRans[6] << " " << EETRans[10] << " " << EETRans[14] << endl;
		//cout << EETRans[3] << " " << EETRans[7] << " " << EETRans[11] << " " << EETRans[15] << endl << endl;

		//cout << anchor[0] << " " << anchor[1] << " " << anchor[2] << " " << anchor[3] << endl << endl;

		if (ROI_IS_COLLIDE == TRUE)
		{
			if (IS_HL_FORCE_GEN == FALSE)
			{
				HDdouble anchor[3];

				spring = hlGenEffects(1);
				hlGetDoublev(HL_PROXY_POSITION, anchor);

				hlEffectd(HL_EFFECT_PROPERTY_GAIN, gGain);
				hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
				hlEffectdv(HL_EFFECT_PROPERTY_POSITION, anchor);
				hlStartEffect(HL_EFFECT_SPRING/*HL_EFFECT_VISCOUS*/, spring);

				IS_HL_FORCE_GEN = TRUE;
			}

		}
		else
		{
			if (IS_HL_FORCE_GEN)
			{
				if (gMagnitude > 0)
				{
					gMagnitude -= 0.3;
					hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
					hlUpdateEffect(spring);
				}
				else
				{
					gMagnitude = 0.8f;

					hlStopEffect(spring);
					IS_HL_FORCE_GEN = FALSE;
				}
			}


		}

		hlEndFrame();

	}
	else
	{
		if (IS_HL_FORCE_GEN)
		{
			hlBeginFrame();

			if (gMagnitude > 0)
			{
				gMagnitude -= 0.3;
				hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
				hlUpdateEffect(spring);
			}
			else
			{
				gMagnitude = 0.8f;

				hlStopEffect(spring);
				IS_HL_FORCE_GEN = FALSE;
			}

			hlEndFrame();
		}
	}


	/*--------------Draw Solid Tea Pot-------------*/
	//glPushMatrix();
	//glTranslatef(PORCenterCameraS.x, PORCenterCameraS.y, PORCenterCameraS.z);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glutSolidSphere(0.01, 100, 100);
	//glPopMatrix();

	ROI_IS_COLLIDE = FALSE;
	ROI_IS_FRONT = FALSE;

	if (IS_PORTAUDIO_START == 0)
	{
		//sine.stop();
	}

	//FPS_RS++;
	//glutSwapBuffers();
	//glFinish();



	if (Finish_Without_Update)
		glFinish();
	else
		glutSwapBuffers();

	FPS_RS++;
}


/*------------------------------------------*/
/*--------------Kinect Function-------------*/
/*------------------------------------------*/
void KineticInit()
{
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
			pBufferColor_RGB = new BYTE[4 * colorPointCount];
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

	 CubicPosi.x =  0.0f;
	 CubicPosi.y = -0.11f;
	 CubicPosi.z = -0.68f;

	 CubicRota.x = -86.0f;
	 CubicRota.y = 72.0f;
	 CubicRota.z = 19.0f;
}

void KineticUpdate()
{
	/*--------------Read color data-------------*/
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

	/*--------------Read depth data-------------*/
	IDepthFrame* pDFrameDepth = nullptr;
	if (pFrameReaderDepth->AcquireLatestFrame(&pDFrameDepth) == S_OK)
	{
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, pBufferDepth);
		pDFrameDepth->CopyFrameDataToArray(depthPointCount, reinterpret_cast<UINT16*>(mDepthImg.data));

		pDFrameDepth->Release();
		pDFrameDepth = nullptr;
	}


	/*--------------Mapper Function (Point Cloud)-------------*/
	Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);	
	
	//blur(mDepthImg, mDepthImg, Size(3, 3));
	//GaussianBlur(mDepthImg, mDepthImg, Size(3, 3), 0, 0);
	//pBufferDepth = reinterpret_cast<UINT16*>(mDepthImg.data);
	//Mapper->MapColorFrameToCameraSpace(depthPointCount, pBufferDepth, colorPointCount, pCSPoints);

	/*--------------Call Window Function-------------*/
	ShowImage();
}

void ShowImage()
{

	namedWindow("Color Map");

	/*--------------Depth Image-------------*/
	//namedWindow("Depth Map");
	//// Convert from 16bit to 8bit
	//mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);
	//cv::imshow("Depth Map", mImg8bit);

	/*--------------Set Mouse Callback Function and Find ROI-------------*/
	cvSetMouseCallback("Color Map", onMouseROI, NULL);

	//if (ROI.data != NULL)
	//{
	//	ROI.release();
	//}

	mColorImg.copyTo(ROI);

	if (ROI_S2 == TRUE && ROI_S1 == FALSE)
	{
		int thickness = 2;
		rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), thickness);
		FindROI();
	}

	if (LR_S2[0] == TRUE && LR_S1[0] == FALSE)
	{
		int thickness = 2;
		rectangle(ROI, LR_p1[0], LR_p2[0], Scalar(232, 117, 0), thickness);

		stringstream ss;
		ss << 1; //把int型態變數寫入到stringstream

		string text2;
		ss >> text2;

	    string text1 = "Line " + text2;

		putText(ROI, text1, Point(LR_p1[0].x, LR_p1[0].y - 5), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(232, 117, 0));

		FindLR();

	}

	if (LR_S2[1] == TRUE && LR_S1[1] == FALSE)
	{
		int thickness = 2;
		rectangle(ROI, LR_p1[1], LR_p2[1], Scalar(232, 117, 0), thickness);

		stringstream ss;
		ss << 2; //把int型態變數寫入到stringstream

		string text2;
		ss >> text2;

		string text1 = "Line " + text2;

		putText(ROI, text1, Point(LR_p1[1].x, LR_p1[1].y - 5), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(232, 117, 0));

		FindLR();

	}

	//if (LRPixelCount[0] > 0)
	//{
	//	for (int i = 0; i < LRPixelCount[0]; i++)
	//	{
	//		int Px = LRPixel_Select_L1[i].x;
	//		int Py = LRPixel_Select_L1[i].y;
	//		circle(ROI, Point(Px, Py), 10, Scalar(0, 0, 255), -1);
	//
	//	}
	//}
	//if (LRPixelCount[1] > 0)
	//{
	//	for (int i = 0; i < LRPixelCount[1]; i++)
	//	{
	//		int Px = LRPixel_Select_L2[i].x;
	//		int Py = LRPixel_Select_L2[i].y;
	//		circle(ROI, Point(Px, Py), 10, Scalar(0, 0, 255), -1);
	//		
	//	}
	//}

	cv::imshow("Color Map", ROI);


}

void onMouseROI(int event, int x, int y, int flags, void* param)
{
	int thickness = 2;

	//Push
	if (event == CV_EVENT_LBUTTONDOWN)
	{ 
		ROI_rect.x = x;
		ROI_rect.y = y;

		ROI_S1 = TRUE;
		ROI_S2 = FALSE;

	}
	else if (event == CV_EVENT_RBUTTONDOWN)
	{
		int LINE_FLAG = -1;


		switch (INIT_STEP)
		{
		case SELECT_LINE1:

			LINE_FLAG = 0;
			//if (LRPixel_Select_L1 != nullptr)
			//{
			//	delete [] LRPixel_Select_L1;
			//	LRPixel_Select_L1 = nullptr;
			//	LRPixelCount[0] = 0;
			//}
			//LRPixel_Select_L1 = new Point2i[LR_StorageSize];

			IS_LR_SELECT[0] = FALSE;
			IS_LR_SELECT[1] = FALSE;

			break;

		case SELECT_LINE2:

			LINE_FLAG = 1;
			//if (LRPixel_Select_L2 != nullptr)
			//{
			//	delete[] LRPixel_Select_L2;
			//	LRPixel_Select_L2 = nullptr;
			//	LRPixelCount[1] = 0;
			//}
			//LRPixel_Select_L2 = new Point2i[LR_StorageSize];
			break;
		}

		LR_rect[LINE_FLAG].x = x;
		LR_rect[LINE_FLAG].y = y;

		LR_S1[LINE_FLAG] = TRUE;
		LR_S2[LINE_FLAG] = FALSE;

		IS_LR_SELECT[LINE_FLAG] = FALSE;


	}

	//Release
	else if (event == CV_EVENT_LBUTTONUP)
	{
		ROI_S1 = FALSE;

		ROI_rect.height = y - ROI_rect.y;
		ROI_rect.width = x - ROI_rect.x;

		ROI_p2 = Point(x, y);

	}
	else if (event == CV_EVENT_RBUTTONUP)
	{
		int LINE_FLAG = -1;

		switch (INIT_STEP)
		{
		case SELECT_LINE1:

			LINE_FLAG = 0;
			INIT_STEP = SELECT_LINE2;

			//IS_PlaceObject = TRUE;

			break;

		case SELECT_LINE2:

			LINE_FLAG = 1;
			INIT_STEP = SELECT_LINE1;

			IS_PlaceObject = FALSE;

			break;
		}

		LR_S1[LINE_FLAG] = FALSE;

		LR_rect[LINE_FLAG].height = y - LR_rect[LINE_FLAG].y;
		LR_rect[LINE_FLAG].width = x - LR_rect[LINE_FLAG].x;

		LR_p2[LINE_FLAG] = Point(x, y);

		IS_LR_SELECT[LINE_FLAG] = TRUE;

		//if (IS_LR_SELECT[0] + IS_LR_SELECT[1] == 2)
		//{
		//	CameraSpacePOR();
		//}

		//cout << endl << INIT_STEP << endl;
	}

	//if (ROI.data != NULL)
	//{
	//	ROI.release();
	//}

	mColorImg.copyTo(ROI);

	//Drag
	if (flags == CV_EVENT_FLAG_LBUTTON)
	{
		ROI_S2 = TRUE;

		ROI_p1 = Point(ROI_rect.x, ROI_rect.y);
		ROI_p2 = Point(x, y);

		rectangle(ROI, ROI_p1, ROI_p2, Scalar(0, 255, 0), 2);
		cv::imshow("Color Map", ROI);

	}
	else if (flags == CV_EVENT_FLAG_RBUTTON)
	{
		int LINE_FLAG = -1;

		switch (INIT_STEP)
		{
		case SELECT_LINE1:

			LINE_FLAG = 0;
			//if (LRPixelCount[0] < LR_StorageSize)
			//{
			//	LRPixel_Select_L1[LRPixelCount[0]].x = x;
			//	LRPixel_Select_L1[LRPixelCount[0]].y = y;
			//
			//	//很慢的方式，但是改Mat裡的RGB值更慢
			//	circle(ROI, Point(LRPixel_Select_L1[LRPixelCount[0]].x, LRPixel_Select_L1[LRPixelCount[0]].y), 10, Scalar(0, 0, 255), -1);
			//	LRPixelCount[0]++;
			//	cout << LRPixelCount[0] << endl;
			//}
			break;

		case SELECT_LINE2:

			LINE_FLAG = 1;
			//if (LRPixelCount[1] < LR_StorageSize)
			//{
			//	LRPixel_Select_L2[LRPixelCount[1]].x = x;
			//	LRPixel_Select_L2[LRPixelCount[1]].y = y;
			//
			//	//很慢的方式，但是改Mat裡的RGB值更慢
			//	circle(ROI, Point(LRPixel_Select_L2[LRPixelCount[1]].x, LRPixel_Select_L2[LRPixelCount[1]].y), 10, Scalar(0, 0, 255), -1);
			//	LRPixelCount[1]++;
			//}
			break;
		}

		//CvFont* font;
		//cvInitFont(font, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f);

		stringstream ss;
		ss << LINE_FLAG + 1; //把int型態變數寫入到stringstream

		string text2;
		ss >> text2;

		string text1 = "Line " + text2;

		putText(ROI, text1, LR_p1[LINE_FLAG], CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(232, 117, 0));

		LR_S2[LINE_FLAG] = TRUE;

		LR_p1[LINE_FLAG] = Point(LR_rect[LINE_FLAG].x, LR_rect[LINE_FLAG].y);
		LR_p2[LINE_FLAG] = Point(x, y);

		rectangle(ROI, LR_p1[LINE_FLAG], LR_p2[LINE_FLAG], Scalar(232, 117, 0), 2);
		cv::imshow("Color Map", ROI);
	}
}


/*---------------------------------------*/
/*--------------ROI Function-------------*/
/*---------------------------------------*/
void FindROI()
{
	Mat ROI_Image;

	/*--------------Find ROI-------------*/
	if (ROI_p2.x > ROI_p1.x && ROI_p2.y > ROI_p1.y)
	{
		ROI_Image = mColorImg.colRange(ROI_p1.x, ROI_p2.x + 1).rowRange(ROI_p1.y, ROI_p2.y + 1).clone();
	}
	else
	{
		OutputDebugString("你按了左鍵。拉框請從左上到右下，不要亂搞\n");
		return;
	}


	/*--------------BGR to YCrCb-------------*/
	Mat ROI_YCrCb;
	cvtColor(ROI_Image, ROI_YCrCb, CV_BGR2YCrCb);

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
			OutputValue(ROI_YCrCb, i, j, 1, &IsRed);

			//threshold = 150 for fluorescent pink
			//threshold = 170 for red
			if (IsRed > 160)
			{

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

	if (ROIcount > 0)
	{
		ROICenterColorS_New.x = static_cast<int>(ROICenterColorS_New.x / ROIcount);
		ROICenterColorS_New.y = static_cast<int>(ROICenterColorS_New.y / ROIcount);

		CameraSpaceROI();
		MoveROI();

	}
	else if (ROIcount == 0)
	{
		ROICenterColorS_Old.x = ROICenterColorS_New.x = 0;
		ROICenterColorS_Old.y = ROICenterColorS_New.y = 0;
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

		Dir.val[0] = ROICenterColorS_New.x - ROICenterColorS_Old.x;
		Dir.val[1] = ROICenterColorS_New.y - ROICenterColorS_Old.y;

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

void CameraSpaceROI()
{
	if (ROICameraSP != nullptr)
	{
		delete[] ROICameraSP;
		ROICameraSP = nullptr;
	}

	ROIDepthCount = 0;

	ROICenterCameraS.x = 0;
	ROICenterCameraS.y = 0;
	ROICenterCameraS.z = 0;

	for (int i = 0; i < ROIcount; i++)
	{
		int index1 = ROIPixel[i].x + ROIPixel[i].y * iWidthColor;
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
			Temp[indx1].X = pCSPoints[indx2].X;
			Temp[indx1].Y = pCSPoints[indx2].Y;
			Temp[indx1].Z = -pCSPoints[indx2].Z;


			//cout << Temp[indx1].X << " " << Temp[indx1].Y << " " << Temp[indx1].Z << endl;

			indx1++;
	
		}
	}

	/*------------------------------------------------------------------*/
	//Bubble sort
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

	//Settint threshold and filtering
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

	delete [] Temp;

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

void ROICameraSPStorage()
{
	if (ROICameraSP_Storage == nullptr)
	{
		ROICameraSP_Storage = new CameraSpacePoint[StorageSize];
		ROICameraSP_MechCoord_Storage = new CameraSpacePoint[StorageSize];

		ROIStorageCount = 0;
	}
	

	//if (IS_KEY_F1_UP && ROIDepthCount > 0)
	//{
	//
	//	if (ROIStorageCount + ROIDepthCount < StorageSize)
	//	{
	//
	//		for (int i = 0; i < ROIDepthCount; i++)
	//		{
	//
	//			//ROICameraSP_Storage[i + ROIStorageCount] = ROICameraSP[i];
	//			ROICameraSP_Storage[i + ROIStorageCount] = ROICameraSP[i];
	//			ROICameraSP_MechCoord_Storage[i + ROIStorageCount] = ROICameraSP_MechCoord[i];
	//			
	//		}
	//
	//		ROIStorageCount += ROIDepthCount;
	//
	//	}
	//	else
	//	{
	//		cout << endl << "The array ROICameraSP_Storage is full, press F2 to clear" << endl;
	//	}
	//}

	if (ARFunc_ROICSP_Proj != nullptr)
	{

		if (ROIStorageCount + 1 < StorageSize)
		{
			CameraSpacePoint Center;
			Center.X = ROICenterCameraS.x;
			Center.Y = ROICenterCameraS.y;
			Center.Z = ROICenterCameraS.z;

			//ROICameraSP_Storage[i + ROIStorageCount] = ROICameraSP[i];
			ROICameraSP_Storage[ROIStorageCount] = /**ARFunc_ROICSP_Proj*/Center;
			ROICameraSP_MechCoord_Storage[ROIStorageCount] = *ROICameraSP_MechCoord;

			ROIStorageCount += 1;

		}
		else
		{
			cout << endl << "The array ROICameraSP_Storage is full, press F2 to clear" << endl;
		}
	}

}


/*-----------------------------------------------------------------*/
/*------------------Linear Regression Function-----------------*/
/*-----------------------------------------------------------------*/
void FindLR()
{
	int total = IS_LR_SELECT[0] + IS_LR_SELECT[1];
	int SET_iter = 0;
	int SET_target = -1;

	switch (total)
	{
		//L1, L2 其中一個好了
	case 1:
		SET_target = 0 * IS_LR_SELECT[0] + 1 * IS_LR_SELECT[1];
		SET_iter = 1;
		break;

		//L1, L2 兩個好了
	case 2:
		SET_target = 0;
		SET_iter = 2;
		break;
	}

	int iter = 0;
	int target = SET_target;


	while (iter < SET_iter)
	{

		/*--------------Find ROI-------------*/
		Mat LR_Image;

		if (LR_p2[target].x > LR_p1[target].x && LR_p2[target].y > LR_p1[target].y)
		{
			LR_Image = mColorImg.colRange(LR_p1[target].x, LR_p2[target].x + 1).rowRange(LR_p1[target].y, LR_p2[target].y + 1).clone();
		}
		else
		{
			OutputDebugString("你按了右鍵。拉框請從左上到右下，不要亂搞\n");
			return;
		}

		/*--------------BGR to YCrCb-------------*/
		Mat LR_YCrCb;
		cvtColor(LR_Image, LR_YCrCb, CV_BGR2YCrCb);

		/*-------------- Color Detection and Tracking-------------*/
		int channels = LR_YCrCb.channels();
		int rows = LR_YCrCb.rows;
		int cols = LR_YCrCb.cols;
		int steps = LR_YCrCb.step;
		
		LRPixelCount[target] = 0;
		LRDepthCount[target] = 0;

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{

				if (LRPixelCount[target] > MAX_PIXEL_COUNT_LR)
				{
					OutputDebugString("--------Array Out of range---------//-------Array Out of range--------//--------Array Out of range--------\n");
					exit(1);
				}

				uchar IsBlue = 0;

				OutputValue(LR_YCrCb, i, j, 2, &IsBlue);

				//threshold < 145
				if (IsBlue > 145)
				{


					InputValue(ROI, 0, i + LR_p1[target].y, j + LR_p1[target].x, 0, 255);
					InputValue(ROI, 0, i + LR_p1[target].y, j + LR_p1[target].x, 1, 0);
					InputValue(ROI, 0, i + LR_p1[target].y, j + LR_p1[target].x, 2, 0);

					LRPixel[target][LRPixelCount[target]].x = j + LR_p1[target].x;
					LRPixel[target][LRPixelCount[target]].y = i + LR_p1[target].y;

					LRPixelCount[target]++;

				}
			}
		}


		iter += 1;
		target += 1;
	}

	if (LRPixelCount[0] > 0 || LRPixelCount[1] > 0)
	{
		CameraSpaceLR();
	}

}

void CameraSpaceLR()
{
	int total = IS_LR_SELECT[0] + IS_LR_SELECT[1];
	int SET_iter = 0;
	int SET_target = -1;

	//cout << total << endl;


	switch (total)
	{
		//L1, L2 其中一個好了
	case 1:
		SET_target = 0 * IS_LR_SELECT[0] + 1 * IS_LR_SELECT[1];
		SET_iter = 1;
		break;

		//L1, L2 兩個好了
	case 2:
		SET_target = 0;
		SET_iter = 2;
		break;
	}

	int iter = 0;
	int target = SET_target;


	while (iter < SET_iter)
	{
		LRDepthCount[target] = 0;

		for (int i = 0; i < LRPixelCount[target]; i++)
		{
			int index1 = LRPixel[target][i].x + LRPixel[target][i].y * iWidthColor;
			if (pCSPoints[index1].Z != -1 * numeric_limits<float>::infinity() && LRDepthCount[target] < MAX_DEPTH_COUNT_LR)
			{
				LRCameraSP[target][LRDepthCount[target]].X = pCSPoints[index1].X;
				LRCameraSP[target][LRDepthCount[target]].Y = pCSPoints[index1].Y;
				LRCameraSP[target][LRDepthCount[target]].Z = -pCSPoints[index1].Z;

				LRDepthCount[target]++;

			}
		}

		//cout << LRDepthCount[target] << endl;

		//Bubble sort
		for (int i = LRDepthCount[target] - 1; i > 0; --i)
		{
			for (int j = 0; j < i; ++j)
			{
				if (LRCameraSP[target][j].Z < LRCameraSP[target][j + 1].Z)
				{
					CameraSpacePoint temp;
		
					temp.X = LRCameraSP[target][j].X;
					temp.Y = LRCameraSP[target][j].Y;
					temp.Z = LRCameraSP[target][j].Z;
		
					LRCameraSP[target][j].X = LRCameraSP[target][j + 1].X;
					LRCameraSP[target][j].Y = LRCameraSP[target][j + 1].Y;
					LRCameraSP[target][j].Z = LRCameraSP[target][j + 1].Z;
		
					LRCameraSP[target][j + 1].X = temp.X;
					LRCameraSP[target][j + 1].Y = temp.Y;
					LRCameraSP[target][j + 1].Z = temp.Z;
				}
			}
		}
		
		//Settint threshold and filtering
		int IndexLim = 0;
		
		for (int i = 0; i < LRDepthCount[target]; i++)
		{
			if (LRCameraSP[target][i].Z < LRCameraSP[target][0].Z*1.5)
			{
				IndexLim = i;
				break;
			}
		}
		
		//Mismatch occur
		if (IndexLim != 0)
		{
			LRDepthCount[target] = IndexLim;
		
			//cout << "Yeeee" << endl;
		}

		//cout << target << ", " << LRDepthCount[target] << endl;

		iter += 1;
		target += 1;
	}

	if (IS_LR_SELECT[1] == TRUE)
	{
		CameraSpacePOR();
	}


}


/*----------------------------------------------------*/
/*--------------Plane of Regression (POR)-------------*/
/*----------------------------------------------------*/
void CameraSpacePOR(void)
{
	int total = IS_LR_SELECT[0] + IS_LR_SELECT[1];

	//cout << total << endl;

	if (total == 2)
	{
		PORDepthCount = 0;

		for (int target = 0; target < 2; target++)
		{
			Mat LR_Image;

			if (LR_p2[target].x > LR_p1[target].x && LR_p2[target].y > LR_p1[target].y)
			{
				LR_Image = mColorImg.colRange(LR_p1[target].x, LR_p2[target].x + 1).rowRange(LR_p1[target].y, LR_p2[target].y + 1).clone();
			}
			else
			{
				OutputDebugString("你按了右鍵。拉框請從左上到右下，不要亂搞\n");
				return;
			}

			int one_iter_count = 0;
			for (int i = 0; i < LR_Image.rows; i++)
			{
				for (int j = 0; j < LR_Image.cols; j++)
				{

					int index1 = (j + LR_p1[target].x) + (i + LR_p1[target].y) * iWidthColor;

					if (pCSPoints[index1].Z != -1 * numeric_limits<float>::infinity() && one_iter_count < MAX_DEPTH_COUNT_POR / 2)
					{
						PORCameraSP[PORDepthCount].X = pCSPoints[index1].X;
						PORCameraSP[PORDepthCount].Y = pCSPoints[index1].Y;
						PORCameraSP[PORDepthCount].Z = -pCSPoints[index1].Z;

						PORCenterCameraS.x += PORCameraSP[PORDepthCount].X;
						PORCenterCameraS.y += PORCameraSP[PORDepthCount].Y;
						PORCenterCameraS.z += PORCameraSP[PORDepthCount].Z;
						

						//cout << PORCameraSP[PORDepthCount].Z << endl;

						PORDepthCount++;

						one_iter_count++;
						
					}

				}
			}

			//cout << PORDepthCount << endl;

		}


		if (PORDepthCount > 0)
		{
			PORCenterCameraS.x = PORCenterCameraS.x / PORDepthCount;
			PORCenterCameraS.y = PORCenterCameraS.y / PORDepthCount;
			PORCenterCameraS.z = PORCenterCameraS.z / PORDepthCount;

			if (!IS_PlaceObject)
				PlaceObject();

		}
		else if (PORDepthCount == 0)
		{
			PORCenterCameraS.x = 0;
			PORCenterCameraS.y = 0;
			PORCenterCameraS.z = 0;
		}


		//for (int i = PORDepthCount - 1; i > 0; --i)
		//{
		//	for (int j = 0; j < i; ++j)
		//	{
		//		if (PORCameraSP[j].Z < PORCameraSP[j + 1].Z)
		//		{
		//			CameraSpacePoint temp;
		//
		//			temp.X = PORCameraSP[j].X;
		//			temp.Y = PORCameraSP[j].Y;
		//			temp.Z = PORCameraSP[j].Z;
		//
		//			PORCameraSP[j].X = PORCameraSP[j + 1].X;
		//			PORCameraSP[j].Y = PORCameraSP[j + 1].Y;
		//			PORCameraSP[j].Z = PORCameraSP[j + 1].Z;
		//
		//			PORCameraSP[j + 1].X = temp.X;
		//			PORCameraSP[j + 1].Y = temp.Y;
		//			PORCameraSP[j + 1].Z = temp.Z;
		//		}
		//	}
		//}
		//
		////Settint threshold and filtering
		//int IndexLim = 0;
		//
		//for (int i = 0; i < PORDepthCount; i++)
		//{
		//	if (PORCameraSP[i].Z < PORCameraSP[0].Z*1.3)
		//	{
		//		IndexLim = i;
		//		break;
		//	}
		//}
		//
		////Mismatch occur
		//if (IndexLim != 0)
		//{
		//	PORDepthCount = IndexLim;
		//
		//	cout << "Yeeee" << endl;
		//}
	}

	


}
void PlaceObject()
{
	if (PORDepthCount > 0 && LRDepthCount[1] > 0)
	{
		FindPlaneOfRegression(PORCameraSP, PORDepthCount, PORNormal);

		//cout << endl << "POR normal: " << PORNormal[0] << " " << PORNormal[1] << " " << PORNormal[2] << " " << PORNormal[3] << endl;

		//讓兩條線的點投影到平面上 增加精準度
		PathGenProjToPlane(LRCameraSP[0], LRDepthCount[0], PORNormal, LRCameraSP[0]);
		PathGenProjToPlane(LRCameraSP[1], LRDepthCount[1], PORNormal, LRCameraSP[1]);

		//dot(Y-axis-GL, PORNormal)
		float theta = acos(PORNormal[1] / sqrt(pow(PORNormal[0], 2) + pow(PORNormal[1], 2) + pow(PORNormal[2], 2)));

		//cross(Y-axis-GL, PORNormal)
		float ArbRotAxis[3] = { -PORNormal[2], 0, PORNormal[0] };



		for (int i = 0; i < 16; i++)
		{
			M_Cubic[i] = 0;
		}
		M_Cubic[0] = M_Cubic[5] = M_Cubic[10] = M_Cubic[15] = 1;


		RotationAboutArbitraryAxes(theta, ArbRotAxis, M_Cubic);

		float TryRotaY[3] = { 0, 1, 0 }, Result[3] = { 0 };
		ROITrans(TryRotaY, 1, M_Cubic, Result);

		if (PORNormal[0] * Result[0] + PORNormal[1] * Result[1] + PORNormal[2] * Result[2] < 0)
		{
			theta = 3.14159265358979323846 - theta;
			for (int i = 0; i < 16; i++)
			{
				M_Cubic[i] = 0;
			}
			M_Cubic[0] = M_Cubic[5] = M_Cubic[10] = M_Cubic[15] = 1;

			RotationAboutArbitraryAxes(theta, ArbRotAxis, M_Cubic);
		}

		if (PORNormal[2] < 0)
		{
			theta = 3.14159265358979323846 + theta;
			for (int i = 0; i < 16; i++)
			{
				M_Cubic[i] = 0;
			}
			M_Cubic[0] = M_Cubic[5] = M_Cubic[10] = M_Cubic[15] = 1;

			RotationAboutArbitraryAxes(theta, ArbRotAxis, M_Cubic);
		}

		RotationAboutArbitraryAxes(-theta, ArbRotAxis, M_Cubic_inv);



		//M_Cubic[12] = M_Cubic[13] = M_Cubic[14] = 0;
		//M_Cubic[15] = 1;

		//M_Cubic_inv[12] = M_Cubic_inv[13] = M_Cubic_inv[14] = 0;
		//M_Cubic_inv[15] = 1;

		//GLfloat MM[16] = { 0 };
		//MM[0] = MM[5] = MM[10] = MM[15] = 1;
		//glPushMatrix();
		//glLoadIdentity();
		//glRotatef(theta, ArbRotAxis[0], ArbRotAxis[1], ArbRotAxis[2]);
		//glGetFloatv(GL_MODELVIEW_MATRIX, MM);
		//glPopMatrix();


		/* Test */
		//TryRotaY[0] = 0;
		//TryRotaY[1] = 1;
		//TryRotaY[2] = 0;
		//ROITrans(TryRotaY, 1, M_Cubic, Result);



		//轉到y平面再做計算((y值相同3311
		RotationAboutArbitraryAxes(theta, ArbRotAxis, LRCameraSP[0], LRDepthCount[0]);
		RotationAboutArbitraryAxes(theta, ArbRotAxis, LRCameraSP[1], LRDepthCount[1]);

		//for (int i = 0; i < LRDepthCount[0]; i++)
		//{
		//	cout << LRCameraSP[0][i].Y << endl;
		//}

		//double RegrL1[3] = { 2, 0, 0 };
		PathGenRegression(LRCameraSP[0], LRDepthCount[0], RegrL1);

		//double RegrL2[3] = { 2, 0, 0 };
		PathGenRegression(LRCameraSP[1], LRDepthCount[1], RegrL2);

		Intersect.X = (RegrL1[2] - RegrL2[2]) / (RegrL2[1] - RegrL1[1]);
		Intersect.Z = RegrL1[1] * Intersect.X + RegrL1[2];
		Intersect.Y = PORCenterCameraS.y;

		RotationAboutArbitraryAxes(-theta, ArbRotAxis, &Intersect, 1);


		theta2 = (acos(1 / sqrt(pow(1, 2) + pow(RegrL1[1], 2))) - 3.14159265358979323846) *180.0f / 3.14159265358979323846;
		//theta2 = 3.14159265358979323846 - acos(1 / sqrt(pow(1, 2) + pow(RegrL1[1], 2)));

		/* Test */
		//GLfloat TransM1[6] = { 0, theta2, 0, 0, 0, 0 };
		//CameraSpacePoint* TryRota2 = new CameraSpacePoint;
		//CameraSpacePoint* Result2 = new CameraSpacePoint;
		//TryRota2->X = 1;
		//TryRota2->Y = 0;
		//TryRota2->Z = 0;
		//PathGenCoordCubicToWorld(TryRota2, 1, TransM1, Result2);




		RotationAboutArbitraryAxes(-theta, ArbRotAxis, LRCameraSP[0], LRDepthCount[0]);
		RotationAboutArbitraryAxes(-theta, ArbRotAxis, LRCameraSP[1], LRDepthCount[1]);


		//for (int i = 0; i < 16; i++)
		//{
		//	M_Cubic[i] = 0;
		//}
		//M_Cubic[0] = M_Cubic[5] = M_Cubic[10] = M_Cubic[15] = 1;


		//RotationAboutArbitraryAxes(theta, ArbRotAxis, M_Cubic);
		//
		//float TryRotaY[3] = { 0, 1, 0 }, Result[3] = { 0 };
		//ROITrans(TryRotaY, 1, M_Cubic, Result);

		//if (PORNormal[0] * Result[0] + PORNormal[1] * Result[1] + PORNormal[2] * Result[2] < 0)
		//{
		//	theta = 3.14159265358979323846 - theta;
		//	for (int i = 0; i < 16; i++)
		//	{
		//		M_Cubic[i] = 0;
		//	}
		//	M_Cubic[0] = M_Cubic[5] = M_Cubic[10] = M_Cubic[15] = 1;

		//	RotationAboutArbitraryAxes(theta, ArbRotAxis, M_Cubic);
		//}

		//if (PORNormal[2] < 0)
		//{
		//	theta = 3.14159265358979323846 + theta;
		//	for (int i = 0; i < 16; i++)
		//	{
		//		M_Cubic[i] = 0;
		//	}
		//	M_Cubic[0] = M_Cubic[5] = M_Cubic[10] = M_Cubic[15] = 1;

		//	RotationAboutArbitraryAxes(theta, ArbRotAxis, M_Cubic);
		//}

		//ROITrans(TryRotaY, 1, M_Cubic, Result);


		IS_PlaceObject = TRUE;

	}



}


/*--------------------------------------------------*/
/*--------------OpenHaptics HL Function-------------*/
/*--------------------------------------------------*/
void initHL()
{
	HDErrorInfo error;

	hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		fprintf(stderr, "Press any key to exit");
		getchar();
		exit(-1);
	}

	hHLRC = hlCreateContext(hHD);
	hlMakeCurrent(hHLRC);

	hlDisable(HL_USE_GL_MODELVIEW);

	//spring = hlGenEffects(1);
}


/*--------------------------------------------*/
/*--------------Load Obj Function-------------*/
/*--------------------------------------------*/
void loadOBJModel()
{
	// read an obj model here
	if (OBJ != NULL){
		free(OBJ);
	}
	OBJ = glmReadOBJ("box33/box5.obj");
	//OBJ = glmReadOBJ("box.obj");

	//printf("%s\n", filename);

	// traverse the color model
	traverseModel();
	
	//GLuint a = OBJ->numvertices;
	//GLuint b = OBJ->numnormals;
	//GLuint c = OBJ->numgroups;
}

void traverseModel()
{
	GLMgroup* group = OBJ->groups;
	float maxx, maxy, maxz;
	float minx, miny, minz;
	float dx, dy, dz;

	maxx = minx = OBJ->vertices[3];
	maxy = miny = OBJ->vertices[4];
	maxz = minz = OBJ->vertices[5];

	for (unsigned int i = 2; i <= OBJ->numvertices; i++){
		GLfloat vx, vy, vz;
		vx = OBJ->vertices[i * 3 + 0];
		vy = OBJ->vertices[i * 3 + 1];
		vz = OBJ->vertices[i * 3 + 2];
		if (vx > maxx) maxx = vx;  if (vx < minx) minx = vx;
		if (vy > maxy) maxy = vy;  if (vy < miny) miny = vy;
		if (vz > maxz) maxz = vz;  if (vz < minz) minz = vz;
	}

	//printf("max\n%f %f, %f %f, %f %f\n", maxx, minx, maxy, miny, maxz, minz);
	dx = maxx - minx;
	dy = maxy - miny;
	dz = maxz - minz;
	//printf("dx,dy,dz = %f %f %f\n", dx, dy, dz);
	GLfloat normalizationScale = myMax(myMax(dx, dy), dz) / 2;
	OBJ->position[0] = (maxx + minx) / 2;
	OBJ->position[1] = (maxy + miny) / 2;
	OBJ->position[2] = (maxz + minz) / 2;

	int gCount = 0;
	while (group){
		//printf("gCount: %i \n", gCount);
		for (unsigned int i = 0; i<group->numtriangles; i++){

			//printf("numtriangles: %i \n", i);

			// triangle index
			int triangleID = group->triangles[i];

			//printf("triangle index: %i \n", triangleID);

			// the index of each vertex
			int indv1 = OBJ->triangles[triangleID].vindices[0];
			int indv2 = OBJ->triangles[triangleID].vindices[1];
			int indv3 = OBJ->triangles[triangleID].vindices[2];

			// vertices
			GLfloat vx, vy, vz;

			//double scale = 0.01;
			double scale = 1;

			vx = OBJ->vertices[indv1 * 3];
			vy = OBJ->vertices[indv1 * 3 + 1];
			vz = OBJ->vertices[indv1 * 3 + 2];
			//printf("vertices1 %f %f %f\n", vx, vy, vz);
			vertices[gCount][i * 9 + 0] = vx * scale;
			vertices[gCount][i * 9 + 1] = vy * scale;
			vertices[gCount][i * 9 + 2] = vz * scale;

			vx = OBJ->vertices[indv2 * 3];
			vy = OBJ->vertices[indv2 * 3 + 1];
			vz = OBJ->vertices[indv2 * 3 + 2];
			//printf("vertices2 %f %f %f\n", vx, vy, vz);
			vertices[gCount][i * 9 + 3] = vx * scale;
			vertices[gCount][i * 9 + 4] = vy * scale;
			vertices[gCount][i * 9 + 5] = vz * scale;

			vx = OBJ->vertices[indv3 * 3];
			vy = OBJ->vertices[indv3 * 3 + 1];
			vz = OBJ->vertices[indv3 * 3 + 2];
			//printf("vertices3 %f %f %f\n", vx, vy, vz); 
			vertices[gCount][i * 9 + 6] = vx * scale;
			vertices[gCount][i * 9 + 7] = vy * scale;
			vertices[gCount][i * 9 + 8] = vz * scale;
			//printf("\n");
			
			//the index of each normals
			int indn1 = OBJ->triangles[triangleID].nindices[0];
			int indn2 = OBJ->triangles[triangleID].nindices[1];
			int indn3 = OBJ->triangles[triangleID].nindices[2];
			
			// the vertex normal
			normals[gCount][i * 9 + 0] = OBJ->normals[indn1 * 3 + 0];
			normals[gCount][i * 9 + 1] = OBJ->normals[indn1 * 3 + 1];
			normals[gCount][i * 9 + 2] = OBJ->normals[indn1 * 3 + 2];
			normals[gCount][i * 9 + 3] = OBJ->normals[indn2 * 3 + 0];
			normals[gCount][i * 9 + 4] = OBJ->normals[indn2 * 3 + 1];
			normals[gCount][i * 9 + 5] = OBJ->normals[indn2 * 3 + 2];
			normals[gCount][i * 9 + 6] = OBJ->normals[indn3 * 3 + 0];
			normals[gCount][i * 9 + 7] = OBJ->normals[indn3 * 3 + 1];
			normals[gCount][i * 9 + 8] = OBJ->normals[indn3 * 3 + 2];
	

			int indt1 = OBJ->triangles[triangleID].tindices[0];
			int indt2 = OBJ->triangles[triangleID].tindices[1];
			int indt3 = OBJ->triangles[triangleID].tindices[2];

			vtextures[gCount][i * 6 + 0] = OBJ->texcoords[indt1 * 2 + 0];
			vtextures[gCount][i * 6 + 1] = OBJ->texcoords[indt1 * 2 + 1];
			vtextures[gCount][i * 6 + 2] = OBJ->texcoords[indt2 * 2 + 0];
			vtextures[gCount][i * 6 + 3] = OBJ->texcoords[indt2 * 2 + 1];
			vtextures[gCount][i * 6 + 4] = OBJ->texcoords[indt3 * 2 + 0];
			vtextures[gCount][i * 6 + 5] = OBJ->texcoords[indt3 * 2 + 1];

			//printf("vtextures: %f %f %f\n", vtextures[gCount][i * 6 + 0], vtextures[gCount][i * 6 + 1], vtextures[gCount][i * 6 + 2]);

			// TODO: texture coordinates should be aligned by yourself
			//OBJ->texcoords[indt1*2];
			//OBJ->texcoords[indt1*2+1];
			//OBJ->texcoords[indt2*2];
			//OBJ->texcoords[indt2*2+1];
			//OBJ->texcoords[indt3*2];
			//OBJ->texcoords[indt3*2+1];
		}
		group = group->next;
		gCount++;

		//vertices;

	}

	//GLMgroup* group2 = OBJ->groups;
	//int index = 5;
	//cout << OBJ->triangles[index].nindices[0] << " " << OBJ->triangles[index].nindices[1] << " " << OBJ->triangles[index].nindices[2] << endl;
	//cout << OBJ->normals[OBJ->triangles[index].nindices[0] * 3 + 0] << "  " << OBJ->normals[OBJ->triangles[index].nindices[0] * 3 + 1] << " " << OBJ->normals[OBJ->triangles[index].nindices[0] * 3 + 2] << endl;
	//cout << OBJ->normals[OBJ->triangles[index].nindices[1] * 3 + 0] << "  " << OBJ->normals[OBJ->triangles[index].nindices[1] * 3 + 1] << " " << OBJ->normals[OBJ->triangles[index].nindices[1] * 3 + 2] << endl;
	//cout << OBJ->normals[OBJ->triangles[index].nindices[2] * 3 + 0] << "  " << OBJ->normals[OBJ->triangles[index].nindices[2] * 3 + 1] << " " << OBJ->normals[OBJ->triangles[index].nindices[2] * 3 + 2] << endl;


	//system("pause");
	// custom normalization matrix for each loaded model
	//normalization_matrix.identity();
	//normalization_matrix = normalization_matrix * myTranslateMatrix(-OBJ->position[0], -OBJ->position[1], -OBJ->position[2]);
	//normalization_matrix = normalization_matrix * myScaleMatrix(1 / normalizationScale, 1 / normalizationScale, 1 / normalizationScale);

//	// ring model related
//	int i = 0;
//	GLMgroup* group = OBJ->groups;
//	while (group) {
//		printf("group %s 's numtriangles = %d\n", group->name, group->numtriangles);
//		if (group->numtriangles > 0){
//			// 紀錄每個group有多少三角片
//			numtriangles[i] = group->numtriangles;
//			i++;
//		}
//		group = group->next;
//	}
//
//	// memory allocate
//	model_vertex = (GLfloat *)malloc(9 * OBJ->numtriangles * sizeof(GLfloat));
//	model_texture = (GLfloat *)malloc(6 * OBJ->numtriangles * sizeof(GLfloat));
//	model_color = (GLfloat *)malloc(12 * OBJ->numtriangles * sizeof(GLfloat));
//	model_vertexNew = (GLfloat *)malloc(9 * OBJ->numtriangles * sizeof(GLfloat));
//
//	// number of triangles
//	OBJ->numtriangles;
//
//	// number of vertices
//	OBJ->numvertices;
//
//	// The center position of the model 
//	OBJ->position[0] = 0;
//	OBJ->position[1] = 0;
//	OBJ->position[2] = 0;
//
//	for (i = 0; i<(int)OBJ->numtriangles; i++)
//	{
//		// the index of each vertex
//		int indv1 = OBJ->triangles[i].vindices[0];
//		int indv2 = OBJ->triangles[i].vindices[1];
//		int indv3 = OBJ->triangles[i].vindices[2];
//
//		// the index of each color
//		int indc1 = indv1;
//		int indc2 = indv2;
//		int indc3 = indv3;
//
//		// vertices
//		model_vertex[9 * i + 0] = OBJ->vertices[indv1 * 3 + 0];
//		model_vertex[9 * i + 1] = OBJ->vertices[indv1 * 3 + 1];
//		model_vertex[9 * i + 2] = OBJ->vertices[indv1 * 3 + 2];
//		model_vertex[9 * i + 3] = OBJ->vertices[indv2 * 3 + 0];
//		model_vertex[9 * i + 4] = OBJ->vertices[indv2 * 3 + 1];
//		model_vertex[9 * i + 5] = OBJ->vertices[indv2 * 3 + 2];
//		model_vertex[9 * i + 6] = OBJ->vertices[indv3 * 3 + 0];
//		model_vertex[9 * i + 7] = OBJ->vertices[indv3 * 3 + 1];
//		model_vertex[9 * i + 8] = OBJ->vertices[indv3 * 3 + 2];
//
//		// texture
//		model_texture[6 * i + 0] = OBJ->texcoords[indv1 * 2 + 0];
//		model_texture[6 * i + 1] = OBJ->texcoords[indv1 * 2 + 1];
//		model_texture[6 * i + 2] = OBJ->texcoords[indv2 * 2 + 0];
//		model_texture[6 * i + 3] = OBJ->texcoords[indv2 * 2 + 1];
//		model_texture[6 * i + 4] = OBJ->texcoords[indv3 * 2 + 0];
//		model_texture[6 * i + 5] = OBJ->texcoords[indv3 * 2 + 1];
//		// colors 
//		model_color[12 * i + 0] = 0;
//		model_color[12 * i + 1] = 0;
//		model_color[12 * i + 2] = 0;
//		model_color[12 * i + 3] = 1;
//		model_color[12 * i + 4] = 0;
//		model_color[12 * i + 5] = 0;
//		model_color[12 * i + 6] = 0;
//		model_color[12 * i + 7] = 1;
//		model_color[12 * i + 8] = 0;
//		model_color[12 * i + 9] = 0;
//		model_color[12 * i + 10] = 0;
//		model_color[12 * i + 11] = 1;
//	}
}

void SetTexObj(char *filename, int ii)
{
	glBindTexture(GL_TEXTURE_2D, /*textures[ii]*/ii);
	
	IplImage *imageCV; // 影像的資料結構
	imageCV = cvLoadImage(filename, CV_LOAD_IMAGE_UNCHANGED); // 讀取影像的

	//cvShowImage("HelloWorld", imageCV);
	//cvWaitKey(0); // 停留視窗

	char *imageGL;

	//imageGL = (unsigned char *)malloc(imageCV->nChannels * imageCV->height*imageCV->width *sizeof(unsigned char *));
	imageGL = new char[3 * imageCV->height*imageCV->width];


	for (int i = 0; i<imageCV->height; i++)
	{
		for (int j = 0; j<imageCV->widthStep; j = j + 3)
		{

			imageGL[(imageCV->height - 1 - i)*imageCV->widthStep + j + 0] = imageCV->imageData[i*imageCV->widthStep + j + 2];
			imageGL[(imageCV->height - 1 - i)*imageCV->widthStep + j + 1] = imageCV->imageData[i*imageCV->widthStep + j + 1];
			imageGL[(imageCV->height - 1 - i)*imageCV->widthStep + j + 2] = imageCV->imageData[i*imageCV->widthStep + j + 0];

			//imageGL[i*imageCV->widthStep + j + 0] = imageCV->imageData[i*imageCV->widthStep + j + 2];
			//imageGL[i*imageCV->widthStep + j + 1] = imageCV->imageData[i*imageCV->widthStep + j + 1];
			//imageGL[i*imageCV->widthStep + j + 2] = imageCV->imageData[i*imageCV->widthStep + j + 0];

			//cout << (int)(imageGL[i*imageCV->widthStep + j + 0]) << " " << (int)(imageGL[i*imageCV->widthStep + j + 1]) << " " << (int)(imageGL[i*imageCV->widthStep + j + 2]) << endl;
			//cout << endl << endl;

		}

		//cout << endl << endl;
	}

	//cout << (int)(imageGL[0]) << " " << (int)(imageGL[1]) << " " << (int)(imageGL[2]) << " " << endl;
	

	//材質控制
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageCV->width, imageCV->height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageGL);

	delete [] imageGL;

	//cvDestroyWindow("HelloWorld"); // 銷毀視窗
	cvReleaseImage(&imageCV); // 釋放IplImage資料結構

	//使用多材質
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);*/
}

void Texture()
{
	//glEnable(GL_TEXTURE_2D);
	glGenTextures(6, textures);
	//glPrioritizeTextures(4, textures, priorities); //1最重要 0最不重要 會先被踢掉
	SetTexObj("box33/box5/Mosaic_Hexagonal_Tile.jpg", textures[0]);
	SetTexObj("box33/box5/Metal_Steel_Textured_White.jpg", textures[1]);
	SetTexObj("box33/box5/Wood_Floor_Light.jpg", textures[2]);
	SetTexObj("box33/box5/Stone_Marble.jpg", textures[3]);
	SetTexObj("box33/box5/Stone_Brushed_Khaki.jpg", textures[4]);
	SetTexObj("box33/box5/Wood_Floor_Dark.jpg", textures[5]);
}


/*---------------------------------------------------*/
/*----------------AR Function Declare----------------*/
/*---------------------------------------------------*/
void ARFunc_FindProj()
{
	/*-----------------------------------*/
	/*改成OBJ頂面名面方程式只要找一次就好*/
	/*-----------------------------------*/

	CameraSpacePoint* CubeTop = new CameraSpacePoint[4];
	
	//塞入OBJ檔頂面的四個點
	CubeTop[0].X = OBJ->vertices[OBJ->triangles[10].vindices[0] * 3 + 0];
	CubeTop[0].Y = OBJ->vertices[OBJ->triangles[10].vindices[0] * 3 + 1];
	CubeTop[0].Z = OBJ->vertices[OBJ->triangles[10].vindices[0] * 3 + 2];

	CubeTop[1].X = OBJ->vertices[OBJ->triangles[10].vindices[1] * 3 + 0];
	CubeTop[1].Y = OBJ->vertices[OBJ->triangles[10].vindices[1] * 3 + 1];
	CubeTop[1].Z = OBJ->vertices[OBJ->triangles[10].vindices[1] * 3 + 2];

	CubeTop[2].X = OBJ->vertices[OBJ->triangles[10].vindices[2] * 3 + 0];
	CubeTop[2].Y = OBJ->vertices[OBJ->triangles[10].vindices[2] * 3 + 1];
	CubeTop[2].Z = OBJ->vertices[OBJ->triangles[10].vindices[2] * 3 + 2];

	CubeTop[3].X = OBJ->vertices[OBJ->triangles[11].vindices[2] * 3 + 0];
	CubeTop[3].Y = OBJ->vertices[OBJ->triangles[11].vindices[2] * 3 + 1];
	CubeTop[3].Z = OBJ->vertices[OBJ->triangles[11].vindices[2] * 3 + 2];

	CameraSpacePoint* CubeNorm = new CameraSpacePoint[2];

	//頂面其中一個三角片的法向量
	CubeNorm[0].X = OBJ->normals[OBJ->triangles[11].nindices[0] * 3 + 0];
	CubeNorm[0].Y = OBJ->normals[OBJ->triangles[11].nindices[0] * 3 + 1];
	CubeNorm[0].Z = OBJ->normals[OBJ->triangles[11].nindices[0] * 3 + 2];

	//起點與終點
	CubeNorm[1].X = 0;
	CubeNorm[1].Y = 0;
	CubeNorm[1].Z = 0;

	GLfloat TransM_AR[16] = { 0 };
	TransM_AR[0] = TransM_AR[5] = TransM_AR[10] = TransM_AR[15] = 1;

	glPushMatrix();

	glLoadIdentity();
	glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
	glMultMatrixf(M_Cubic);
	glRotatef(theta2, 0, 1, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, TransM_AR);

	glPopMatrix();


	ROITrans(CubeNorm, 2, TransM_AR, CubeNorm);
	ROITrans(CubeTop, 4, TransM_AR, CubeTop);

	//求解OBJ檔頂面之平面方程式
	ARFuncNormal[0] = CubeNorm[0].X - CubeNorm[1].X;
	ARFuncNormal[1] = CubeNorm[0].Y - CubeNorm[1].Y;
	ARFuncNormal[2] = CubeNorm[0].Z - CubeNorm[1].Z;
	ARFuncNormal[3] = -(ARFuncNormal[0] * CubeTop[2].X + ARFuncNormal[1] * CubeTop[2].Y + ARFuncNormal[2] * CubeTop[2].Z);

		//IS_AR_PLANE_FUNC_FIND = TRUE;


	//求解ROI中心點到OBJ檔頂面之投影
	CameraSpacePoint* ROICenter = new CameraSpacePoint;
	ARFunc_ROICSP_Proj = new CameraSpacePoint;

	ROICenter->X = ROICenterCameraS.x;
	ROICenter->Y = ROICenterCameraS.y;
	ROICenter->Z = ROICenterCameraS.z;

	PathGenProjToPlane(ROICenter, 1, ARFuncNormal, ARFunc_ROICSP_Proj);

	if (ARFunc_InsideTriCheck(ARFunc_ROICSP_Proj, &CubeTop[0], &CubeTop[1], &CubeTop[2]))
	{
		IS_ARFunc_InsideTriCheck = TRUE;
	}
	else if (ARFunc_InsideTriCheck(ARFunc_ROICSP_Proj, &CubeTop[1], &CubeTop[0], &CubeTop[3]))
	{
		IS_ARFunc_InsideTriCheck = TRUE;
	}
	else
	{
		IS_ARFunc_InsideTriCheck = FALSE;
	}

}

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

/*------------------------------------*/
/*--------------main loop-------------*/
/*------------------------------------*/
int main(int argc, char** argv)
{
	KineticInit();
	GLInit();
	initHL();
	InitPortAudio();
	MtInit();


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(iWidthColor, iHeightColor);
	glutCreateWindow("Perspective Projection");
	glutFullScreen();
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(KineticUpdate);
	
	Texture();
	loadOBJModel();
	
	glutTimerFunc(1000, timer, 0);
	//glutTimerFunc(125, timer, 1);

	BuildPopupMenu();


	//typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	//PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	//wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	//wglSwapIntervalEXT(1);

	glutMainLoop();

	if (IS_PORTAUDIO_INIT)
	{
		sine.close();
	}

	cout << "Close I/O = " << MtClose() << endl;
	cout << "End process" << endl;

	return 0;
}  