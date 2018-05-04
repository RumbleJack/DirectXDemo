#include "stdafx.h"
#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass()
{
	m_vertexShader	= nullptr;
	m_pixelShader	= nullptr;
	m_layout		= nullptr;
	m_matrixBuffer	= nullptr;
	m_sampleState	= nullptr;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// 初始化顶点和像素着色器
	result = InitializeShader(device, hwnd, L"../Engine/texture.vs", L"../Engine/texture.ps");
	if (!result)
		return false;
	return true;
}

void TextureShaderClass::Shutdown()
{
	// 析构顶点和像素着色器及相关对象
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	// 设置将用于绘制的着色器参数
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
		return false;

	// 使用着色器绘制缓存内容
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT				result;
	ID3D10Blob*			errorMessage;
	ID3D10Blob*			vertexShaderBuffer;
	ID3D10Blob*			pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int		numElements;
	D3D11_BUFFER_DESC	matrixBufferDesc;
	D3D11_SAMPLER_DESC	samplerDesc;

	// 初始化指针
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// 编译顶点着色器
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 编译失败
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		// 无法找到文件
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// 编译像素着色器
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 编译失败
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		// 无法找到文件
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// 从缓冲创建顶点着色器
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	// 从缓冲创建像素着色器
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	// 创建顶点输入布局的描述
	// 必须与ModelClass和shader中定义的顶点结构类型相匹配
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	// 获取顶点输入布局中的元素个数
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 创建顶点输入布局
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	// 释放顶点、像素着色器缓存
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// 建立顶点着色器中动态矩阵常量缓冲的描述
	// DYNAMIC表示资源是暂时的，可能经常被替换
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 创建常量缓冲指针，可以用它访问顶点着色器中的常量缓存
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// 建立纹理采样器状态描述
	samplerDesc.Filter =   D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 创建纹理采样器状态
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	// 释放采样器状态
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// 释放矩阵常量缓存
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	// 释放顶点输入数据布局
	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	// 释放像素着色器
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	// 释放顶点着色器
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	return;
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
		fout << compileErrors[i];

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT						result;
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	MatrixBufferType*			dataPtr;
	unsigned int				bufferNumber;

	// 转置矩阵，为送入着色器做准备（转置矩阵是因为着色器中传入的顶点坐标为行向量，需要转置矩阵进行左乘，如果是列向量，则需用原矩阵右乘）
	worldMatrix			= XMMatrixTranspose(worldMatrix);
	viewMatrix			= XMMatrixTranspose(viewMatrix);
	projectionMatrix	= XMMatrixTranspose(projectionMatrix);

	// 锁定常量缓存，用于写入
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	// 获取常量缓冲区指针
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 将矩阵赋值到常量缓冲区
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 解锁常量缓存
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 设置顶点着色器中的常量缓冲位置 
	bufferNumber = 0;

	// 更新顶点着色器中的常量缓冲数据
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 为像素着色器设置纹理资源
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 设置顶点输入布局
	deviceContext->IASetInputLayout(m_layout);

	// 设置顶点和像素着色器，它们将被用于绘制三角形
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	
	// 设置像素着色器中的采样器状态
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 绘制三角形
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
