#include "test10_1a.h"

Test10_1A::Test10_1A(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//向工具栏里面加入特别的Widget
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(OnMenuNew()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(OnMenuSave()));
	connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(OnMenuHelp()));
	connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(OnMenuSettings()));
    
}

Test10_1A::~Test10_1A()
{

}

int Test10_1A::OnMenuHelp()
{
	return 0;
}

int Test10_1A::OnMenuNew()
{
	return 0;
}

int Test10_1A::OnMenuSave()
{
	
	return 0;
}

int Test10_1A::OnMenuSettings()
{
	return 0;
}




