// �ļ���: d3dclass.cpp
#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;

	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilView = nullptr;

	m_rasterState = nullptr;
	m_depthStencilState = nullptr;
	m_depthDisabledStencilState = nullptr;
	m_alphaEnableBlendingState = nullptr;
	m_alphaDisableBlendingState = nullptr;
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	// ��¼�Ƿ�����ʾ��ͬ������
	m_vsync_enabled = vsync;

	// ȡ��ˢ��Ƶ�ʵķ��Ӻͷ�ĸ
	UINT	numerator, denominator;
	if (!Initialize_RefreshRate(screenWidth, screenHeight, numerator, denominator))
		return false;

	// ��ʼ���豸�ͽ�����
	if (!Initialize_DeviceAndSwapChain(screenWidth, screenHeight, hwnd, fullscreen, numerator, denominator))
		return false;

	// ��ʼ������Ŀ����ͼ
	if (!Initialize_renderTargetView())
		return false;

	//  ��ʼ�����ģ�建��
	if (!Initialize_DepthStencilBuffer(screenWidth, screenHeight))
		return false;

	//  ��ʼ�����ģ��״̬
	if (!Initialize_depthStencilState())
		return false;

	//  ��ʼ�����ģ����ͼ
	if (!Initialize_DepthStencilView())
		return false;

	// ��ʼ����դ��״̬
	if (!Initialize_RasterizerState())
		return false;

	// ��ʼ����դ���ӵ�
	if (!Initialize_Viewport(screenWidth,  screenHeight))
		return false;

	// ��ʼ������
	if (!Initialize_Matrix( screenWidth,  screenHeight,  screenDepth,  screenNear))
		return false;

	// ��ʼ�����״̬
	if (!Initialize_BlendState())
		return false;

	return true;
}

void D3DClass::Shutdown()
{
	// ������ǰ����Ϊ����ģʽ�����򽻻������׳��쳣
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// ������������������ɫ
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// ����󻺴�
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// �����Ȼ���
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	// ���󻺴�������ʾ����Ļ�ϣ���ǰ��������ɣ�
	if (m_vsync_enabled)
	{
		// ������Ļˢ����
		m_swapChain->Present(1, 0);
	}
	else
	{
		// ������ٶ�ˢ��
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

bool D3DClass::Initialize_RefreshRate(unsigned int screenWidth, unsigned int screenHeight, UINT& numerator, UINT& denominator)
{
	HRESULT				result;
	int error;

	// ���� DirectX ͼ�νӿڹ���
	IDXGIFactory*	factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	// ʹ�ù���Ϊ��ͼ�νӿڣ��Կ�������һ��������
	IDXGIAdapter*		adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return false;

	// ö�����������������������
	IDXGIOutput*		adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		return false;

	// ��ȡ�ʺ��������������ʾ������DXGI_FORMAT_R8G8B8A8_UNORM��ʾ��ʽ��ģʽ������ 
	unsigned int		numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
		return false;

	// ����һ���б����������ʾ��/��Ƶ����ϵ����п��ܵ���ʾģʽ��
	DXGI_MODE_DESC*	displayModeList;
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	// ���displayModeList
	DXGI_ADAPTER_DESC	adapterDesc;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
		return false;

	// ����������ʾģʽ���ҵ�ƥ����Ļ�ߴ���Ǹ������洢��ģʽ��ˢ��Ƶ�ʵķ��Ӻͷ�ĸ
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
				//break;
			}
		}
	}

	// ��ȡ�Կ�����
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
		return false;

	// �����ֽ�Ϊ��λ�洢ר��ͼ�ο��ڴ�
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ���Կ���תΪ���ֽ��ַ����洢
	size_t stringLength;
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
		return false;

	// �ͷ�displayModeList
	delete[] displayModeList;
	displayModeList = 0;

	// �ͷ����������(������)
	adapterOutput->Release();
	adapterOutput = 0;

	// �ͷ�������(�Կ�)
	adapter->Release();
	adapter = 0;

	// �ͷŹ���
	factory->Release();
	factory = 0;

	return true;
}

bool D3DClass::Initialize_DeviceAndSwapChain(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen, UINT& numerator, UINT& denominator)
{
	HRESULT				result;

	// ��ʼ������������
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ����һ�����󻺴桢���趨��ߴ硢��ʽ��ˢ���ʡ�ʹ�÷�ʽ
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width =  screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator =   numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// ���û���Ŀ�괰��
	swapChainDesc.OutputWindow = hwnd;

	// �رն����
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ����ȫ���򴰿�ģʽ
	if ( fullscreen )
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	// ��ɨ����˳�����������Ϊδָ��
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��ʾ�����󻺴�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// ����λ�߼���־
	swapChainDesc.Flags = 0;

	// �����ض��ȼ�Ϊ DirectX 11.
	D3D_FEATURE_LEVEL	 featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ������������D3D�豸��D3D�豸������
	//������豸��û��֧��DirectX11���Կ����� D3D_DRIVER_TYPE_HARDWARE �滻Ϊ D3D_DRIVER_TYPE_REFERENCE �� DirectX11��ʹ��CPU�����ƣ�
	//������豸���ж����Կ�������Ҫö���Կ������û�ѡ��ʹ�������Կ���֧��DirectX11��Ȼ��ʹ�ø��Կ������豸)
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
		return false;

	return true;
}

bool D3DClass::Initialize_renderTargetView()
{
	HRESULT				result;

	// ȡ�ú󻺴��ָ��
	ID3D11Texture2D*	 backBufferPtr;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		return false;

	// ʹ�ú󻺴�ָ�봴������Ŀ����ͼ
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
		return false;

	// �ͷź󻺴�ָ��
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	return true;
}

bool D3DClass::Initialize_DepthStencilBuffer(int screenWidth, int screenHeight)
{
	HRESULT				result;

	// ��ʼ����Ȼ����������
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	// ������Ȼ����������
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ������Ȼ��������using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	return true;
}

bool D3DClass::Initialize_depthStencilState()
{
	HRESULT				result;

	// ��ʼ�����ģ��״̬������
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// �������ģ��״̬����
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// �������ص�ģ�����
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �������ص�ģ�����
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �������ģ��״̬
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
		return false;

	// �������ģ��״̬
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	// ������ģ������
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Ϊ�ر�Z-Buffer���������ģ��״̬������һ��Ψһ��ͬ����DepthEnable��Ϊfalse
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �������ģ��״̬
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
		return false;

	return true;
}

bool D3DClass::Initialize_DepthStencilView()
{
	HRESULT				result;

	// ��ʼ�����ģ����ͼ����
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// �������ģ����ͼ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// �������ģ����ͼ
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
		return false;

	// �󶨻���Ŀ����ͼ�����ģ����ͼ��������ƹܵ�
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return true;
}

bool D3DClass::Initialize_RasterizerState()
{
	HRESULT				result;

	// ������դ�����ӣ��⽫������λ��ƶ����
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// �ɸոս����������Ӵ�����դ��״̬
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
		return false;

	// ���ù�դ��״̬
	m_deviceContext->RSSetState(m_rasterState);

	return true;
}

bool D3DClass::Initialize_Viewport(int screenWidth, int screenHeight)
{

	// ���û����ӵ�
	D3D11_VIEWPORT		 viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// ���������ӵ�
	m_deviceContext->RSSetViewports(1, &viewport);

	
	return true;
}

bool D3DClass::Initialize_Matrix(int screenWidth, int screenHeight,float screenDepth, float screenNear)
{
	HRESULT				result;

	// ����ͶӰ����
	float				 fieldOfView, screenAspect;
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Ϊ3D���ƴ���͸��ͶӰ����
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// Ϊ2D���ƴ�������ͶӰ����
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// ��ʼ���������Ϊ��λ��
	m_worldMatrix = XMMatrixIdentity();

	return true;
}

bool D3DClass::Initialize_BlendState()
{
	HRESULT				result;

	// Clear the blend state description.
	D3D11_BLEND_DESC	blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}
	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
		return false;
	return true;
}
