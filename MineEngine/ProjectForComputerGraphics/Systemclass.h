// 文件名: systemclass.h
#pragma once

// 预处理指令  加速编译过程，通过排除一些不常用的API来减小Win32头文件的大小
#define WIN32_LEAN_AND_MEAN

// 包含文件 
#include <windows.h>

// 工程内包含文件 
#include "inputclass.h"
#include "graphicsclass.h"
#include "applicationclass.h"

// 类名: SystemClass
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();
private:
	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	bool Frame();

private:
	// 应用程序名称
	LPCWSTR			m_applicationName;
	// 应用程序的实例
	HINSTANCE		m_hinstance;
	// 窗口句柄
	HWND			m_hwnd;

	//分别用于处理设备输入和图像绘制
	InputClass*		m_Input;
	GraphicsClass*	m_Graphics;

	// 绘制地形
	ApplicationClass* m_Application;
};

// Windows窗口回调函数
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


