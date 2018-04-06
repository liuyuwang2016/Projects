#include "widget.h"

//����3D���磬������������:
//����һ���У��㽫ѧ����μ���3D���磬����3D���������Ρ�
//��һ��ʹ�õ�һ�εĴ��룬��Ȼ�ڿγ�˵������ֻ���ܸı��˴��롣

//��һ������Lionel Brits (��telgeuse)��д�ġ��ڱ���������ֻ�����ӵĴ��������͡���Ȼֻ��ӿγ�����д�Ĵ��룬�����ǲ������еġ�
//���������Ȥ֪�������ÿһ�д�����������еĻ���������������Դ�룬���������һ�ε�ͬʱ����Դ����и��١�
//�������ڻ�ӭ���������������ĵ�ʮ�Ρ�������Ϊֹ����Ӧ������������һ����ת���������һȺ�����ˣ���3D���ҲӦ����Щ�о��˰ɣ�
//���������һ�£���Ҫ����嶯��Ҫ��ʼд��Quake
//IV���ò���...:)��ֻ����ת�������廹����������һ�����Ծ�һ��ս�Ŀ���˵Ķ���....:)��
//������Щ����������Ҫ����һ����һ��ġ�������Щ�ġ���̬3D���磬�����пռ�������ɶȺͻ��ڵ�Ч���羵����ڡ�Ť���ȵȣ�
//��Ȼ��Ҫ�и����֡��ʾ�ٶȡ���һ�ξ�Ҫ����һ��������3D����"�ṹ"���Լ������������������ߡ�
//���ݽṹ
//������Ҫʹ��һϵ�е������������ı��3D����ʱ�����Ż������Ӷȵ�����������������Ѷ�Ҳ����֮������
//�������ԭ�����Ǳ��뽫���ݹ���,ʹ����и���Ŀɲ����Է���ڳ����嵥ͷ��������sector(����)�Ķ��塣
//ÿ��3D��������Ͽ��Կ�����sector(����)�ļ��ϡ�һ��sector(����)������һ�����䡢һ�������塢��������һ���պϵ����䡣

const float piover180 = 0.0174532925f;

GLWidget::GLWidget(QWidget *parent) :
QGLWidget(parent), m_show_full_screen(false), m_yrot(0.0f),
m_xpos(0.0f), m_zpos(0.0f), m_heading(0.0f), m_walkbias(0.0f),
m_walkbiasangle(0.0f), m_lookupdown(0.0f), m_filter(0), m_blend(false)
{
	showNormal();
}

GLWidget::~GLWidget()
{
}

//����Ĵ������������������OpenGL�����Ĵ�С�������ܴ��ڵĴ�С�Ƿ��Ѿ��ı�(�ٶ���û��ʹ��ȫ��ģʽ)��
//�������޷��ı䴰�ڵĴ�Сʱ(��������ȫ��ģʽ��)���������Խ�����һ��--�ڳ���ʼʱ�������ǵ�͸��ͼ��
//OpenGL�����ĳߴ罫�����ó�����ʾʱ���ڴ��ڵĴ�С��
void GLWidget::resizeGL(int w, int h)
{
	if (h == 0)// ��ֹ�����
	{
		h = 1;// ������Ϊ1
	}
	glViewport(0, 0, w, h); //���õ�ǰ���ӿ�
	//���漸��Ϊ͸��ͼ������Ļ����ζ��ԽԶ�Ķ���������ԽС����ô��������һ����ʵ��۵ĳ�����
	//�˴�͸�Ӱ��ջ��ڴ��ڿ�Ⱥ͸߶ȵ�45���ӽ������㡣0.1f��100.0f�������ڳ��������ܻ�����ȵ������յ㡣
	//glMatrixMode(GL_PROJECTION)ָ�������������д��뽫Ӱ��projection matrix(ͶӰ����)��
	//ͶӰ������Ϊ���ǵĳ�������͸�ӡ� glLoadIdentity()���������á�������ѡ�ľ���״̬�ָ�����ԭʼ״̬��
	//����glLoadIdentity()֮������Ϊ��������͸��ͼ��
	//glMatrixMode(GL_MODELVIEW)ָ���κ��µı任����Ӱ�� modelview matrix(ģ�͹۲����)��
	//ģ�͹۲�����д�������ǵ�����ѶϢ�������������ģ�͹۲��������������������Щ����ĺ��壬����ż���
	//���Ժ�Ľ̳���һ����ҽ��͡�ֻҪ֪�����������һ�����ʵ�͸�ӳ����Ļ���������ô����
	glMatrixMode(GL_PROJECTION);// ѡ��ͶӰ����
	glLoadIdentity();// ����ͶӰ����
	//�����ӿڵĴ�С
	gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);	//ѡ��ģ�͹۲����
	glLoadIdentity(); // ����ģ�͹۲����
}

//���µĴ�����У����ǽ���OpenGL�������е����á����ǽ����������Ļ���õ���ɫ������Ȼ��棬����smooth shading(��Ӱƽ��)���ȵȡ�
//�������ֱ��OpenGL���ڴ���֮��Żᱻ���á��˹��̽��з���ֵ�������Ǵ˴��ĳ�ʼ��û��ô���ӣ����ڻ��ò��ŵ����������ֵ��
void GLWidget::initializeGL()
{
	loadTexture();
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Set The Blending Function For Translucency
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// This Will Clear The Background Color To Black
	glClearDepth(1.0);									// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);								// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	setupWorld();

	m_blend = !m_blend;
	if (!m_blend)
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
}

//��һ�ΰ��������еĻ�ͼ���롣�κ�����������Ļ����ʾ�Ķ��������ڴ˶δ����г��֡�
//�Ժ��ÿ���̳����Ҷ��������̵Ĵ˴������µĴ��롣�������OpenGL�Ѿ������˽�Ļ�����������glLoadIdentity()����֮��
//�������һЩOpenGL�����������������Ρ�
//�������OpenGL���֣������ҵ��¸��̡̳�Ŀǰ����������ȫ�����ǽ���Ļ���������ǰ������������ɫ�������Ȼ��沢�����ó�����
//������û�л����κζ�����
void GLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// ��� ���� �� ��Ȼ���
	glLoadIdentity();						// ���õ�ǰ����

	GLfloat x_m, y_m, z_m, u_m, v_m;				// �������ʱ X, Y, Z, U �� V ����ֵ
	GLfloat xtrans = -m_xpos;						// ������Ϸ����X��ƽ��ʱ�Ĵ�С
	GLfloat ztrans = -m_zpos;						// ������Ϸ����Z��ƽ��ʱ�Ĵ�С
	GLfloat ytrans = -m_walkbias - 0.25f;				// ����ͷ�������°ڶ�
	GLfloat sceneroty = 360.0f - m_yrot;				// λ����Ϸ�߷����360�Ƚ�
	int numtriangles;						// ��������������������
	glRotatef(m_lookupdown, 1.0f, 0, 0);					// ������ת
	glRotatef(sceneroty, 0, 1.0f, 0);					// ������Ϸ���������Է�����������ת
	glTranslatef(xtrans, ytrans, ztrans);				// ����Ϸ��Ϊ���ĵ�ƽ�Ƴ���
	glBindTexture(GL_TEXTURE_2D, m_texture[m_filter]);			// ���� filter ѡ�������
	numtriangles = m_sector1.numtriangles;				// ȡ��Sector1������������
	for (int loop_m = 0; loop_m < numtriangles; loop_m++)		// �������е�������
	{
		glBegin(GL_TRIANGLES);					// ��ʼ����������
		glNormal3f(0.0f, 0.0f, 1.0f);			// ָ��ǰ��ķ���
		x_m = m_sector1.triangle[loop_m].vertex[0].x;	// ��һ��� X ����
		y_m = m_sector1.triangle[loop_m].vertex[0].y;	// ��һ��� Y ����
		z_m = m_sector1.triangle[loop_m].vertex[0].z;	// ��һ��� Z ����
		u_m = m_sector1.triangle[loop_m].vertex[0].u;	// ��һ��� U  ��������
		v_m = m_sector1.triangle[loop_m].vertex[0].v;	// ��һ��� V  ��������

		glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);	// ������������Ͷ���
		x_m = m_sector1.triangle[loop_m].vertex[1].x;	// �ڶ���� X ����
		y_m = m_sector1.triangle[loop_m].vertex[1].y;	// �ڶ���� Y ����
		z_m = m_sector1.triangle[loop_m].vertex[1].z;	// �ڶ���� Z ����
		u_m = m_sector1.triangle[loop_m].vertex[1].u;	// �ڶ���� U  ��������
		v_m = m_sector1.triangle[loop_m].vertex[1].v;	// �ڶ���� V  ��������

		glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);	// ������������Ͷ���
		x_m = m_sector1.triangle[loop_m].vertex[2].x;	// ������� X ����
		y_m = m_sector1.triangle[loop_m].vertex[2].y;	// ������� Y ����
		z_m = m_sector1.triangle[loop_m].vertex[2].z;	// ������� Z ����
		u_m = m_sector1.triangle[loop_m].vertex[2].u;	// �ڶ���� U  ��������
		v_m = m_sector1.triangle[loop_m].vertex[2].v;	// �ڶ���� V  ��������
		glTexCoord2f(u_m, v_m); glVertex3f(x_m, y_m, z_m);	// ������������Ͷ���
		glEnd();						// �����λ��ƽ���
	}
}

//��ʾ����
//���������Ѿ������ڴ棬������һ��Ҫ����Ļ����ʾ������ĿǰΪֹ�������������Ķ���Щ�򵥵���ת��ƽ�ơ�
//�����ǵľ�ͷʼ��λ��ԭ��(0��0��0)�����κ�һ�������3D���涼�������û���������������ߺͱ��������ǵ����Ҳһ����
//ʵ��������ܵ�һ��;����ֱ���ƶ���ͷ�������Ծ�ͷΪ���ĵ�3D��������������������Ҳ����ô���ʵ�֡����ǵĽ���������£�
//�����û���ָ����ת���任��ͷλ�á�
//Χ��ԭ�㣬���뾵ͷ�෴����ת��������ת���硣(���˲�����ͷ��ת�Ĵ��)
//���뾵ͷƽ�Ʒ�ʽ�෴�ķ�ʽ��ƽ������(���˲�����ͷ�ƶ��Ĵ��)��

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_F2:
	{
					   m_show_full_screen = !m_show_full_screen;
					   if (m_show_full_screen)
					   {
						   showFullScreen();
					   }
					   else
					   {
						   showNormal();
					   }
					   updateGL();
					   break;
	}
	case Qt::Key_Escape:
	{
						   qApp->exit();
						   break;
	}
	case Qt::Key_PageUp:
	{
						   m_lookupdown -= 1.0f;
						   updateGL();
						   break;
	}
	case Qt::Key_PageDown:
	{
							 m_lookupdown += 1.0f;
							 updateGL();
							 break;
	}
	case Qt::Key_Right:
	{
						  m_heading -= 1.0f;
						  m_yrot = m_heading;							// ������ת����
						  updateGL();
						  break;
	}
	case Qt::Key_Left:
	{
						 m_heading += 1.0f;
						 m_yrot = m_heading;							// ���Ҳ���ת����
						 updateGL();
						 break;
	}
	case Qt::Key_Up:
	{
					   m_xpos -= (float)sin(m_heading*piover180) * 0.05f;			// ����Ϸ�����ڵ�Xƽ���ƶ�
					   m_zpos -= (float)cos(m_heading*piover180) * 0.05f;			// ����Ϸ�����ڵ�Zƽ���ƶ�
					   if (m_walkbiasangle >= 359.0f)					// ���walkbiasangle����359��
					   {
						   m_walkbiasangle = 0.0f;					// �� walkbiasangle ��Ϊ0
					   }
					   else								// ����
					   {
						   m_walkbiasangle += 10;					// ��� walkbiasangle < 359 �������� 10
					   }
					   m_walkbias = (float)sin(m_walkbiasangle * piover180) / 20.0f;		// ʹ��Ϸ�߲�����Ծ��
					   updateGL();
					   break;
	}
	case Qt::Key_Down:
	{
						 m_xpos += (float)sin(m_heading*piover180) * 0.05f;			// ����Ϸ�����ڵ�Xƽ���ƶ�
						 m_zpos += (float)cos(m_heading*piover180) * 0.05f;			// ����Ϸ�����ڵ�Zƽ���ƶ�
						 if (m_walkbiasangle <= 1.0f)					// ���walkbiasangleС��1��
						 {
							 m_walkbiasangle = 359.0f;					// ʹ walkbiasangle ���� 359
						 }
						 else								// ����
						 {
							 m_walkbiasangle -= 10;					// ��� walkbiasangle > 1 ��ȥ 10
						 }
						 m_walkbias = (float)sin(m_walkbiasangle * piover180) / 20.0f;		// ʹ��Ϸ�߲�����Ծ��
						 updateGL();
						 break;
	}
		//���ʵ�ֺܼ򵥡������ҷ�������º���ת����yrot
		//��Ӧ���ӻ���١���ǰ��������º�����ʹ��sine��cosine�����������ɾ�ͷλ��(����ҪЩ�����Ǻ���ѧ��֪ʶ:-)��Piover180
		//��һ���ܼ򵥵�����������������Ⱥͻ��ȡ�
		//���������ܻ��ʣ�walkbias��ʲô��˼������NeHe�ķ����ĵ���:-)�������Ͼ��ǵ�������ʱͷ���������°ڶ��ķ��ȡ�
		//����ʹ�ü򵥵�sine���Ҳ������ھ�ͷ��Y��λ�á��������������ֻ��ǰ���ƶ��Ļ�������������û��ô���ˡ�
	case Qt::Key_B:
	{
					  m_blend = !m_blend;
					  if (!m_blend)
					  {
						  glDisable(GL_BLEND);
						  glEnable(GL_DEPTH_TEST);
					  }
					  else
					  {
						  glEnable(GL_BLEND);
						  glDisable(GL_DEPTH_TEST);
					  }
					  updateGL();
					  break;
	}
	case Qt::Key_F:
	{
					  m_filter += 1;
					  if (m_filter > 2)
					  {
						  m_filter = 0;
					  }
					  updateGL();
	}
	}
}

//�����ļ�
//�ڳ����ڲ�ֱ�Ӵ洢���ݻ��ó����Ե�̫���������Ȥ���Ӵ����������������ϣ�������Ǵ�������ĵ��ԣ��������������鲻ͬ�����磬
//�����ñ������±��������һ���ô������û������л��������ϲ��޸����Ƕ�����֪��������ζ��������Щ���ϵġ�
//�����ļ�����������׼��ʹ���ı���ʽ�������༭���������ף�д�Ĵ���Ҳ���١��Ƚ�������Ҳ���ʹ�ö������ļ���
//�����ǣ��������ܴ��ļ���ȡ�����������أ����ȣ�����һ������SetupWorld()���º�����������ļ�����Ϊfilein������ʹ��ֻ����ʽ���ļ���
//���Ǳ�����ʹ�����֮��ر��ļ������һ�����������ڵĴ��룺
void GLWidget::setupWorld()
{
	QFile file(":/world/World.txt");
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, tr("Warning"), tr("Can't open world file."));
		return;
	}

	QTextStream stream(&file);
	//���Ƕ����ν��г�ʼ���������벿������
	QString oneline;							// �洢���ݵ��ַ���
	int numtriangles;							// ���ε�����������
	float x, y, z, u, v;							// 3D �� ��������

	readStr(&stream, oneline); // ����һ������
	sscanf(oneline.toLatin1().data(), "NUMPOLLIES %d\n", &numtriangles); // ��������������

	m_sector1.triangle = new TRIANGLE[numtriangles];				// Ϊnumtriangles�������η����ڴ沢�趨ָ��
	m_sector1.numtriangles = numtriangles;					// ��������1�е�����������
	// ���������е�ÿ��������
	for (int triloop = 0; triloop < numtriangles; triloop++)		// �������е�������
	{
		// ���������ε�ÿ������
		for (int vertloop = 0; vertloop < 3; vertloop++)		// �������еĶ���
		{
			readStr(&stream, oneline);				// ����һ������
			// ������ԵĶ�������
			sscanf(oneline.toLatin1().data(), "%f %f %f %f %f", &x, &y, &z, &u, &v);
			// ���������ݴ�����ԵĶ���
			m_sector1.triangle[triloop].vertex[vertloop].x = x;	// ���� 1,  �� triloop ��������, ��  vertloop ������, ֵ x=x
			m_sector1.triangle[triloop].vertex[vertloop].y = y;	// ���� 1,  �� triloop ��������, ��  vertloop ������, ֵ y=y
			m_sector1.triangle[triloop].vertex[vertloop].z = z;	// ���� 1,  �� triloop ��������, ��  vertloop ������, ֵ z=z
			m_sector1.triangle[triloop].vertex[vertloop].u = u;	// ���� 1,  �� triloop ��������, ��  vertloop ������, ֵ u=u
			m_sector1.triangle[triloop].vertex[vertloop].v = v;	// ���� 1,  �� triloop ��������, ��  vertloop ������, ֵ v=v
		}
	}
	//�����ļ���ÿ�������ζ���������ʽ����:
	//X1 Y1 Z1 U1 V1
	//X2 Y2 Z2 U2 V2
	//X3 Y3 Z3 U3 V3
	file.close();
}

//��ÿ���������ı��ж�����������кܶ�취����������һ���������ļ��в��������е��ж��������������Ϣ��
//���к�ע�Ͳ�Ӧ�ñ����롣���Ǵ�����һ������readstr()�ĺ��������������������ļ��ж���һ�������������һ���Ѿ���ʼ�������ַ�����
//������Ǵ��룺
void GLWidget::readStr(QTextStream *stream, QString &string)
{
	do								// ѭ����ʼ
	{
		string = stream->readLine();
	} while (string[0] == '/' || string[0] == '\n' || string.isEmpty());		// �����Ƿ��б�Ҫ���д���
}

void GLWidget::loadTexture()
{
	QImage image(":/image/Crate.bmp");
	image = image.convertToFormat(QImage::Format_RGB888);
	image = image.mirrored();
	glGenTextures(1, &m_texture[0]);// ��������
	// Create Nearest Filtered Texture
	glBindTexture(GL_TEXTURE_2D, m_texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image.width(), image.height(),
		0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

	// Create Linear Filtered Texture
	glBindTexture(GL_TEXTURE_2D, m_texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image.width(), image.height(),
		0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

	// Create MipMapped Texture
	glBindTexture(GL_TEXTURE_2D, m_texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image.width(), image.height(),
		GL_RGB, GL_UNSIGNED_BYTE, image.bits());
}

//�㶨�������Ѿ�������Լ��ĵ�һ֡���档������㲻��ʲôQuake������...,���Ǿ���Ҳ����Carmack����Abrash��
//���г���ʱ�������԰���F��B��
//PgUp �� PgDown ��������Ч����PgUp /
//PgDown�򵥵�������б��ͷ�����NeHe���������Ļ���������ʹ�õ�����ȡ�����ҵ�ѧУID֤���ϵ���Ƭ���������˸���Ч��....:)��
//������Ҳ���ڿ�����һ������ʲô�������ǲ�Ҫ����ʹ����Щ������ʵ��������3D���棬д��������Ŀ��Ҳ������ˡ�
//��Ҳ��ϣ��������Ϸ�в�ֹ����һ��Sector��������ʵ��������������Ĳ��֣�����������Ҫʹ�ö����(����3������)��
//�������ڵĴ���ʵ������������Sector���޳��˱���(����ͷ���û��ƵĶ����)�������һ�д�������Ľ̳̣�������Ҫ�������ѧ֪ʶ������

