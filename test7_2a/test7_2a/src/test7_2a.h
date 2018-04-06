#ifndef TEST7_2A_H
#define TEST7_2A_H

#include <QtWidgets/QMainWindow>
#include "ui_test7_2a.h"
#include "GeneratedFiles/ui_test7_2a.h"

class test7_2a : public QMainWindow
{
	Q_OBJECT

public:
	test7_2a(QWidget *parent = 0);
	~test7_2a();

private slots:
	int OnBtnOpen();
	int OnBtnSave();

private:
	Ui::test7_2aClass ui;
};

#endif // TEST7_2A_H
