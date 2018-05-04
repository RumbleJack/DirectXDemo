// 文件名: fontclass.h

#pragma once
#include "textureclass.h"


// 类名: FontClass
class DllExport FontClass
{
private:
	//顶点着色器输入类型
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
	// 描述字体数据
	struct FontType
	{
		// 字体的纹理坐标及像素尺寸
		float	left, right;
		int		size;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext* ,char*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	// 根据传入sentence，构造顶点着色器输入数据vertices,drawX，drawY指定了绘制矩形的左上角坐标
	void BuildVertexArray(void* vertices, const char* sentence,
		float drawX, float drawY, float Width_scale = 1.0, float Height_scale = 1.0,bool isHorizontal = true);
private:
	bool LoadFontData(char*);
	void ReleaseFontData();

	// 载入和释放模型纹理数据，对应于像素（片元）着色器
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	// 存储每个字体的纹理坐标及像素尺寸
	FontType*		m_Font;
	//纹理
	TextureClass*	m_Texture;

	//字体纹理本身高度为16像素
	static const int  FONT_TEXTURE_HIGHT = 16;
};
