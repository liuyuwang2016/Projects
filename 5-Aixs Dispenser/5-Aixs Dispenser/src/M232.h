#ifndef _M232_H
#define _M232_H
/*--------------M232---------------*/
#include "src/dllmain.h"
#using <System.dll>

/*------------M232 need using------------*/
using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace System::Threading;

/*---------------------------------*/
/*--------------M232---------------*/
/*---------------------------------*/
void MtHome(void);
void MtReflashWait(void);
void MtInit(void);
void MtMove(void);
void Mt_XMove(float x);
void Mt_YMove(float y);
void Mt_ZMove(float z);
void Mt_UMove(float u);
void Mt_VMove(float v);
void MtCheck(void);
void Mt_Line_Move(void);
void Mt_Calib_Move(void);

float Mt_x, Mt_y, Mt_z, Mt_u, Mt_v;
float Mt_speed = 1;
//To adjust error due to calibration while .obj moves along the plates on y axis
Rs232MotionData* md = new struct Rs232MotionData;	//Tracking machine status

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

/*add one MtHome button to the keyboard function*/
void MtHome()
{
	MtCmd("mt_emg 0");
	Sleep(100);
	MtCmd("mt_delay 20");
	MtCmd("mt_m_acc 20");
	MtCmd("mt_v_acc 20");
	MtCmd("mt_speed 30");
	MtCmd("mt_check_ot 0,0,0,0");
	MtCmd("mt_set_home_acc 50");
	MtCmd("mt_set_leave_home_speed 2,2,2,2,50,2");
	//MtCmd("mt_go_home 50,50,10,50,2,2,1,255"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 順序x, 順序y, 順序z, 順序u, 255 = u不回home)
	//MtCmd("mt_go_home 50,50,10,10,2,2,1,3"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 順序x, 順序y, 順序z, 順序u, 255 = u不回home)
	MtCmd("mt_home 50,50,10,50,50,20,2,2,1,2,2,255"); // 回home的速度(速度x, 速度y, 速度z, 速度u, 順序x, 順序y, 順序z, 順序u, 255 = u不回home)
	Sleep(1000);
	do{
		MtReflashWait();
		if (MtFlag(MotionStatus::emg_signal))
		{
			MtCmd("mt_abort_home");
			return;
		}
	} while (MtFlag(MotionStatus::home_x) || MtFlag(MotionStatus::home_y) || MtFlag(MotionStatus::home_z) || MtFlag(MotionStatus::home_u) || MtFlag(MotionStatus::home_v)); //home_x 回到原點的過程 = true
	
	MtCmd("mt_home_finish");
	MtCmd("mt_speed1 30");
	MtCmd("mt_soft_limit 0,-2,400"); //Axe no., Left limit, Right limit
	MtCmd("mt_soft_limit 1,-2,400");
	MtCmd("mt_soft_limit 2,-2,100");
	MtCmd("mt_soft_limit 3,0,360");
	MtCmd("mt_soft_limit 4,-70,70");
	//MtCmd("mt_check_ot 1,1,1,0"); //enable limit 
	MtCmd("mt_check_ot 1,1,1,1");
	MtCmd("mt_out 11,1"); //door switch
	MtCmd("mt_m_acc 150");  //door switch
	MtCmd("mt_v_acc 80");  //door switch
}

void MtInit(void)
{
	array<System::String^>^ serialPorts = nullptr;
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
	for each(System::String^ port in serialPorts)
	{
		Console::WriteLine(port);
	}
	//cout << MtTestYvan(2, 3) << endl;
	char ptr[5] = "COM6";
	//cout << "Connect I/O = " << MtConnect(ptr) << endl;
	long IS_CONECT = 0;
	//
	do
	{
		IS_CONECT = MtConnect(ptr);
		cout << "Connect I/O = " << IS_CONECT << endl;
	} while (IS_CONECT != 1);
	
	Sleep(50);
	long MtEmpty();
	MtHome();
}
#endif