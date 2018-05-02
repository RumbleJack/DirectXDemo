// �ļ���: systemclass.cpp
#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = nullptr;
	m_Graphics = nullptr;
	m_Application = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{
	//Ϊ�������������Ĭ�ϸ��ƹ��캯������������֮��Ĳ��������ﶨ����һ���պ���
}

SystemClass::~SystemClass()
{
	//ĳЩwindows��������ExitThread()������������������������ڴ�й¶
	//��Ȼ�����и���ȫ�ĺ����汾����С��Ϊ��
	//��������������shutdown()���������
}

bool SystemClass::Initialize()
{
	bool result;

	// ��ʼ������
	int	 screenWidth = 0;
	int	 screenHeight = 0;
	InitializeWindows(screenWidth, screenHeight);


	// ��ʼ������ģ��
	m_Input = new InputClass;
	if (!m_Input)
		return false;
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}





	// ��ʼ��ͼ�λ���ģ��
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
		return false;
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	// �������λ���ģ��
	m_Application = new ApplicationClass;
	if (!m_Application)
	{
		return false;
	}

	// ��ʼ�����λ���ģ��
	result = m_Application->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// ����ͼ�λ���ģ��
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}


	// �ͷŵ��λ���ģ��
	if (m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = 0;
	}


	// ��������ģ��
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = nullptr;
	}

	// ��������
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	bool  result;

	// ��ʼ�� message �ṹ��
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// �ڽ��յ�WM_QUIT���������ɣ������û��������������˳�ѭ��
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
			// ���л��ƣ�ͬʱ�����豸���룩
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
	
	// ����ǰ��ȡ�豸����
	result = m_Input->Frame();
	if (!result)
		return false;

	// ����û��Ƿ���escape��ϣ���˳�
	result = m_Input->IsEscapePressed();
	if (result)
		return false;

	//��ȡ����״̬
	const unsigned char(*keyboardState)[256] = m_Input->GetKeyboardState();

	// ��ȡ���(λ����)λ��
	int mouseX, mouseY;
	m_Input->GetMouseDeltaPosition(mouseX, mouseY);

	// ����ͼ��
	result = m_Graphics->Frame(mouseX, mouseY, keyboardState);
	if (!result)
		return false;

	// ���Ƶ���
	//result = m_Application->Frame();
	//if (!result)
	//{
	//	return false;
	//}


	return true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// ��ȡ��Ӧ�ó����ʵ��
	m_hinstance = GetModuleHandle(NULL);

	// Ϊ��Ӧ������
	m_applicationName = L"Engine";

	// ��������
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

	// ע�ᴰ����
	RegisterClassEx(&wc);

	// �ж��ͻ�������Ļ�ֱ���
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;
	int posX, posY;
	// ����Ϊȫ���򴰿�ģʽ
	if (FULL_SCREEN)
	{
		// ȫ��ģʽ������󻯣�����dmBitsPerPelΪ32λ
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
		screenWidth = 800;
		screenHeight = 600;

		// ������λ�����õ���Ļ����
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// �������ڲ���ȡ���
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// ��ʾ���沢����Ϊ��ǰ����
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// ��ʾ�����
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

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)	
	{
		// �׳�WM_QUIT
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// �׳�WM_QUIT
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}