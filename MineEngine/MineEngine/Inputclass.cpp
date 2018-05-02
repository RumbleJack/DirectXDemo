////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	// 初始化所有键状态为未被按下
	for (int i = 0; i<256; i++)
	{
		m_keys[i] = false;
	}
	return;
}


void InputClass::KeyDown(unsigned int input)
{
	// 保存按键状态
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// 清除按键状态
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// 判断按键状态
	return m_keys[key];
}