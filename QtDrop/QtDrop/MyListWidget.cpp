#include "MyListWidget.h"

MyListWidget::MyListWidget(QWidget *parent)
	: QListWidget(parent)
{

}

MyListWidget::~MyListWidget()
{

}

void MyListWidget::mouseMoveEvent(QMouseEvent* event)
{
	//记下起点
	m_dragPosition = event->pos();
	m_dragItem = this->itemAt(event->pos());

	//记得调用父类的函数
	QListWidget::mousePressEvent(event);
}

void MyListWidget::mousePressEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton))
	{
		return;
	}
	if ((event->pos() - m_dragPosition).manhattanLength()
		< QApplication::startDragDistance())
		return;
	// 找到拖动的项
	if (!m_dragItem) return;
	QString text = m_dragItem->text();

	// 创建数据
	QDrag *drag = new QDrag(this);

	QMimeData *mimeData = new QMimeData;
	mimeData->setText(text);
	drag->setMimeData(mimeData);

	// 设置拖动时的图像显示
	QPixmap drag_img(120, 18);
	QPainter painter(&drag_img);
	painter.drawText(QRectF(0, 0, 120, 18),
		text,
		QTextOption(Qt::AlignVCenter));
	drag->setPixmap(drag_img);

	// 启动拖放start a drag
	Qt::DropAction result = drag->exec(Qt::CopyAction | Qt::MoveAction);

	// 检查操作有没有成功/有没有被取消
	if (Qt::IgnoreAction != result)
	{

	}
	else
	{
		// 按住ctrl键: Qt::CopyAction
		// 没按： Qt::MoveAction
	}
}
