// �ļ���: BitmapClass.h
#pragma once

// �����ļ� 
#include <d3d11.h>
#include <directxmath.h>
using namespace std;
using namespace DirectX;

// �����������ļ�  
#include "textureclass.h"

// ����: BitmapClass
class BitmapClass
{
private:
	//������ɫ����������
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
	//���������
	ID3D11Buffer    *m_vertexBuffer, *m_indexBuffer;
	int				m_vertexCount, m_indexCount;

	//�����ģ��
	TextureClass*	m_Texture;

	//���ƶ�άͼ������ģ���Ļ�ߴ硢ͼ��ߴ硢���һ�λ���λ��
	int m_screenWidth,  m_screenHeight;
	int m_bitmapWidth,  m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};
