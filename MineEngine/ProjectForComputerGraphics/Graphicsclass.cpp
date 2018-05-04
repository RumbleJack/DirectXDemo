// �ļ���: graphicsclass.cpp
#include "stdafx.h"
#include "graphicsclass.h"
#include <fstream>
#include <iomanip>
#include <dinput.h>
using namespace std;

#define PI 3.1415

struct STRUCT_ASCAN_DATA
{
	void Init()
	{
		Data = NULL;
		DataSize = 0;
		SoundDataRange = 0;
	}
	// һά���飬����ֵΪ����
	short* Data;
	// �����С
	long  DataSize;
	long SoundDataRange;
};
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
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
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

	// ����WorldClass
	m_World = new WorldClass;
	if (!m_World)
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

	// ����ModelClass2
	m_Model_2 = new ModelClass;
	if (!m_Model_2)
		return false;

	// ��ʼ��ModelClass
	result = m_Model_2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Engine/data/stone01.tga", "../Engine/data/sphere.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube object.", L"Error", MB_OK);
		return false;
	}
	// ����ModelClass2
	m_Model_3 = new ModelClass;
	if (!m_Model_3)
		return false;

	// ��ʼ��ModelClass
	result = m_Model_3->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "../Engine/data/stone01.tga", "../Engine/data/myname.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube object.", L"Error", MB_OK);
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

	//// ������άͼ��
	//m_Bitmap = new BitmapClass;
	//if (!m_Bitmap)
	//	return false;

	//// ��ʼ����άͼ��
	//result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
	//	screenWidth, screenHeight, "../Engine/data/stone01.tga", 256, 256);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
	//	return false;
	//}

	// Ϊ2άͼ�δ����۲����
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//// �����ı�����
	//m_Text = new TextClass;
	//if (!m_Text)
	//	return false;

	//// ��ʼ���ı�����
	//result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
	//	return false;
	//}

	//��ʼ���豸����ṹ��
	m_inputDeviceState = new InputDeviceState;

	m_LineShader = new LineShaderClass;
	if (!m_LineShader)
		return false;
	result = m_LineShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Line shader object.", L"Error", MB_OK);
		return false;
	}
	m_AScanLine = new LineClass;
	if (!m_AScanLine)
		return false;
	result = m_AScanLine->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the  Line  shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_AScanLine)
	{
		m_AScanLine->Shutdown();
		delete m_AScanLine;
		m_AScanLine = 0;
	}
	if (m_LineShader)
	{
		m_LineShader->Shutdown();
		delete m_LineShader;
		m_LineShader = 0;
	}

	// �ͷ�����״ָ̬��
	delete m_inputDeviceState;

	// �ͷ��ı�����
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

	// �ͷ� model ����
	if (m_Model_2)
	{
		m_Model_2->Shutdown();
		delete m_Model_2;
		m_Model_2 = 0;
	}

	// �ͷ� model ����
	if (m_Model_3)
	{
		m_Model_3->Shutdown();
		delete m_Model_3;
		m_Model_3 = 0;
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

bool GraphicsClass::Frame(int mouseX, int mouseY, const unsigned char(*keyboardState)[256])
{
	bool result;

	// ���������豸״̬
	SetInputDeviceState(mouseX, mouseY, keyboardState);

	// �������λ��
	//result = m_Text->SetMousePosition( mouseX, mouseY, m_Direct3D->GetDeviceContext());
	//if (!result)
	//	return false;

	// ����ͼ�񳡾�
	result = Render();
	if (!result)
		return false;
	return true;
}

bool GraphicsClass::Render()
{
	// ������濪ʼ���Ƴ���
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ������άͼ��
	//Render3D();

	// ���ƶ�άͼ��
	Render2D();

	// �����Ƴ�����ʾ����Ļ
	m_Direct3D->EndScene();
	return true;
}

bool GraphicsClass::Render3D()
{

	//Ϊÿһ֡����viewMatrix
	AdjustCameraParameter();
	m_Camera->Render();

	Render3D_1();
	Render3D_2();
	Render3D_3();

	return true;
}

bool GraphicsClass::Render3D_1()
{
	//���ƹ���
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// ÿ֡������ת����
	static float rotation = PI * (float)0.0;
	rotation += PI * 0.005f;
	if (rotation > PI * 2)
		rotation -= 0.0f;

	// ȡ�����磬�۲��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ������תWorld����
	m_World->SetRotation(0, rotation, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);

	// ������������������ͼ��ܵ�׼������
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// ʹ����ɫ������ģ��
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	// ����ƽ��World����
	m_World->SetPosition(15, -5, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);
	
	// ʹ����ɫ������ģ��
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	// ����ƽ��World����
	m_World->SetPosition(-15, -5, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);
	// ʹ����ɫ������ģ��
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	return true;
}

bool GraphicsClass::Render3D_2()
{
	//����С��
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// ȡ�����磬�۲��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ������������������ͼ��ܵ�׼������
	m_Model_2->Render(m_Direct3D->GetDeviceContext());

	for (int i = 10; i >= -10; i-=2)
	{
		for (int j = 30; j >= 0; j-=2)
		{
			// ����ƽ��World����
			m_World->SetPosition(j, 30 - j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);

			// ʹ����ɫ������ģ��
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;

			// ����ƽ��World����
			m_World->SetPosition(-j, -30 + j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// ʹ����ɫ������ģ��
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;

			// ����ƽ��World����
			m_World->SetPosition(-j, 30 - j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// ʹ����ɫ������ģ��
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;

			// ����ƽ��World����
			m_World->SetPosition(j, -30 + j, i);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// ʹ����ɫ������ģ��
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

			// ����ƽ��World����
			m_World->SetPosition(j, k, 10);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// ʹ����ɫ������ģ��
			result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model_2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			if (!result)
				return false;


			// ����ƽ��World����
			m_World->SetPosition(-j, k, 10);
			m_World->Render();
			m_World->GetWorldMatrix(worldMatrix);
			// ʹ����ɫ������ģ��
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
	//�����ν���
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// ȡ�����磬�۲��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ������������������ͼ��ܵ�׼������
	m_Model_3->Render(m_Direct3D->GetDeviceContext());

	// ����ƽ��World����
	m_World->SetPosition(0, 40, 0);
	m_World->Render();
	m_World->GetWorldMatrix(worldMatrix);

	// ʹ����ɫ������ģ��
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model_3->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model_3->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
		return false;

	return true;
}

bool GraphicsClass::RenderScanData()
{
	m_Direct3D->TurnZBufferOff();

	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;;
	bool result;

	// ȡ�����磬�۲��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��������Aɨ�����ݹ����
	const int	 AScanVertexSize = 3000;
	int			 renderSize = 0;
	static float AScanVertexBuffer[AScanVertexSize * 2];

	// ����Aɨ���߻�������
	LONG left = -m_screenWidth / 2 ;
	LONG top = m_screenHeight / 2 ;
	LONG right = m_screenWidth / 2 ;
	LONG bottom = -m_screenHeight / 2 ;
	RECT renderPos = { left, top, right,bottom };

	// ȡ��Aɨ������
	const STRUCT_ASCAN_DATA * pAScanData = GetAndLockAScanData();
	if (pAScanData == NULL) {
		return true;
	}
	result = InitAScanVertexData(pAScanData, AScanVertexBuffer, renderSize);
	if (!result)
		return false;

	// ���²�����Aɨ������
	result = m_AScanLine->UpdateBuffers(m_Direct3D->GetDeviceContext(), renderPos, AScanVertexBuffer, renderSize);
	if (!result)
		return false;
	result = m_AScanLine->UpdateColor(1.0f, 1.0f, 0.0f);
	if (!result)
		return false;
	result = m_AScanLine->Render(m_Direct3D->GetDeviceContext(), LineClass::LINE_STRIP);
	if (!result)
		return false;
	result = m_LineShader->Render(m_Direct3D->GetDeviceContext(),
		m_AScanLine->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_AScanLine->GetColor());
	if (!result)
		return false;
	m_Direct3D->TurnZBufferOn();
	return true;
}

bool GraphicsClass::InitAScanVertexData(const STRUCT_ASCAN_DATA * pAScanData, float * AScanVertexBuffer, int & renderSize)
{
	long dataSize = pAScanData->DataSize;
	const short* AScanData = pAScanData->Data;
	long AScanDataRange = pAScanData->SoundDataRange;

	long intvBegin = 0;
	long intvWidth = 100;
	if (dataSize > 3000)
	{
		return false;
	}
	// ÿ���׶�ӦAɨ�����ݵ���
	float sampleRate = static_cast<float>(dataSize) / AScanDataRange;
	// ��Ҫ��������Ļ�ϵ�������ʼ����ֹ����
	int offsetAScanDataIndex = static_cast<int>(sampleRate * intvBegin);
	int endAScanDataIndex = min(static_cast<int>(sampleRate * (intvBegin + intvWidth)), dataSize);

	// ��Ļ�Զ��ӦAɨ������
	int screenWidth = static_cast<int>(sampleRate * intvWidth);
	// ����������ݶ���
	for (int AScanDataIndex = offsetAScanDataIndex; AScanDataIndex < endAScanDataIndex; AScanDataIndex++)
	{
		AScanVertexBuffer[2 * (AScanDataIndex - offsetAScanDataIndex)]
			= static_cast<float>((AScanDataIndex - offsetAScanDataIndex)) / screenWidth;
		AScanVertexBuffer[2 * (AScanDataIndex - offsetAScanDataIndex) + 1]
			= static_cast<float>(AScanData[AScanDataIndex]) / 512;
	}

	renderSize = endAScanDataIndex - offsetAScanDataIndex;
	return true;
}

STRUCT_ASCAN_DATA * GraphicsClass::GetAndLockAScanData()
{
	const int AScanVertexSize = 300;
	short * AScanDataBuff = new short[AScanVertexSize];

	static STRUCT_ASCAN_DATA *backBufer = new STRUCT_ASCAN_DATA;
	static STRUCT_ASCAN_DATA *frontBufer = new STRUCT_ASCAN_DATA;

	// ����ÿһ֡������
	static float offset = 0;
	offset += 0.01f;
	for (int i = 0; i < AScanVertexSize; i++) {
		AScanDataBuff[i] = static_cast<short>(512.0f * abs(
			sin(static_cast<float>(i) * (2 * static_cast<float>(PI) / AScanVertexSize) + offset)
		));
	}

	// ��ʾǰ����
	backBufer->Data = static_cast<short*>(AScanDataBuff);
	backBufer->DataSize = AScanVertexSize;
	backBufer->SoundDataRange = 120;

	// ����ǰ�󻺳塢����ʾǰ����
	swap(backBufer, frontBufer);
	return frontBufer;
}

bool GraphicsClass::Render2D()
{
	RenderScanData();
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;;
	bool result;

	// ȡ�����磬�۲��ͶӰ����
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);


	// ����2Dͼ��ǰ���ر�Z-Buffer���ر�z-buffer�󣬺���Ƶ�ͼ����ǰ���Ȼ��Ƶ�ͼ���ں󣻶�����z-buffer��z����С����ǰ��z�������ں�
	m_Direct3D->TurnZBufferOff();
	// ����άͼ�񶥵�������������ͼ�ιܵ���׼������
	//result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 300, 300);
	//if (!result)
	//	return false;
	//// ʹ��������ɫ�����ƶ�άͼ��
	//result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	//if (!result)
	//	return false;
	// ����2Dͼ�κ󣬿���Z-Buffer
	m_Direct3D->TurnZBufferOn();


	// ����2Dͼ��ǰ���ر�Z-Buffer,��Alpha���
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOnAlphaBlending();
	// �����ı��ַ���
	//result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	//if (!result)
	//	return false;
	// ����2Dͼ�κ󣬿���Z-Buffer,�ر�Alpha���
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
	// ���嵱ǰ������λ��( ��ʼ�����λ�� )
	static float deltaCameraPositionX = 0.0;
	static float deltaCameraPositionY = 5.0;
	static float deltaCameraPositionZ = -20.0f;

	// ��ǰ�������������ϵ�����λ��
	float speed = 0.5;
	if (m_inputDeviceState->keyboardState[0][DIK_W] & 0x80)
		deltaCameraPositionZ = 1.0 *speed;
	if (m_inputDeviceState->keyboardState[0][DIK_S] & 0x80)
		deltaCameraPositionZ = -1.0 *speed;
	if (m_inputDeviceState->keyboardState[0][DIK_A] & 0x80)
		deltaCameraPositionX = -1.0 *speed;
	if (m_inputDeviceState->keyboardState[0][DIK_D] & 0x80)
		deltaCameraPositionX = 1.0 *speed;

	// ���嵱ǰ�����ת��
	static float curCameraRotationX = 0.0f;
	static float curCameraRotationY = 0.0f;
	static float curCameraRotationZ = 0.0f;

	// �������������������ϵ����ת��(��λ����Ϊ�Ƕ�)
	curCameraRotationX += m_inputDeviceState->mouseDeltaY / 10;
	curCameraRotationY += m_inputDeviceState->mouseDeltaX / 10;

	// �������λ�ú���ת��
	m_Camera->UpdePositionFromWorldToCamera(  deltaCameraPositionX, deltaCameraPositionY, deltaCameraPositionZ);
	m_Camera->SetRotation(curCameraRotationX, curCameraRotationY, curCameraRotationZ);

	// ��ÿһ֡�����λ������λ
	deltaCameraPositionX = 0.0f;
	deltaCameraPositionY = 0.0f;
	deltaCameraPositionZ = 0.0f;

	return true;
}

