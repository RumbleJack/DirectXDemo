// 文件名: d3dclass.cpp
#include "stdafx.h"
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
	// 记录垂直同步设置、屏幕尺寸
	m_vsync_enabled = vsync;
	m_screenWidth = screenWidth; 
	m_screenHeight = screenHeight;
	m_hwnd = hwnd;

	// 取得刷新频率的分子和分母
	UINT	numerator, denominator;
	if (!Initialize_RefreshRate( numerator, denominator))
		return false;

	// 初始化设备和交换链
	if (!Initialize_DeviceAndSwapChain(fullscreen, numerator, denominator))
		return false;

	// 初始化绘制目标视图
	if (!Initialize_renderTargetView())
		return false;

	//  初始化深度模板缓存
	if (!Initialize_DepthStencilBufferAndView())
		return false;

	//  初始化深度模板状态
	if (!Initialize_DepthStencilState())
		return false;

	// 初始化光栅器状态
	if (!Initialize_RasterizerState())
		return false;

	// 初始化光栅器视点
	if (!Initialize_Viewport())
		return false;

	// 初始化矩阵
	if (!Initialize_Matrix( screenDepth,  screenNear))
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

//void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
//{
//	worldMatrix = m_worldMatrix;
//	return;
//}

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

bool D3DClass::Initialize_RefreshRate(UINT& numerator, UINT& denominator)
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
		if (displayModeList[i].Width == (unsigned int)m_screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)m_screenHeight)
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

	// 释放适配器输出(监视器)、适配器(显卡)、工厂
	RELEASE_RESOURCE(adapterOutput);
	RELEASE_RESOURCE(adapter);
	RELEASE_RESOURCE(factory);

	return true;
}

bool D3DClass::Initialize_DeviceAndSwapChain( bool fullscreen, UINT& numerator, UINT& denominator)
{
	HRESULT				result;
	D3D_FEATURE_LEVEL	 expectedFeatureLevel, supportFeatureLevel;
	expectedFeatureLevel = D3D_FEATURE_LEVEL_11_0;// 设置特定等级为 DirectX 11.
	UINT createDeviceFlags = 0;
	//#if defined(DEBUG) || defined(_DEBUG)  
	//	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	//#endif
	result = D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		&expectedFeatureLevel, 1,
		D3D11_SDK_VERSION,
		&m_device, &supportFeatureLevel, &m_deviceContext);
	if (FAILED(result))
		return false;
	if (supportFeatureLevel != expectedFeatureLevel)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}
	//result = D3D11CreateDeviceAndSwapChain(
	//	NULL, // 默认显示适配器
	//	D3D_DRIVER_TYPE_HARDWARE,  // 硬件实现绘制
	//	NULL, // 硬件实现绘制不需要该参数
	//	createDeviceFlags,	  // 不设置特殊用途Flag 如3D11_CREATE_DEVICE_DEBUG
	//	&expectedFeatureLevel, 1, // 指定备选D3D版本,指定数组大小
	//	D3D11_SDK_VERSION, 
	//	&swapChainDesc, &m_swapChain, &m_device, &supportFeatureLevel, &m_deviceContext);

	// 初始化交换链描述
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	// 设置后缓存属性
	swapChainDesc.BufferDesc.Width = m_screenWidth;
	swapChainDesc.BufferDesc.Height = m_screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 将扫描线顺序为未指定
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;				  // 将缩放设置为未指定
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	// 多重采样抗锯齿设置，此处关闭多采样
	if (MSAA4X_ENABLED)
	{
		UINT num4xMsaaQuality;
		m_device->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM, 8, &num4xMsaaQuality);
		swapChainDesc.SampleDesc.Count = 8;
		swapChainDesc.SampleDesc.Quality = num4xMsaaQuality - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;		// 仅创建一个后缓存
	swapChainDesc.OutputWindow = m_hwnd;  // 设置绘制目标窗口
										  // 设置全屏或窗口模式
	if (fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;// 显示后丢弃后缓存
	swapChainDesc.Flags = 0;// 不置位高级标志

	IDXGIDevice* dxgiDevice = 0;
	m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory* dxgiFactory = 0;
	(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
	(dxgiFactory->CreateSwapChain(m_device, &swapChainDesc, &m_swapChain));
	RELEASE_RESOURCE(dxgiDevice);
	RELEASE_RESOURCE(dxgiAdapter);
	RELEASE_RESOURCE(dxgiFactory);
	// 重新创建设备交换链，开启多重采样
	// Turn4xMsaaOn();
	return true;
}

bool D3DClass::Initialize_renderTargetView()
{
	HRESULT				result;

	// 取得后缓存的指针，0指示缓存索引（此处只有一个后缓存）
	ID3D11Texture2D*	 backBufferPtr;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		return false;

	// 使用后缓存指针创建绘制目标视图
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
		return false;

	// 释放后缓存
	RELEASE_RESOURCE(backBufferPtr);

	return true;
}

bool D3DClass::Initialize_DepthStencilBufferAndView()
{
	HRESULT				result;

	// 初始化深度缓存的描述子
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	// 建立深度缓存的描述子
	depthBufferDesc.Width = m_screenWidth;
	depthBufferDesc.Height = m_screenHeight;
	depthBufferDesc.MipLevels = 1;	// 多级渐近纹理层（mipmap level）的数量。对于深度/模板缓冲区来说，我们的纹理只需要一个多级渐近纹理层。
	depthBufferDesc.ArraySize = 1;	// 在纹理数组中的纹理数量。对于深度/模板缓冲区来说，我们只需要一个纹理。
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 多重采样抗锯齿设置，此处关闭多采样
	if (MSAA4X_ENABLED)
	{
		UINT num4xMsaaQuality;
		m_device->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM, 8, &num4xMsaaQuality);
		depthBufferDesc.SampleDesc.Count = 8;
		depthBufferDesc.SampleDesc.Quality = num4xMsaaQuality - 1;
	}
	else
	{
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
	}
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT; // D3D11_USAGE_DYNAMIC
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0; // D3D11_CPU_ACCESS_WRITE
	depthBufferDesc.MiscFlags = 0;		// 可选标志

										// 创建深度缓存的纹理，深度缓存及模板缓存由d3d类管理，无需初始值（第二个参数为NULL）
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	//// 初始化深度模板视图描述
	//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	//depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; // 多重采样需要指定为D3D11_DSV_DIMENSION_TEXTURE2DMS
	//depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 创建深度模板视图，当m_depthStencilBuffer指定为强类型时，第二个参数可以为空
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView);
	if (FAILED(result))
		return false;

	// 绑定绘制目标视图和深度模板视图到输出合并阶段,第一个参数是将要绑定的渲染目标的数量，此时后缓存只有一个
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return true;
}


bool D3DClass::Initialize_DepthStencilState()
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


bool D3DClass::Initialize_RasterizerState()
{
	HRESULT				result;

	// 建立光栅描述子，这将决定如何绘制多边形
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;	// 是否使用线性反走样
	rasterDesc.CullMode = D3D11_CULL_BACK;		// 三角形正面背面绘制方式
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = false;		// 是否使用深度裁剪
	rasterDesc.FillMode = D3D11_FILL_SOLID; // 实心填充
	rasterDesc.FrontCounterClockwise = false;	// false指顺时针为正面
	rasterDesc.MultisampleEnable = true;	// 是否使用多重采样
	rasterDesc.ScissorEnable = false;		// 是否使用裁剪矩形
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 由刚刚建立的描述子创建光栅器状态
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
		return false;

	// 设置光栅器状态
	m_deviceContext->RSSetState(m_rasterState);

	return true;
}

bool D3DClass::Initialize_Viewport()
{
	// 设置绘制视口
	D3D11_VIEWPORT		 viewport;
	// 指定客户区子窗口
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)m_screenWidth;
	viewport.Height = (float)m_screenHeight;
	// 指定绘制对象对应的深度缓存的最大最小值，通常与深度缓存的范围一致
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// 将视口绑定到设备上下文的光栅化阶段，第一个参数指定视口数，第二个参数为视口指针
	m_deviceContext->RSSetViewports(1, &viewport);
	return true;
}

bool D3DClass::Initialize_Matrix(float screenDepth, float screenNear)
{
	// 设置投影矩阵
	float		   fieldOfView, screenAspect;
	fieldOfView = PI / 4.0f;
	screenAspect = (float)m_screenWidth / (float)m_screenHeight;
	// 为3D绘制创建透视投影矩阵
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 为2D绘制创建正交投影矩阵
	m_orthoMatrix = XMMatrixOrthographicLH((float)m_screenWidth, (float)m_screenHeight, screenNear, screenDepth);
	return true;
}

bool D3DClass::Initialize_BlendState()
{
	HRESULT				result;

	// 创建混合状态描述
	D3D11_BLEND_DESC	blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	// 创建允许Alpha混合的状态描述
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;	// 开启混合
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;	// 源混合因子为1
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 目标混合因子为（1-源颜色的Alpha通道）
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 混合操作为 源+目标
																		// 以下三项意义不明确，都设为D3D11_BLEND_ONE，应该不会影响结果
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	// 该项意义不明确，猜测是指定RGBA哪几个通道混合，设置为D3D11_COLOR_WRITE_ENABLE_ALL，则所有通道进行混合
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 使用混合描述创建混合状态
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}
	// 将允许Alpha混合的状态描述修改为不允许
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	// 使用混合描述创建混合状态
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
		return false;
	return true;
}
