#pragma once

// 类名: GraphicsClass
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
	// 二维模型
	BitmapClass*		m_Bitmap;
	TextureShaderClass* m_TextureShader;
};