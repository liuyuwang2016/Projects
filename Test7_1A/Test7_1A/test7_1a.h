#ifndef TEST7_1A_H
#define TEST7_1A_H

#include <QtWidgets/QMainWindow>
#include "ui_test7_1a.h"

class Test7_1A : public QMainWindow
{
	Q_OBJECT

public:
	Test7_1A(QWidget *parent = 0);
	~Test7_1A();
private slots:
	int OnBtnLogin();
private:
	Ui::Test7_1AClass ui;
};

#endif // TEST7_1A_H
