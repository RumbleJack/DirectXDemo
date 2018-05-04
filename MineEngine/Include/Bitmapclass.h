// 文件名: BitmapClass.h
#pragma once

// 包含本工程文件  
#include "textureclass.h"

// 类名: BitmapClass
class DllExport BitmapClass
{
private:
	//顶点着色器输入类型
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	// 初始化、释放、绘制模型
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, 
		int screenWidth, int screenHeight, char* textureFilename,
		int bitmapWidth, int bitmapHeight);
	void Shutdown();
	// rect通过像素坐标指定绘制矩形位置
	bool Render(ID3D11DeviceContext* deviceContext, RECT& rect);
	bool UpdateTexture(ID3D11Device*, ID3D11DeviceContext*, unsigned char* textureFilename,
		int bitmapWidth, int bitmapHeight);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	void GetScreenSize(int&width, int& height);

private:
	// 初始化缓冲区、释放缓冲区、绘制缓冲区，对应于顶点着色器
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	// 调整绘制矩形位置
	bool UpdateBuffers(ID3D11DeviceContext*, RECT& rect);

	// 载入和释放模型纹理数据，对应于像素（片元）着色器
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	//顶点和索引
	ID3D11Buffer    *m_vertexBuffer, *m_indexBuffer;
	int				m_vertexCount, m_indexCount;
	//纹理
	TextureClass*	m_Texture;

	//绘制二维图像所需的：屏幕尺寸、图像尺寸
	int m_screenWidth,  m_screenHeight;
	int m_bitmapWidth,  m_bitmapHeight;
	// 最近一次绘制位置
	RECT m_previousPosRect;
};
