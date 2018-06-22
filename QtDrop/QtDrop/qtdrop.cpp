#include "qtdrop.h"
#include <QtGui>
QtDrop::QtDrop(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setAcceptDrops(true);
}

QtDrop::~QtDrop()
{

}

void QtDrop::dragEnterEvent(QDragEnterEvent *event)
{
	qDebug() << "-----------format--------";
	QStringList formats = event->mimeData()->formats();
	for (int i = 0; i < formats.size(); i++)
	{
		QString fmt = formats.at(i);
		qDebug() << "Format:" << fmt;
	}
	if (event->mimeData()->hasFormat("text/plain"))
	{
		event->acceptProposedAction();
	}
}

void QtDrop::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("text/plain"))
	{
		QString text = event->mimeData()->text();
		ui.m_label->setText(text);
		event->acceptProposedAction();
	}
}
