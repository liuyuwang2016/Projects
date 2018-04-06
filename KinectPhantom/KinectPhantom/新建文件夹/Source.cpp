#include <windows.h>
#include <iostream>
#include "dllmain.h"


using namespace std;

#using <System.dll>
using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;

bool MtHome(void);
void MtReflashWait(void);

Rs232MotionData* md = new struct Rs232MotionData;

void MtReflashWait()
{
	int i = mdr.wdt;
	int timeout = 0;

	while (i == mdr.wdt && timeout < 50)
	{
		MtReflash(md);
		timeout += 1;
		Sleep(1);
	}


}

bool MtHome()
{
	MtCmd("mt_emg 0");
	Sleep(100);
	MtCmd("mt_delay 20");
	MtCmd("mt_m_acc 20");
	MtCmd("mt_v_acc 20");
	MtCmd("mt_speed 30");
	MtCmd("mt_check_ot 0,0,0,0");
	MtCmd("mt_set_home_acc 50");
	MtCmd("mt_leave_home_speed 2,2,2,2");
	MtCmd("mt_go_home 50,50,10,50,2,2,1,255"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 順序x, 順序y, 順序z, 順序u, 255 = u不回home)

	Sleep(1000);
	
	do{
		MtReflashWait();
		if (MtFlag(MotionStatus::emg_signal))
		{
			MtCmd("mt_abort_home");
			return false;
		}
	} while (MtFlag(MotionStatus::home_x) || MtFlag(MotionStatus::home_y) || MtFlag(MotionStatus::home_z)); //home_x 回到原點的過程 = true

	MtCmd("mt_home_finish");
	MtCmd("mt_speed1 30");
	MtCmd("mt_soft_limit 0,-2,300"); //Axe no., Left limit, Right limit
	MtCmd("mt_soft_limit 1,-2,300");
	MtCmd("mt_soft_limit 2,-2,100");
	MtCmd("mt_soft_limit 3,-450,450");
	MtCmd("mt_check_ot 1,1,1,0"); //enable limit (by axe no)
	MtCmd("mt_out 11,1"); //door switch
	MtCmd("mt_m_acc 150");  //door switch
	MtCmd("mt_v_acc 80");  //door switch


	return true;
}


int main()
{
	array<String^>^ serialPorts = nullptr;
	try
	{
		// Get a list of serial port names.
		serialPorts = SerialPort::GetPortNames();
	}
	catch (Win32Exception^ ex)
	{
		Console::WriteLine(ex->Message);
	}

	Console::WriteLine("The following serial ports were found:");

	// Display each port name to the console.
	for each(String^ port in serialPorts)
	{
		Console::WriteLine(port);
	}

	//cout << MtTestDebby(2, 3) << endl;

	char ptr[5] = "COM3";
	//cout << "Connect I/O = " << MtConnect(ptr) << endl;

	long IS_CONECT = 0;

	//剛開機時，第一次連線都會失敗不知為何
	do
	{
		IS_CONECT = MtConnect(ptr);
		cout << "Connect I/O = " << IS_CONECT << endl;
	} while (IS_CONECT != 1);

	Sleep(50);

	//cout << "MtReflash: " << MtReflash(md) << endl;
	//
	//cout << "md->x : " << md->x << endl;
	//cout << "md->y : " << md->y << endl;
	
	//MtCmd("mt_emg 0");

	bool IS_HOME = MtHome();

	MtReflash(md);
	cout << "md->x : " << md->x << endl;
	cout << "md->y : " << md->y << endl;
	Sleep(100);

	for (int i = 0; i < 5; i++)
	{
		MtCmd("mt_out 12,1"); //12是出膠 9是閃燈
		Sleep(100);
		MtCmd("mt_out 12,0");
		Sleep(100);
	}
	

	//MtCmd("mt_speed 100");
	//MtCmd("mt_mr_x 30"); //mr = 相對座標; m = 絕對座標
	//MtCmd("mt_mr_y 30"); //mr = 相對座標; m = 絕對座標
	//Sleep(100);

	MtCmd("mt_m_x 145"); //mr = 相對座標; m = 絕對座標
	MtCmd("mt_m_y 124"); //mr = 相對座標; m = 絕對座標
	MtCmd("mt_m_z 50"); //mr = 相對座標; m = 絕對座標
	Sleep(100);

	int i = 0;
	//等待機器完成移動後再執行其他步驟
	while (!MtFlag(MotionStatus::motion_idle))
	{
		cout << i << endl;

		i++;
	}

	MtReflash(md);
	cout << "md->x : " << md->x << endl;
	cout << "md->y : " << md->y << endl;

	cout << "Close I/O = " << MtClose() << endl;


	cout << "End process" << endl;

	

	system("pause");
	return 0;

}