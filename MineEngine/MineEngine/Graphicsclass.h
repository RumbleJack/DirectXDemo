////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


///////////////////////
// 包含本工程文件    //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"

/////////////
// GLOBALS //
/////////////
const bool	FULL_SCREEN = false;
//垂直同步
const bool	VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render(float);
	bool outputVideoCardInfo();
private:
	D3DClass*		m_Direct3D;
	CameraClass*	m_Camera;
	ModelClass*		m_Model;
	LightShaderClass* m_LightShader;
	LightClass*		m_Light;
};
