// �ļ���: LineClass.h
#pragma once

// �����������ļ�  
#include <d3d11.h>
#include "textureclass.h"
#include <directxmath.h>
using namespace DirectX;
// ����: LineClass
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
	//������ɫ����������
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
	// ��ʼ�����������ͷŻ����������ƻ���������Ӧ�ڶ�����ɫ��
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	

private:
	//���������
	ID3D11Buffer    *m_vertexBuffer, *m_indexBuffer;
	int				 m_validVertexCount, m_validIndexCount;
	static const int m_maxVertexCount = 5120;

	// ��Ļ�ߴ�
	int m_screenWidth, m_screenHeight;
	// ������ɫ
	XMFLOAT4 m_pixelColor;
};
