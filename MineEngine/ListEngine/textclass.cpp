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

	// 存储屏幕尺寸
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 创建字体类
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

	// 初始化文本颜色
	UpdateColor(1.0,1.0,1.0);

	// 初始化顶点索引缓冲
	vertexBuffer = 0;
	indexBuffer = 0;
	// 设置句子最大长度
	maxLength = maxLen;
	// 设置顶点、索引总数
	vertexCount = 6 * maxLen;
	indexCount = vertexCount;

	// 创建、初始化顶点、索引数组
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

	// 建立顶点缓冲描述
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// 将子资源指向顶点数据
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// 创建顶点缓冲
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return ;
	}

	// 建立索引缓冲描述
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// 将子资源指向索引数据
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// 创建索引缓冲
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
	// 设置文本颜色
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

	// 获取并检查文本长度
	int numLetters = (int)strlen(text);
	if (numLetters > this->maxLength)
	{
		return false;
	}

	// 创建顶点数组
	VertexType* vertices;
	vertices = new VertexType[this->vertexCount];
	if (!vertices)
		return false;
	memset(vertices, 0, (sizeof(VertexType) * this->vertexCount));

	// 使用字体类构造顶点位置数组
	font->BuildVertexArray((void*)vertices, text, (float)beiginX, (float)beginY, Width_scale, Height_scale, isHorizontal);

	// 更新顶点缓存
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

	// 设置缓冲步长和偏移
	unsigned int stride, offset;
	stride = sizeof(VertexType);
	offset = 0;
	// 设置输入汇编器中的顶点缓冲区和索引缓冲区为活动状态，这样缓冲就能够被绘制了
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 设置顶点缓冲将被绘制的原子类型，这里设置为三角形
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 设置像素着色器缓冲
	XMFLOAT4 pixelColor;
	pixelColor = XMFLOAT4(this->red, this->green, this->blue, 1.0f);

	// 使用字体着色器绘制
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
