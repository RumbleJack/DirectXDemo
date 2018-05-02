// 文件名: BitmapClass.h
#pragma once

// 包含文件 
#include <d3d11.h>
#include <directxmath.h>
using namespace std;
using namespace DirectX;

// 包含本工程文件  
#include "textureclass.h"

// 类名: BitmapClass
class BitmapClass
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

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, 
		int screenWidth, int screenHeight, char* textureFilename,
		int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	//顶点和索引
	ID3D11Buffer    *m_vertexBuffer, *m_indexBuffer;
	int				m_vertexCount, m_indexCount;

	//纹理和模型
	TextureClass*	m_Texture;

	//绘制二维图像所需的：屏幕尺寸、图像尺寸、最近一次绘制位置
	int m_screenWidth,  m_screenHeight;
	int m_bitmapWidth,  m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};
