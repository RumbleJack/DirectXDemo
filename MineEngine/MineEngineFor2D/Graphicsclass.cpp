////////////////////////////////////////////////////////////////////////////////
// �ļ���: graphicsclass.cpp
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

	// ����D3DClass����
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
		return false;

	// ��ʼ��D3DClass����
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//����Կ���Ϣ
	outputVideoCardInfo();

	// ����CameraClass
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// ����ModelClass
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// ��ʼ��ModelClass
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Engine/data/lena.tga","../Engine/data/model.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	// ����LightShaderClass
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
		return false;

	// ��ʼ��LightShaderClass
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// ����TextureShaderClass
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
		return false;

	// ��ʼ��TextureShaderClass
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// ����LightClass
	m_Light = new LightClass;
	if (!m_Light)
		return false;

	// ��ʼ��LightClass
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// ������άͼ��
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
		return false;

	// ��ʼ����άͼ��
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

	// �ͷ� LightClass ����
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// �ͷ� LightShaderClass ����
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// �ͷ� model ����
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// �ͷ� camera ����
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	// �ͷ� Direct3D ����.
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
	// ÿ֡������ת����
	rotation += XM_PI * 0.005f;
	if (rotation > XM_PI * 2)
		rotation -= 0.0f;

	// �������λ��
	result = m_Text->SetMousePosition(mouseX, mouseY, m_Direct3D->GetDeviceContext());
	if (!result)
		return false;

	// ����ͼ�񳡾�
	result = Render(rotation);
	if (!result)
		return false;
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;;
	bool result;

	// ������濪ʼ���Ƴ���
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// �������λ�����ɹ۲����
	m_Camera->SetPosition(0.0f, 5.0f, -20.0f);
	m_Camera->Render();

	// ȡ�����磬�۲��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ��תworld������ᵼ�»��ƶ������ת
	worldMatrix = XMMatrixRotationY(rotation);

	// ������������������ͼ��ܵ�׼������
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// ʹ����ɫ������ģ��
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	// ȡ������ͶӰ����(����Ļ����Ϊ��λ)
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ����2Dͼ��ǰ���ر�Z-Buffer���ر�z-buffer�󣬺���Ƶ�ͼ����ǰ���Ȼ��Ƶ�ͼ���ں󣻶�����z-buffer��z����С����ǰ��z�������ں�
	m_Direct3D->TurnZBufferOff();

	// ����άͼ�񶥵�������������ͼ�ιܵ���׼������
	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 300, 300);
	if (!result)
		return false;

	// ʹ��������ɫ�����ƶ�άͼ��
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result)
		return false;

	// ����2Dͼ�κ󣬿���Z-Buffer
	m_Direct3D->TurnZBufferOn();

	// ����2Dͼ��ǰ���ر�Z-Buffer,��Alpha���
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOnAlphaBlending();

	// Render the text strings.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
		return false;

	// ����2Dͼ�κ󣬿���Z-Buffer,�ر�Alpha���
	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();

	// �����Ƴ�����ʾ����Ļ
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