////////////////////////////////////////////////////////////////////////////////
// �ļ���: textureshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
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

	// ��ʼ�������������ɫ��
	result = InitializeShader(device, hwnd, L"../Engine/texture.vs", L"../Engine/texture.ps");
	if (!result)
		return false;
	return true;
}

void TextureShaderClass::Shutdown()
{
	// ���������������ɫ������ض���
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	// ���ý����ڻ��Ƶ���ɫ������
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
		return false;

	// ʹ����ɫ�����ƻ�������
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	// ��ʼ��ָ��
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

#ifdef DIRECTX_2010
	// ���붥����ɫ��
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);
#else
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
#endif
	if (FAILED(result))
	{
		// ����ʧ��
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		// �޷��ҵ��ļ�
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}
#ifdef DIRECTX_2010
	// ����������ɫ��
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
#else
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
#endif

	if (FAILED(result))
	{
		// ����ʧ��
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		// �޷��ҵ��ļ�
		else
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		return false;
	}

	// �ӻ��崴��������ɫ��
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
		return false;

	// �ӻ��崴��������ɫ��
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
		return false;

	// �����������벼�ֵ�����
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	// ������ModelClass��shader�ж���Ķ���ṹ������ƥ��
	polygonLayout[0].SemanticName = "POSITION";  // ��ɫ���ļ�������ƥ��
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;
	polygonLayout[1].SemanticName = "TEXCOORD";  // ��ɫ���ļ�������ƥ��
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ��ȡ�������벼���е�Ԫ�ظ���
	unsigned int		numElements;
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	// �����������벼��
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;
	// �ͷŶ��㡢������ɫ������
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// ����������ɫ���ж�̬������������������򶥵���ɫ����������ת������
	D3D11_BUFFER_DESC	matrixBufferDesc;
	// DYNAMIC��ʾ��Դ����ʱ�ģ����ܾ������滻
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// ������������ָ�룬�����������ʶ�����ɫ���еĳ�������
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// �������������״̬����
	D3D11_SAMPLER_DESC	samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// �������������״̬
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	// �ͷŲ�����״̬
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// �ͷž���������
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	// �ͷŶ����������ݲ���
	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	// �ͷ�������ɫ��
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	// �ͷŶ�����ɫ��
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
	
	// ת�þ���Ϊ������ɫ����׼����ת�þ�������Ϊ��ɫ���д���Ķ�������Ϊ����������Ҫת�þ��������ˣ��������������������ԭ�����ҳˣ�
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// �����������棬����д��
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	// ��ȡ����������ָ��
	MatrixBufferType*	dataPtr;
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	// ������ֵ������������
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	// ������������
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���ö�����ɫ���еĳ�������λ�� 
	unsigned int	bufferNumber;
	bufferNumber = 0;
	// ���¶�����ɫ���еĳ�����������
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	// Ϊ������ɫ������������Դ
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���ö������벼��
	deviceContext->IASetInputLayout(m_layout);

	// ���ö����������ɫ�������ǽ������ڻ���������
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	
	// ����������ɫ���еĲ�����״̬
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// ���û�����㣬���������Σ��������ε�ָ����BitmapClass��ָ����
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
