#include "stdafx.h"
#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	// ����
	SystemClass* System = new SystemClass;
	if (!System)
		return 0;

	// ��ʼ����ִ��
	bool result = System->Initialize();
	if (result)
		System->Run();

	// ����
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}