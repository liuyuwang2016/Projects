#ifndef TEST10_2A_H
#define TEST10_2A_H

#include <QtWidgets/QMainWindow>
#include "ui_test10_2a.h"

class Test10_2A : public QMainWindow
{
	Q_OBJECT

public:
	Test10_2A(QWidget *parent = 0);
	~Test10_2A();

private:
	Ui::Test10_2AClass ui;
};

#endif // TEST10_2A_H
