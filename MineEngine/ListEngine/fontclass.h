// �ļ���: fontclass.h

#pragma once
#include "textureclass.h"


// ����: FontClass
class DllExport FontClass
{
private:
	//������ɫ����������
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
	// ������������
	struct FontType
	{
		// ������������꼰���سߴ�
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
	// ���ݴ���sentence�����춥����ɫ����������vertices,drawX��drawYָ���˻��ƾ��ε����Ͻ�����
	void BuildVertexArray(void* vertices, const char* sentence,
		float drawX, float drawY, float Width_scale = 1.0, float Height_scale = 1.0,bool isHorizontal = true);
private:
	bool LoadFontData(char*);
	void ReleaseFontData();

	// ������ͷ�ģ���������ݣ���Ӧ�����أ�ƬԪ����ɫ��
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	// �洢ÿ��������������꼰���سߴ�
	FontType*		m_Font;
	//����
	TextureClass*	m_Texture;

	//����������߶�Ϊ16����
	static const int  FONT_TEXTURE_HIGHT = 16;
};
