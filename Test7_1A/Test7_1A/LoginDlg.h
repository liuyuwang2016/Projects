#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include "ui_LoginDlg.h"

class LoginDlg : public QDialog
{
	Q_OBJECT

public:
	LoginDlg(const char* user, QWidget *parent = 0);
	~LoginDlg();
private slots:
	int OnBtnOk();
	int OnBtnCancel();
private:
	Ui::LoginDlg ui;
public:
	QString m_user;
	QString m_password;
};

#endif // LOGINDLG_H
