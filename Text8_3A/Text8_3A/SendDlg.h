#ifndef SENDDLG_H
#define SENDDLG_H

#include <QDialog>
#include "ui_SendDlg.h"

class SendDlg : public QDialog
{
	Q_OBJECT

public:
	SendDlg(QWidget *parent = 0);
	~SendDlg();

private:
	Ui::SendDlg ui;
};

#endif // SENDDLG_H
