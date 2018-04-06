#define _CRT_SECURE_NO_WARNINGS /* VS2013,2015��Ҫ��һ�� */
#include <stdio.h>

#include "osapi/osapi.h"

OS_Mutex g_mutex;
char g_key[16]; // Generator��������Checker��ȡ��

class KeyGenerator : public OS_Thread
{
private:
	virtual int Routine()
	{
		int times = 0;
		while (1)
		{
			// ����key
			g_mutex.Lock();
			for (int i = 0; i < 16; i++)
			{
				OS_Thread::Msleep(5);
				g_key[i] = times;
			}
			g_mutex.Unlock();

			times++;
			if (times >= 128) times = 0;
			//OS_Thread::Msleep(50);
		}
		return 0;
	}
};

class KeyChecker : public OS_Thread
{
private:
	// �߳�������
	virtual int Routine()
	{
		while (1)
		{
			// ���ݴ���
			// ���������
			g_mutex.Lock();
			for (int i = 1; i < 16; i++)
			{
				if (g_key[i] != g_key[i - 1])
				{
					printf("������!!\n");
					PrintKey();
					//return 0;
				}
			}
			g_mutex.Unlock();

			//OS_Thread::Msleep(50);
		}
		return 0; // �����˳�
	}

	void PrintKey()
	{
		printf("Key: ");
		for (int i = 0; i < 16; i++)
			printf("%02X ", g_key[i]);
		printf("\n");
	}
};


int main()
{
	KeyGenerator a;
	a.Run();

	KeyChecker b;
	b.Run();

	getchar();


	return 0;
}
