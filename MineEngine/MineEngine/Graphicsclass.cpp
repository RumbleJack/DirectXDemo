////////////////////////////////////////////////////////////////////////////////
// 文件名: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "graphicsclass.h"
#include <fstream>
#include <iomanip>
using namespace std;

GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
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

	// 初始化D3DClass对象
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//输出显卡信息
	outputVideoCardInfo();

	// 创建CameraClass
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// 设置相机初始位置
	m_Camera->SetPosition(0.0f, 5.0f, -20.0f);

	// 创建ModelClass
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// 初始化ModelClass
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Engine/data/lena.tga","../Engine/data/model.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	// 创建LightShaderClass
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
		return false;

	// 初始化LightShaderClass
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// 创建LightClass
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// 初始化LightClass
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
	return true;
}

void GraphicsClass::Shutdown()
{
	// 释放 LightClass 对象
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// 释放 LightShaderClass 对象
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// 释放 model 对象
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 释放 camera 对象
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	// 释放 Direct3D 对象.
	if (m_Direct3D != nullptr)
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

	static float rotation = 0.0f;

	// 每帧更新旋转变量
	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// 绘制图像场景
	result = Render(rotation);
	if (!result)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 清除缓存开始绘制场景
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 基于相机位置生成观察矩阵
	m_Camera->Render();

	// 取得世界，观察和投影矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 旋转world矩阵，这会导致绘制对象的旋转
	worldMatrix = XMMatrixRotationY(rotation);

	// 将顶点和索引缓冲放入图像管道准备绘制
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 使用着色器绘制模型
	// Render the model using the light shader.

	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
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
	return true;
}