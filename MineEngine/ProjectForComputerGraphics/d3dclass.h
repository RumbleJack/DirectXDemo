// �ļ���: d3dclass.h
#pragma once 

// ����: D3DClass
// ����: D3DClass
class DllExport D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	// ��ȡ�豸���豸������
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	// ��ȡĬ�ϵ�͸��ͶӰ������ͶӰ����
	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	// �����ر�Z���桢Alpha��ϡ�4�������
	void TurnZBufferOn();
	void TurnZBufferOff();
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	void GetVideoCardInfo(char*, int&);

private:
	bool Initialize_RefreshRate( UINT& numerator, UINT& denominator);
	bool Initialize_DeviceAndSwapChain(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen, UINT& numerator, UINT& denominator);
	bool Initialize_renderTargetView();
	bool Initialize_DepthStencilBuffer(int screenWidth, int screenHeight);
	bool Initialize_DepthStencilView();
	bool Initialize_Viewport();

	bool Initialize_DepthStencilState();
	bool Initialize_RasterizerState();
	bool Initialize_BlendState();
	bool Initialize_Matrix(float screenDepth, float screenNear);

private:
	bool					 m_vsync_enabled;

	// D3D����
	ID3D11Device*			 m_device;
	ID3D11DeviceContext*	 m_deviceContext;
	IDXGISwapChain*			 m_swapChain;

	ID3D11RenderTargetView*  m_renderTargetView;	// �󻺴�
	ID3D11Texture2D*		 m_depthStencilBuffer;
	ID3D11DepthStencilView*  m_depthStencilView;	// ���ģ�建��


													// D3D״̬����
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11RasterizerState*	 m_rasterState;
	ID3D11BlendState*		 m_alphaEnableBlendingState;
	ID3D11BlendState*		 m_alphaDisableBlendingState;

	// ͸��ͶӰ��ƽ��ͶӰ��������
	XMMATRIX				 m_projectionMatrix;
	XMMATRIX				 m_orthoMatrix;

	// �Ƿ���4�������
	HWND m_hwnd;
	int  m_screenWidth, m_screenHeight;

	// �Կ���������
	int						m_videoCardMemory;
	char					m_videoCardDescription[128];
};
