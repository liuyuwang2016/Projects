#ifndef SENDTASK_H
#define SENDTASK_H

#include <QThread>

class SendTask : public QThread
{
	Q_OBJECT

public:
	SendTask(QObject *parent);
	~SendTask();

	int GetStatus();
	int GetProgress();
private:
	void run();
private:
	char m_filepath[128];
	int m_filesize;//文件的总字节数
	int m_bytesread;//读取并处理了多少个字节
	int m_status; //任务的状态
};

#endif // SENDTASK_H
