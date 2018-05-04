// 文件名: graphicsclass.h
#pragma once

// 包含本工程文件  
#include "d3dclass.h"
#include "cameraclass.h"
#include "worldclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "textureshaderclass.h"
#include "LineClass.h"
#include "LineShaderClass.h"

// 类名: GraphicsClass
class DllExport GraphicsClass
{
private:
	//记录输入设备状态（包括键盘和鼠标）
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
		const unsigned char (*keyboardState)[256]; // 常量数组的指针
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

	// 设置外部输入设备状态
	void SetInputDeviceState(int, int, const unsigned char(*keyboardState)[256]);
	bool Frame(int mouseX, int mouseY, const unsigned char(*keyboardState)[256]);
	bool Render();
	virtual bool Render2D() = 0;
	// 输出显卡信息
	bool outputVideoCardInfo();
protected:
	// 负责D3D绘制环境的初始化
	D3DClass*		m_Direct3D;
	
	// 世界、相机矩阵（投影矩阵为常量，定义在D3DClass中）
	WorldClass*		m_World;
	CameraClass*	m_Camera;
	
	// 文本绘制类
	TextClass*			m_Text;
	// 二维图形着色器类
	TextureShaderClass* m_TextureShader;
	LineShaderClass   * m_LineShader;

	// 用于二维绘制的矩阵
	XMMATRIX m_basicWorldMatrix;
	XMMATRIX m_basicViewMatrix;
	XMMATRIX m_orthoProjectMatrix;

	// 由DirectX Input读入的设备状态
	InputDeviceState* m_inputDeviceState;

	int m_screenWidth;
	int m_screenHeight;
};
