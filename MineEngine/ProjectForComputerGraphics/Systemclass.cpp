// 文件名: systemclass.cpp
#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = nullptr;
	m_Graphics = nullptr;
	m_Application = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{
	//为避免编译器定义默认复制构造函数，导致意料之外的操作。这里定义了一个空函数
}

SystemClass::~SystemClass()
{
	//某些windows函数，如ExitThread()不会调用析构函数，将导致内存泄露
	//虽然现在有更安全的函数版本，但小心为上
	//所以析构工作在shutdown()函数中完成
}

bool SystemClass::Initialize()
{
	bool result;

	// 初始化窗口
	int	 screenWidth = 0;
	int	 screenHeight = 0;
	InitializeWindows(screenWidth, screenHeight);


	// 初始化输入模块
	m_Input = new InputClass;
	if (!m_Input)
		return false;
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}





	// 初始化图形绘制模块
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
		return false;
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	// 创建地形绘制模块
	m_Application = new ApplicationClass;
	if (!m_Application)
	{
		return false;
	}

	// 初始化地形绘制模块
	result = m_Application->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// 析构图形绘制模块
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}


	// 释放地形绘制模块
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}


	// 析构输入模块
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = nullptr;
	}

	// 析构窗口
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	bool  result;

	// 初始化 message 结构体
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// 在接收到WM_QUIT、或绘制完成（包括用户主动结束）后退出循环
	bool done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			done = true;
		else 
		{
			// 进行绘制（同时处理设备输入）
			result = Frame();
			if (!result)
				done = true;
		}
	}
	return;
}

bool SystemClass::Frame()
{
	bool result;
	
	// 绘制前获取设备输入
	result = m_Input->Frame();
	if (!result)
		return false;

	// 检查用户是否按下escape键希望退出
	result = m_Input->IsEscapePressed();
	if (result)
		return false;

	//获取键盘状态
	const unsigned char(*keyboardState)[256] = m_Input->GetKeyboardState();

	// 获取鼠标(位移量)位置
	int mouseX, mouseY;
	m_Input->GetMouseDeltaPosition(mouseX, mouseY);

	// 绘制图形
	result = m_Graphics->Frame(mouseX, mouseY, keyboardState);
	if (!result)
		return false;

	// 绘制地形
	//result = m_Application->Frame();
	//if (!result)
	//{
	//	return false;
	//}


	return true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// 获取该应用程序的实例
	m_hinstance = GetModuleHandle(NULL);

	// 为该应用命名
	m_applicationName = L"Engine";

	// 建立窗口
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 注册窗口类
	RegisterClassEx(&wc);

	// 判定客户桌面屏幕分辨率
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;
	int posX, posY;
	// 设置为全屏或窗口模式
	if (FULL_SCREEN)
	{
		// 全屏模式设置最大化，并设dmBitsPerPel为32位
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 将显示设置改为全屏
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 将窗口位置设置到左上角
		posX = posY = 0;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		// 将窗口位置设置到屏幕中央
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 创建窗口并获取句柄
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 显示桌面并设置为当前焦点
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 显示鼠标光标
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// 显示鼠标光标
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 删除窗口
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 删除应用程序实例
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)	
	{
		// 抛出WM_QUIT
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// 抛出WM_QUIT
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}