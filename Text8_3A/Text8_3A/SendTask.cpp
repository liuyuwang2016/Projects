#include "SendTask.h"

SendTask::SendTask(QObject *parent)
	: QThread(parent)
{

}

SendTask::~SendTask()
{

}

int SendTask::GetStatus()
{
	return 0;
}

int SendTask::GetProgress()
{
	return 0;

}

void SendTask::run()
{
	FILE* fp = fopen(m_filepath, "rb");
	if (!fp)
	{
		m_status = -1;
		return;
	}
	fseek(fp, 0, SEEK_END);
	m_filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char buf[1024];
	int part = 0;
	while (1)
	{

	}
	fclose(fp);
	return 0;

}