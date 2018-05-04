// Filename: textclass.h
#pragma once


#include "fontclass.h"
#include "fontshaderclass.h"

//template<class _Ty>
//struct DllExport std::less;
//template<class _Ty>
//class DllExport std::_ALLOCATOR;
//template<class _Kty,
//	class _Ty,
//	class _Pr = less<_Kty>,
//	class _Alloc = allocator<pair<const _Kty, _Ty> > >
//	class DllExport std::map;


class DllExport TextClass
{
private:
	// 用于绘制的字符串数据
	class SentenceType
	{
	public:
		SentenceType() {}
		SentenceType(int maxLength, ID3D11Device* device);
		bool UpdateColor(float, float, float);
		bool UpdateSentence( const char*, int , int , float Width_scale, float Height_scale ,
			 ID3D11DeviceContext*, int m_screenWidth, int m_screenHeight, FontClass* font, bool isHorizontal = true);
		bool RenderSentence(ID3D11DeviceContext*,  
			XMMATRIX, XMMATRIX, XMMATRIX, 
			FontShaderClass* , FontClass* );
		void Release();
	public:
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int screenWidth, int screenHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);

	long AddSentences(int maxLength, ID3D11Device* device);
	bool DeleteSentences(long);
	bool UpdateColor(long,float, float, float);
	bool UpdateSentence(long,const char*, int left, int top, float Width_scale, float Height_scale,
		ID3D11DeviceContext*,bool isHorizontal = true);
private:
	bool ReleaseSentence(map<long, SentenceType*>::iterator it);
private:
	// 将要绘制的句子
	map<long, SentenceType*> m_sentences;
	long m_sentencesID;

	// 管理单个字体顶点的位置坐标和纹理坐标，以及纹理信息
	FontClass*			m_Font;
	// 字体专用着色器
	FontShaderClass*	m_FontShader;

	int m_screenWidth, m_screenHeight;
};