// ListEngine.cpp : 定义动态链接库导出函数

#include "stdafx.h"
#include "ListEngine.h"

DllExport int nListEngine=0;

DllExport int fnListEngine(void)
{
	return 42;
}


CListEngine::CListEngine()
{
	return;
}
