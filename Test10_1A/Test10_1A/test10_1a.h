#ifndef TEST10_1A_H
#define TEST10_1A_H

#include <QtWidgets/QMainWindow>
#include "ui_test10_1a.h"
#include <QLineEdit>
class Test10_1A : public QMainWindow
{
	Q_OBJECT

public:
	Test10_1A(QWidget *parent = 0);
	~Test10_1A();
private slots:
	int OnMenuNew();
	int OnMenuSave();
	int OnMenuHelp();
	int OnMenuSettings();
private:
	Ui::Test10_1AClass ui;
};

#endif // TEST10_1A_H
