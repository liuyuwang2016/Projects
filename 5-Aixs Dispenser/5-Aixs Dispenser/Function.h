#include "src/Declae.h"

#pragma region Init
void init()
{
	KinectInit();     //Kinect
	GLInit();         //OpenGL
	InitPortAudio();  //audio
	MtInit();         //dispenser machine
}
#pragma endregion Init

#pragma region SpecialKeys
/*Rotate and translate in special key can move virtual objects by hand */
void SpecialKeys(int key, int x, int y)
{
	//��Ҫ��ʱ��Ż����һ��
	//Press F1 to store pt
	if (key == GLUT_KEY_F1 && !ROI_IS_COLLIDE && ARFunc_InsideTriCheck)
	{
		ROICameraSPStorage();
	}
	//Press F4 to enable or disable transparent mode ��F4ת��͸����
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
	}
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
	else if (key == GLUT_KEY_DOWN )
	{
		ObjPosi.y += 0.001f;
		cout << endl << "ObjPosi.y : " << ObjPosi.y << endl;
	}
	else if (key == GLUT_KEY_UP)
	{
		ObjPosi.y -= 0.001f;
		cout << endl << "ObjPosi.y : " << ObjPosi.y << endl;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		ObjPosi.x += 0.001f;
		cout << endl << "ObjPosi.x : " << ObjPosi.x << endl;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		ObjPosi.x -= 0.001f;
		cout << endl << "ObjPosi.x : " << ObjPosi.x << endl;
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
		//cout << "ץȡ�������ص��� " << ROIDepthCount << "��~" << endl;
		
		glEnd();
		glPopMatrix();
		//�����������ץȡ���ıʼ���λ�õ��������λ�õľ��룬��λ���ס�
		/*
		 * �˴���������ɫ׷�ٵ����ⶨ�ͷ�Χ�ⶨ���������ɫ׷�����ⶨ�ͷ�Χ�ĵ�
		 */
		float sum_X, sum_Y, sum_Z;
		for (int i = 0; i < ROIDepthCount; i++)
		{
			sum_X += ROICameraSP[i].X;
			sum_Y += ROICameraSP[i].Y;
			sum_Z += ROICameraSP[i].Z;
		}
		sum_X = sum_X / ROIDepthCount;
		sum_Y = sum_Y / ROIDepthCount;
		sum_Z = sum_Z / ROIDepthCount;
		float distance = sqrt(abs(sum_X)*abs(sum_X) + abs(sum_Y)*abs(sum_Y) + abs(sum_Z)*abs(sum_Z));
		//cout << ROIDepthCount << ", " << distance * 100  << endl;
	}
}
/*
 * #1 �տ�ʼ���ɹ���ԭ�����������ʱ��ץȡƽ��ķ���ʽ��kinectUpdate�У����ǻ�����renderscene���У������Ҫ�Ȱѵ��������Ĵ�����
 * #2 �����ﻭ��ɹ�������frame��FPSֻ��3������Ҫ��취�ѻ��ܶ���������С��Ϊ��һ��ƽ�棬Ӧ�ÿ��Լ�С��������2018-3-15
 * #3 ������ץȡ����ƽ������ߣ���Ϊ���ƽ������в�ͬ�ĺ�������ʽ������ʶ�������Ĳ�ͬ���к����Ĵ���ƽ��Ļ�Ӧ��ֻ��Ҫ��һ�Σ����ڷ�������ģ�ͣ�
 *    �����ߵĻ�ץȡ��֮����Ҫֱ������ָ���㽺�����ƶ�������ط���Ҫ˼��
 */

void DrawPlaneInScene()
{
	if (PlaneDepthCount != 0)
	{
		glPushMatrix();
		glPointSize(2.0f);
		glBegin(GL_POINTS);
		for (int i = 0; i < PlaneDepthCount; i++)
		{
			glColor3ub(255, 255, 0);
			glVertex3f(PlaneSP[i].X, PlaneSP[i].Y, PlaneSP[i].Z);
			//cout << "��������" << index4 << "��" << endl;
		}
		//RANSAC(temp[index4].X,temp[index4].Y,temp[index4].Z);
		glEnd();
		glPopMatrix();
	}

	GLfloat TransGLtoMach[16] = { 0 };
	TransGLtoMach[0] = TransGLtoMach[5] = TransGLtoMach[10] = TransGLtoMach[15] = 1;
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(TipPosi.x, TipPosi.y, TipPosi.z);
	//http://cuiqingcai.com/1658.html ��תʾ��
	glRotatef(-90, 1, 0, 0);//��ת���Zֵ�ͻ���������ϵ�෴
	//glMultMatrixf(const GLfloat *m);//��mָ����16��ֵ��Ϊһ�������뵱ǰ������ˣ����ѽ���洢�ڵ�ǰ������ 
	//glMultMatrix ���赱ǰ������C��ô�þ���M ����glMultMatrix �Զ���v�ı任�ʹ�ԭ����C*v���C * M * v
	//http://blog.csdn.net/mathgeophysics/article/details/11434345
	glMultMatrixf(M_Cubic_inv);//opengl����ת��������
	//glTranslatef(-Intersect.X, -Intersect.Y, -Intersect.Z);
	glGetFloatv(GL_MODELVIEW_MATRIX, TransGLtoMach);
	glPopMatrix();

}

void RANSAC(CameraSpacePoint* spacePoints, int Iter)
{
	//http://blog.csdn.net/fengdingha/article/details/22285147
	//RANSAC code

}
/*
*
* ��Ҫ�Ľ��Ĳ��֣���ײ���
*
*
*/
void CollisionDetection()
{
	/*--------------------��ײ���-------------------*/
	/*--------------Collision Detection-------------*/
	if (ROIDepthCount != 0 && ARFunc_IS_ON)
	{
		GLfloat TransM[16] = { 0 };
		TransM[0] = TransM[5] = TransM[10] = TransM[15] = 1;
		glPushMatrix();
		glLoadIdentity();
		//�ƶ���ԭ��,ӫĻ���ľ���ԭ��
		glMultMatrixf(M_Cubic_inv);
		//��objģ�͵������ƶ���opengl����ԭ��
		glTranslatef(-Intersect.X, -Intersect.Y, -Intersect.Z);
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
		//��ץ���ĵ��Kinect������ϵ�ϱ任��objģ�͵�����ϵ��
		ROITrans(ROICameraSP, ROIDepthCount, TransM, ROICameraSP_TouchDetec);

		int CollideCount = 0;
		int CollideCount_Circle = 0;

		for (int ROICount = 0; ROICount < ROIDepthCount; ROICount++)
		{
			GLMgroup* group = OBJ->groups;
			int gCount = 0;
			//�������е�group�����������
			while (group)
			{
				for (int TriCount = 0; TriCount < group->numtriangles; TriCount++)
				{
					//vertices[gCount][Tricount*9+0]ָ���ǵ�gCount��������εĵ�tricount�������Xֵ,���xֵΪ�����ģ������ϵ��ֵ
					float vx = ROICameraSP_TouchDetec[ROICount].X - vertices[gCount][TriCount * 9 + 0];
					float vy = ROICameraSP_TouchDetec[ROICount].Y - vertices[gCount][TriCount * 9 + 1];
					float vz = ROICameraSP_TouchDetec[ROICount].Z - vertices[gCount][TriCount * 9 + 2];
					//����Ϊ�����ķ�������x,y,z��ֵ
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
							//����� case3 ����˼�� gCount ���� 3,Ҳ����˵ԭ�������еĵ�����group��ԲȦ��
						case 3:
							/*the circle of the obj*/
							CollideCount_Circle++;
							break;
							//���������case����Ϊ��
						default:
							CollideCount++;//����46
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
			//�����ﱻд���ˣ���collidecount�ﵽ�����ֵ��ʱ���˵��ȷʵ��ײ��
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
	/*find depth points and do not collide, call ARFunc_FindProj()��
	*���û�з�����ײ����ô��ִ��AR��ʽ*/
	if (ROIDepthCount > 0 && !ROI_IS_COLLIDE)
	{
		/*use this to get the projection point����������ҵ�ͶӰ��*/
		ARFunc_FindProj();
		//ȷ���������ڲ���AR���ܿ�����ʱ������ִ��
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
//��opengl������ϵת��������������ϵ
void CoordGLtoMachine()
{
	/*--------------Transfer Prob Tip from GL Coord. to Machine Coord.-------------*/
	if (ROIDepthCount > 0 && ROICameraSP != nullptr && !ROI_IS_COLLIDE)
	{
		GLfloat TransM_toMechCoord[16] = { 0 };
		TransM_toMechCoord[0] = TransM_toMechCoord[5] = TransM_toMechCoord[10] = TransM_toMechCoord[15] = 1;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		glTranslatef(TipPosi.x, TipPosi.y, TipPosi.z);
		//http://cuiqingcai.com/1658.html ��תʾ��
		
		glRotatef(-90, 1, 0, 0);//��ת���Zֵ�ͻ���������ϵ�෴
		
		//glMultMatrixf(const GLfloat *m);//��mָ����16��ֵ��Ϊһ�������뵱ǰ������ˣ����ѽ���洢�ڵ�ǰ������ 
		//glMultMatrix ���赱ǰ������C��ô�þ���M ����glMultMatrix �Զ���v�ı任�ʹ�ԭ����C*v���C * M * v
		//http://blog.csdn.net/mathgeophysics/article/details/11434345
		glMultMatrixf(M_Cubic_inv);//opengl����ת��������
		
	    glTranslatef(-Intersect.X, -Intersect.Y, -Intersect.Z);
		
		glGetFloatv(GL_MODELVIEW_MATRIX, TransM_toMechCoord);
		glPopMatrix();
		/*printf("ת������\n");
		printf("m[0]:% 7.5f m[4]:% 7.5f m[8] :% 7.5f m[12]:% 7.5f\n", TransM_toMechCoord[0], TransM_toMechCoord[4], TransM_toMechCoord[8], TransM_toMechCoord[12]);
		printf("m[1]:% 7.5f m[5]:% 7.5f m[9] :% 7.5f m[13]:% 7.5f\n", TransM_toMechCoord[1], TransM_toMechCoord[5], TransM_toMechCoord[9], TransM_toMechCoord[13]);
		printf("m[2]:% 7.5f m[6]:% 7.5f m[10]:% 7.5f m[14]:% 7.5f\n", TransM_toMechCoord[2], TransM_toMechCoord[6], TransM_toMechCoord[10], TransM_toMechCoord[14]);
		printf("m[3]:% 7.5f m[7]:% 7.5f m[11]:% 7.5f m[16]:% 7.5f\n", TransM_toMechCoord[3], TransM_toMechCoord[7], TransM_toMechCoord[11], TransM_toMechCoord[15]);*/
		
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
		/*normal GL coordinate�����ﻹ��opengl��׼ȷ����ϵ*/
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

void MoveCubicInY_Axis()//����ģ��֮����Ӱ��
{
	/*--------------cubic movement deviation--------------*/
	/*--------------Calculate deviation & Draw Cubic ����ҲҪ�����ƶ��ľ���-------------*/
	CameraSpacePoint* Y_Devia = new CameraSpacePoint[2];
	Y_Devia[0].X = Y_Devia[0].Y = Y_Devia[0].Z = 0;
	Y_Devia[1].X = Y_Devia[1].Y = Y_Devia[1].Z = 0;
	//�洢�ĵ����0�����Ҽ�⵽cubic��Ҫ�ƶ����ڰ�A��ʱ��CUBIC_MOVE = TURE;
	if (ROIStorageCount > 0 && CUBIC_MOVE)
	{
		float dy = 0;
		switch (STORAGE_TYPE)
		{
			//���ݴ��淽ʽ�Ĳ�ͬ��ͶӰ����߲���ͶӰ�������о��ߡ����費ͬ��·��
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
		glTranslatef(Intersect.X, Intersect.Y, Intersect.Z);
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
	//ֱ��������cubic�����ƶ�
	glTranslatef(DeviaDueToY->X, DeviaDueToY->Y, DeviaDueToY->Z);

	//DrawCubic();
	//ARFunc_IS_ON = FALSE;
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

void RenderScene()
{

	RenderSceneBackground();
	DrawProbeTip();
	DrawPlaneInScene();
	CollisionDetection(); //��ײ�����Ҫ�Ľ�
	ARFuncPackage();
	CoordGLtoMachine();
	DrawStoragePoint();
	MoveCubicInY_Axis();

	/*--------------Show Prob Tip Coordinate on Screen-------------*/
	glPushMatrix();
	glColor3ub(0, 255, 0);
	/*glPrintf is used to draw the probe tip's coordinate position*/
	if (ROICameraSP_MechCoord != nullptr)
	{
		glPrintf(">>glut(%i, %i)", ROICenterColorS_Old.x - 60, ROICenterColorS_Old.y - 35);
		glPrintf("(%.3f, %.3f, %.3f)", ROICameraSP_MechCoord[0].X - 0.172, ROICameraSP_MechCoord[0].Y - 0.158, ROICameraSP_MechCoord[0].Z + 0.576);
	}
	glPopMatrix();

	ChangeLineColor();
	ROI_IS_COLLIDE = FALSE;
	if (Finish_Without_Update)
		glFinish();
	else
		glutSwapBuffers();
	FPS_RS++;
}
#pragma endregion RenderScene