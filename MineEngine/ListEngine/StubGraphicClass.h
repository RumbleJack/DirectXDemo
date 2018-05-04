#pragma once

// ����: GraphicsClass
class DllExport StubGraphicsClass: public GraphicsClass
{
public:
	StubGraphicsClass();
	StubGraphicsClass(const StubGraphicsClass&);
	virtual ~StubGraphicsClass();

	virtual bool Initialize(int, int, HWND);
	virtual void Shutdown();
	virtual bool Frame();

private:
	bool Render();
	bool Render2D();

private:
	// ��άģ��
	BitmapClass*		m_Bitmap;
	TextureShaderClass* m_TextureShader;
};