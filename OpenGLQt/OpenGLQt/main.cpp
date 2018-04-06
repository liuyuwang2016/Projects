#include "openglqt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OpenGLQt w;
	w.show();
	return a.exec();
}
