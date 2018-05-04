// 文件名: fontshaderclass.h
#ifndef _FONTSHADERCLASS_H_
#define _FONTSHADERCLASS_H_

// 类名: FontShaderClass
class DllExport FontShaderClass
{
private:
	struct ConstantVertexBufferType
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
	FontShaderClass();
	FontShaderClass(const FontShaderClass&);
	~FontShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, 
		XMMATRIX worldMatrix, XMMATRIX viewMatrix,XMMATRIX projectionMatrix, 
		ID3D11ShaderResourceView* texture, 
		XMFLOAT4 pixelColor);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	// 着色器
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*  m_pixelShader;
	// 着色器输入参数布局
	ID3D11InputLayout*  m_layout;
	// 着色器常量缓冲
	ID3D11Buffer* m_constantVertexBuffer;
	ID3D11Buffer* m_pixelColorBuffer;
	// 采样器状态，传递像素着色器采样状态
	ID3D11SamplerState* m_sampleState;
};

#endif