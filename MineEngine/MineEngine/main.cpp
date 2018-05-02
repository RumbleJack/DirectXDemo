////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	// System指针初始化
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// SystemClass初始化，并调用其run()
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Shutdown()并释放system 实例.
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}