#ifndef QTDROP_H
#define QTDROP_H

#include <QtWidgets/QMainWindow>
#include "ui_qtdrop.h"

class QtDrop : public QMainWindow
{
	Q_OBJECT

public:
	QtDrop(QWidget *parent = 0);
	~QtDrop();
private:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
private:
	Ui::QtDropClass ui;
};

#endif // QTDROP_H
