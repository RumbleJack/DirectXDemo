// �ļ���:  LineShaderClass.h
#pragma once
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;
#include <d3dcompiler.h>
#include <fstream>
using namespace std;
class LineShaderClass
{
private:
	//�����ݽṹ�����򶥵���ɫ����������
	struct ConstVSBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct ConstPSBufferType
	{
		XMFLOAT4 pixelColor;
	};
public:
	LineShaderClass();
	LineShaderClass(const LineShaderClass&);
	~LineShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT4 pixelColor);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	// ������ɫ������������
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	// ��ɫ��
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*  m_pixelShader;

	// ��ɫ�������������
	ID3D11InputLayout*  m_layout;
	// ��ɫ���������壬��������ת������
	ID3D11Buffer*	   m_matrixBuffer;
	ID3D11Buffer*	   m_pixelColorBuffer;
};

