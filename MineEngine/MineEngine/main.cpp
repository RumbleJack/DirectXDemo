////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	// Systemָ���ʼ��
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// SystemClass��ʼ������������run()
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Shutdown()���ͷ�system ʵ��.
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}