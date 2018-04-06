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
	// 选择要打开的文件
	QString filepath = QFileDialog::getOpenFileName(
		this, // 父窗口
		GBK::ToUnicode("选择文件") // 标题caption
		);

	// 为空时表示用户取消了操作,没有选中任何文件
	if (filepath.length() > 0)
	{
		qDebug() << filepath;
		string gbk_name = GBK::FromUnicode(filepath);
		FILE* fp = fopen(gbk_name.c_str(), "rb");
		//文件的大小
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
	// 选择要打开的文件
	QString filepath = QFileDialog::getSaveFileName(
		this, // 父窗口
		GBK::ToUnicode("保存") // 标题caption
		);

	// 为空时表示用户取消了操作,没有选中任何文件
	if (filepath.length() > 0)
	{
		QString text = ui.plainTextEdit->toPlainText();
		string gbk_text = GBK::FromUnicode(text);
		string gbk_filename = GBK::FromUnicode(filepath);

		// 打开文件 (第17章）
		FILE* fp = fopen(gbk_filename.c_str(), "wb");
		fwrite(gbk_text.c_str(), 1, gbk_text.length(), fp);
		fclose(fp);  // 关闭文件
	}

	return 0;
}