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

	// 创建二维图像
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

	// 创建TextureShaderClass
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
	// 绘制图像场景
	result = Render();
	if (!result)
		return false;
	return true;
}

bool StubGraphicsClass::Render()
{
	// 清除缓存开始绘制场景
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 绘制
	Render2D();

	// 将绘制场景显示到屏幕
	m_Direct3D->EndScene();
	return true;
}

bool StubGraphicsClass::Render2D()
{
	bool result;

	// 绘制2D图形前，关闭Z-Buffer（关闭z-buffer后，后绘制的图形在前，先绘制的图形在后；而开启z-buffer，z坐标小的在前，z坐标大的在后）
	m_Direct3D->TurnZBufferOff();
	// 将二维图像顶点和索引缓存放入图形管道，准备绘制
	RECT renderPos = { 0 - m_screenWidth / 2, m_screenHeight / 2, m_screenWidth / 2, -m_screenHeight / 2 };
	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), renderPos);
	if (!result)
		return false;
	// 使用纹理着色器绘制二维图像
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
		m_Bitmap->GetIndexCount(),
		m_basicWorldMatrix, m_basicViewMatrix, m_orthoProjectMatrix,
		m_Bitmap->GetTexture());
	if (!result)
		return false;
	// 绘制2D图形后，开启Z-Buffer
	m_Direct3D->TurnZBufferOn();

	// 绘制2D图形前，关闭Z-Buffer,打开Alpha混合
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOnAlphaBlending();

	// 此处绘制文字

	// 绘制2D图形后，开启Z-Buffer,关闭Alpha混合
	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();

	return true;
}
