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
	//�������
	m_dragPosition = event->pos();
	m_dragItem = this->itemAt(event->pos());

	//�ǵõ��ø���ĺ���
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
	// �ҵ��϶�����
	if (!m_dragItem) return;
	QString text = m_dragItem->text();

	// ��������
	QDrag *drag = new QDrag(this);

	QMimeData *mimeData = new QMimeData;
	mimeData->setText(text);
	drag->setMimeData(mimeData);

	// �����϶�ʱ��ͼ����ʾ
	QPixmap drag_img(120, 18);
	QPainter painter(&drag_img);
	painter.drawText(QRectF(0, 0, 120, 18),
		text,
		QTextOption(Qt::AlignVCenter));
	drag->setPixmap(drag_img);

	// �����Ϸ�start a drag
	Qt::DropAction result = drag->exec(Qt::CopyAction | Qt::MoveAction);

	// ��������û�гɹ�/��û�б�ȡ��
	if (Qt::IgnoreAction != result)
	{

	}
	else
	{
		// ��סctrl��: Qt::CopyAction
		// û���� Qt::MoveAction
	}
}
