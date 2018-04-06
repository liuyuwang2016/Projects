#include "LoginDlg.h"

LoginDlg::LoginDlg(const char* user, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.textUser->setText(user);
	connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(OnBtnOk()));
	connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancel()));
}

LoginDlg::~LoginDlg()
{

}

int LoginDlg::OnBtnOk()
{
	m_user = ui.textUser->text();
	m_password = ui.textPasswd->text();

	accept();//¹Ø±Õ¶Ô»°¿ò
	return 0;
}
int LoginDlg::OnBtnCancel()
{
	reject();
	return 0;
}