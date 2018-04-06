#ifndef TEXT8_3A_H
#define TEXT8_3A_H

#include <QtWidgets/QMainWindow>
#include "ui_text8_3a.h"

class Text8_3A : public QMainWindow
{
	Q_OBJECT

public:
	Text8_3A(QWidget *parent = 0);
	~Text8_3A();
private slots:
	int OnBtnSelect();
	int OnBtnSend();
private:
	Ui::Text8_3AClass ui;
};

#endif // TEXT8_3A_H
