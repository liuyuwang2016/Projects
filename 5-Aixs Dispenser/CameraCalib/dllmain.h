#pragma region Initialization
#include <windows.h>
#include <stdexcept>
using namespace std;
struct Rs232MotionData{
	double x, y, z, u, v, w;
	double laser;
	long   out, status, length, in;
	long   flowNumber;
	long   wdt;//这个是什么？
	long   cmdid;
};
Rs232MotionData mdr, mdr1, mdr2;
int MtTestYvan(int a, int b);
long MtConnect(/*long ptr*/char* name);
long MtClose(void);
long MtReflash(/*struct*/ Rs232MotionData *m);
long MotionComClose(void);
long MotionComStatus(void);
long MotionComSignal(long signal);
long MotionFlag(long b);
long MotionInput(long b);
long MotionCmd(char* cmd);
void MtRs232Baud(long b);
long MtStart(long ptr);
long MtFlag(long b);
long MtInput(long b);
long MtSize(void);
long MtHead(void);
long MtTail(void);
long MtCmd(char *cmd);
long MtEmpty(void);
long MtThread(void);
unsigned long MtCheckSum(void);
unsigned long MtCheckWdt(void);
unsigned long MtError(void);
char* MtLaserCmd(char *cmd);
char* MtReceived(void);
long RemoteWDT(void);
#define  ms_hx   1
#define  ms_hy   2
#define  ms_hz  3
#define  ms_hu   4
#define  ms_hv   5
#define  ms_hw   6
#define  ms_idle 7
#define  ms_emg  8
#define  ms_emgflag  9
#define  ms_intflag  10
#define  ms_dfistop  11
#define  ms_blendflag  12
#define  ms_ms_state  13
#define  ms_acc_end  14
#define  ms_initial 15
//#define  ms_hx   1
//#define  ms_hy   2
//#define  ms_hz  3
//#define  ms_hu   4
//#define  ms_hv   5
//#define  ms_hw   6
//#define  ms_idle 7
//#define  ms_emg  8
//#define  ms_emgflag  9
//#define  ms_intflag  10
//#define  ms_dfistop  11
//#define  ms_blendflag  12
//#define  ms_ms_state  13
//#define  ms_acc_end  14
//#define  ms_initial 15
enum MotionStatus
{
	home_x = 0,
	home_y = 1,
	home_z = 2,
	home_u = 3,
	idle = 4,
	emg_signal = 5,
	emg_flag = 6,
	home_v = 7,
	in_stop = 8,
	home_w = 9,
	motorpower = 10,
	motion_idle = 11,
	door = 12,
	buffer = 13,
	flag0 = 14,
	flag1 = 15,
	i0 = 16,
	i1 = 17,
	i2 = 18,
	i3 = 19,
	i4 = 20,
	i5 = 21,
	i6 = 22,
	i7 = 23,
};
extern void Trace(char* pStr, ...);
int WriteABufferD(char *lpBuf, DWORD dwToWrite);
HANDLE hCom, handle;
OVERLAPPED oCOM;
DCB dcb;
BOOL CreateFlag;
int dwError;
int checksum;
int emgflag;
int stopflag;
int DTRFlag = 1;
extern long EpcioCheckSum(char *s);
extern void EpcioCmdFormat(char *s, char *r);
extern int ConnectFlag;
DWORD threadid = 0;
long flowNumber;
extern char cmd[100];
char data[40][30];
extern char SendCmdFlag;
extern char received[5024];
char ccmd[1000] = "";
char ReadBuf[5048];
char laser_sendcmd[100] = "";
char laser_recvcmd[100] = "";
int string_received = 0;
int ErrCnt = 0;
char ack[2] = { 0, 0 };
extern DWORD WINAPI MotionThreadProc(LPVOID lpParameter);
char cmdbuf[510][80];
volatile int head = 0, tail = 0;
int thread_loop = 0;
unsigned int thread_flag = 0;
int thread_wdt = 0;
int wait_time = 5;
int pause_thread = 0;
int RemoteFlag = 0;
long *SendFlag;
long *ReadFlag;
char *SendString;
char *ReadString;
char buf[5000];
int head1 = 0;
int CommErr = 0;
int CommWdt = 0;
char LastString[5000];
long cmd_in_buffer = 0;
long baud = 115200;
#pragma endregion Initialization

void sleep(int t)
{
	long i, j;
	for (j = 0; j<t; j++)
	{
		for (i = 0; i<200000; i++);
	}
}
//这是什么？？
long EpcioCheckSum(char *s)
{
	int i;
	long cs;
	int len;
	cs = 0;
	len = strlen(s);
	for (i = 0; i<len; i++)
	{
		cs += *(s + i);
	}
	return cs;
}

int EpcioLen()
{
	COMSTAT comstat;
	DWORD num;
	ClearCommError(hCom, &num, &comstat);
	return  comstat.cbInQue;
}
//这是什么？
int Calc_CRC(char *buf)
{
	int CRC1, k;
	int num;
	int i, j;
	int length;
	length = strlen(buf);
	CRC1 = -1;
	for (i = 0; i<length; i++)
	{
		CRC1 = CRC1 ^ buf[i];
		for (j = 0; j<8; j++)
		{
			k = CRC1 & 1;
			CRC1 = ((CRC1 & 0xfffe) / 2) & 0x7fff;
			if (k>0)
			{
				num = CRC1 ^ 0xa001;
				CRC1 = (short)(num << 16 >> 16);
			}
		}
	}
	return CRC1;
}

void EpcioCmdFormat(char *s, char *r)
{
	int cs;
	char temp[1024];
	flowNumber++;
	if (flowNumber>99)
		flowNumber = 0;
	sprintf(temp, "%d:%s", flowNumber, s);
	cs = Calc_CRC(temp);
	sprintf(r, "    %d:%s\n", cs, temp);
}

int ReadABuffer(char *lpBuf, DWORD dwToRead)
{
	int dwRead;
	int dSucc;
	COMSTAT comstat;
	DWORD num;
	long i;
	*lpBuf = 0;
	for (i = 0; i<20001; i++)
	{
		sleep(1);
		ClearCommError(hCom, &num, &comstat);
		if (comstat.cbInQue >= dwToRead)
			break;
		if (i == 20000)
		{
			*lpBuf = '0';
			return 0;
		}
	}
	dwToRead = comstat.cbInQue;
	oCOM.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	/*******/
	DWORD* temp = new DWORD;
	ReadFile(hCom, lpBuf, dwToRead, temp, &oCOM);
	dwRead = static_cast<int>(*temp);
	/*******/
	//ReadFile(hCom, lpBuf, dwToRead, &dwRead, &oCOM);
	dSucc = WaitForSingleObject(oCOM.hEvent, 50);
	if (dSucc == WAIT_FAILED)
	{
		*lpBuf = '0';
		dwError = GetLastError();
		return 0;
	}
	else
	{
		if (dSucc == WAIT_OBJECT_0)
			ResetEvent(oCOM.hEvent);
		else
		{
			*lpBuf = '0';
			dwError = GetLastError();
			return 0;
		}
	}
	CloseHandle(oCOM.hEvent);
	return dwRead;
}

void Read()
{
	int dwRead, dwRead1;
	COMSTAT comstat;
	DWORD num;
	long i;
	unsigned char ch;
	char *buf1;
	long cs, cs1;
	ClearCommError(hCom, &num, &comstat);
	if (comstat.cbInQue == 0)
	{
		if (thread_flag>0)
			thread_flag--;
		return;
	}
	dwRead1 = comstat.cbInQue;
	if (dwRead1>1000)
		dwRead1 = 1000;
	dwRead = ReadABuffer(ReadBuf, dwRead1);
	for (i = 0; i<dwRead; i++)
	{
		ch = ReadBuf[i];
		if (ch == 0)
		{
			CommErr = 1;
			head1 = 0;
			ErrCnt++;
		}
		else
			if (ch<10)
			{
				CommWdt = ch;
				head1 = 0;
			}
			else
			{
				buf[head1] = ch;
				head1 = (head1 + 1) % 5000;
			}
		if (ch == 10 || ch == 13)
		{
			buf[head1 - 1] = 0;
			cs = atol(buf);
			buf1 = strstr(buf, "~");
			if (buf1 != NULL)
			{
				cs1 = Calc_CRC(buf1 + 1);
				if (cs == cs1)
				{
					strcpy(laser_recvcmd, buf1 + 1);
					string_received = 1;
					head1 = 0;
					return;
				}
				else
				{
					buf[head1 - 1] = 10;
					buf[head1] = 0;
					cs1 = Calc_CRC(buf1 + 1);
					if (cs == cs1)
					{
						strcpy(laser_recvcmd, buf1 + 1);
						string_received = 1;
						head1 = 0;
						return;
					}
				}
				head1 = 0;
				string_received = -1;
				return;
			}
			buf1 = strstr(buf, ":");
			head1 = 0;
			if (buf1 != NULL)
			{
				cs1 = Calc_CRC(buf1 + 1);
				if (cs == cs1)
				{
					char *str, *str1;
					int j;
					str = buf1 + 1;
					j = 0;
					str1 = data[j];
					while (*str != 0)
					{
						if (*str != ',')
						{
							*str1 = *str;
							str1++;
						}
						else
						{
							*str1 = 0;
							j++;
							str1 = data[j];
						}
						str++;
					}
					*str1 = 0;
					if (j == 13)
					{
						mdr.x = atof(data[0]);
						mdr.y = atof(data[1]);
						mdr.z = atof(data[2]);
						mdr.u = atof(data[3]);
						mdr.v = atof(data[4]);
						mdr.w = atof(data[5]);
						mdr.laser = atof(data[6]);
						mdr.out = atol(data[7]) >> 13;
						mdr.length = atol(data[8]);
						mdr.status = atol(data[9]);
						mdr.in = atol(data[10]) >> 12;
						mdr.flowNumber = atol(data[11]);
						mdr.wdt = atol(data[12]);
						mdr.cmdid = atol(data[13]);
					}
				}
			}
		}
	}
	thread_flag = 2000;
}

//--------------------------------------------------------------
int WriteABufferD(char *lpBuf, DWORD dwToWrite)
{
	DWORD dwWrite;
	int dSucc;
	// int j; 
	int wdt = 0;
	oCOM.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (WriteFile(hCom, lpBuf, dwToWrite, &dwWrite, &oCOM) == FALSE)
	{
		dwError = GetLastError();
		dSucc = WaitForSingleObject(oCOM.hEvent, 100);
	}
	CloseHandle(oCOM.hEvent);
	return dwWrite;
}

int WriteABuffer(char *lpBuf, DWORD dwToWrite)
{
	int cnt;
	int dtrcnt = 0;
	int wdt1 = 0;
	int wdt;
	char send[1024];
	cnt = 0;
	strcpy(send, lpBuf);
	send[dwToWrite] = 10;
	send[0] = 0;
	dwToWrite += 1;
	if (laser_sendcmd[0] != 0)
	{
		WriteABufferD(lpBuf, dwToWrite);
		return 1;
	}
	while (cnt<10 && thread_loop == 1)
	{
		wdt = 0;
		CommErr = 0;
		wdt = CommWdt;
		WriteABufferD(send, dwToWrite);
		wdt1 = 0;
		while (wdt == CommWdt && CommErr == 0 && wdt1<500 && thread_loop == 1)
		{
			sleep(1);
			wdt1++;
			Read();
		}
		if (CommErr == 0 && wdt1<500)
			break;
		cnt += 1;
	}
	wdt1 = 0;
	wdt = mdr.wdt;
	while (wdt == mdr.wdt && wdt1<50)
	{
		sleep(1);
		wdt1++;
		Read();
	}
	if (cnt<10)
		return 1;
	return 0;
}

char sendbuf[1024];
char recvbuf[1024] = "";
char temp[1024];
unsigned int _checksum = 0;
int pause_thred = 0;
void CalCheckSum(unsigned char *str)
{
	unsigned char c;
	while (*str != 0)
	{
		c = *str;
		_checksum += c;
		str++;
	}
}

DWORD WINAPI MotionThreadProc(LPVOID lpParameter)
{
	int i = 0;
	thread_loop = 1;
	thread_flag = 0;
	while (thread_loop)
	{
		while (pause_thread)
			sleep(1);
		thread_wdt++;
		Read();
		if (thread_wdt % 500 == 0)    // 500 about 10 ms 
			WriteABufferD(ack, 1);
		while (head != tail && thread_loop)
		{
			ccmd[0] = 0;
			for (i = 0; i<10; i++)
			{
				if (i != 0)
					strcat(ccmd, "|");
				strcat(ccmd, cmdbuf[tail]);
				tail++;
				cmd_in_buffer--;
				if (tail == 500)
					tail = 0;
				if (tail == head)
					break;
			}
			if (strlen(ccmd)>5)
				/*Mt232::Mymt232::*/MotionCmd(ccmd);
		}
		if (laser_sendcmd[0] != 0)
		{
			int cnt;
			laser_recvcmd[0] = 0;
			Read();
			string_received = 0;
			/*Mt232::Mymt232::*/MotionCmd(laser_sendcmd);
			cnt = 0;
			while (string_received == 0 && cnt<150)
			{
				Read();
				sleep(1);
				cnt += 1;
			}
			laser_sendcmd[0] = 0;
		}
	}
	thread_loop = -1;
	EscapeCommFunction(hCom, CLRDTR);
	ExitThread(0);
	return 0;
}

long /*Mymt232::*/MotionComClose(void)
{
	if (hCom == INVALID_HANDLE_VALUE)
		return 0;
	CloseHandle(hCom);
	return 1;
}

long /*Mymt232::*/MtClose(void)
{
	thread_loop = 0;
	CreateFlag = FALSE;
	thread_flag = 0;
	EscapeCommFunction(hCom, CLRDTR);
	if (hCom == INVALID_HANDLE_VALUE)
		return 0;
	CloseHandle(hCom);
	return 1;
}

long /*Mymt232::*/MotionComStatus(void)
{
	unsigned long ModemStat;
	GetCommModemStatus(hCom, &ModemStat);
	return ModemStat;
}

long /*Mymt232::*/MotionComSignal(long signal)
{
	switch (signal)
	{
	case 0:
		signal = CLRDTR;
		break;
	case 1:
		signal = SETDTR;
		break;
	case 2:
		signal = CLRRTS;
		break;
	case 3:
		signal = SETRTS;
		break;
	default:
		return 0;
	}
	EscapeCommFunction(hCom, signal);
	return signal;
}

void /*Mymt232::*/MtRs232Baud(long b)
{
	baud = b;
}

long /*Mymt232::*/MtConnect(/*long ptr*/char* name)
{
	//char name[][10]={"COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","COM10","COM11","COM12","COM13","COM14","COM15","COM16","COM17","COM18","COM19","COM20"};
	char msg[100];
	//char *name = (char *)ptr;
	DWORD num;
	COMSTAT comstat;
	COMMTIMEOUTS timeouts;
	ErrCnt = 0;
	if (CreateFlag)
	{
		CreateFlag = FALSE;
		CloseHandle(hCom);
	}
	/*-----*/
	//LPCWSTR ppp = (WCHAR *)name;
	WCHAR wsz[64];
	swprintf(wsz, L"%S", name);
	LPCWSTR ppp = wsz;
	hCom = CreateFile(name/*ppp*/,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,                    // exclusive access
		NULL,                 // no security attrs
		OPEN_EXISTING,
		//FILE_FLAG_OVERLAPPED, // overlapped I/O
		FILE_FLAG_NO_BUFFERING,
		NULL);
	/*-----*/
	if (hCom == INVALID_HANDLE_VALUE)
		return 2;
	else
		CreateFlag = TRUE;
	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(hCom, &dcb))
		return GetLastError();
	sprintf(msg, "%s: baud=%ld parity=N data=8 stop=1", name, baud);
	/*-----*/
	LPCWSTR pppp = (WCHAR *)msg;
	BuildCommDCB(msg/*pppp*/, &dcb);
	/*-----*/
	/*
	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fOutxCtsFlow = TRUE;
	dcb.fOutxDsrFlow = 1;
	dcb.fDsrSensitivity = 1;
	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
	dcb.fParity = 0;
	dcb.fNull = 0;
	dcb.Parity = EVENPARITY;
	*/
	if (!SetCommState(hCom, &dcb))
	{
		return 0;
	}
	//	SetCommMask(hCom, EV_RXCHAR | EV_TXEMPTY | EV_ERR);
	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 1;
	SetCommTimeouts(hCom, &timeouts);
	EscapeCommFunction(hCom, CLRRTS);
	EscapeCommFunction(hCom, SETDTR);
	DTRFlag = 1;
	WriteABufferD(ack, 1);
	sleep(10);
	WriteABufferD(ack, 1);
	sleep(10);
	WriteABufferD(ack, 1);
	sleep(10);
	WriteABufferD(ack, 1);
	sleep(10);
	ClearCommError(hCom, &num, &comstat);
	if (comstat.cbInQue == 0)
		return -1;
	/*-----*/
	//while (1)
	//{
	//	ClearCommError(hCom, &num, &comstat);
	//	if (comstat.cbInQue != 0) break;
	//	Sleep(10);
	//}
	//ReadFile(hPort, &data, 1, &dwBytesRead, NULL);
	/*-----*/
	handle = CreateThread(NULL, 0, MotionThreadProc, NULL, 0, &threadid);
	return 1;
}

long /*Mymt232::*/MtStart(long ptr)
{
	EscapeCommFunction(hCom, SETDTR);
	DTRFlag = 1;
	handle = CreateThread(NULL, 0, MotionThreadProc, NULL, 0, &threadid);
	return 1;
}

long /*Mymt232::*/MotionFlag(long b)
{
	long k;
	long n;
	n = mdr.status;
	k = 1;
	k = k << b;
	if ((k & n))
		return 1;
	else
		return 0;
}

long /*Mymt232::*/MotionInput(long b)
{
	long k;
	long n;
	k = 1;
	k = k << b;
	n = mdr.in;
	if ((k & n))
		return 1;
	else
		return 0;
}

long /*Mymt232::*/MtFlag(long b)
{
	long k;
	long n;
	n = mdr.status;
	k = 1;
	k = k << b;
	if ((k & n))
		return 1;
	else
		return 0;
}

long /*Mymt232::*/MtInput(long b)
{
	long k;
	long n;
	k = 1;
	k = k << b;
	n = mdr.in;
	if ((k & n))
		return 1;
	else
		return 0;
}

long /*Mymt232::*/MtSize(void)
{
	return cmd_in_buffer;
}

long /*Mymt232::*/MtHead(void)
{
	return head;
}

long /*Mymt232::*/MtTail(void)
{
	return tail;
}

long /*Mymt232::*/RemoteWDT(void)
{
	return thread_wdt;
}

long /*Mymt232::*/MotionCmd(char* cmd)
{
	EpcioCmdFormat((char *)cmd, sendbuf);
	return WriteABuffer(sendbuf, strlen(sendbuf) - 1);
}

long /*Mymt232::*/MtReflash(/*struct*/ Rs232MotionData *m)
{
	*m = mdr;
	/*	if(thread_flag!=0 && DTRFlag==1)
	{
	EscapeCommFunction(hCom,CLRDTR);
	DTRFlag=0;
	}
	if(thread_flag==0 && DTRFlag==0)
	{
	EscapeCommFunction(hCom,SETDTR);
	DTRFlag=1;
	}
	*/
	return thread_flag;
}

long /*Mymt232::*/MtEmpty(void)
{
	int i;
	i = 0;
	if (head == tail)
		return 1;
	return 0;
}

long /*Mymt232::*/MtThread(void)
{
	int i;
	i = 0;
	return thread_loop;
}

long /*Mymt232::*/ MtCmd(char *cmd)
{
	int wdt;
	if (!strcmp(cmd, "mt_emg 1"))
	{
		head = 0;
		tail = 0;
		cmd_in_buffer = 0;
	}
	//	strlwr(cmd);
	strcpy(cmdbuf[head], cmd);
	head++;
	cmd_in_buffer++;
	if (head == 500)
		head = 0;
	wdt = 0;
	while (head == tail && wdt<500)
	{
		sleep(1);
		wdt++;
	}
	return 1;
}

unsigned long /*Mymt232::*/MtCheckSum(void)
{
	return _checksum;
}

unsigned long /*Mymt232::*/MtCheckWdt(void)
{
	return mdr.wdt;
}

unsigned long /*Mymt232::*/MtError(void)
{
	return ErrCnt;
}

char* /*Mymt232::*/MtLaserCmd(char *cmd)
{
	char send[1024];
	strcpy(send, cmd);
	//	strlwr(send);
	strcpy(laser_sendcmd, send);
	while (laser_sendcmd[0] != 0)
		sleep(1);
	return 	laser_recvcmd;
}

char* /*Mymt232::*/MtReceived(void)
{
	return 	laser_recvcmd;
}

int MtTestYvan(int a, int b)
{
	return (a + b);
}
