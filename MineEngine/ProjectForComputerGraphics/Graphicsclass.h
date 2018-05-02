// �ļ���: graphicsclass.h
#pragma once

// �����������ļ�  
#include "d3dclass.h"

#include "cameraclass.h"
#include "worldclass.h"

#include "modelclass.h"
#include "bitmapclass.h"
#include "textclass.h"

#include "lightshaderclass.h"
#include "textureshaderclass.h"

#include "lightclass.h"
#include "header.h"

// ����: GraphicsClass
class GraphicsClass
{
private:
	//��¼�����豸״̬���������̺���꣩
	struct InputDeviceState
	{
		const unsigned char (*keyboardState)[256]; // const pionter of array 
		int mouseDeltaX;
		int mouseDeltaY;
	};
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, const unsigned char (*keyboardState)[256]);

private:
	bool SetInputDeviceState(int, int, const unsigned char(*keyboardState)[256]);

	bool Render();
	bool Render3D();
	bool Render2D();

private:
	bool Render3D_1();
	bool Render3D_2();
	bool Render3D_3();

	bool AdjustCameraParameter();

	bool outputVideoCardInfo();

private:
	D3DClass*		m_Direct3D;
	CameraClass*	m_Camera;
	WorldClass*		m_World;

	ModelClass*		m_Model;
	ModelClass*		m_Model_2;
	ModelClass*		m_Model_3;
	BitmapClass*	m_Bitmap;
	TextClass*		m_Text;

	TextureShaderClass* m_TextureShader;
	LightShaderClass*   m_LightShader;

	LightClass*		m_Light;
	// ��¼�����豸״̬���������̺���꣩
	InputDeviceState * m_inputDeviceState;

};
