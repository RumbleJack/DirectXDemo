////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


///////////////////////
// �����������ļ�    //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "bitmapclass.h"
#include "textclass.h"

#include "lightshaderclass.h"
#include "textureshaderclass.h"
#include "lightclass.h"

/////////////
// ȫ�ֱ��� //
/////////////
const bool	FULL_SCREEN = false;    
const bool	VSYNC_ENABLED = true;	//��ֱͬ��
const float SCREEN_DEPTH = 1000.0f; 
const float SCREEN_NEAR = 0.1f;

////////////////////////////////////////////////////////////////////////////////
// ����: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int);
	bool Render();

private:
	bool Render(float);

private:
	bool outputVideoCardInfo();

private:
	D3DClass*		m_Direct3D;
	CameraClass*	m_Camera;
	ModelClass*		m_Model;
	BitmapClass*	m_Bitmap;
	TextClass*		m_Text;

	TextureShaderClass* m_TextureShader;
	LightShaderClass* m_LightShader;
	LightClass*		m_Light;


};
