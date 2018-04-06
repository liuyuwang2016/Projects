#ifndef KINECTSENSOR_H
#define KINECTSENSOR_H

#include <QWidget>
#include "ui_kinectsensor.h"

class KinectSensor : public QWidget
{
	Q_OBJECT

public:
	KinectSensor(QWidget *parent = 0);
	~KinectSensor();

private:
	Ui::KinectSensor ui;
};

#endif // KINECTSENSOR_H
