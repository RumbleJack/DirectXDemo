////////////////////////////////////////////////////////////////////////////////
// 文件名: BitmapClass.cpp
#include "BitmapClass.h"

BitmapClass::BitmapClass()
{
	//顶点和索引
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	//纹理和模型
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

	// 存储屏幕尺寸
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 存储bitmap的尺寸
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// 初始化上次绘制位置
	m_previousPosX = -1;
	m_previousPosY = -1;

	// 初始化顶点和索引缓冲
	result = InitializeBuffers(device);
	if (!result)
		return false;

	// 为模型载入纹理
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
		return false;

	return true;
}

void BitmapClass::Shutdown()
{
	// 释放模型纹理
	ReleaseTexture();

	// 释放顶点和索引缓冲
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	// 更新动态顶点缓冲，指定顶点绘制新的位置
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
		return false;

	// 将顶点和索引缓冲放入图形管道中，准备用于绘制
	RenderBuffers(deviceContext);

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

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC		vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA  vertexData, indexData;
	HRESULT result;

	// 设置顶点数量
	m_vertexCount = 6;
	m_indexCount = m_vertexCount;

	// 创建顶点数组
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// 创建索引数组
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// 载入顶点数据与索引数据
	for (int i = 0; i<m_indexCount; i++)
		indices[i] = i;
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// 建立动态顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 赋值给子资源结构体一个顶点数据的指针
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	// 建立静态索引缓冲描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 赋值给子资源结构体一个索引数据的指针
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建索引缓冲
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	// 释放顶点和索引数组
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}
bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float					 left, right, top, bottom;
	VertexType				 *vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType				 *verticesPtr;
	HRESULT					 result;

	// 判断位置是否发生变化
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
		return true;

	// 更新最新位置
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 计算位图左右上下侧屏幕坐标
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	right = left + (float)m_bitmapWidth;
	top = (float)(m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)m_bitmapHeight;

	// 创建顶点数组
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	//载入顶点数据
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 锁定顶点缓冲并写入
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 释放顶点数组
	delete[] vertices;
	vertices = 0;

	return true;
}
void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// 设置顶点缓冲步幅和偏移量
	stride = sizeof(VertexType);
	offset = 0;

	// 设置输入汇编器上的顶点缓冲区和索引缓冲区为活动状态，这样缓冲就能够被绘制了
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 设置顶点缓冲将被绘制的原子类型，这里设置为三角形
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// 创建TextureClass
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;

	// 初始化TextureClass
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
		return false;

	return true;
}

void BitmapClass::ReleaseTexture()
{
	// 释放纹理对象
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
