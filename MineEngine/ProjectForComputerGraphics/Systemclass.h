// �ļ���: systemclass.h
#pragma once

// Ԥ����ָ��  ���ٱ�����̣�ͨ���ų�һЩ�����õ�API����СWin32ͷ�ļ��Ĵ�С
#define WIN32_LEAN_AND_MEAN

// �����ļ� 
#include <windows.h>

// �����ڰ����ļ� 
#include "inputclass.h"
#include "graphicsclass.h"
#include "applicationclass.h"

// ����: SystemClass
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
	// Ӧ�ó�������
	LPCWSTR			m_applicationName;
	// Ӧ�ó����ʵ��
	HINSTANCE		m_hinstance;
	// ���ھ��
	HWND			m_hwnd;

	//�ֱ����ڴ����豸�����ͼ�����
	InputClass*		m_Input;
	GraphicsClass*	m_Graphics;

	// ���Ƶ���
	ApplicationClass* m_Application;
};

// Windows���ڻص�����
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


