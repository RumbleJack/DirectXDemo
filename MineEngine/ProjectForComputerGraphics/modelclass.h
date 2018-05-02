// 文件名: modelclass.h
#pragma once

// 包含文件 
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
using namespace std;
using namespace DirectX;


// 包含本工程文件    
#include "textureclass.h"

// 类名: ModelClass
class ModelClass
{
private:
	//顶点着色器输入类型
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};


	//读取3D模型
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();


	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char* textureFilename, char* modelFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();
private:
	//顶点和索引
	ID3D11Buffer    *m_vertexBuffer,*m_indexBuffer;
	int				m_vertexCount, m_indexCount;

	//纹理和模型
	TextureClass*	m_Texture;
	ModelType*		m_model;
};
