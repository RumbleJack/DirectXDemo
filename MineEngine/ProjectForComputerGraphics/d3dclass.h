// 文件名: d3dclass.h
#pragma once 

// 链接
#pragma comment(lib, "d3d11.lib")		// contains all the Direct3D functionality
#pragma comment(lib, "dxgi.lib")		//contains tools to interface with the hardware
#pragma comment(lib, "d3dcompiler.lib") //contains functionality for compiling shaders


#include <d3d11.h>						//该头文件中包含了windows.h
#include <directxmath.h>
using namespace DirectX;

// 类名: D3DClass
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
		float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char* cardName, int& memory);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

private:
	bool Initialize_RefreshRate(unsigned int screenWidth, unsigned int screenHeight, UINT& numerator, UINT& denominator);
	bool Initialize_BlendState();
	bool Initialize_DeviceAndSwapChain(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen, UINT& numerator, UINT& denominator);
	bool Initialize_renderTargetView();
	bool Initialize_DepthStencilBuffer(int screenWidth, int screenHeight);
	bool Initialize_depthStencilState();
	bool Initialize_DepthStencilView();
	bool Initialize_RasterizerState();
	bool Initialize_Viewport(int screenWidth, int screenHeight);
	bool Initialize_Matrix(int screenWidth, int screenHeight, float screenDepth, float screenNear);

private:
	bool				m_vsync_enabled;
	int					m_videoCardMemory;
	char				m_videoCardDescription[128];

	// D3D声明
	IDXGISwapChain		* m_swapChain;
	ID3D11Device		* m_device;
	ID3D11DeviceContext	* m_deviceContext;

	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D		* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;

	// D3D状态声明
	ID3D11RasterizerState* m_rasterState;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;
	
	// 变换矩阵声明
	XMMATRIX			m_projectionMatrix;
	XMMATRIX			m_worldMatrix;
	XMMATRIX			m_orthoMatrix;
};
