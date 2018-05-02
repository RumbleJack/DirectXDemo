////////////////////////////////////////////////////////////////////////////////
// �ļ���: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	//���������
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	//�����ģ��
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

	// ����ģ�����ݣ���������λ�á��������ꡢ�Լ����㷨������
	result = LoadModel(modelFilename);
	if (!result)
		return false;

	// ��ʼ���������������
	result = InitializeBuffers(device);
	if (!result)
		return false;

	// Ϊģ����������
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
		return false;

	return true;
}

void ModelClass::Shutdown()
{
	// �ͷ�ģ������
	ReleaseTexture();

	// �ͷŶ������������
	ShutdownBuffers();

	// �ͷ�ģ������
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// ������������������ͼ�ιܵ��У�׼�����ڻ���
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

	// ������������
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	// ������������
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;

	// ���붥����������������
	for (int i = 0; i<m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		indices[i] = i;
	}

	// ������̬���㻺������
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ��ֵ������Դ�ṹ��һ���������ݵ�ָ��
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������㻺��
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	// ������̬������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ��ֵ������Դ�ṹ��һ���������ݵ�ָ��
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// ������������
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

	// ���ö��㻺�岽����ƫ����
	stride = sizeof(VertexType);
	offset = 0;

	// �������������ϵĶ��㻺����������������Ϊ�״̬������������ܹ���������
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���ö��㻺�彫�����Ƶ�ԭ�����ͣ���������Ϊ������
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// ����TextureClass
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;

	// ��ʼ��TextureClass
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
		return false;

	return true;
}

void ModelClass::ReleaseTexture()
{
	// �ͷ��������
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

	// ��ģ���ļ�
	fin.open(filename);
	if (fin.fail())
		return false;

	// ���붥�����ֵ
	fin.get(input);
	while (input != ':')
		fin.get(input);
	fin >> m_vertexCount;

	// ��λ��������
	m_indexCount = m_vertexCount;

	// Ϊ����ģ������ռ�
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
		return false;

	// ��ָ���ƶ������ݿ�ʼλ��
	fin.get(input);
	while (input != ':')
		fin.get(input);
	fin.get(input);  //���뻻�з�
	fin.get(input);	 //������һ�����з�

	// �洢�������ݡ��������ꡢ������
	for (i = 0; i<m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// �ر�ģ���ļ�
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