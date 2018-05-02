////////////////////////////////////////////////////////////////////////////////
// 文件名: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	//顶点和索引
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	//纹理和模型
	m_Texture	= nullptr;
	m_model		= nullptr;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename, char* modelFilename)
{
	bool result;

	// 载入模型数据（包括顶点位置、纹理坐标、以及顶点法向量）
	result = LoadModel(modelFilename);
	if (!result)
		return false;

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

void ModelClass::Shutdown()
{
	// 释放模型纹理
	ReleaseTexture();

	// 释放顶点和索引缓冲
	ShutdownBuffers();

	// 释放模型数据
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 将顶点和索引缓冲放入图形管道中，准备用于绘制
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC		vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA  vertexData, indexData;
	HRESULT result;

	// 创建顶点数组
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// 创建索引数组
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// 载入顶点数据与索引数据
	for (int i = 0; i<m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		indices[i] = i;
	}

	// 建立静态顶点缓冲描述
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

void ModelClass::ShutdownBuffers()
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

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
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

void ModelClass::ReleaseTexture()
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

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	// 打开模型文件
	fin.open(filename);
	if (fin.fail())
		return false;

	// 读入顶点计数值
	fin.get(input);
	while (input != ':')
		fin.get(input);
	fin >> m_vertexCount;

	// 置位索引计数
	m_indexCount = m_vertexCount;

	// 为读入模型申请空间
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
		return false;

	// 将指针移动至数据开始位置
	fin.get(input);
	while (input != ':')
		fin.get(input);
	fin.get(input);  //读入换行符
	fin.get(input);	 //读入下一个换行符

	// 存储顶点数据、纹理坐标、法向量
	for (i = 0; i<m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 关闭模型文件
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = nullptr;
	}

	return;
}