// 文件名:  textureshaderclass.h
#pragma once


class DllExport TextureShaderClass
{
private:
	//该数据结构用于向顶点着色器传递数据
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

	// 设置着色器参数及绘制
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	// 着色器
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*  m_pixelShader;

	// 着色器输入参数布局
	ID3D11InputLayout*  m_layout;
	// 着色器常量缓冲，传递三个转化矩阵
	ID3D11Buffer*	m_matrixBuffer;
	// 采样器常量状态，传递像素着色器采样状态
	ID3D11SamplerState* m_sampleState;
};

