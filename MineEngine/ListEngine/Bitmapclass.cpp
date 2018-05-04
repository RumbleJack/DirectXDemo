// �ļ���: BitmapClass.cpp
#include "stdafx.h"
#include "BitmapClass.h"

BitmapClass::BitmapClass()
{
	//���������
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	//�����ģ��
	m_Texture = nullptr;
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	int screenWidth, int screenHeight, char* textureFilename,
	int bitmapWidth, int bitmapHeight)
{
	bool result;

	// �洢��Ļ�ߴ�
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// �洢bitmap�ĳߴ�
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// ��ʼ���ϴλ���λ��
	m_previousPosRect.top = -1;
	m_previousPosRect.left = -1;
	m_previousPosRect.bottom = -1;
	m_previousPosRect.right = -1;


	// ��ʼ���������������
	result = InitializeBuffers(device);
	if (!result)
		return false;

	// Ϊģ����������
	if (textureFilename != NULL)
	{
		result = LoadTexture(device, deviceContext, textureFilename);
		if (!result)
			return false;
	}
	return true;
}

void BitmapClass::Shutdown()
{
	// �ͷ�ģ������
	ReleaseTexture();

	// �ͷŶ������������
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext * deviceContext, RECT & rect)
{
	bool result;

	// ���¶�̬���㻺�壬ָ����������µ�λ��
	result = UpdateBuffers(deviceContext, rect );
	if (!result)
		return false;

	// ������������������ͼ�ιܵ��У�׼�����ڻ���
	RenderBuffers(deviceContext);

	return true;
}

bool BitmapClass::UpdateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	unsigned char * textureFilename, int bitmapWidth, int bitmapHeight)
{
	ReleaseTexture();
	bool result;

	// ����TextureClass
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;
	result = m_Texture->InitializeFromPixel(device, deviceContext, textureFilename, bitmapWidth, bitmapHeight);
	if (!result)
		return false;

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void BitmapClass::GetScreenSize(int & width, int& height)
{
	width = m_screenWidth;
	height = m_screenHeight;
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	// ���ö�������
	m_vertexCount = 6;
	m_indexCount = m_vertexCount;

	// �������㡢��������
	VertexType* vertices;
	unsigned long* indices;
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// ���붥����������������
	for (int i = 0; i < m_indexCount; i++)
		indices[i] = i;
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// ������̬���㻺����������̬������������
	D3D11_BUFFER_DESC		vertexBufferDesc, indexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ��ֵ������Դ�ṹ��һ���������ݵ�ָ�롢һ���������ݵ�ָ��
	D3D11_SUBRESOURCE_DATA  vertexData, indexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �������㡢��������
	HRESULT result;
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	// �ͷŶ������������
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext *deviceContext, RECT & rect)
{
	// �ж�λ���Ƿ����仯
	if (rect == m_previousPosRect)
		return true;

	// ��������λ��
	m_previousPosRect = rect;

	// ������������
	VertexType	 *vertices;
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	//���붥������(ƽ��ͶӰʱ������������Ǵ����������꣬����Long��û�����)
	vertices[0].position = XMFLOAT3((float)rect.left, (float)rect.top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3((float)rect.right, (float)rect.bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].position = XMFLOAT3((float)rect.left, (float)rect.bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = XMFLOAT3((float)rect.left, (float)rect.top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[4].position = XMFLOAT3((float)rect.right, (float)rect.top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[5].position = XMFLOAT3((float)rect.right, (float)rect.bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// �������㻺�岢д��
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT					 result;
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	VertexType	 *verticesPtr;
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);

	// �ͷŶ�������
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// ���ö��㻺�岽����ƫ����
	stride = sizeof(VertexType);
	offset = 0;

	// �������������еĶ��㻺����������������Ϊ�״̬������������ܹ���������
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���ö��㻺�彫�����Ƶ�ԭ�����ͣ���������Ϊ������
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// ����TextureClass
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
		return false;

	return true;
}

void BitmapClass::ReleaseTexture()
{
	// �ͷ��������
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}

	return;
}
