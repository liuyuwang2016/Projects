#include "qtdrop.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtDrop w;
	w.show();
	return a.exec();
}
