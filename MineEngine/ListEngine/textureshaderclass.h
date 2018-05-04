// �ļ���:  textureshaderclass.h
#pragma once


class DllExport TextureShaderClass
{
private:
	//�����ݽṹ�����򶥵���ɫ����������
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	TextureShaderClass();
	TextureShaderClass(const TextureShaderClass&);
	~TextureShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, 
		XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix, 
		ID3D11ShaderResourceView* texture);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	// ������ɫ������������
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	// ��ɫ��
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*  m_pixelShader;

	// ��ɫ�������������
	ID3D11InputLayout*  m_layout;
	// ��ɫ���������壬��������ת������
	ID3D11Buffer*	m_matrixBuffer;
	// ����������״̬������������ɫ������״̬
	ID3D11SamplerState* m_sampleState;
};

