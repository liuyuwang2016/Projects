#include "text8_3a.h"
#include <QFileDialog>
#include "SendDlg.h"
Text8_3A::Text8_3A(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.btnSelect, SIGNAL(clicked()), this, SLOT(OnBtnSelect()));
	connect(ui.btnSend, SIGNAL(clicked()), this, SLOT(OnBtnSend()));
}

Text8_3A::~Text8_3A()
{

}

int Text8_3A::OnBtnSelect()
{
	return 0;
}

int Text8_3A::OnBtnSend()
{
	SendDlg dlg;
	dlg.exec();
	return 0;
}
