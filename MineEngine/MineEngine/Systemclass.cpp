////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

SystemClass::SystemClass()
{
	//当析构对象指针时，如果指针不为空，它们将被认为指向一个对象。
	m_Input = nullptr;
	m_Graphics = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{
	//为避免编译器默认定义复制构造函数，这里定义了一个空函数
}

SystemClass::~SystemClass()
{
	//某些windows函数，如ExitThread()不会调用析构函数，将导致内存泄露
	//虽然现在有更安全的函数版本，但小心为上
	//所以析构工作在shutdown()函数中完成
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// 初始化屏幕参数为0，在初始化窗口的函数中，将根据宏设置给它们赋值
	screenWidth = 0;
	screenHeight = 0;

	// 初始化窗口
	InitializeWindows(screenWidth, screenHeight);

	// 创建InputClass实例，该对象用于处理用户键盘输入
	m_Input = new InputClass;
	if (!m_Input){ return false; }

	// 初始化InputClass实例
	m_Input->Initialize();

	// 创建GraphicsClass实例，该对象用于绘制应用程序的所有图形
	m_Graphics = new GraphicsClass;
	if (!m_Graphics){ return false; }

	// 初始化GraphicsClass实例
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result){ return false; }

	return true;
}

void SystemClass::Shutdown()
{
	// 释放GraphicsClass实例
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	// 释放InputClass实例
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	// 关闭窗口
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// 初始化 message 结构体
	ZeroMemory(&msg, sizeof(MSG));

	// 在接收到用户或窗口发出的quit Message前不断执行循环
	done = false;
	while (!done)
	{
		// 处理 windows Message.当取得WM_QUIT时，PeekMessage将返回0
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 判断windows是否发出结束信号
		if (msg.message == WM_QUIT)
			done = true;
		else
		{
			// 否则，进行Frame处理
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

	// 检查用户是否按下 escape 想要退出应用
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// 处理GraphicsClass类的Frame()
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	// 检查是否有键盘被按下
	case WM_KEYDOWN:
		// 使用InputClass记录下此按键按下状态
		m_Input->KeyDown((unsigned int)wparam);
		break;
	// 检查是否有键盘被弹起
	case WM_KEYUP:
		// 使用InputClass记录下此按键弹起状态
		m_Input->KeyUp((unsigned int)wparam);
		break;

	// 其余消息交给默认消息处理函数
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	return 0;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// 将外部指针指向该对象实例
	ApplicationHandle = this;

	// 获取该应用程序的实例
	m_hinstance = GetModuleHandle(NULL);

	// 为该应用命名
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
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

	// 判定客户桌面分辨率Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 设置为全屏或窗口模式
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
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
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// 将窗口放置到屏幕中央
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 创建窗口并获取句柄
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 隐藏鼠标光标
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

	// 释放指向本类的指针
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	// 检查窗口是否被destroyed.WM_DESTROY抛出WM_QUIT
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// 检查窗口是否被 closed.WM_CLOSE抛出WM_QUIT
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	// 所有其他消息被送至SystemClass的消息处理函数中
	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}