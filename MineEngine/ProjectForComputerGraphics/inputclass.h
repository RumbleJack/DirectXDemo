// 类名: inputclass.h
#pragma once 

// PRE-PROCESSING DIRECTIVES 知名directInput的版本
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

//为了返回键盘状态，定义了一个常量数组指针（不知道该怎么直接定义函数返回值为数组指针，只好用typedef。。。）
typedef const unsigned char(*PCSTR256)[256];


// 类名: InputClass
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	void GetMouseDeltaPosition(int& DeltaX, int& DeltaY);
	PCSTR256 GetKeyboardState() const;

	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsAPressed();
	bool IsZPressed();
	bool IsPgUpPressed();
	bool IsPgDownPressed();

	bool IsF1Toggled();
	bool IsF2Toggled();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8*			m_directInput;
	IDirectInputDevice8*	m_keyboard;
	IDirectInputDevice8*	m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE  m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

	bool m_F1_released;
	bool m_F2_released;
};
