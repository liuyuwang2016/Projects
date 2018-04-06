#ifndef UI_H
#define UI_H

#include <QtWidgets/QMainWindow>
#include "ui_ui.h"

class UI : public QMainWindow
{
	Q_OBJECT

public:
	UI(QWidget *parent = 0);
	~UI();

private:
	Ui::UIClass ui;
};

#endif // UI_H
