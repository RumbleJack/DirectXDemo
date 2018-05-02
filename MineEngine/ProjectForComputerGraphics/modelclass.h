// �ļ���: modelclass.h
#pragma once

// �����ļ� 
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
using namespace std;
using namespace DirectX;


// �����������ļ�    
#include "textureclass.h"

// ����: ModelClass
class ModelClass
{
private:
	//������ɫ����������
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};


	//��ȡ3Dģ��
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
	//���������
	ID3D11Buffer    *m_vertexBuffer,*m_indexBuffer;
	int				m_vertexCount, m_indexCount;

	//�����ģ��
	TextureClass*	m_Texture;
	ModelType*		m_model;
};
