#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QListWidget>
#include <QtGui>
#include <QApplication>
class MyListWidget : public QListWidget
{
	Q_OBJECT

public:
	MyListWidget(QWidget *parent);
	~MyListWidget();

private:
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
private:
	QPoint m_dragPosition; //�Ϸ����
	QListWidgetItem* m_dragItem; //���Ϸŵ�Item
};

#endif // MYLISTWIDGET_H
