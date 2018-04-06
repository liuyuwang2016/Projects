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
	int m_filesize;//�ļ������ֽ���
	int m_bytesread;//��ȡ�������˶��ٸ��ֽ�
	int m_status; //�����״̬
};

#endif // SENDTASK_H
