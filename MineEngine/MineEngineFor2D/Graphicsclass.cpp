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
	m_TextureShader = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
	m_Bitmap = nullptr;
	m_Text = 0;
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

	// 创建TextureShaderClass
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
		return false;

	// 初始化TextureShaderClass
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// 创建LightClass
	m_Light = new LightClass;
	if (!m_Light)
		return false;

	// 初始化LightClass
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// 创建二维图像
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
		return false;

	// 初始化二维图像
	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		screenWidth, screenHeight, "../Engine/data/stone01.tga", 256, 256);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

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

bool GraphicsClass::Frame(int mouseX, int mouseY)
{
	bool result;

	static float rotation = XM_PI * (float)0.0;
	// 每帧更新旋转变量
	rotation += XM_PI * 0.005f;
	if (rotation > XM_PI * 2)
		rotation -= 0.0f;

	// 设置鼠标位置
	result = m_Text->SetMousePosition(mouseX, mouseY, m_Direct3D->GetDeviceContext());
	if (!result)
		return false;

	// 绘制图像场景
	result = Render(rotation);
	if (!result)
		return false;
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;;
	bool result;

	// 清除缓存开始绘制场景
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 基于相机位置生成观察矩阵
	m_Camera->SetPosition(0.0f, 5.0f, -20.0f);
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
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	// 取得正交投影矩阵(以屏幕坐标为单位)
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 绘制2D图形前，关闭Z-Buffer（关闭z-buffer后，后绘制的图形在前，先绘制的图形在后；而开启z-buffer，z坐标小的在前，z坐标大的在后）
	m_Direct3D->TurnZBufferOff();

	// 将二维图像顶点和索引缓存放入图形管道，准备绘制
	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 300, 300);
	if (!result)
		return false;

	// 使用纹理着色器绘制二维图像
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result)
		return false;

	// 绘制2D图形后，开启Z-Buffer
	m_Direct3D->TurnZBufferOn();

	// 绘制2D图形前，关闭Z-Buffer,打开Alpha混合
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOnAlphaBlending();

	// Render the text strings.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
		return false;

	// 绘制2D图形后，开启Z-Buffer,关闭Alpha混合
	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();

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