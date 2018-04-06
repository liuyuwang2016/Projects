#ifndef TEST11_3A_H
#define TEST11_3A_H

#include <QtWidgets/QMainWindow>
#include "ui_test11_3a.h"

class Test11_3A : public QMainWindow
{
	Q_OBJECT

public:
	Test11_3A(QWidget *parent = 0);
	~Test11_3A();

private:
	Ui::Test11_3AClass ui;
};

#endif // TEST11_3A_H
