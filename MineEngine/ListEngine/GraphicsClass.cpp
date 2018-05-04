////////////////////////////////////////////////////////////////////////////////
// 文件名: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"


#include "graphicsclass.h"

#include <fstream>
#include <iomanip>
using namespace std;

GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_World = nullptr;
	m_Camera = nullptr;

	m_inputDeviceState = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// 创建D3DClass对象
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
		return false;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	m_Direct3D->GetOrthoMatrix(m_orthoProjectMatrix);

	// 创建文本对象
	m_Text = new TextClass;
	if (!m_Text)
		return false;
	// 初始化文本对象
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
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

	m_LineShader = new LineShaderClass;
	if (!m_LineShader)
		return false;
	result = m_LineShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Line shader object.", L"Error", MB_OK);
		return false;
	}
	

	// 创建WorldClass
	m_World = new WorldClass;
	if (!m_World)
		return false;
	m_World->GetDefaultWorldMatrix(m_basicWorldMatrix);

	// 创建CameraClass
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;
	// 为二维图形绘制计算固定的相机矩阵
	m_Camera->GetBasicViewMatrix(m_basicViewMatrix);

	//初始化设备输入结构体
	m_inputDeviceState = new InputDeviceState;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_inputDeviceState)
	{
		delete m_inputDeviceState;
		m_inputDeviceState = 0;
	}


	// 释放 world 对象
	if (m_World)
	{
		delete m_World;
		m_World = 0;
	}
	// 释放 camera 对象
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	
	if (m_LineShader)
	{
		m_LineShader->Shutdown();
		delete m_LineShader;
		m_LineShader = 0;
	}
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	// 释放 Direct3D 对象.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	// 绘制图像场景
	result = Render();
	if (!result)
		return false;

	return true;
}

void GraphicsClass::SetInputDeviceState(int mouseDeltaX, int mouseDeltaY, const unsigned char(*keyboardState)[256])
{
	m_inputDeviceState->mouseDeltaX = mouseDeltaX;
	m_inputDeviceState->mouseDeltaY = mouseDeltaY;
	m_inputDeviceState->keyboardState = keyboardState;

	return;
}

bool GraphicsClass::Frame(int mouseX, int mouseY, const unsigned char(*keyboardState)[256])
{
	bool result;

	// 设置输入设备状态
	SetInputDeviceState(mouseX, mouseY, keyboardState);

	// 绘制图像场景
	result = Frame();
	if (!result)
		return false;
	return true;
}

bool GraphicsClass::Render()
{
	// 清除后缓存（指定后缓存清除颜色）、深度模板缓存
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	if (!Render2D())
		return false;

	// 将绘制场景显示到屏幕
	m_Direct3D->EndScene();
	return true;
}

bool GraphicsClass::outputVideoCardInfo()
{
	ofstream outFile("videoCardInfo.txt");
	if (!outFile)
		return false;
	char * videoCardName = new char[129];
	int videoCardMemory = 0;
	m_Direct3D->GetVideoCardInfo(videoCardName, videoCardMemory);
	outFile << "videoCardName" << "          " << videoCardName << endl
		<< "videoCardMemory(MB)" << "          " << videoCardMemory << endl;
	outFile.close();
	delete []videoCardName;
	return true;
}
