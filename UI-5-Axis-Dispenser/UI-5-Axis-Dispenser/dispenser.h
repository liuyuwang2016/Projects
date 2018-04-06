#ifndef DISPENSER_H
#define DISPENSER_H

#include <QtWidgets/QMainWindow>
#include "ui_dispenser.h"
#include "dllmain.h"
#include "Kinect_v2.h"

class Dispenser : public QMainWindow
{
	Q_OBJECT

public:
	Dispenser(QWidget *parent = 0);
	~Dispenser();

private:
	Ui::DispenserClass ui;
};

#endif // DISPENSER_H
