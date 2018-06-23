#include "qtdrop.h"

QtDrop::QtDrop(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.listWidget->addItem("book");
	ui.listWidget->addItem("heart");
	ui.listWidget->addItem("location");
}

QtDrop::~QtDrop()
{

}

