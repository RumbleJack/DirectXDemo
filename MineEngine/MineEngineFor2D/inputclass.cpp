////////////////////////////////////////////////////////////////////////////////
// 文件名: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"

InputClass::InputClass()
{
	m_directInput = nullptr;
	m_keyboard = nullptr;
	m_mouse = nullptr;
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// 存储屏幕尺寸，用于定位鼠标光标
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 初始化鼠标在屏幕的位置
	m_mouseX = 0;
	m_mouseY = 0;

	// 初始化DirectInput接口
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
		return false;

	// 初始化DirectInput键盘设备
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// 设置数据格式，这里使用预定义数据格式
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;
	
	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
		return false;

	// 获取键盘
	result = m_keyboard->Acquire();
	if(FAILED(result))
		return false;

	// 初始化DirectInput鼠标设备
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
		return false;

	// 设置数据格式，这里使用预定义数据格式
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
		return false;

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// 获取鼠标
	result = m_mouse->Acquire();
	if(FAILED(result))
		return false;

	return true;
}

void InputClass::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = nullptr;
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = nullptr;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = nullptr;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
		return false;

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
		return false;

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// 读取键盘设备
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
		
	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	// 读取鼠标设备
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// 更新鼠标位置
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// 确保鼠标位置在屏幕内
	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }
	
	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
	
	return;
}

bool InputClass::IsEscapePressed()
{
	// 检查escape键是否被按下
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
		return true;
	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}