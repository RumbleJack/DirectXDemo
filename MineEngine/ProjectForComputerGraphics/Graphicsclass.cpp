// 文件名: graphicsclass.cpp

#include "graphicsclass.h"
#include <fstream>
#include <iomanip>
#include <dinput.h>
using namespace std;

#define PI 3.1415

GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_World = nullptr;
	m_Model = nullptr;
	m_TextureShader = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
	m_Bitmap = nullptr;
	m_inputDeviceState = nullptr;
	m_Text = nullptr;


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

	// 创建WorldClass
	m_World = new WorldClass;
	if (!m_World)
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

	// 创建ModelClass2
	m_Model_2 = new ModelClass;
	if (!m_Model_2)
		return false;

	// 初始化ModelClass
	result = m_Model_2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Engine/data/stone01.tga", "../Engine/data/sphere.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube object.", L"Error", MB_OK);
		return false;
	}
	// 创建ModelClass2
	m_Model_3 = new ModelClass;
	if (!m_Model_3)
		return false;

	// 初始化ModelClass
	result = m_Model_3->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Engine/data/stone01.tga", "../Engine/data/myname.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube object.", L"Error", MB_OK);
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

	//// 创建二维图像
	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap)
	//	return false;

	//// 初始化二维图像
	//result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
	//	screenWidth, screenHeight, "../Engine/data/stone01.tga", 256, 256);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
	//	return false;
	//}

	// 为2维图形创建观察矩阵
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//// 创建文本对象
	//m_Text = new TextClass;
	//if (!m_Text)
	//	return false;

	//// 初始化文本对象
	//result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
	//	return false;
	//}

	//初始化设备输入结构体
	m_inputDeviceState = new InputDeviceState;

	return true;
}

void GraphicsClass::Shutdown()
{

	// 释放输入状态指针
	delete m_inputDeviceState;

	// 释放文本对象
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

	// 释放 model 对象
	if (m_Model_2)
	{
		m_Model_2->Shutdown();
		delete m_Model_2;
		m_Model_2 = 0;
	}

	// 释放 model 对象
	if (m_Model_3)
	{
		m_Model_3->Shutdown();
		delete m_Model_3;
		m_Model_3 = 0;
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

bool GraphicsClass::Frame(int mouseX, int mouseY, const unsigned char(*keyboardState)[256])
{
	bool result;

	// 设置输入设备状态
	SetInputDeviceState(mouseX, mouseY, keyboardState);

	// 设置鼠标位置
	//result = m_Text->SetMousePosition( mouseX, mouseY, m_Direct3D->GetDeviceContext());
	//if (!result)
	//	return false;

	// 绘制图像场景
	result = Render();
	if (!result)
		return false;
	return true;
}

bool GraphicsClass::Render()
{
	// 清除缓存开始绘制场景
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 绘制三维图形
	Render3D();

	// 绘制二维图形
	//Render2D();

	// 将绘制场景显示到屏幕
	m_Direct3D->EndScene();
	return true;
}

bool GraphicsClass::Render3D()
{

	//为每一帧调整viewMatrix
	AdjustCameraParameter();
	m_Camera->Render();

	Render3D_1();
	Render3D_2();
	Render3D_3();

	return true;
}

bool GraphicsClass::Render3D_1()
{
	//绘制怪物
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 每帧更新旋转变量
	static float rotation = PI * (float)0.0;
	rotation += PI * 0.005f;
	if (rotation > PI * 2)
		rotation -= 0.0f;

	// 取得世界，观察和投影矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 创建旋转World矩阵
	m_World->SetRotation(0, rotation, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);

	// 将顶点和索引缓冲放入图像管道准备绘制
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 使用着色器绘制模型
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	// 创建平移World矩阵
	m_World->SetPosition(15, -5, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);
	
	// 使用着色器绘制模型
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	// 创建平移World矩阵
	m_World->SetPosition(-15, -5, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);
	// 使用着色器绘制模型
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	return true;
}

bool GraphicsClass::Render3D_2()
{
	//绘制小球
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 取得世界，观察和投影矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 将顶点和索引缓冲放入图像管道准备绘制
	m_Model_2->Render(m_Direct3D->GetDeviceContext());

	for (int i = 10; i >= -10; i-=2)
	{
		for (int j = 30; j >= 0; j-=2)
		{
			// 创建平移World矩阵
			m_World->SetPosition(j, 30 - j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);

			// 使用着色器绘制模型
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;

			// 创建平移World矩阵
			m_World->SetPosition(-j, -30 + j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// 使用着色器绘制模型
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;

			// 创建平移World矩阵
			m_World->SetPosition(-j, 30 - j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// 使用着色器绘制模型
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;

			// 创建平移World矩阵
			m_World->SetPosition(j, -30 + j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// 使用着色器绘制模型
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;
		}
	}
	for (int j = 30; j >= 0; j -= 2)
	{
		for (int k = -(30 - j); k < 30 - j; k += 2)
		{

			// 创建平移World矩阵
			m_World->SetPosition(j, k, 10);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// 使用着色器绘制模型
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;


			// 创建平移World矩阵
			m_World->SetPosition(-j, k, 10);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// 使用着色器绘制模型
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;
		}
	}
	return true;
}

bool GraphicsClass::Render3D_3()
{
	//绘制任杰文
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 取得世界，观察和投影矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 将顶点和索引缓冲放入图像管道准备绘制
	m_Model_3->Render(m_Direct3D->GetDeviceContext());

	// 创建平移World矩阵
	m_World->SetPosition(0, 40, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);

	// 使用着色器绘制模型
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_3->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model_3->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	return true;
}

bool GraphicsClass::Render2D()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;;
	bool result;

	// 取得世界，观察和投影矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
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
	// 绘制文本字符串
	//result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	//if (!result)
	//	return false;
	// 绘制2D图形后，开启Z-Buffer,关闭Alpha混合
	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();

	return true;
}

bool GraphicsClass::SetInputDeviceState(int mouseDeltaX, int mouseDeltaY, const unsigned char(*keyboardState)[256])
{
	m_inputDeviceState->mouseDeltaX = mouseDeltaX;
	m_inputDeviceState->mouseDeltaY = mouseDeltaY;
	m_inputDeviceState->keyboardState = keyboardState;

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

bool GraphicsClass::AdjustCameraParameter()
{
	// 定义当前相机相对位移( 初始化相机位置 )
	static float deltaCameraPositionX = 0.0;
	static float deltaCameraPositionY = 5.0;
	static float deltaCameraPositionZ = -20.0f;

	// 当前相机在世界坐标系的相对位移
	float speed = 0.5;
	if (m_inputDeviceState->keyboardState[0][DIK_W] & 0x80)
		deltaCameraPositionZ = 1.0 *speed;
	if (m_inputDeviceState->keyboardState[0][DIK_S] & 0x80)
		deltaCameraPositionZ = -1.0 *speed;
	if (m_inputDeviceState->keyboardState[0][DIK_A] & 0x80)
		deltaCameraPositionX = -1.0 *speed;
	if (m_inputDeviceState->keyboardState[0][DIK_D] & 0x80)
		deltaCameraPositionX = 1.0 *speed;

	// 定义当前相机旋转量
	static float curCameraRotationX = 0.0f;
	static float curCameraRotationY = 0.0f;
	static float curCameraRotationZ = 0.0f;

	// 计算在相对于世界坐标系的旋转量(单位假设为角度)
	curCameraRotationX += m_inputDeviceState->mouseDeltaY / 10;
	curCameraRotationY += m_inputDeviceState->mouseDeltaX / 10;

	// 更新相机位置和旋转角
	m_Camera->UpdePositionFromWorldToCamera(  deltaCameraPositionX, deltaCameraPositionY, deltaCameraPositionZ);
	m_Camera->SetRotation(curCameraRotationX, curCameraRotationY, curCameraRotationZ);

	// 将每一帧的相对位移量复位
	deltaCameraPositionX = 0.0f;
	deltaCameraPositionY = 0.0f;
	deltaCameraPositionZ = 0.0f;

	return true;
}