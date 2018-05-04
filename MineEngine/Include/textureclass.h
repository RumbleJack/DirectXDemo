#pragma once 
// �ļ���: textureclass.h

// ����: TextureClass
class DllExport TextureClass
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char* );
	bool InitializeFromPixel(ID3D11Device*, ID3D11DeviceContext*,unsigned char*,int width, int height );
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(char*, int&, int&);

private:
	// ����ͼ������
	unsigned char*   m_targaData;
	// ����������ͼ��Դ
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;

};