#include "dispenser.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Dispenser w;
	w.show();
	return a.exec();
}
