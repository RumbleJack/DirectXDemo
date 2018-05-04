#include "stdafx.h"
#include "LineShaderClass.h"

LineShaderClass::LineShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_pixelColorBuffer = nullptr;
}

LineShaderClass::LineShaderClass(const LineShaderClass& other)
{
}

LineShaderClass::~LineShaderClass()
{
}

bool LineShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// 初始化顶点和像素着色器
	result = InitializeShader(device, hwnd, L"../Engine/Line.vs", L"../Engine/Line.ps");
	if (!result)
		return false;
	return true;
}

void LineShaderClass::Shutdown()
{
	// 析构顶点和像素着色器及相关对象
	ShutdownShader();

	return;
}

bool LineShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMFLOAT4 pixelColor)
{
	bool result;

	// 设置将用于绘制的着色器参数
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, pixelColor);
	if (!result)
		return false;

	// 使用着色器绘制缓存内容
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LineShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	// 初始化指针
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	// 必须与ModelClass和shader中定义的顶点结构类型相匹配
	polygonLayout[0].SemanticName = "POSITION";  // 着色器文件中名称匹配
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	// 获取顶点输入布局中的元素个数
	unsigned int		numElements;
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

	// 建立顶点着色器中动态矩阵常量缓冲的描述，向顶点着色器传递三个转化矩阵
	D3D11_BUFFER_DESC	matrixBufferDesc;
	// DYNAMIC表示资源是暂时的，可能经常被替换
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstVSBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// 创建常量缓冲指针，可以用它访问顶点着色器中的常量缓存
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// 建立像素着色器中常量缓冲的描述
	D3D11_BUFFER_DESC pixelBufferDesc;
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(ConstPSBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;
	// 创建常量缓冲指针，可以用它访问像素着色器中的常量缓存
	result = device->CreateBuffer(&pixelBufferDesc, NULL, &m_pixelColorBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void LineShaderClass::ShutdownShader()
{

	if (m_pixelColorBuffer)
	{
		m_pixelColorBuffer->Release();
		m_pixelColorBuffer = nullptr;
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

void LineShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool LineShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMFLOAT4 pixelColor)
{
	HRESULT						result;

	// 转置矩阵，为送入着色器做准备（转置矩阵是因为着色器中传入的顶点坐标为行向量，需要转置矩阵进行左乘，如果是列向量，则需用原矩阵右乘）
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 锁定常量缓存，用于写入
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	// 获取常量缓冲区指针
	ConstVSBufferType*	dataPtr;
	dataPtr = (ConstVSBufferType*)mappedResource.pData;
	// 将矩阵赋值到常量缓冲区，指针原本数据通过D3D自动delete了吗？
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	// 解锁常量缓存
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 设置顶点着色器中的常量缓冲位置 
	unsigned int bufferNumber = 0;
	// 更新顶点着色器中的常量缓冲数据
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 锁定常量缓存，用于写入
	result = deviceContext->Map(m_pixelColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	// 获取常量缓冲区指针
	ConstPSBufferType* dataPtr2;
	dataPtr2 = (ConstPSBufferType*)mappedResource.pData;
	// 将矩阵赋值到常量缓冲区
	dataPtr2->pixelColor = pixelColor;
	// 解锁常量缓存
	deviceContext->Unmap(m_pixelColorBuffer, 0);
	// 像素着色器中的常量缓冲位置 
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelColorBuffer);

	return true;
}

void LineShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 设置顶点输入布局
	deviceContext->IASetInputLayout(m_layout);

	// 设置顶点和像素着色器，它们将被用于绘制三角形
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 设置绘制起点
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
