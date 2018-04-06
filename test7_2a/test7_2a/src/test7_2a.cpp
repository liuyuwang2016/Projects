#include "test7_2a.h"
#include <QMessageBox>
#include <QFileDialog>
#include "GBK.h"
#include <QDebug>

test7_2a::test7_2a(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.btnOpen, SIGNAL(clicked()), this, SLOT(OnBtnOpen()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(OnBtnSave()));
}

test7_2a::~test7_2a()
{

}

int test7_2a::OnBtnOpen()
{
	// ѡ��Ҫ�򿪵��ļ�
	QString filepath = QFileDialog::getOpenFileName(
		this, // ������
		GBK::ToUnicode("ѡ���ļ�") // ����caption
		);

	// Ϊ��ʱ��ʾ�û�ȡ���˲���,û��ѡ���κ��ļ�
	if (filepath.length() > 0)
	{
		qDebug() << filepath;
		string gbk_name = GBK::FromUnicode(filepath);
		FILE* fp = fopen(gbk_name.c_str(), "rb");
		//�ļ��Ĵ�С
		fseek(fp, 0, SEEK_END);
		int filesize = ftell(fp);
		char* buf = new char[filesize + 1];
		int n = fread(buf, 1, filesize, fp);
		if (n > 0)
		{
			buf[n] = 0;
			ui.plainTextEdit->setPlainText(GBK::ToUnicode(buf));
		}
		delete[] buf;
		fclose(fp);
	}
	return 0;
}


int test7_2a::OnBtnSave()
{
	// ѡ��Ҫ�򿪵��ļ�
	QString filepath = QFileDialog::getSaveFileName(
		this, // ������
		GBK::ToUnicode("����") // ����caption
		);

	// Ϊ��ʱ��ʾ�û�ȡ���˲���,û��ѡ���κ��ļ�
	if (filepath.length() > 0)
	{
		QString text = ui.plainTextEdit->toPlainText();
		string gbk_text = GBK::FromUnicode(text);
		string gbk_filename = GBK::FromUnicode(filepath);

		// ���ļ� (��17�£�
		FILE* fp = fopen(gbk_filename.c_str(), "wb");
		fwrite(gbk_text.c_str(), 1, gbk_text.length(), fp);
		fclose(fp);  // �ر��ļ�
	}

	return 0;
}