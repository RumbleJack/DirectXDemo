// �ļ���: graphicsclass.h
#pragma once

// �����������ļ�  
#include "d3dclass.h"
#include "cameraclass.h"
#include "worldclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "textureshaderclass.h"
#include "LineClass.h"
#include "LineShaderClass.h"

// ����: GraphicsClass
class DllExport GraphicsClass
{
private:
	//��¼�����豸״̬���������̺���꣩
	struct InputDeviceState
	{
		InputDeviceState() :keyboardState(NULL), mouseDeltaX(0), mouseDeltaY(){}
		~InputDeviceState()
		{
			if (keyboardState) {
				keyboardState = NULL;
				delete []keyboardState;
			}
		}
		const unsigned char (*keyboardState)[256]; // ���������ָ��
		int mouseDeltaX;
		int mouseDeltaY;
	};
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	virtual ~GraphicsClass();

protected:
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame() ;	

	// �����ⲿ�����豸״̬
	void SetInputDeviceState(int, int, const unsigned char(*keyboardState)[256]);
	bool Frame(int mouseX, int mouseY, const unsigned char(*keyboardState)[256]);
	bool Render();
	virtual bool Render2D() = 0;
	// ����Կ���Ϣ
	bool outputVideoCardInfo();
protected:
	// ����D3D���ƻ����ĳ�ʼ��
	D3DClass*		m_Direct3D;
	
	// ���硢�������ͶӰ����Ϊ������������D3DClass�У�
	WorldClass*		m_World;
	CameraClass*	m_Camera;
	
	// �ı�������
	TextClass*			m_Text;
	// ��άͼ����ɫ����
	TextureShaderClass* m_TextureShader;
	LineShaderClass   * m_LineShader;

	// ���ڶ�ά���Ƶľ���
	XMMATRIX m_basicWorldMatrix;
	XMMATRIX m_basicViewMatrix;
	XMMATRIX m_orthoProjectMatrix;

	// ��DirectX Input������豸״̬
	InputDeviceState* m_inputDeviceState;

	int m_screenWidth;
	int m_screenHeight;
};
