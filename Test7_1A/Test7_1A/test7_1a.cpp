#include "test7_1a.h"
#include "LoginDlg.h"
#include <QDebug>
Test7_1A::Test7_1A(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.btnLogin, SIGNAL(clicked()), this, SLOT(OnBtnLogin()));
}

Test7_1A::~Test7_1A()
{

}

int Test7_1A::OnBtnLogin()
{
	LoginDlg dlg("Yuwang", this);
	int ret = dlg.exec();//�Ի�����ʾ����������
	if (ret == QDialog::Accepted)
	{
		//���û�����ȷ����ʱ��ȡ���û�������
		qDebug() << dlg.m_user << dlg.m_password;
	}
	else
	{
		//�û�ѡ��ȡ��
	}
	return 0;
}
