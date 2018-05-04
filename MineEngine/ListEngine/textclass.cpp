// Filename: textclass.cpp
#include "stdafx.h"
#include "textclass.h"

TextClass::TextClass()
{
	m_sentences.clear();
	m_sentencesID = 0;

	m_Font = 0;
	m_FontShader = 0;

}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight )
{
	bool result;

	// �洢��Ļ�ߴ�
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ����������
	m_Font = new FontClass;
	if(!m_Font)
	{
		return false;
	}
	result = m_Font->Initialize(device, deviceContext,"../Engine/data/fontdata.txt", "../Engine/data/font.tga");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
	{
		return false;
	}
	result = m_FontShader->Initialize(device, hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	for (map<long, SentenceType*>::iterator pos = m_sentences.begin(); pos != m_sentences.end(); pos++)
	{
		ReleaseSentence(pos);
	}
	m_sentences.clear();
	m_sentencesID = 0;

	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX baseViewMatrix,XMMATRIX orthoMatrix)
{
	bool result;
	for (map<long,SentenceType*>::iterator pos = m_sentences.begin(); pos != m_sentences.end(); pos++)
	{
		result = pos->second->RenderSentence(deviceContext, worldMatrix, baseViewMatrix, orthoMatrix,
			m_FontShader, m_Font);
		if (!result)
			return false;
	}
	return true;
}

long TextClass::AddSentences(int maxLength, ID3D11Device * device)
{
	m_sentences.insert(std::map<long, SentenceType*>::value_type(m_sentencesID,new SentenceType(maxLength, device)));
	return m_sentencesID++;
}

bool TextClass::DeleteSentences(long sentenceNum)
{
	if (sentenceNum >= m_sentencesID)
		return false;
	
	map<long, SentenceType*>::iterator it = m_sentences.find(sentenceNum);
	if (it != m_sentences.end())
	{
		ReleaseSentence(it);
	}
	m_sentences.erase(it);
	return true;
}

bool TextClass::UpdateColor(long sentenceNum, float red, float green, float blue)
{
	return m_sentences[sentenceNum]->UpdateColor(red, green, blue);
}

bool TextClass::UpdateSentence(long sentenceNum, const char* text,
	int left, int top, float Width_scale, float Height_scale,
	ID3D11DeviceContext* deviceContext,bool isHorizontal)
{
	return m_sentences[sentenceNum]->UpdateSentence(text, left, top, Width_scale, Height_scale,
		deviceContext, m_screenWidth, m_screenHeight, m_Font, isHorizontal);
}

bool TextClass::ReleaseSentence(map<long, SentenceType*>::iterator it)
{
	it->second->Release();
	delete it->second;
	it->second = NULL;
	return true;
}

TextClass::SentenceType::SentenceType(int maxLen, ID3D11Device * device)
{
	HRESULT result;

	// ��ʼ���ı���ɫ
	UpdateColor(1.0,1.0,1.0);

	// ��ʼ��������������
	vertexBuffer = 0;
	indexBuffer = 0;
	// ���þ�����󳤶�
	maxLength = maxLen;
	// ���ö��㡢��������
	vertexCount = 6 * maxLen;
	indexCount = vertexCount;

	// ��������ʼ�����㡢��������
	VertexType* vertices;
	unsigned long* indices;
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return ;
	}
	indices = new unsigned long[indexCount];
	if (!indices)
		return ;
	memset(vertices, 0, (sizeof(VertexType) * vertexCount));
	for (int i = 0; i<indexCount; i++){
		indices[i] = i;
	}

	// �������㻺������
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// ������Դָ�򶥵�����
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// �������㻺��
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return ;
	}

	// ����������������
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// ������Դָ����������
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// ������������
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		return ;
	}

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return ;
}

bool TextClass::SentenceType::UpdateColor(float red, float green, float blue )
{
	// �����ı���ɫ
	this->red = red;
	this->green = green;
	this->blue = blue;
	return true;
}

bool TextClass::SentenceType::UpdateSentence(const char* text,
	int beiginX, int beginY, float Width_scale, float Height_scale,
	ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, FontClass* font, bool isHorizontal)
{
	HRESULT result;

	// ��ȡ������ı�����
	int numLetters = (int)strlen(text);
	if (numLetters > this->maxLength)
	{
		return false;
	}

	// ������������
	VertexType* vertices;
	vertices = new VertexType[this->vertexCount];
	if (!vertices)
		return false;
	memset(vertices, 0, (sizeof(VertexType) * this->vertexCount));

	// ʹ�������๹�춥��λ������
	font->BuildVertexArray((void*)vertices, text, (float)beiginX, (float)beginY, Width_scale, Height_scale, isHorizontal);

	// ���¶��㻺��
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(this->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * this->vertexCount));
	deviceContext->Unmap(this->vertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}

bool TextClass::SentenceType::RenderSentence(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX baseViewMatrix,XMMATRIX orthoMatrix,
	FontShaderClass* fontShader, FontClass* font)
{
	bool result;

	// ���û��岽����ƫ��
	unsigned int stride, offset;
	stride = sizeof(VertexType);
	offset = 0;
	// �������������еĶ��㻺����������������Ϊ�״̬������������ܹ���������
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���ö��㻺�彫�����Ƶ�ԭ�����ͣ���������Ϊ������
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ����������ɫ������
	XMFLOAT4 pixelColor;
	pixelColor = XMFLOAT4(this->red, this->green, this->blue, 1.0f);

	// ʹ��������ɫ������
	result = fontShader->Render(deviceContext, this->indexCount, worldMatrix, baseViewMatrix, orthoMatrix, font->GetTexture(),
		pixelColor);
	if (!result)
		return false;

	return true;
}

void TextClass::SentenceType::Release()
{
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	};
	return;
}
