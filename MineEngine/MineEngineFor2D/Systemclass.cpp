////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

SystemClass::SystemClass()
{
	//����������ָ��ʱ�����ָ�벻Ϊ�գ����ǽ�����Ϊָ��һ������
	m_Input = nullptr;
	m_Graphics = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{
	//Ϊ���������Ĭ�϶��帴�ƹ��캯�������ﶨ����һ���պ���
}

SystemClass::~SystemClass()
{
	//ĳЩwindows��������ExitThread()������������������������ڴ�й¶
	//��Ȼ�����и���ȫ�ĺ����汾����С��Ϊ��
	//��������������shutdown()���������
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// ��ʼ����Ļ����Ϊ0���ڳ�ʼ�����ڵĺ����У������ݺ����ø����Ǹ�ֵ
	screenWidth = 0;
	screenHeight = 0;

	// ��ʼ������
	InitializeWindows(screenWidth, screenHeight);

	// ����InputClassʵ�����ö������ڴ����û���������
	m_Input = new InputClass;
	if (!m_Input){ return false; }

	// ��ʼ��InputClassʵ��
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// ����GraphicsClassʵ�����ö������ڻ���Ӧ�ó��������ͼ��
	m_Graphics = new GraphicsClass;
	if (!m_Graphics){ return false; }

	// ��ʼ��GraphicsClassʵ��
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result){ return false; }

	return true;
}

void SystemClass::Shutdown()
{
	// �ͷ�GraphicsClassʵ��
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	// �ͷ�InputClassʵ��
	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// �رմ���
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// ��ʼ�� message �ṹ��
	ZeroMemory(&msg, sizeof(MSG));

	// �ڽ��յ��û��򴰿ڷ�����quit Messageǰ����ִ��ѭ��
	done = false;
	while (!done)
	{
		// ���� windows Message.��ȡ��WM_QUITʱ��PeekMessage������0
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// �ж�windows�Ƿ񷢳������ź�
		if (msg.message == WM_QUIT)
		{ 
			done = true;
		}
		else
		{
			// ���򣬽���Frame����
			result = Frame();
			if (!result)
				done = true;
		}

		// ����û��Ƿ���escape��ϣ���˳�
		if (m_Input->IsEscapePressed() == true)
		{
			done = true;
		}
	}
	return;
}

bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;

	// ����ÿһ֡�ļ����������
	result = m_Input->Frame();
	if (!result)
		return false;

	// Get the location of the mouse from the input object,
	m_Input->GetMouseLocation(mouseX, mouseY);

	// ����GraphicsClass���Frame()
	result = m_Graphics->Frame(mouseX, mouseY);
	if (!result)
	{
		return false;
	}

	//// Finally render the graphics to the screen.
	//result = m_Graphics->Render();
	//if (!result)
	//{
	//	return false;
	//}


	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// ���ⲿָ��ָ��ö���ʵ��
	ApplicationHandle = this;

	// ��ȡ��Ӧ�ó����ʵ��
	m_hinstance = GetModuleHandle(NULL);

	// Ϊ��Ӧ������
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

	// ע�ᴰ����
	RegisterClassEx(&wc);

	// �ж��ͻ�����ֱ���Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// ����Ϊȫ���򴰿�ģʽ
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ����ʾ���ø�Ϊȫ��
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// ������λ�����õ����Ͻ�
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// �����ڷ��õ���Ļ����
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// �������ڲ���ȡ���
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// ���������
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// ��ʾ�����
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// ɾ������
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ɾ��Ӧ�ó���ʵ��
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �ͷ�ָ�����ָ��
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	// ��鴰���Ƿ�destroyed.WM_DESTROY�׳�WM_QUIT
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// ��鴰���Ƿ� closed.WM_CLOSE�׳�WM_QUIT
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	// ����������Ϣ������SystemClass����Ϣ��������
	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}