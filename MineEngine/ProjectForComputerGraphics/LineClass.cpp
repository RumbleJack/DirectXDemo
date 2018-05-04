// �ļ���: LineClass.cpp
#include "stdafx.h"
#include "LineClass.h"

LineClass::LineClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_pixelColor = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
}

LineClass::LineClass(const LineClass& other)
{
}

LineClass::~LineClass()
{
}

bool LineClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
	int screenWidth, int screenHeight)
{
	bool result;

	// �洢��Ļ�ߴ�
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ��ʼ���������������
	result = InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

void LineClass::Shutdown()
{

	// �ͷŶ������������
	ShutdownBuffers();

	return;
}

bool LineClass::Render(ID3D11DeviceContext * deviceContext, RENDER_STYLE renderStyle)
{
	// ���ö��㻺�岽����ƫ����
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// �������������׶εĶ��㻺����������������Ϊ�״̬������������ܹ���������
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���ö��㻺�彫�����Ƶ�ԭ������
	switch (renderStyle)
	{
	case POINT:
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case LINE_LIST:
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case LINE_STRIP:
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		break;
	case TRIANGLE_LIST:
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case TRIANGLE_STRIP:
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;
	default:
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;
	}

	return true;
}

int LineClass::GetIndexCount()
{
	return m_validIndexCount;
}

bool LineClass::InitializeBuffers(ID3D11Device* device)
{
	// ������Ч��������
	m_validVertexCount = 0;
	m_validIndexCount = m_validVertexCount;

	// �������㡢��������
	VertexType* vertices;
	unsigned long* indices;
	vertices = new VertexType[m_maxVertexCount];
	if (!vertices)
		return false;
	indices = new unsigned long[m_maxVertexCount];
	if (!indices)
		return false;

	// ���붥����������������
	for (int i = 0; i < m_maxVertexCount; i++)
		indices[i] = i;
	memset(vertices, 0, (sizeof(VertexType) * m_maxVertexCount));

	// ������̬���㻺����������̬������������
	D3D11_BUFFER_DESC		vertexBufferDesc, indexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_maxVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_maxVertexCount;
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

void LineClass::ShutdownBuffers()
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

bool LineClass::UpdateColor(float red, float green, float blue)
{
	// �����ı���ɫ
	m_pixelColor = XMFLOAT4(red, green, blue, 1.0f);
	return true;
}
XMFLOAT4 LineClass::GetColor()
{
	return m_pixelColor;
}
bool LineClass::UpdateBuffers(ID3D11DeviceContext *deviceContext, RECT & rect, const float* vertexData, int vertexNum)
{
	if (vertexData == nullptr || vertexNum > m_maxVertexCount)
		return false;

	m_validVertexCount = vertexNum;
	m_validIndexCount = vertexNum;

	// ���¶�������
	VertexType	 *vertices;
	vertices = new VertexType[m_validVertexCount /*+ 2*/];
	if (!vertices)
		return false;
	// ���ƾ���
	float offsetX = (float)rect.left;
	float offsetY = (float)rect.bottom;
	float renderWidth = (float)rect.right - (float)rect.left;
	float renderHeight = (float)rect.top - (float)rect.bottom;
	for (int i = 0; i < m_validVertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(vertexData[i * 2 ] * renderWidth + offsetX,
			vertexData[i*2 +1] * renderHeight + offsetY, 0.0f);
	}
	/*--------------����------------------*/
	//vertices[vertexNum].position = XMFLOAT3(-m_screenWidth/2 + 227,227, 0.0f);
	//vertices[vertexNum+1].position = XMFLOAT3(-m_screenWidth / 2 + 226, 100, 0.0f);
	//m_validVertexCount = vertexNum + 2;
	//m_validIndexCount = vertexNum + 2 ;
	/*-----------------------------------*/


	// �������㻺�岢д��
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT					 result;
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	VertexType	 *verticesPtr;
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_validVertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);

	// �ͷŶ�������
	delete[] vertices;
	vertices = 0;

	return true;
}

bool LineClass::UpdateRawBuffers(ID3D11DeviceContext *deviceContext, RECT & rect, const float * vertexData, int vertexNum)
{
	if (vertexData == nullptr || vertexNum > m_maxVertexCount)
		return false;

	m_validVertexCount = vertexNum;
	m_validIndexCount = vertexNum;

	// ���¶�������
	VertexType	 *vertices;
	vertices = new VertexType[m_validVertexCount /*+ 2*/];
	if (!vertices)
		return false;
	// ���ƾ���
	float offsetX = (float)rect.left;
	float offsetY = (float)rect.bottom;
	float renderWidth = (float)rect.right - (float)rect.left;
	float renderHeight = (float)rect.top - (float)rect.bottom;
	for (int i = 0; i < m_validVertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(vertexData[i * 2] + offsetX,
			vertexData[i * 2 + 1] + offsetY, 0.0f);
	}
	// �������㻺�岢д��
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT					 result;
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	VertexType	 *verticesPtr;
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_validVertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);

	// �ͷŶ�������
	delete[] vertices;
	vertices = 0;

	return true;
}



