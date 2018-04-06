#ifndef OPENGLQT_H
#define OPENGLQT_H

#include <QtWidgets/QMainWindow>
#include "ui_openglqt.h"

class OpenGLQt : public QMainWindow
{
	Q_OBJECT

public:
	OpenGLQt(QWidget *parent = 0);
	~OpenGLQt();

private:
	Ui::OpenGLQtClass ui;
};

#endif // OPENGLQT_H
