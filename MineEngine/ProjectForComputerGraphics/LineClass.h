// 文件名: LineClass.h
#pragma once

// 包含本工程文件  
#include <d3d11.h>
#include "textureclass.h"
#include <directxmath.h>
using namespace DirectX;
// 类名: LineClass
class LineClass
{
public:
	enum RENDER_STYLE {
		POINT,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
	};
private:
	//顶点着色器输入类型
	struct VertexType
	{
		XMFLOAT3 position;
	};

public:
	LineClass();
	LineClass(const LineClass&);
	~LineClass();
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*,
		int screenWidth, int screenHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext,RENDER_STYLE renderStyle = LINE_STRIP);
	
	bool UpdateBuffers(ID3D11DeviceContext*, RECT& rect, const float* vertexData, int vertexNum);
	bool UpdateRawBuffers(ID3D11DeviceContext*, RECT& rect, const float* vertexData, int vertexNum);

	bool UpdateColor(float red, float green, float blue);
	XMFLOAT4 GetColor();
	int	GetIndexCount();

private:
	// 初始化缓冲区、释放缓冲区、绘制缓冲区，对应于顶点着色器
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	

private:
	//顶点和索引
	ID3D11Buffer    *m_vertexBuffer, *m_indexBuffer;
	int				 m_validVertexCount, m_validIndexCount;
	static const int m_maxVertexCount = 5120;

	// 屏幕尺寸
	int m_screenWidth, m_screenHeight;
	// 划线颜色
	XMFLOAT4 m_pixelColor;
};
