#include "stdafx.h"
#include "GraphicsClass.h"

#include "StubGraphicClass.h"

StubGraphicsClass::StubGraphicsClass()
{
	m_Bitmap = nullptr;
	m_TextureShader = nullptr;
}

StubGraphicsClass::StubGraphicsClass(const StubGraphicsClass &)
{
}

StubGraphicsClass::~StubGraphicsClass()
{
}

bool StubGraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	GraphicsClass::Initialize(screenWidth, screenHeight, hwnd);

	bool result;

	// ������άͼ��
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
		return false;
	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		screenWidth, screenHeight, "../Engine/data/stone01.tga", 256, 256);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// ����TextureShaderClass
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
		return false;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void StubGraphicsClass::Shutdown()
{
	GraphicsClass::Shutdown();

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
}


bool StubGraphicsClass::Frame()
{
	bool result;
	// ����ͼ�񳡾�
	result = Render();
	if (!result)
		return false;
	return true;
}

bool StubGraphicsClass::Render()
{
	// ������濪ʼ���Ƴ���
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ����
	Render2D();

	// �����Ƴ�����ʾ����Ļ
	m_Direct3D->EndScene();
	return true;
}

bool StubGraphicsClass::Render2D()
{
	bool result;

	// ����2Dͼ��ǰ���ر�Z-Buffer���ر�z-buffer�󣬺���Ƶ�ͼ����ǰ���Ȼ��Ƶ�ͼ���ں󣻶�����z-buffer��z����С����ǰ��z�������ں�
	m_Direct3D->TurnZBufferOff();
	// ����άͼ�񶥵�������������ͼ�ιܵ���׼������
	RECT renderPos = { 0 - m_screenWidth / 2, m_screenHeight / 2, m_screenWidth / 2, -m_screenHeight / 2 };
	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), renderPos);
	if (!result)
		return false;
	// ʹ��������ɫ�����ƶ�άͼ��
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
		m_Bitmap->GetIndexCount(),
		m_basicWorldMatrix, m_basicViewMatrix, m_orthoProjectMatrix,
		m_Bitmap->GetTexture());
	if (!result)
		return false;
	// ����2Dͼ�κ󣬿���Z-Buffer
	m_Direct3D->TurnZBufferOn();

	// ����2Dͼ��ǰ���ر�Z-Buffer,��Alpha���
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOnAlphaBlending();

	// �˴���������

	// ����2Dͼ�κ󣬿���Z-Buffer,�ر�Alpha���
	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();

	return true;
}
