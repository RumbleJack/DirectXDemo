// �ļ���: BitmapClass.h
#pragma once

// �����������ļ�  
#include "textureclass.h"

// ����: BitmapClass
class DllExport BitmapClass
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

	// ��ʼ�����ͷš�����ģ��
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, 
		int screenWidth, int screenHeight, char* textureFilename,
		int bitmapWidth, int bitmapHeight);
	void Shutdown();
	// rectͨ����������ָ�����ƾ���λ��
	bool Render(ID3D11DeviceContext* deviceContext, RECT& rect);
	bool UpdateTexture(ID3D11Device*, ID3D11DeviceContext*, unsigned char* textureFilename,
		int bitmapWidth, int bitmapHeight);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	void GetScreenSize(int&width, int& height);

private:
	// ��ʼ�����������ͷŻ����������ƻ���������Ӧ�ڶ�����ɫ��
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	// �������ƾ���λ��
	bool UpdateBuffers(ID3D11DeviceContext*, RECT& rect);

	// ������ͷ�ģ���������ݣ���Ӧ�����أ�ƬԪ����ɫ��
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	//���������
	ID3D11Buffer    *m_vertexBuffer, *m_indexBuffer;
	int				m_vertexCount, m_indexCount;
	//����
	TextureClass*	m_Texture;

	//���ƶ�άͼ������ģ���Ļ�ߴ硢ͼ��ߴ�
	int m_screenWidth,  m_screenHeight;
	int m_bitmapWidth,  m_bitmapHeight;
	// ���һ�λ���λ��
	RECT m_previousPosRect;
};
