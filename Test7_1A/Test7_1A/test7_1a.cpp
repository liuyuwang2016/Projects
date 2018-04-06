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
	int ret = dlg.exec();//对话框显示，程序阻塞
	if (ret == QDialog::Accepted)
	{
		//当用户输入确定的时候取得用户的输入
		qDebug() << dlg.m_user << dlg.m_password;
	}
	else
	{
		//用户选择取消
	}
	return 0;
}
