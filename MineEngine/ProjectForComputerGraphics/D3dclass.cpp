// 文件名: d3dclass.cpp
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
	// 记录是否与显示器同步设置
	m_vsync_enabled = vsync;

	// 取得刷新频率的分子和分母
	UINT	numerator, denominator;
	if (!Initialize_RefreshRate(screenWidth, screenHeight, numerator, denominator))
		return false;

	// 初始化设备和交换链
	if (!Initialize_DeviceAndSwapChain(screenWidth, screenHeight, hwnd, fullscreen, numerator, denominator))
		return false;

	// 初始化绘制目标视图
	if (!Initialize_renderTargetView())
		return false;

	//  初始化深度模板缓存
	if (!Initialize_DepthStencilBuffer(screenWidth, screenHeight))
		return false;

	//  初始化深度模板状态
	if (!Initialize_depthStencilState())
		return false;

	//  初始化深度模板视图
	if (!Initialize_DepthStencilView())
		return false;

	// 初始化光栅器状态
	if (!Initialize_RasterizerState())
		return false;

	// 初始化光栅器视点
	if (!Initialize_Viewport(screenWidth,  screenHeight))
		return false;

	// 初始化矩阵
	if (!Initialize_Matrix( screenWidth,  screenHeight,  screenDepth,  screenNear))
		return false;

	// 初始化混合状态
	if (!Initialize_BlendState())
		return false;

	return true;
}

void D3DClass::Shutdown()
{
	// 在析构前设置为窗口模式，否则交换链会抛出异常
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

	// 设置用于清除缓存的颜色
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 清除后缓存
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 清除深度缓存
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	// 将后缓存数据显示到屏幕上（当前绘制已完成）
	if (m_vsync_enabled)
	{
		// 锁定屏幕刷新率
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 以最快速度刷新
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

	// 创建 DirectX 图形接口工厂
	IDXGIFactory*	factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	// 使用工厂为主图形接口（显卡）创建一个适配器
	IDXGIAdapter*		adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return false;

	// 枚举主适配器输出（监视器）
	IDXGIOutput*		adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		return false;

	// 获取适合适配器输出（显示器）的DXGI_FORMAT_R8G8B8A8_UNORM显示格式的模式数量。 
	unsigned int		numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
		return false;

	// 创建一个列表来保存此显示器/视频卡组合的所有可能的显示模式。
	DXGI_MODE_DESC*	displayModeList;
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;

	// 填充displayModeList
	DXGI_ADAPTER_DESC	adapterDesc;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
		return false;

	// 遍历所有显示模式，找到匹配屏幕尺寸的那个。并存储该模式下刷新频率的分子和分母
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

	// 获取显卡描述
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
		return false;

	// 以兆字节为单位存储专用图形卡内存
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 将显卡名转为多字节字符并存储
	size_t stringLength;
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
		return false;

	// 释放displayModeList
	delete[] displayModeList;
	displayModeList = 0;

	// 释放适配器输出(监视器)
	adapterOutput->Release();
	adapterOutput = 0;

	// 释放适配器(显卡)
	adapter->Release();
	adapter = 0;

	// 释放工厂
	factory->Release();
	factory = 0;

	return true;
}

bool D3DClass::Initialize_DeviceAndSwapChain(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen, UINT& numerator, UINT& denominator)
{
	HRESULT				result;

	// 初始化交换链描述
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 设置一个单后缓存、并设定其尺寸、格式、刷新率、使用方式
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

	// 设置绘制目标窗口
	swapChainDesc.OutputWindow = hwnd;

	// 关闭多采样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 设置全屏或窗口模式
	if ( fullscreen )
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	// 将扫描线顺序和缩放设置为未指定
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 显示后丢弃后缓存
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 不置位高级标志
	swapChainDesc.Flags = 0;

	// 设置特定等级为 DirectX 11.
	D3D_FEATURE_LEVEL	 featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 创建交换链，D3D设备，D3D设备上下文
	//（如果设备上没有支持DirectX11的显卡，则将 D3D_DRIVER_TYPE_HARDWARE 替换为 D3D_DRIVER_TYPE_REFERENCE ， DirectX11将使用CPU来绘制）
	//（如果设备具有多张显卡，则需要枚举显卡并让用户选择使用哪张显卡来支持DirectX11、然后使用该显卡创建设备)
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
		return false;

	return true;
}

bool D3DClass::Initialize_renderTargetView()
{
	HRESULT				result;

	// 取得后缓存的指针
	ID3D11Texture2D*	 backBufferPtr;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		return false;

	// 使用后缓存指针创建绘制目标视图
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
		return false;

	// 释放后缓存指针
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	return true;
}

bool D3DClass::Initialize_DepthStencilBuffer(int screenWidth, int screenHeight)
{
	HRESULT				result;

	// 初始化深度缓存的描述子
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	// 建立深度缓存的描述子
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

	// 创建深度缓存的纹理using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	return true;
}

bool D3DClass::Initialize_depthStencilState()
{
	HRESULT				result;

	// 初始化深度模板状态的描述
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 建立深度模板状态描述
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 正面像素的模板操作
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 背面像素的模板操作
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 创建深度模板状态
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
		return false;

	// 设置深度模板状态
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	// 清空深度模板描述
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// 为关闭Z-Buffer创建此深度模板状态。与另一个唯一不同在于DepthEnable设为false
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

	// 创建深度模板状态
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
		return false;

	return true;
}

bool D3DClass::Initialize_DepthStencilView()
{
	HRESULT				result;

	// 初始化深度模板视图描述
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 设置深度模板视图描述
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 创建深度模板视图
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
		return false;

	// 绑定绘制目标视图和深度模板视图到输出绘制管道
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return true;
}

bool D3DClass::Initialize_RasterizerState()
{
	HRESULT				result;

	// 建立光栅描述子，这将决定如何绘制多边形
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

	// 由刚刚建立的描述子创建光栅器状态
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
		return false;

	// 设置光栅器状态
	m_deviceContext->RSSetState(m_rasterState);

	return true;
}

bool D3DClass::Initialize_Viewport(int screenWidth, int screenHeight)
{

	// 设置绘制视点
	D3D11_VIEWPORT		 viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// 创建绘制视点
	m_deviceContext->RSSetViewports(1, &viewport);

	
	return true;
}

bool D3DClass::Initialize_Matrix(int screenWidth, int screenHeight,float screenDepth, float screenNear)
{
	HRESULT				result;

	// 设置投影矩阵
	float				 fieldOfView, screenAspect;
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 为3D绘制创建透视投影矩阵
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 为2D绘制创建正交投影矩阵
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// 初始化世界矩阵为单位阵
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
