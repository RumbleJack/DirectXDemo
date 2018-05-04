// Filename: fontclass.cpp
#include "stdafx.h"
#include "fontclass.h"

FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename)
{
	bool result;

	// 载入字体数据
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// 载入纹理数据
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	ReleaseTexture();
	ReleaseFontData();
	return;
}

bool FontClass::LoadFontData(char* filename)
{
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}
	// 读入字体的纹理坐标及像素尺寸
	ifstream fin;
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// 读入95个ascii字符（32~126）
	char temp;
	for (int i = 0; i<95; i++)
	{
		// 跳过Ascii码
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		// 跳过Ascii字符
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}
	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}

	return;
}

bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY, 
	float Width_scale, float Height_scale, bool isHorizontal)
{
	// 获取顶点着色器输入数据指针
	VertexType* vertexPtr = (VertexType*)vertices;
	// 获取句子长度
	int numLetters = (int)strlen(sentence);

	// 初始化顶点索引
	int index = 0;
	// 将每个字符绘制到四边形上
	for (int i = 0; i<numLetters; i++)
	{
		int letter = ((int)sentence[i]) - 32;

		if (isHorizontal)
		{
			// 如果字符是空格，则移动3个像素
			if (letter == 0)
			{
				drawX = drawX + 3.0f;
			}
			else
			{
				// 第一个三角形
				vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size * Width_scale),
					(drawY - FONT_TEXTURE_HIGHT* Height_scale),
					0.0f);  // Bottom right.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3(drawX, (drawY - FONT_TEXTURE_HIGHT* Height_scale), 0.0f);  // Bottom left.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
				index++;

				// 第二个三角形
				vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size * Width_scale, drawY, 0.0f);  // Top right.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size * Width_scale), (drawY - FONT_TEXTURE_HIGHT* Height_scale), 0.0f);  // Bottom right.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
				index++;

				// 移动绘制顶点横坐标
				drawX = drawX + m_Font[letter].size * Width_scale + 1.0f;
			}
		}
		else
		{
			// 如果字符是空格，则移动3个像素
			if (letter == 0)
			{
				drawY = drawY + 3.0f;
			}
			else
			{
				// 第一个三角形
				vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // bettom left.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3((drawX + FONT_TEXTURE_HIGHT * Height_scale ),
					(drawY + m_Font[letter].size * Width_scale),0.0f);  // top right.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3((drawX + FONT_TEXTURE_HIGHT * Height_scale),
					drawY , 0.0f); // Bottom right.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
				index++;

				// 第二个三角形
				vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3(drawX ,(drawY + m_Font[letter].size * Width_scale), 0.0f);  // Top left.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
				index++;

				vertexPtr[index].position = XMFLOAT3((drawX + FONT_TEXTURE_HIGHT * Height_scale),
					(drawY + m_Font[letter].size * Width_scale), 0.0f);  // top right.
				vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
				index++;

				// 移动绘制顶点纵坐标
				drawY = drawY + m_Font[letter].size * Width_scale + 1.0f;
			}
		}
	}

	return;
}
