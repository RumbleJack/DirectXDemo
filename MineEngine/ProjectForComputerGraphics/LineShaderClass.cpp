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

	// ��ʼ�������������ɫ��
	result = InitializeShader(device, hwnd, L"../Engine/Line.vs", L"../Engine/Line.ps");
	if (!result)
		return false;
	return true;
}

void LineShaderClass::Shutdown()
{
	// ���������������ɫ������ض���
	ShutdownShader();

	return;
}

bool LineShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMFLOAT4 pixelColor)
{
	bool result;

	// ���ý����ڻ��Ƶ���ɫ������
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, pixelColor);
	if (!result)
		return false;

	// ʹ����ɫ�����ƻ�������
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LineShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	// ��ʼ��ָ��
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// ���붥����ɫ��
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
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

	// ����������ɫ��
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	// ������ModelClass��shader�ж���Ķ���ṹ������ƥ��
	polygonLayout[0].SemanticName = "POSITION";  // ��ɫ���ļ�������ƥ��
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

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
	matrixBufferDesc.ByteWidth = sizeof(ConstVSBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// ������������ָ�룬�����������ʶ�����ɫ���еĳ�������
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	// ����������ɫ���г������������
	D3D11_BUFFER_DESC pixelBufferDesc;
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(ConstPSBufferType);
	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;
	// ������������ָ�룬������������������ɫ���еĳ�������
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
	ConstVSBufferType*	dataPtr;
	dataPtr = (ConstVSBufferType*)mappedResource.pData;
	// ������ֵ��������������ָ��ԭ������ͨ��D3D�Զ�delete����
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	// ������������
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���ö�����ɫ���еĳ�������λ�� 
	unsigned int bufferNumber = 0;
	// ���¶�����ɫ���еĳ�����������
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// �����������棬����д��
	result = deviceContext->Map(m_pixelColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	// ��ȡ����������ָ��
	ConstPSBufferType* dataPtr2;
	dataPtr2 = (ConstPSBufferType*)mappedResource.pData;
	// ������ֵ������������
	dataPtr2->pixelColor = pixelColor;
	// ������������
	deviceContext->Unmap(m_pixelColorBuffer, 0);
	// ������ɫ���еĳ�������λ�� 
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelColorBuffer);

	return true;
}

void LineShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���ö������벼��
	deviceContext->IASetInputLayout(m_layout);

	// ���ö����������ɫ�������ǽ������ڻ���������
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// ���û������
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
