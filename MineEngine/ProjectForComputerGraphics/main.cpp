#include "stdafx.h"
#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	// 构造
	SystemClass* System = new SystemClass;
	if (!System)
		return 0;

	// 初始化并执行
	bool result = System->Initialize();
	if (result)
		System->Run();

	// 析构
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}