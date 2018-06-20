#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"
#include <DirectXTex.h>
#include "Shobjidl.h"	// Для диалога

D3DClass::D3DClass()
{
	D3DGC = new D3DGlobalContext;

	ActiveAdapter = 0;
	D3DGC->EnableMSAA = false;
	D3DGC->EnableFXAA = false;
	D3DGC->ShadowsOn = true;
	D3DGC->SoftShadowsOn = true;
	D3DGC->PCF_Amount = 8.0f;
	D3DGC->PCF_Step = 1.0f;
	D3DGC->ShadowCLAMP = 1.0f;
	D3DGC->Shadow_Divider = 1024;
	D3DGC->ClustersAmount = 16;	// Clustering Render base clusters XYZ amount

	SMWidth = 1024;// Ширина Shadow Map
	SMHeight = 1024;// Высота Shadow Map

	D3DGC->MSAAQualityCount = 1;//1 2 4 8
	D3DGC->MSAAQualityChoosen = 0; //0 2 16 32
	
	D3DGC->hwnd							= nullptr;
	D3DGC->Adapter						= nullptr;
	D3DGC->DX_device					= nullptr;
	D3DGC->DX_deviceContext				= nullptr;
	D3DGC->DX_swapChain					= nullptr;
	D3DGC->D10_device					= nullptr;
	D3DGC->BackBuffer2DT				= nullptr;
	D3DGC->BackBuffer_SRV				= nullptr;
	D3DGC->BackBuffer_RTV				= nullptr;
	D3DGC->BackBuffer_UAV				= nullptr;
	D3DGC->DWriteFactory				= nullptr;
	D3DGC->m_alphaEnableBlendingState	= nullptr;
	D3DGC->m_alphaDisableBlendingState	= nullptr;
	D3DGC->m_alpha_TOnT_BlendingState	= nullptr;	// Текст на текстуре
	D3DGC->depthStencil_State			= nullptr;
	D3DGC->CubeMap_DepthStencil_State	= nullptr;
	D3DGC->m_depthStencilView			= nullptr;
	D3DGC->mGeometryBlendState			= nullptr;
	D3DGC->m_alphaParticleBlendingState = nullptr;
	D3DGC->m_depthStencilBuffer			= nullptr;
	D3DGC->DefaultRasterizerState		= nullptr;
	D3DGC->WireFrameRasterizerState		= nullptr;
	D3DGC->m_EngineInputClass			= nullptr;
	D3DGC->RasterStateCullNone			= nullptr;
	D3DGC->depthStencil_Disabled_State	= nullptr;
	D3DGC->depthStencil_NoWrite_Particles = nullptr;
	D3DGC->Global_VS_ConstantsBuffer	= nullptr;
	D3DGC->ShadowMapLightView			= nullptr;
	D3DGC->LightRender_RS				= nullptr;
	D3DGC->LightRender_DS				= nullptr;
	D3DGC->DSV_ShadowMap3D				= nullptr;


#if defined( DEBUG ) || defined( _DEBUG )
	D3DGC->DebugDevice = nullptr;
#endif

// FXAA
	D3DGC->g_pcbFXAA					= nullptr;
	PerFrameConstants_Debug				= nullptr;
	D3DGC->BackBuffer_ProxyTexture		= nullptr;
	D3DGC->BackBuffer_ProxyTextureSRV	= nullptr;
	D3DGC->BackBuffer_ProxyTextureRTV	= nullptr;
	D3DGC->BackBuffer_CopyResolveTexture= nullptr;
	D3DGC->BackBuffer_CopyResolveTextureSRV = nullptr;
	D3DGC->BackBuffer_CopyResolveTextureUAV = nullptr;

	D3DGC->ScreenShootTexture			= nullptr;

// Shadow Maps & Lights
	D3DGC->Wrap_Model_Texture			= nullptr;
	D3DGC->CLight_ShadowMap_Sampler		= nullptr;
	D3DGC->CLight_SM_PCF_Sampler		= nullptr;
	D3DGC->FlatObject_Sampler			= nullptr;

// Direct Write Fonts
	D3DGC->sharedTex11					= nullptr;
	D3DGC->sharedTex11_SRV				= nullptr;
	D3DGC->sharedTex11_MAPED			= nullptr;
	D3DGC->keyedMutex11					= nullptr;
	D3DGC->keyedMutex10					= nullptr;
	D3DGC->D2DRenderTarget				= nullptr;
	D3DGC->D2DFactory					= nullptr;
	
	LightShaderForDraw					= -1;
	ShadowMapShaderIndex				= -1;

	Light = nullptr;

	lightBufferSRV = nullptr;
	LightGridBufferSRV = nullptr;

	ShadowMap3D = nullptr;
	SRV_ShadowMap3D = nullptr;

	PointLightSize = sizeof ( PointLight );

	mPointLightParameters.reserve ( MAX_LIGHTS );

	FreeLightIndex.reserve ( MAX_LIGHTS );

	SpotLightsWithShadowsIndexes.reserve ( MAX_LIGHTS );

	LightPos.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	LightPosCopy.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	Temp.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	Temp2.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };

	ClustersAmount = 0;

	mLightGridBufferSize = 64 * 1024 * 32; // 32 MB;

	// Размеры текстуры для отрисовки шрифтов 
	// достаточные для размера шрифта 74.0f с Outline 10
	// Максимальная длинна 8192 для DX10
	D3DGC->sharedTex11_Width			= 8192;
	D3DGC->sharedTex11_Height			= 128;

	FXAAShaderIndex						= -1;
	BlureHorizComputeShaderIndex		= -1;
	BlureVertComputeShaderIndex			= -1;
	ShadowMapShaderIndex				= -1;
}


D3DClass::~D3DClass()
{
	delete D3DGC;
}

bool D3DClass::Initialize(HWND hwnd, int screenWidth, int screenHeight, bool vsync, bool fullscreen,
	float screenDepth, float screenNear, FrustumClass* frustum )
{
	HRESULT result;
	IDXGIFactory1*			 Factory1	= nullptr;
	IDXGIOutput*        AdapterOutput	= nullptr;

	D3DGC->hwnd							= hwnd;
	D3DGC->ScreenWidth					= screenWidth;
	D3DGC->ScreenHeight					= screenHeight;
	D3DGC->ScreenDimentions				= { float ( screenWidth ), float ( screenHeight ) };
	D3DGC->ScreenRatio = float(D3DGC->ScreenWidth / D3DGC->ScreenHeight);
	D3DGC->FarPlane						= screenDepth;
	D3DGC->NearPlane					= screenNear;
	UINT numModes = 0;
	UINT numerator = 0;
	UINT denominator = 0;
	size_t stringLength = 0;
	DXGI_ADAPTER_DESC1 AdapterDesc;
	DXGI_MODE_DESC* displayModeList		= nullptr;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevelSuccess;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
//	D3D11_DEPTH_STENCIL_DESC depthStencil_Disabled_Desc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;
	D3D11_BLEND_DESC blendStateDescription;

	m_videoCardDescription = new char[128];
	
	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Before we can initialize Direct3D we have to get the refresh rate from the video card/monitor. Each computer may be slightly different so we will need to query for that information.
	// We query for the numerator and denominator values and then pass them to DirectX during the setup and it will calculate the proper refresh rate. If we don't do this and just set the refresh rate
	// to a default value which may not exist on all computers then DirectX will respond by performing a blit instead of a buffer flip which will degrade performance and give us annoying
	// errors in the debug output.

	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476877%28v=vs.85%29.aspx
	// http://cppstudio.com/post/8453/

	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&Factory1);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize CreateDXGIFactory1.", Error, MB_OK);
			goto ERROR_EXIT;
	}

	for (UINT i = 0; Factory1->EnumAdapters1(i, &FoundAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		Adapters.push_back(FoundAdapter1);
	}

	for (UINT i = 0; i < Adapters.size(); ++i)
	{
		Adapters[i]->GetDesc1(&AdapterDesc);

		if (AdapterDesc.VendorId != 5140)
		{

			m_videoCardMemory = (int)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

			if ((AdapterDesc.VendorId == 4318 || AdapterDesc.VendorId == 4098) && m_videoCardMemory > 512)
			{
				ActiveAdapter = i;

				//                wchar_t* a = new wchar_t[250];
				//                wchar_t* b = new wchar_t[10];
				//                wcscpy_s(a, 250, L"Vendor ID =");
				//                _itow_s(AdapterDesc.VendorId,b,250, 10);
				//                wcscat_s(a,250,b);
				//                MessageBox(hwnd, a,L"Message", MB_OK);
				UINT error;
				error = wcstombs_s(&stringLength, m_videoCardDescription, 128, AdapterDesc.Description, 127);
				if (error != 0)
				{
					MessageBox(hwnd, L"Could not copy VideoAdapter name.", Error, MB_OK);
					RCUBE_RELEASE( Factory1 );
					goto ERROR_EXIT;
				}
				// Объединяем строки имени адаптера и количества памяти
				sprintf_s(m_videoCardDescription, 128, "%s with %d Mb.", m_videoCardDescription, m_videoCardMemory);
				//                break; // Включить для выбора 1-го встретившегося адаптера на ноутбуке
			}

		}
	}

	result = Adapters[0]->EnumOutputs(0, &AdapterOutput);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not get Adapters[0]->EnumOutputs.", Error, MB_OK);
		RCUBE_RELEASE( Factory1 );
		goto ERROR_EXIT;
	}

	result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not get AdapterOutput->GetDisplayModeList.", Error, MB_OK);
		RCUBE_RELEASE( Factory1 );
		goto ERROR_EXIT;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		MessageBox(hwnd, L"No DisplayMode in List.", Error, MB_OK);
		RCUBE_ARR_DELETE( displayModeList );
		RCUBE_RELEASE( AdapterOutput );
		RCUBE_RELEASE( Factory1 );
		goto ERROR_EXIT;
	}

	AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);

	UINT lastwidth = 0;

	wchar_t *Temp = new wchar_t[15];

	for (UINT i = 0; i < numModes; ++i)
	{

		if (displayModeList[i].Width >= 800 && displayModeList[i].Height >= 600)
			// Если режим последний в списке, то не проверять displayModeList[i + 1].Width - ВЫХОД ЗА ПРЕДЕЛЫ
			if (i == (numModes - 1))
				goto Goon;
		if (lastwidth < displayModeList[i].Width &&
			displayModeList[i].Width < displayModeList[i + 1].Width)
		{
Goon:       Display_Mode *Mode = new Display_Mode; // освобождается в Shutdown
			Mode->Freqency = displayModeList[i].RefreshRate.Numerator;
			Mode->Hieght = displayModeList[i].Height;
			Mode->Width = displayModeList[i].Width;
			Mode->Number = i;

			_itow_s(Mode->Width, Temp, 15, 10);
			wcscpy_s(Mode->Resolution, 15, Temp);
			wcscat_s(Mode->Resolution, 15, L" x ");
			_itow_s(Mode->Hieght, Temp, 15, 10);
			wcscat_s(Mode->Resolution, 15, Temp);

			ActualDisplayModes.push_back(Mode);

			lastwidth = displayModeList[i].Width;
		}
		if (displayModeList[i].Width == screenWidth)
		{
			if (displayModeList[i].Height == screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	RCUBE_ARR_DELETE( Temp );
	RCUBE_ARR_DELETE( displayModeList );
	   RCUBE_RELEASE( AdapterOutput );
	
	D3DGC->Adapter = Adapters[ActiveAdapter];

	// Now that we have the refresh rate from the system we can start the DirectX initialization. The first thing we'll do is fill out the description of the swap chain.
	// The swap chain is the front and back buffer to which the graphics will be drawn. Generally you use a single back buffer, do all your drawing to it, and then swap it to the front buffer which
	// then displays on the user's screen. That is why it is called a swap chain.

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// The next part of the description of the swap chain is the refresh rate. The refresh rate is how many times a second it draws the back buffer to the front buffer.
	// If vsync is set to true in our graphicsclass.h header then this will lock the refresh rate to the system settings (for example 60hz). That means it will only draw the screen 60 times a second
	// (or higher if the system refresh rate is more than 60). However if we set vsync to false then it will draw the screen as many times a second as it can, however this can cause some visual artifacts.

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;// | DXGI_USAGE_UNORDERED_ACCESS;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false; // false - для запуска на полный экран
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// After setting up the swap chain description we also need to setup one more variable called the feature level. This variable tells DirectX what version we plan to use. Here we set the feature level to 11.0 which is DirectX 11. You can set this to 10 or 9 to use a lower level version of DirectX if you plan on supporting multiple versions or running on lower end hardware.

	//  ++++++++++++++++++++++++++++++++++    QualityLevel Analysing and setting   +++++++++++++++++++++++++++++++++++++++++++
	// Set the feature level to DirectX 11.
	D3D_FEATURE_LEVEL featureLevels[1] =
	{
		D3D_FEATURE_LEVEL_11_0, // First check D3D 11 support
	};

	UINT nDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;// | D3D11_CREATE_DEVICE_DEBUG;

#if defined( DEBUG ) || defined( _DEBUG )
	// If the project is in a debug build, enable the debug layer.
	nDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;// | D3D11_CREATE_DEVICE_DEBUGGABLE;
#endif

	// Try to Init D3DDivece for DX11 usage
	result = D3D11CreateDevice(D3DGC->Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, nDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &D3DGC->DX_device, &featureLevelSuccess, &D3DGC->DX_deviceContext);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"D3D11CreateDevice Failed.", Error, 0);
		RCUBE_RELEASE( Factory1 );
		goto ERROR_EXIT;
	}
	if (featureLevelSuccess != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(hwnd, L"Direct3D Feature Level 11 unsupported.", Error, 0);
		RCUBE_RELEASE( Factory1 );
		goto ERROR_EXIT;
	}

#if defined( DEBUG ) || defined( _DEBUG )

	const char c_szName[] = "MyDeViCe KAF";
	D3DGC->DX_device->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);

	const char c_szName1[] = "FIRST CONTEXT KAF";
	D3DGC->DX_deviceContext->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName1) - 1, c_szName1);

	result = D3DGC->DX_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&D3DGC->DebugDevice));
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize DebugDevice.", Error, MB_OK);
//		AdaptersRelease();
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
	}
#endif
	UINT MSAAQualityNumber;
	// Check MSAA Quality Levels supported
	result = D3DGC->DX_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, D3DGC->MSAAQualityCount, &MSAAQualityNumber);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CheckMultisampleQualityLevels.", Error, 0);
		RCUBE_RELEASE( Factory1 );
		goto ERROR_EXIT;
	}
	//    assert(m4xMsaaQuality > 0 );

	D3DGC->MSAAQualityChoosen = MSAAQualityNumber - 1;
	// Use 4X MSAA?
	if (D3DGC->EnableMSAA)
	{
		swapChainDesc.SampleDesc.Count = D3DGC->MSAAQualityCount; // m4xMsaaQuality is returned via CheckMultisampleQualityLevels().
		swapChainDesc.SampleDesc.Quality = D3DGC->MSAAQualityChoosen; //m4xMsaaQuality-1;
	}

	// No MSAA
	else {
		// Turn multisampling off.
		swapChainDesc.SampleDesc.Count = D3DGC->MSAAQualityCount = 1;
		swapChainDesc.SampleDesc.Quality = D3DGC->MSAAQualityChoosen = 0;
	}

	//  ----------------------------------   QualityLevel Analysing and setting   ------------------------------------------

	// Now that the swap chain description and feature level have been filled out we can create the swap chain, the Direct3D device, and the Direct3D device context. The Direct3D device and Direct3D
	// device context are very important, they are the interface to all of the Direct3D functions. We will use the device and device context for almost everything from this point forward.
	// Those of you reading this who are familiar with the previous versions of DirectX will recognize the Direct3D device but will be unfamiliar with the new Direct3D device context.
	// Basically they took the functionality of the Direct3D device and split it up into two different devices so you need to use both now.
	// Note that if the user does not have a DirectX 11 video card this function call will fail to create the device and device context. Also if you are testing DirectX 11 functionality yourself
	// and don't have a DirectX 11 video card then you can replace D3D_DRIVER_TYPE_HARDWARE with D3D_DRIVER_TYPE_REFERENCE and DirectX will use your CPU to draw instead of the video card hardware.
	// Note that this runs 1/1000 the speed but it is good for people who don't have DirectX 11 video cards yet on all their machines.

	// Create the swap chain, Direct3D device, and Direct3D device context.
	//    result = D3D11CreateDeviceAndSwapChain(m_adapter1, D3D_DRIVER_TYPE_UNKNOWN, NULL,0, &featureLevel, 1,
	//                           D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);

	result = Factory1->CreateSwapChain(D3DGC->DX_device, &swapChainDesc, &D3DGC->DX_swapChain);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CreateSwapChain.", Error, 0);
		RCUBE_RELEASE( Factory1 );
		goto ERROR_EXIT;
	}

	// Release the adapter.
	//    Adapters.clear();

	// Release the factory.
	RCUBE_RELEASE( Factory1 );

	// Sometimes this call to create the device will fail if the primary video card is not compatible with DirectX 11. Some machines may have the primary card as a DirectX 10 video card and the
	// secondary card as a DirectX 11 video card. Also some hybrid graphics cards work that way with the primary being the low power Intel card and the secondary being the high power Nvidia card.
	// To get around this you will need to not use the default device and instead enumerate all the video cards in the machine and have the user choose which one to use and then specify that card when
	// creating the device.
	// Now that we have a swap chain we need to get a pointer to the back buffer and then attach it to the swap chain. We'll use the CreateRenderTargetView function to attach the back buffer
	// to our swap chain.

	// Get the pointer to the back buffer.
	result = D3DGC->DX_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&D3DGC->BackBuffer2DT );
	if (FAILED(result))
	{
		MessageBox(hwnd, L"m_swapChain->GetBuffer.", Error, 0);
		goto ERROR_EXIT;
	}

	// Create the render target view with the back buffer pointer.
	result = D3DGC->DX_device->CreateRenderTargetView(D3DGC->BackBuffer2DT, NULL, &D3DGC->BackBuffer_RTV );
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CreateRenderTargetView.", Error, 0);
		goto ERROR_EXIT;
	}

	result = D3DGC->DX_device->CreateShaderResourceView(D3DGC->BackBuffer2DT, NULL, &D3DGC->BackBuffer_SRV );
	if ( FAILED( result ) )
	{
		MessageBox( hwnd, L"CreateShaderResourceView BackBuffer_SRV.", Error, 0 );
		goto ERROR_EXIT;
	}
/*
	result = D3DGC->DX_device->CreateUnorderedAccessView( D3DGC->BackBuffer2DT, NULL, &D3DGC->BackBuffer_UAV);
	if ( FAILED( result ) )
	{
		MessageBox( hwnd, L"Error CreateUnorderedAccessView BackBuffer_UAV.", 0, 0 );
		Shutdown();
		return false;
	}
*/
	// We will also need to set up a depth buffer description. We'll use this to create a depth buffer so that our polygons can be rendered properly in 3D space. At the same time we will attach
	// a stencil buffer to our depth buffer. The stencil buffer can be used to achieve effects such as motion blur, volumetric shadows, and other things.

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32G8X24_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = D3DGC->MSAAQualityCount;//1;
	depthBufferDesc.SampleDesc.Quality = D3DGC->MSAAQualityChoosen;//0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Now we create the depth/stencil buffer using that description. You will notice we use the CreateTexture2D function to make the buffers, hence the buffer is just a 2D texture.
	// The reason for this is that once your polygons are sorted and then rasterized they just end up being colored pixels in this 2D buffer. Then this 2D buffer is drawn to the screen.

	// Create the texture for the depth buffer using the filled out description.
	result = D3DGC->DX_device->CreateTexture2D(&depthBufferDesc, NULL, &D3DGC->m_depthStencilBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"DepthStensil CreateTexture2D.", Error, 0);
		goto ERROR_EXIT;
	}

	// Now we need to setup the depth stencil description. This allows us to control what type of depth test Direct3D will do for each pixel.

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;// true
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; //D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; ;//D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// With the description filled out we can now create a depth stencil state.

	// Create the depth stencil state.
	result = D3DGC->DX_device->CreateDepthStencilState(&depthStencilDesc, &D3DGC->depthStencil_State);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CreateDepthStencilState.", Error, 0);
		goto ERROR_EXIT;
	}

	depthStencilDesc.DepthEnable = false;
	result = D3DGC->DX_device->CreateDepthStencilState ( &depthStencilDesc, &D3DGC->depthStencil_Disabled_State );
	if ( FAILED ( result ) )
	{
		MessageBox ( hwnd, L"Create depthStencil_Disabled_State.", Error, 0 );
		goto ERROR_EXIT;
	}

	ZeroMemory ( &depthStencilDesc, sizeof ( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
	result = D3DGC->DX_device->CreateDepthStencilState ( &depthStencilDesc, &D3DGC->CubeMap_DepthStencil_State );
	if ( FAILED ( result ) )
	{
		MessageBox ( hwnd, L"CreateDepthStencilState.", Error, 0 );
		goto ERROR_EXIT;
	}

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory ( &depthStencilDesc, sizeof ( depthStencilDesc ) );

	//	Для FireParticles ( анимация частиц огня )
	// Выключаем запись в Depth = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;//D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Now create the new depth stencil.

	// Create the state using the device.
	result = D3DGC->DX_device->CreateDepthStencilState ( &depthStencilDesc, &D3DGC->depthStencil_NoWrite_Particles );
	if ( FAILED ( result ) )
	{
		MessageBox ( hwnd, L"Create depthStencil_NoWrite_Particles.", Error, 0 );
		goto ERROR_EXIT;
	}

	// ShadowMap
	{
		CD3D11_DEPTH_STENCIL_DESC desc ( D3D11_DEFAULT );
		// NOTE: Complementary Z => GREATER test
		desc.DepthEnable = true;
		desc.StencilEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;//D3D11_COMPARISON_GREATER;//D3D11_COMPARISON_LESS;
		result = D3DGC->DX_device->CreateDepthStencilState ( &desc, &D3DGC->LightRender_DS );
		if ( FAILED ( result ) )
		{
			MessageBox ( hwnd, L"Create ShadowMap DepthStencilState.", Error, 0 );
			goto ERROR_EXIT;
		}
	}

	// ShadowMap Rasterizer State
	{
		CD3D11_RASTERIZER_DESC desc ( D3D11_DEFAULT );
		desc.CullMode = D3D11_CULL_FRONT; //D3D11_CULL_BACK; //D3D11_CULL_FRONT; //D3D11_CULL_NONE;//
		desc.FillMode = D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;//D3D11_FILL_SOLID;//
		desc.FrontCounterClockwise = false;
		desc.ScissorEnable = false;
		desc.DepthClipEnable = true;//true;
		desc.MultisampleEnable = true;//false;
		desc.AntialiasedLineEnable = true;
		desc.DepthBias = 1;//1.e5;//0.0f;//1.e5;
		desc.SlopeScaledDepthBias = 1.0f;// 1.0f;//8.0;//0.0f;//8.0;
		desc.DepthBiasClamp = 1.0f;
		D3DGC->DX_device->CreateRasterizerState ( &desc, &D3DGC->LightRender_RS );
	}

	// The next thing we need to create is the description of the view of the depth stencil buffer. We do this so that Direct3D knows to use the depth buffer as a depth stencil texture.
	// After filling out the description we then call the function CreateDepthStencilView to create it.
	{
		// Initailze the depth stencil view.
		CD3D11_DEPTH_STENCIL_VIEW_DESC DSD (
			// Для выключеного MSAA МОЖНО ставить D3D11_DSV_DIMENSION_TEXTURE2D
			// Для ВКЛ MSAA нужно D3D11_DSV_DIMENSION_TEXTURE2DMS
			D3D11_DSV_DIMENSION_TEXTURE2DMS,
			//D3D11_DSV_DIMENSION_TEXTURE2D,
			DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
			0,
			0,
			1 );
		// Create the depth stencil view.
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// 2-й параметр  поставил NULL вместо &depthStencilViewDesc и заработало сглаживание
		result = D3DGC->DX_device->CreateDepthStencilView ( D3DGC->m_depthStencilBuffer, &DSD, &D3DGC->m_depthStencilView );
		if ( FAILED ( result ) )
		{
			MessageBox ( hwnd, L"CreateDepthStencilView.", Error, 0 );
			goto ERROR_EXIT;
		}
	}
	// With that created we can now call OMSetRenderTargets. This will bind the render target view and the depth stencil buffer to the output render pipeline.
	// This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created. With the graphics written to the back buffer we can then swap it to the front
	// and display our graphics on the user's screen.

	// Можно удалить следующую строку
	D3DGC->DX_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, D3DGC->m_depthStencilView);

	// Now that the render targets are setup we can continue on to some extra functions that will give us more control over our scenes for future tutorials. First thing is we'll create is a
	// rasterizer state. This will give us control over how polygons are rendered. We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces
	// of polygons. By default DirectX already has a rasterizer state set up and working the exact same as the one below but you have no control to change it unless you set up one yourself.

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = D3DGC->EnableMSAA; // false
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = D3DGC->DX_device->CreateRasterizerState(&rasterDesc, &D3DGC->DefaultRasterizerState );
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CreateRasterizerState.", Error, 0);
		goto ERROR_EXIT;
	}
//	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthClipEnable = false;
	result = D3DGC->DX_device->CreateRasterizerState(&rasterDesc, &D3DGC->RasterStateCullNone );
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CreateRasterizerState CULLNONE.", Error, 0);
		goto ERROR_EXIT;
	}

	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	result = D3DGC->DX_device->CreateRasterizerState ( &rasterDesc, &D3DGC->WireFrameRasterizerState );
	if ( FAILED ( result ) )
	{
		MessageBox ( hwnd, L"CreateRasterizerState FILL_WIREFRAME.", Error, 0 );
		goto ERROR_EXIT;
	}

	// Now set the rasterizer state.
	D3DGC->DX_deviceContext->RSSetState(D3DGC->DefaultRasterizerState );

	// The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. Set this to be the entire size of the window.

	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	D3DGC->DX_deviceContext->RSSetViewports(1, &viewport);

	// Now we will create the projection matrix. The projection matrix is used to translate the 3D scene into the 2D viewport space that we previously created. We will need to keep a copy of
	// this matrix so that we can pass it to our shaders that will be used to render our scenes.

	// Setup the projection matrix.
	fieldOfView = (float)XM_PIDIV4;//XM_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	D3DGC->ProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// We will also create another matrix called the world matrix. This matrix is used to convert the vertices of our objects into vertices in the 3D scene. This matrix will also be used to rotate,
	// translate, and scale our objects in 3D space. From the start we will just initialize the matrix to the identity matrix and keep a copy of it in this object. The copy will be needed to be passed
	// to the shaders for rendering also.

	// Initialize the world matrix to the identity matrix.
	D3DGC->WorldMatrix = XMMatrixIdentity();

	// This is where you would generally create a view matrix. The view matrix is used to calculate the position of where we are looking at the scene from. You can think of it as a camera and you only
	// view the scene through this camera. Because of its purpose I am going to create it in a camera class in later tutorials since logically it fits better there and just skip it for now.
	// And the final thing we will setup in the Initialize function is an orthographic projection matrix. This matrix is used for rendering 2D elements like user interfaces on the screen allowing
	// us to skip the 3D rendering. You will see this used in later tutorials when we look at rendering 2D graphics and fonts to the screen.

	// Create an orthographic projection matrix for 2D rendering.
	D3DGC->OrthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// First initialize the blend state description.
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	// https://msdn.microsoft.com/en-us/library/ff476086(v=vs.85).aspx
	// To create an alpha enabled blend state description change BlendEnable to TRUE and DestBlend to D3D11_BLEND_INV_SRC_ALPHA. The other settings are set to their default values which can be
	// looked up in the Windows DirectX Graphics Documentation.

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f; // ВОТ ЭТО РАБОТАЕТ   НО НЕМНОГО  НЕ ТАК КАК НУЖНО НАМ  МОЖЕТ ПОТОМУ ЧТО В ТЕКСТУРАХ БЛЕНДИНГ УКАЗАН НЕ ТАК КАК НУЖНО НАМ

	// We then create an alpha enabled blending state using the description we just setup.

	// Create the blend state using the description.
	result = D3DGC->DX_device->CreateBlendState(&blendStateDescription, &D3DGC->m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CreateBlendState.", Error, 0);
		goto ERROR_EXIT;
	}

	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	result = D3DGC->DX_device->CreateBlendState(&blendStateDescription, &D3DGC->m_alphaParticleBlendingState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Create m_alphaParticleBlendingState.", Error, 0);
		goto ERROR_EXIT;
	}	// Now to create an alpha disabled state we change the description we just made to set BlendEnable to FALSE. The rest of the settings can be left as they are.


	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//D3D11_BLEND_ONE; //D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f; // ВОТ ЭТО РАБОТАЕТ   НО НЕМНОГО  НЕ ТАК КАК НУЖНО НАМ  МОЖЕТ ПОТОМУ ЧТО В ТЕКСТУРАХ БЛЕНДИНГ УКАЗАН НЕ ТАК КАК НУЖНО НАМ
	result = D3DGC->DX_device->CreateBlendState( &blendStateDescription, &D3DGC->m_alpha_TOnT_BlendingState);
	if ( FAILED( result ) )
	{
		MessageBox( hwnd, L"Create m_alpha_TOnT_BlendingState.", Error, 0 );
		goto ERROR_EXIT;
	}	// Now to create an alpha disabled state we change the description we just made to set BlendEnable to FALSE. The rest of the settings can be left as they are.



	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	// We then create an alpha disabled blending state using the modified blend state description. We now have two blending states we can switch between to turn on and off alpha blending.

	// Create the blend state using the description.
	result = D3DGC->DX_device->CreateBlendState(&blendStateDescription, &D3DGC->m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"CreateBlendState for Particles.", Error, 0);
		goto ERROR_EXIT;
	}

	{
		// Для кластеринга
		CD3D11_BLEND_DESC desc ( D3D11_DEFAULT );
		result = D3DGC->DX_device->CreateBlendState ( &desc, &D3DGC->mGeometryBlendState );
		if ( FAILED ( result ) )
		{
			MessageBox ( hwnd, L"CreateBlendState for Clustering Shading.", Error, 0 );
			goto ERROR_EXIT;
		}
	}
	// DirectWrite
	result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&D3DGC->DWriteFactory));
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize DWrite CreateFactory.", Error, MB_OK);
		goto ERROR_EXIT;
	}
	{
		// +++++++++++++++++++++++++++     FXAA     ++++++++++++++++++++++++++++++++++
		D3DGC->g_pcbFXAA = new ConstantBuffer<CB_FXAA> ( D3DGC->DX_device, D3DGC->DX_deviceContext, D3D11_USAGE_DEFAULT );
		CB_FXAA pFXAA = { 1.0f / D3DGC->ScreenWidth, 1.0f / D3DGC->ScreenHeight, 0.0f, 0.0f };
		D3DGC->g_pcbFXAA->Update ( &pFXAA );

		DXGI_SAMPLE_DESC SDesc;
		ZeroMemory ( &SDesc, sizeof ( DXGI_SAMPLE_DESC ) );
		SDesc.Count = swapChainDesc.SampleDesc.Count;
		SDesc.Quality = swapChainDesc.SampleDesc.Quality;

		DXGI_SURFACE_DESC pBackBufferSurfaceDesc;
		ZeroMemory ( &pBackBufferSurfaceDesc, sizeof ( DXGI_SURFACE_DESC ) );

		pBackBufferSurfaceDesc.Format = swapChainDesc.BufferDesc.Format;
		pBackBufferSurfaceDesc.Width = swapChainDesc.BufferDesc.Width;
		pBackBufferSurfaceDesc.Height = swapChainDesc.BufferDesc.Height;
		pBackBufferSurfaceDesc.SampleDesc = SDesc;

		if ( !FxaaIntegrateResource ( D3DGC->DX_device, &pBackBufferSurfaceDesc ) )
		{
			MessageBox ( hwnd, L"Could not initialize FXAA.", Error, MB_OK );
			goto ERROR_EXIT;
		}
	}
	if ( !InitD2D_D3D101_DWrite() )
	{
		MessageBox( hwnd, L"Could not initialize DWrite.", Error, MB_OK );
		goto ERROR_EXIT;
	}
	// ---------------------------     FXAA     ----------------------------------

	//  ++++++++++++++++++   Для D3DClass и Shadows   ++++++++++++++++++++++++
	// Create sampler state
	{
		// All Models Sampler
		// FXAA
		CD3D11_SAMPLER_DESC desc ( D3D11_DEFAULT );
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		result = D3DGC->DX_device->CreateSamplerState ( &desc, &D3DGC->Wrap_Model_Texture );
		if (FAILED ( result ))
		{
			MessageBox ( hwnd, L"Could not initialize Wrap_Model_Texture sampler.", Error, MB_OK );
			goto ERROR_EXIT;
		}
	}
	{
		// Если делать не CLAMP то появляются дублирующие тени
		CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		result = D3DGC->DX_device->CreateSamplerState(&desc, &D3DGC->CLight_ShadowMap_Sampler );
		if (FAILED ( result ))
		{
			MessageBox ( hwnd, L"Could not initialize CLight_ShadowMap_Sampler sampler.", Error, MB_OK );
			return false;
		}
	}
	{
		// PCF Filter Sampler
		CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS;
		result = D3DGC->DX_device->CreateSamplerState(&desc, &D3DGC->CLight_SM_PCF_Sampler );
		if (FAILED ( result ))
		{
			MessageBox ( hwnd, L"Could not Initialize CLight_SM_PCF_Sampler.", Error, MB_OK );
			return false;
		}
	}
	{
		// All 2D Object default Sampler
		CD3D11_SAMPLER_DESC desc ( D3D11_DEFAULT );
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		result = D3DGC->DX_device->CreateSamplerState ( &desc, &D3DGC->FlatObject_Sampler );
		if (FAILED ( result ))
		{
			MessageBox ( hwnd, L"Could not initialize FlatObject_Sampler sampler.", Error, MB_OK );
			return false;
		}
	}
	//  ------------------   Для Shadows   -------------------------


	Frustum = frustum;

	// Размер окна при отресовке теней
	ZeroMemory ( &LightViewPort, sizeof ( D3D11_VIEWPORT ) );
	LightViewPort.Width = static_cast<float>(SMWidth);
	LightViewPort.Height = static_cast<float>( SMHeight );
	LightViewPort.MinDepth = 0.0f;
	LightViewPort.MaxDepth = 1.0f;
	LightViewPort.TopLeftX = 0.0f;
	LightViewPort.TopLeftY = 0.0f;

	// Создаём константный буфер для базоого света Diffuse,Ambient,Specular
	// cb_LightBuffer

	Light = new PerFrameConstants;
	ZeroMemory ( Light, sizeof ( PerFrameConstants ) );

	Light->mCameraNearFar = XMVECTOR{ D3DGC->NearPlane, D3DGC->FarPlane, 0.0f, 0.0f };

	Light->mFramebufferDimensionsX = D3DGC->ScreenWidth;
	Light->mFramebufferDimensionsY = D3DGC->ScreenHeight;
	Light->mFramebufferDimensionsZ = 0;     // Unused
	Light->mFramebufferDimensionsW = 0;     // Unused

	Light->mUI.lightingOnly = false;
	Light->mUI.clusteredGridScale = D3DGC->ClustersAmount;
	Light->mUI.shadowsOn = D3DGC->ShadowsOn;
	Light->mUI.softshadowsOn = D3DGC->SoftShadowsOn;
	Light->mUI.PCF_Amount = D3DGC->PCF_Amount;
	Light->mUI.PCF_Step = 1.0f;
	Light->mUI.ShadowCLAMP = 1.0f;
	Light->mUI.Shadow_Divider = 1024.0f;
	// Create constant buffers
	PerFrameConstants_Debug = new ConstantBuffer<PerFrameConstants> ( D3DGC->DX_device, D3DGC->DX_deviceContext, D3D11_USAGE_DEFAULT );

	SetAllLightDefault ();

	mLightGridBuffer = new StructuredBuffer<LightGridEntry> ( D3DGC->DX_device, mLightGridBufferSize, D3D11_BIND_SHADER_RESOURCE, true );

	// + Создаём константные буфер
	D3DGC->Global_VS_ConstantsBuffer = new ConstantBuffer<ConstantBufferData> ( D3DGC->DX_device, D3DGC->DX_deviceContext, D3D11_USAGE_DEFAULT );
	// Костантный буфер для ShadowMap
	D3DGC->ShadowMapLightView = new  ConstantBuffer<CB_ShadowMap> ( D3DGC->DX_device, D3DGC->DX_deviceContext, D3D11_USAGE_DEFAULT );
	// - Создаём константные буфер

	return true;

ERROR_EXIT:
	Shutdown ();
	return false;
}

// The Shutdown function will release and clean up all the pointers used in the Initialize function, its pretty straight forward. However before doing that I put in a call to force the swap
// chain to go into windowed mode first before releasing any pointers. If this is not done and you try to release the swap chain in full screen mode it will throw some exceptions.
// So to avoid that happening we just always force windowed mode before shutting down Direct3D.

void D3DClass::Shutdown()
{
	DeleteAllLights ();
	RCUBE_DELETE ( Light );
	RCUBE_DELETE ( mLightGridBuffer );
	RCUBE_DELETE ( PerFrameConstants_Debug );

	for(int i=0, j = (int)ActualDisplayModes.size();  i < j; i++)
	{
		delete ActualDisplayModes[i];
	}
	ActualDisplayModes.clear();

	// Delete all custom samplers
	int c = (int)CustomSamplers.size ();
	for ( int i = 0; i < c; ++i )
	{
		RCUBE_ARR_DELETE ( CustomSamplers[i].Name );
		RCUBE_RELEASE ( CustomSamplers[i].Sampler );
	}

	// Delete all custom rasterizers
	c = (int)CustomRasterizers.size ();
	for (int i = 0; i < c; ++i)
	{
		RCUBE_ARR_DELETE ( CustomRasterizers[i].Name );
		RCUBE_RELEASE ( CustomRasterizers[i].Rasterizer );
	}

	// Delete all custom DepthStencils
	c = (int)CustomDepthStencils.size ();
	for (int i = 0; i < c; ++i)
	{
		RCUBE_ARR_DELETE ( CustomDepthStencils[i].Name );
		RCUBE_RELEASE ( CustomDepthStencils[i].DepthStencilState );
	}

// Fonts
	RCUBE_RELEASE( D3DGC->sharedTex11 );
	RCUBE_RELEASE( D3DGC->sharedTex11_SRV );
	RCUBE_RELEASE( D3DGC->sharedTex11_MAPED );
	RCUBE_RELEASE( D3DGC->keyedMutex11 );
	RCUBE_RELEASE( D3DGC->keyedMutex10 );
	RCUBE_RELEASE( D3DGC->D2DRenderTarget );
	RCUBE_RELEASE( D3DGC->D2DFactory );

// FXAA	
	RCUBE_DELETE ( D3DGC->g_pcbFXAA );
	RCUBE_RELEASE( D3DGC->BackBuffer_ProxyTexture );
	RCUBE_RELEASE( D3DGC->BackBuffer_ProxyTextureSRV );
	RCUBE_RELEASE( D3DGC->BackBuffer_ProxyTextureRTV );
	RCUBE_RELEASE( D3DGC->BackBuffer_CopyResolveTexture );
	RCUBE_RELEASE( D3DGC->BackBuffer_CopyResolveTextureSRV );
	RCUBE_RELEASE( D3DGC->BackBuffer_CopyResolveTextureUAV );
// FXAA	
// Engine default Samplers
	RCUBE_RELEASE( D3DGC->Wrap_Model_Texture );
	RCUBE_RELEASE( D3DGC->CLight_ShadowMap_Sampler );
	RCUBE_RELEASE( D3DGC->CLight_SM_PCF_Sampler );
	RCUBE_RELEASE ( D3DGC->FlatObject_Sampler );

	RCUBE_RELEASE( D3DGC->DWriteFactory );
	RCUBE_RELEASE( D3DGC->m_alphaDisableBlendingState );
	RCUBE_RELEASE( D3DGC->m_alphaParticleBlendingState );
	RCUBE_RELEASE( D3DGC->m_alpha_TOnT_BlendingState );
	RCUBE_RELEASE( D3DGC->m_alphaEnableBlendingState );
	RCUBE_RELEASE( D3DGC->depthStencil_Disabled_State );
	RCUBE_RELEASE ( D3DGC->depthStencil_NoWrite_Particles );
	RCUBE_RELEASE( D3DGC->DefaultRasterizerState );
	RCUBE_RELEASE ( D3DGC->DefaultRasterizerState );
	RCUBE_RELEASE ( D3DGC->WireFrameRasterizerState );
	RCUBE_RELEASE( D3DGC->RasterStateCullNone );
	RCUBE_RELEASE( D3DGC->m_depthStencilView );
	RCUBE_RELEASE( D3DGC->depthStencil_State );
	RCUBE_RELEASE( D3DGC->CubeMap_DepthStencil_State );
	RCUBE_RELEASE( D3DGC->mGeometryBlendState );
	RCUBE_RELEASE( D3DGC->m_depthStencilBuffer );
	RCUBE_RELEASE( D3DGC->BackBuffer_RTV );
	RCUBE_RELEASE( D3DGC->BackBuffer2DT );
	RCUBE_RELEASE( D3DGC->BackBuffer_SRV );
	RCUBE_RELEASE( D3DGC->BackBuffer_UAV );
	RCUBE_RELEASE( D3DGC->ScreenShootTexture );
	RCUBE_DELETE ( D3DGC->Global_VS_ConstantsBuffer );
	RCUBE_DELETE ( D3DGC->ShadowMapLightView );
	RCUBE_RELEASE ( D3DGC->LightRender_RS );
	RCUBE_RELEASE ( D3DGC->LightRender_DS );
	RCUBE_RELEASE ( D3DGC->DSV_ShadowMap3D ); // Можно убрать. Всегда релизится после отрисовки теней

	if (D3DGC->DX_swapChain){
		D3DGC->DX_swapChain->SetFullscreenState(false, NULL);
		RCUBE_RELEASE(D3DGC->DX_swapChain);
	}

	RCUBE_RELEASE( D3DGC->D10_device );
	RCUBE_RELEASE(D3DGC->DX_deviceContext);
	AdaptersRelease();

#if defined( DEBUG ) || defined( _DEBUG )
	D3DGC->DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	D3DGC->DebugDevice->Release();
#endif

	RCUBE_RELEASE(D3DGC->DX_device);
	//    if (D3DGC->Adapter)                        D3DGC->Adapter->Release();

	RCUBE_ARR_DELETE ( m_videoCardDescription );
}

// In the D3DClass I have a couple helper functions. The first two are BeginScene and EndScene. BeginScene will be called whenever we are going to draw a new 3D scene at the beginning of each frame.
// All it does is initializes the buffers so they are blank and ready to be drawn to. The other function is Endscene, it tells the swap chain to display our 3D scene once all the drawing has completed
// at the end of each frame.

void D3DClass::BeginScene(XMFLOAT4& Colour)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = Colour.x;
	color[1] = Colour.y;
	color[2] = Colour.z;
	color[3] = Colour.w;

	// Clear the back buffer.
	D3DGC->DX_deviceContext->ClearRenderTargetView(D3DGC->BackBuffer_RTV, color);

	// Clear the depth buffer.
	D3DGC->DX_deviceContext->ClearDepthStencilView(D3DGC->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		D3DGC->DX_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		D3DGC->DX_swapChain->Present(0, 0);
	}
}

// The next three helper functions give copies of the projection, world, and orthographic matrices to calling functions. Most shaders will need these matrices for rendering so there needed
// to be an easy way for outside objects to get a copy of them. We won't call these functions in this tutorial but I'm just explaining why they are in the code.

void D3DClass::GetProjectionMatrix(XMMATRIX &projectionMatrix)
{
	projectionMatrix = D3DGC->ProjectionMatrix;
}


void D3DClass::GetWorldMatrix(XMMATRIX &worldMatrix)
{
	worldMatrix = D3DGC->WorldMatrix;
}


void D3DClass::GetOrthoMatrix(XMMATRIX &orthoMatrix)
{
	orthoMatrix = D3DGC->OrthoMatrix;
}


// These are the new functions for enabling and disabling the Z buffer. To turn Z buffering on we set the original depth stencil. To turn Z buffering off we set the new depth stencil that has
// depthEnable set to false. Generally the best way to use these functions is first do all your 3D rendering, then turn the Z buffer off and do your 2D rendering, and then turn the Z buffer on again.

void D3DClass::TurnZBufferOn()
{
	D3DGC->DX_deviceContext->OMSetDepthStencilState(D3DGC->depthStencil_State, 1);
}


void D3DClass::TurnZBufferOff()
{
	D3DGC->DX_deviceContext->OMSetDepthStencilState( D3DGC->depthStencil_Disabled_State, 1);
}

void D3DClass::ZBufferNoWrite ()
{
	D3DGC->DX_deviceContext->OMSetDepthStencilState ( D3DGC->depthStencil_NoWrite_Particles, 1 );
}

// The first new function TurnOnAlphaBlending allows us to turn on alpha blending by using the OMSetBlendState function with our m_alphaEnableBlendingState blending state.

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	D3DGC->DX_deviceContext->OMSetBlendState(D3DGC->m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}

// he second new function TurnOffAlphaBlending allows us to turn off alpha blending by using the OMSetBlendState function with our m_alphaDisableBlendingState blending state.

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	D3DGC->DX_deviceContext->OMSetBlendState(D3DGC->m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}



void D3DClass::TurnOnParticleBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	D3DGC->DX_deviceContext->OMSetBlendState(D3DGC->m_alphaParticleBlendingState, blendFactor, 0xffffffff);

	return;
}


void D3DClass::TurnOnTextOnTextureBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	D3DGC->DX_deviceContext->OMSetBlendState( D3DGC->m_alpha_TOnT_BlendingState, blendFactor, 0xffffffff );

	return;
}


ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
{
	return D3DGC->m_depthStencilView;
}


void D3DClass::SetBackBufferRenderTarget()
{
	D3DGC->DX_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, D3DGC->m_depthStencilView);
}


void D3DClass::AdaptersRelease()
{
	UINT i = 0;
	UINT j = (UINT)Adapters.size();
	while ( i < j )
	{
		Adapters[i]->Release();
		++i;
	};

	Adapters.clear();
}


char* D3DClass::GetVideoCardString()
{
	return &m_videoCardDescription[0];
}


void D3DClass::SetDefaultResterizeState()
{
	D3DGC->DX_deviceContext->RSSetState(D3DGC->DefaultRasterizerState );
}


void D3DClass::RenderWireFrameResterizeState ()
{
	D3DGC->DX_deviceContext->RSSetState ( D3DGC->WireFrameRasterizerState );
}


void D3DClass::SetCullNoneResterizeState ()
{
	D3DGC->DX_deviceContext->RSSetState( D3DGC->RasterStateCullNone );
}


bool D3DClass::FxaaIntegrateResource(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Height = pBackBufferSurfaceDesc->Height;
	desc.Width = pBackBufferSurfaceDesc->Width;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	result = pd3dDevice->CreateTexture2D(&desc, 0, &D3DGC->BackBuffer_CopyResolveTexture);
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create g_pCopyResolveTexture", Error, 0 );
		return false;
	}

	desc.SampleDesc.Count = pBackBufferSurfaceDesc->SampleDesc.Count;
	desc.SampleDesc.Quality = pBackBufferSurfaceDesc->SampleDesc.Quality;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //| D3D11_BIND_UNORDERED_ACCESS;  Если только без MSAA

	result = pd3dDevice->CreateTexture2D(&desc, 0, &D3DGC->BackBuffer_ProxyTexture );
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create g_pProxyTexture", Error, 0 );
		return false;
	}
	result = pd3dDevice->CreateShaderResourceView(D3DGC->BackBuffer_ProxyTexture, NULL, &D3DGC->BackBuffer_ProxyTextureSRV);
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create BackBuffer_ProxyTextureSRV", Error, 0 );
		return false;
	}
	result = pd3dDevice->CreateRenderTargetView(D3DGC->BackBuffer_ProxyTexture, 0, &D3DGC->BackBuffer_ProxyTextureRTV );
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create BackBuffer_ProxyTextureRTV", Error, 0 );
		return false;
	}
	result = pd3dDevice->CreateShaderResourceView(D3DGC->BackBuffer_CopyResolveTexture, NULL, &D3DGC->BackBuffer_CopyResolveTextureSRV);
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create BackBuffer_CopyResolveTextureSRV", Error, 0 );
		return false;
	}

	result = pd3dDevice->CreateUnorderedAccessView( D3DGC->BackBuffer_CopyResolveTexture, NULL, &D3DGC->BackBuffer_CopyResolveTextureUAV );
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"Error CreateUnorderedAccessView BackBuffer_CopyResolveTextureUAV.", Error, 0 );
		Shutdown();
		return false;
	}


#if defined( DEBUG ) || defined( _DEBUG )
	const char c_szName0[] = "D3DGC->BackBuffer_CopyResolveTexture";
	D3DGC->BackBuffer_CopyResolveTexture->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName0) - 1, c_szName0);
	const char c_szName[] = "D3DGC->BackBuffer_ProxyTexture";
	D3DGC->BackBuffer_ProxyTexture->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);
	const char c_szName1[] = "D3DGC->BackBuffer_ProxyTextureSRV";
	D3DGC->BackBuffer_ProxyTextureSRV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName1) - 1, c_szName1);
	const char c_szName2[] = "D3DGC->BackBuffer_ProxyTextureRTV";
	D3DGC->BackBuffer_ProxyTextureRTV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName2) - 1, c_szName2);
	const char c_szName3[] = "D3DGC->BackBuffer_CopyResolveTextureSRV";
	D3DGC->BackBuffer_CopyResolveTextureSRV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName3) - 1, c_szName3);
//	const char c_szName4[] = "D3DGC->ScreenShootTexture";
//	D3DGC->ScreenShootTexture->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName4) - 1, c_szName4);
#endif

	return true;
}


void D3DClass::FXAAScene()
{
	D3DGC->DX_deviceContext->PSSetConstantBuffers ( 12, 1, &D3DGC->g_pcbFXAA->Buffer );

		D3DGC->DX_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, 0);

		ID3D11Resource* MyresDest = nullptr;

		if (D3DGC->EnableMSAA && D3DGC->MSAAQualityCount > 1)
		{
			// resolve first
			D3DGC->DX_deviceContext->ResolveSubresource(D3DGC->BackBuffer_CopyResolveTexture, 0, D3DGC->BackBuffer2DT, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
			D3DGC->DX_deviceContext->PSSetShaderResources(0, 1, &D3DGC->BackBuffer_CopyResolveTextureSRV);
		}
		else
		{
			D3DGC->BackBuffer_ProxyTextureSRV->GetResource( &MyresDest );
			D3DGC->DX_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer2DT );
			D3DGC->DX_deviceContext->PSSetShaderResources(0, 1, &D3DGC->BackBuffer_ProxyTextureSRV);
		}

		D3DGC->DX_deviceContext->IASetInputLayout(NULL);
		D3DGC->DX_deviceContext->Draw(3, 0);
/*
		D3DGC->DX_deviceContext->CopyResource(D3DGC->ScreenShootTexture, D3DGC->backBufferPtr);
		union _block
		{
			byte  *data;
			unsigned int *udata;
		} block;

		D3D11_MAPPED_SUBRESOURCE  mapResource;
		hr = D3DGC->DX_deviceContext->Map(D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource);

		unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
		block.data = new byte[amount];
		memcpy(block.data, mapResource.pData, amount);
		D3DGC->DX_deviceContext->Unmap(D3DGC->ScreenShootTexture, 0);
		Image img;
		D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;
		D3DGC->ScreenShootTexture->GetDesc(TextureDesc);
		img.height = TextureDesc->Height;
		img.width = TextureDesc->Width;
		img.format = TextureDesc->Format;
		img.rowPitch = mapResource.RowPitch;
		img.slicePitch = mapResource.DepthPitch;
		img.pixels = block.data;

		hr = SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), L"2___BACKBUFFER.png");
		if (FAILED(hr))
		{
			assert(L"Could not save 2___BACKBUFFER.png file.");
		}

		delete[] block.data;
		delete TextureDesc;
*/
		RCUBE_RELEASE( MyresDest );
		// Нужно вернуть DepthBuffer
		D3DGC->DX_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, D3DGC->m_depthStencilView);
}


bool D3DClass::SaveTextureToPNG( ID3D11ShaderResourceView* Texture)
{
	bool Result = true;
	ID3D11Resource* tempResource = nullptr;
	ID3D11Texture2D* InputTexture = nullptr;
	D3D11_TEXTURE2D_DESC* InputTextureDesc = new D3D11_TEXTURE2D_DESC;
	D3D11_TEXTURE2D_DESC* desc	= new D3D11_TEXTURE2D_DESC;

	Texture->GetResource( &tempResource );
	tempResource->QueryInterface<ID3D11Texture2D>( &InputTexture );
	InputTexture->GetDesc( InputTextureDesc );

	ZeroMemory( desc, sizeof( D3D11_TEXTURE2D_DESC ) );
	desc->BindFlags = 0;//D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc->Height = InputTextureDesc->Height;
	desc->Width = InputTextureDesc->Width;
	desc->ArraySize = 1;
	desc->SampleDesc.Count = 1;
	desc->SampleDesc.Quality = 0;
	desc->MipLevels = 1;
	desc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc->BindFlags = 0;
	desc->Usage = D3D11_USAGE_STAGING;
	// Сохранение возможно только для текстуры без MSAA 
	// Нужно делать ResolveSubresource

	hr = D3DGC->DX_device->CreateTexture2D( desc, 0, &D3DGC->ScreenShootTexture );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Can't create ScreenShotTexture", Error, 0 );
		Result = false;
		goto END;
	}

	// Если текстура MSAA, то делаем ResolveSubresurces
	if ( InputTextureDesc->SampleDesc.Count > 1 )
	{
		D3DGC->DX_deviceContext->ResolveSubresource( D3DGC->BackBuffer_CopyResolveTexture, 0, tempResource, 0, DXGI_FORMAT_R8G8B8A8_UNORM );
	// D3DGC->BackBuffer_CopyResolveTextureSRV
		ID3D11Resource* Resource;
		D3DGC->BackBuffer_CopyResolveTextureSRV->GetResource( &Resource );
		D3DGC->DX_deviceContext->CopyResource( D3DGC->ScreenShootTexture, Resource );
		RCUBE_RELEASE ( Resource );
	}
	else
	{
		D3DGC->DX_deviceContext->CopyResource( D3DGC->ScreenShootTexture, tempResource );
	}

	union _block
	{
		byte  *data;
		unsigned int *udata;
	} block;

	D3D11_MAPPED_SUBRESOURCE  mapResource;
	hr = D3DGC->DX_deviceContext->Map( D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource );

	unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
	block.data = new byte[amount];
	memcpy( block.data, mapResource.pData, amount );
	D3DGC->DX_deviceContext->Unmap( D3DGC->ScreenShootTexture, 0 );

	Image img;
	img.height = desc->Height;
	img.width = desc->Width;
	img.format = desc->Format;
	img.rowPitch = mapResource.RowPitch;
	img.slicePitch = mapResource.DepthPitch;
	img.pixels = block.data;

	hr = SaveToWICFile( img, WIC_FLAGS_NONE, GetWICCodec( WIC_CODEC_PNG ), L"1_Texture.png" );
	if ( FAILED( hr ) )
	{
		assert( L"Could not save Texture.png file." );
		Result = false;
	}

	delete[] block.data;

END:

	delete InputTextureDesc;
	delete desc;

	RCUBE_RELEASE ( tempResource );
	RCUBE_RELEASE( InputTexture );
	RCUBE_RELEASE( D3DGC->ScreenShootTexture );

	return Result;
}


bool D3DClass::InitD2D_D3D101_DWrite()
{

	//Create our Direc3D 10.1 Device
	hr = D3D10CreateDevice1( D3DGC->Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &D3DGC->D10_device );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize D3D10CreateDevice1.", Error, MB_OK );
		return false;
	}

	//Create Shared Texture that Direct3D 10.1 will render on
	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory( &sharedTexDesc, sizeof( sharedTexDesc ) );
	
	sharedTexDesc.Width = D3DGC->sharedTex11_Width;
	sharedTexDesc.Height = D3DGC->sharedTex11_Height;
	sharedTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = D3DGC->DX_device->CreateTexture2D( &sharedTexDesc, NULL, &D3DGC->sharedTex11 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11.", Error, MB_OK );
		return false;
	}

	hr = D3DGC->DX_device->CreateShaderResourceView( D3DGC->sharedTex11, NULL, &D3DGC->sharedTex11_SRV );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize ShaderResourceView sharedTex11.", Error, MB_OK );
		return false;
	}

	// Создаём копию текстуры sharedTex11 с которой может работать CPU
	sharedTexDesc.BindFlags = 0;
	sharedTexDesc.MiscFlags = 0;
	sharedTexDesc.Usage = D3D11_USAGE_STAGING;
	sharedTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	hr = D3DGC->DX_device->CreateTexture2D( &sharedTexDesc, NULL, &D3DGC->sharedTex11_MAPED );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11_MAPED.", Error, MB_OK );
		return false;
	}

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = D3DGC->sharedTex11->QueryInterface( __uuidof( IDXGIKeyedMutex ), ( void** ) &D3DGC->keyedMutex11 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11->QueryInterfac.", Error, MB_OK );
		return false;
	}

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	hr = D3DGC->sharedTex11->QueryInterface( __uuidof( IDXGIResource ), ( void** ) &sharedResource10 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11->QueryInterface.", Error, MB_OK );
		return false;
	}

	hr = sharedResource10->GetSharedHandle( &sharedHandle10 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedResource10->GetSharedHandle.", Error, MB_OK );
		return false;
	}
	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1
	IDXGISurface1 *sharedSurface10;

	hr = D3DGC->D10_device->OpenSharedResource( sharedHandle10, __uuidof( IDXGISurface1 ), ( void** ) ( &sharedSurface10 ) );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize d3d101Device->OpenSharedResource.", Error, MB_OK );
		return false;
	}

	hr = sharedSurface10->QueryInterface( __uuidof( IDXGIKeyedMutex ), ( void** ) &D3DGC->keyedMutex10 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedSurface10->QueryInterface.", Error, MB_OK );
		return false;
	}

	// Create D2D factory
	hr = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof( ID2D1Factory ), ( void** ) &D3DGC->D2DFactory );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize D2D1CreateFactory.", Error, MB_OK );
		return false;
	}

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory( &renderTargetProperties, sizeof( renderTargetProperties ) );

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat( DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED );

	hr = D3DGC->D2DFactory->CreateDxgiSurfaceRenderTarget( sharedSurface10, &renderTargetProperties, &D3DGC->D2DRenderTarget );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not Create DxgiSurfaceRenderTarget.", Error, MB_OK );
		return false;
	}

	sharedSurface10->Release();

	return true;
}

void D3DClass::BlurScene(ID3D11ComputeShader* Horizont, ID3D11ComputeShader* Vertical, int BlurAmont)
{
	//  ++++++++++++++++++++++++++     BLUR     ++++++++++++++++++++++++++++++++++
	// ComputeSHader Blur possible if any MSAA OFF
	if ( !D3DGC->EnableMSAA )
	{
	ID3D11Resource* MyresDest;
	D3DGC->BackBuffer_ProxyTextureSRV->GetResource( &MyresDest );

	// resolve first
//		D3DGC->DX_deviceContext->ResolveSubresource(D3DGC->BackBuffer_CopyResolveTexture, 0, D3DGC->BackBuffer2DT, 0, DXGI_FORMAT_R8G8B8A8_UNORM );
//		D3DGC->DX_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer_CopyResolveTexture);

		D3DGC->DX_deviceContext->CopyResource( MyresDest, D3DGC->BackBuffer2DT );

		D3DGC->DX_deviceContext->CSSetShaderResources(0, 1, &D3DGC->BackBuffer_ProxyTextureSRV);
		D3DGC->DX_deviceContext->CSSetUnorderedAccessViews(0, 1, &D3DGC->BackBuffer_CopyResolveTextureUAV, 0);

		int c = 0;
		//	int BlurAmont = 1; // Сила Blur
		do
		{
			D3DGC->DX_deviceContext->CSSetShader(Horizont, NULL, 0);
			D3DGC->DX_deviceContext->Dispatch(D3DGC->ScreenWidth / 96, D3DGC->ScreenHeight, 1);

			D3DGC->DX_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer_CopyResolveTexture);

			D3DGC->DX_deviceContext->CSSetShader(Vertical, NULL, 0);
			D3DGC->DX_deviceContext->Dispatch(D3DGC->ScreenWidth, D3DGC->ScreenHeight / 96, 1);

			D3DGC->DX_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer_CopyResolveTexture);
			++c;
		} while (c < BlurAmont);

		D3DGC->DX_deviceContext->CopyResource(D3DGC->BackBuffer2DT, D3DGC->BackBuffer_CopyResolveTexture);

		MyresDest->Release ();
	}

	
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	D3DGC->DX_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	D3DGC->DX_deviceContext->CSSetShaderResources(0, 1, nullSRV);
	//  --------------------------    BLUR      ----------------------------------
}


int D3DClass::CreateCustomRasterizerState ( D3D11_RASTERIZER_DESC& Desc )
{
// https://msdn.microsoft.com/query/dev15.query?appId=Dev15IDEF1&l=EN-US&k=k(D3D11%2FD3D11_RASTERIZER_DESC);k(D3D11_RASTERIZER_DESC);k(DevLang-C%2B%2B);k(TargetOS-Windows)&rd=true
	ID3D11RasterizerState* RS;
	hr = D3DGC->DX_device->CreateRasterizerState ( &Desc, &RS );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Could not Create CustomRasterizerState.", Error, MB_OK );
		return -1;
	}

	CustomRasterizer NewRasterizer;

	NewRasterizer.Rasterizer = RS;
	NewRasterizer.Name = nullptr;

	int Number = (int)CustomRasterizers.size ();

	CustomRasterizers.push_back ( NewRasterizer );

	return Number;
}


bool D3DClass::ChangeCustomRasterizerState( int Number, D3D11_RASTERIZER_DESC& Desc )
{
	int Num = (int)CustomRasterizers.size ();
	if (Number > Num)
		return false;

	ID3D11RasterizerState* RS;

	RCUBE_RELEASE ( CustomRasterizers[Number].Rasterizer );

	hr = D3DGC->DX_device->CreateRasterizerState ( &Desc, &RS );
	if (FAILED ( hr ))
	{
		MessageBox ( NULL, L"Could not Update CustomRasterizerState.", Error, MB_OK );
		return false;
	}

	CustomRasterizers[Number].Rasterizer = RS;

	return true;
}


ID3D11RasterizerState* D3DClass::GetCustomRasterizerState ( int Number )
{
	int Num = (int)CustomRasterizers.size ();
	if (Number > Num)
		return nullptr;

	return CustomRasterizers[Number].Rasterizer;
}



template<typename T>
inline T clamp ( T v, T lb, T ub )
{
	return min ( max ( v, lb ), ub );
}


inline void D3DClass::UpdateClipRegion (
	float lc,          // Light x/y coordinate (view space)
	float& lz,          // Light z coordinate (view space)
	float& lightRadius,
	const float& cameraScale, // Project scale for coordinate (_11 or _22 for x/y respectively)
	float& clipMin,
	float& clipMax )
{
	float rSq = lightRadius * lightRadius;
	float lcSq = lc * lc;
	float lzSq = lz * lz;
	float lcSqPluslzSq = lcSq + lzSq;
	float d = rSq * lcSq - lcSqPluslzSq * (rSq - lzSq);

	if (d > 0)
	{
		float a = lightRadius * lc;
		float b = sqrtf ( d );
		float nx0 = (a + b) / lcSqPluslzSq;
		float nx1 = (a - b) / lcSqPluslzSq;
		float lcSqPluslzSq_Minus_rSq = lcSqPluslzSq - rSq;
		// X
		//		UpdateClipRegionRoot(nx0, lc, lz, lcSqPluslzSq, lightRadius, cameraScale, clipMin, clipMax);
		float nz = (lightRadius - nx0 * lc) / lz;
		float pz = lcSqPluslzSq_Minus_rSq / (lz - (nz / nx0) * lc);
		if (pz > 0.0f) {
			float c = -nz * cameraScale / nx0;
			if (nx0 > 0.0f)
			{                      // Left side boundary
				clipMin = max ( clipMin, c );
			}
			else
			{                       // Right side boundary
				clipMax = min ( clipMax, c );
			}
		}
		// Y
		//		UpdateClipRegionRoot(nx1, lc, lz, lcSqPluslzSq, lightRadius, cameraScale, clipMin, clipMax);
		nz = (lightRadius - nx1 * lc) / lz;
		pz = lcSqPluslzSq_Minus_rSq / (lz - (nz / nx1) * lc);
		if (pz > 0.0f) {
			float c = -nz * cameraScale / nx1;
			if (nx1 > 0.0f)
			{                      // Left side boundary
				clipMin = max ( clipMin, c );
			}
			else
			{                       // Right side boundary
				clipMax = min ( clipMax, c );
			}
		}
	}
}


// Returns bounding box [min.xy, max.xy] in clip [-1, 1] space.
inline XMVECTOR D3DClass::ComputeClipRegion (
	XMFLOAT3& lightPosView,
	float& lightRadius,
	const float& float11,
	const float& float22,
	const float& mCameraNearFarX )
{
	// Early out with empty rectangle if the light is too far behind the view frustum
	//	XMVECTOR clipRegion = XMVECTOR{ 1.0f, 1.0f, 0.0f, 0.0f };
	XMVECTOR clipRegion = { 1.0f, 1.0f, 0.0f, 0.0f };
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++       Отсеиваем свет невидимый на экране         ++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (lightPosView.z + lightRadius >= mCameraNearFarX)
	{
		//		XMFLOAT2 clipMin = XMFLOAT2(-1.0f, -1.0f);
		//		XMFLOAT2 clipMax = XMFLOAT2(1.0f, 1.0f);
		XMFLOAT2 clipMin = { -1.0f, -1.0f };
		XMFLOAT2 clipMax = { 1.0f, 1.0f };

		UpdateClipRegion ( lightPosView.x, lightPosView.z, lightRadius, float11, clipMin.x, clipMax.x );
		UpdateClipRegion ( -lightPosView.y, lightPosView.z, lightRadius, float22, clipMin.y, clipMax.y );

		clipRegion = XMVECTOR{ clipMin.x, clipMin.y, clipMax.x, clipMax.y };
	}

	return clipRegion;
}


inline void D3DClass::GenerateLightFragments (
	const FragmentFactory& fragmentFactory,
	LightGridBuilder* builder,
	XMMATRIX& ProjMatrix,
	XMMATRIX& CameraMatrix,
	PointLight* light,
	int lightIndex )
{

	LightGridDimensions dim = builder->dimensions ();
	RCube_VecFloat234 mCameraNearFar;
	RCubeMatrix MyMatrix;
	//	XMFLOAT4X4 mCameraProj;

	//	XMStoreFloat4x4(&mCameraProj, ProjMatrix);
	MyMatrix.XMM = ProjMatrix;

	RCube_VecFloat234 clipRegion1, clipRegion, position, direction;

	mCameraNearFar.Vec = Light->mCameraNearFar;

	float attEnd;

	position.Fl3 = light->position;
	direction.Fl3 = light->direction;

	//	clipRegion1.Vec = XMVECTOR{ 1.0f, 1.0f, 0.0f, 0.0f };
	// Преобразовываем координаты центра источника света в соответствии откуда на него смотрим, чтобы правильно 
	// расчитать кластера занимаемые светом на экране
	/*	if (light->angel <= 80.0f && light->angel != 0.0f)
	{
	attEnd = light->attenuationEnd / 1.58f;//2;
	LightPos.Vec = (XMVector3Normalize(direction.Vec) * attEnd + position.Vec);
	}
	if (light->angel == 0.0f || light->angel > 80.0f)
	*/ {
		LightPos.Fl3 = light->position;
		attEnd = light->attenuationEnd;
	}

	//	LightPos.Fl3 = light->position;
	LightPosCopy.Vec = XMVector3TransformCoord ( LightPos.Vec, CameraMatrix ); //camView WorldPos

	clipRegion1.Vec = ComputeClipRegion ( LightPosCopy.Fl3, attEnd, MyMatrix.XMF._11, MyMatrix.XMF._22, mCameraNearFar.Fl4.x );
	//	clipRegion1.Vec = ComputeClipRegion(LightPosCopy.Fl3, attEnd, mCameraProj._11, mCameraProj._22, mCameraNearFar.Fl4.x);
	//	clipRegion1.Vec = ComputeClipRegion(LightPosCopy.Fl3, light->attenuationEnd, mCameraProj._11, mCameraProj._22, mCameraNearFar.Fl4.x);
	// compute view space quad
	//	clipRegion.Vec = (clipRegion1.Vec + XMVECTOR{ 1.0f, 1.0f, 1.0f, 1.0f }) / 2; // map coordinates to [0..1]
	clipRegion.Vec = (clipRegion1.Vec + XMVECTOR{ 1.0f, 1.0f, 1.0f, 1.0f }) / 2; // map coordinates to [0..1]

	int intClipRegion[4];

	intClipRegion[0] = (int)(clipRegion.Fl4.x * dim.width);
	intClipRegion[1] = (int)(clipRegion.Fl4.y * dim.height);
	intClipRegion[2] = (int)(clipRegion.Fl4.z * dim.width);
	intClipRegion[3] = (int)(clipRegion.Fl4.w * dim.height);

	if (intClipRegion[0] < 0) intClipRegion[0] = 0;
	if (intClipRegion[1] < 0) intClipRegion[1] = 0;
	if (intClipRegion[2] >= dim.width) intClipRegion[2] = dim.width - 1;
	if (intClipRegion[3] >= dim.height) intClipRegion[3] = dim.height - 1;

	float y_x = mCameraNearFar.Fl4.y - mCameraNearFar.Fl4.x;
	float center_z = (light->position.z - mCameraNearFar.Fl4.x) / y_x;
	float dist_z = light->attenuationEnd / y_x;

	int intZBounds[2];
	intZBounds[0] = (int)((center_z - dist_z)* dim.depth);
	intZBounds[1] = (int)((center_z + dist_z)* dim.depth);

	if (intZBounds[0] < 0) intZBounds[0] = 0;
	if (intZBounds[1] < 0) intZBounds[1] = 0;
	else if (intZBounds[1] >= dim.depth) intZBounds[1] = dim.depth - 1;


	for (int y = intClipRegion[1] / 4; y <= intClipRegion[3] / 4; ++y)
	{
		int y4 = y * 4;
		for (int x = intClipRegion[0] / 4; x <= intClipRegion[2] / 4; ++x)
		{
			int x4 = x * 4;
			for (int z = intZBounds[0] / 4; z <= intZBounds[1] / 4; ++z)
			{
				int z4 = z * 4;
				int x1 = clamp ( intClipRegion[0] - x4, 0, 3 );
				int x2 = clamp ( intClipRegion[2] - x4, 0, 3 );
				int y1 = clamp ( intClipRegion[1] - y4, 0, 3 );
				int y2 = clamp ( intClipRegion[3] - y4, 0, 3 );
				int z1 = clamp ( intZBounds[0] - z4, 0, 3 );
				int z2 = clamp ( intZBounds[1] - z4, 0, 3 );

				uint64_t coverage = fragmentFactory.coverage ( x1, x2, y1, y2, z1, z2 );

				builder->pushFragment ( dim.cellIndex ( x, y, z ), lightIndex, coverage );
			}
		}
	}
}


ID3D11ShaderResourceView * D3DClass::FrameLights ( ID3D11DeviceContext* d3dDeviceContext, const UIConstants* ui )
{
	int n = ui->clusteredGridScale;

	if (ClustersAmount != n)
	{
		mLightGridBuilder.reset ( LightGridDimensions ( 2 * n, n, 8 * n ) );

		//		mLightGridBuilder.clearAllFragments ();
		//	FragmentFactory fragmentFactory;
		ClustersAmount = n;
	}
	//	else
	mLightGridBuilder.clearAllFragments ();

	// ???????????????????????????????????????????????????????????????????????????????????????????????????????
	// ???????????????????????????????????????????????????????????????????????????????????????????????????????
	// ???????????????????????????????????????????????????????????????????????????????????????????????????????
	// ???????????????????????????????????????????????????????????????????????????????????????????????????????

	if (mActiveLights == 0)
		assert ( "Нет ни одного источника света в сцене" );// return nullptr;
														   // ???????????????????????????????????????????????????????????????????????????????????????????????????????
														   // ???????????????????????????????????????????????????????????????????????????????????????????????????????
														   // ???????????????????????????????????????????????????????????????????????????????????????????????????????

	PointLight* light = mLightBuffer->MapDiscard ( d3dDeviceContext );

	int c = 0;
	mVisibleLights = 0;

	PointLight *Dest = &light[0];
	PointLight **Source = &mPointLightParameters[0];

	// Обновляем массив источников света для шейдера.
	while (c < mActiveLights)
	{
		PointLight *Source1 = *Source;

		// Увеличиваем радиус свету, чтобы он не пропадал с экрана раньше времени
		float Radius = Source1->attenuationEnd;// * 1.5f;
											   // Убираем свет из отрисовки, если он не виден на экране
											   // если свет вообще нужно рисовать и по Frustum  
		if (Source1->Dummy > -1 && Frustum->CheckSphere ( Source1->position, Radius )) //
		{
			/*			if ( !Frustum->CheckSphere( Source1->position, Radius ) )
			{
			if ( Source1->)
			}
			else
			{
			*/
			GenerateLightFragments ( fragmentFactory, &mLightGridBuilder, D3DGC->ProjectionMatrix, D3DGC->ViewMatrix, *Source, Source1->Dummy );
			// Copy light list into shader buffer
			memcpy ( Dest, Source1, PointLightSize );
			//			*Dest = *Source1;
			//			}
			++mVisibleLights;
		}
		else
			Dest->Dummy = -1; // Источник света не виден в кадре
							  //			memcpy( Dest, &TempL, PointLightSize );

		++Source;
		++Dest;
		++c;
	}

	mLightBuffer->Unmap ( d3dDeviceContext );

	LightGridEntry* gpuBuffer = mLightGridBuffer->MapDiscard ( d3dDeviceContext );
	mLightGridBuilder.buildAndUpload ( gpuBuffer/*, mLightGridBufferSize * 16*/ );
	mLightGridBuffer->Unmap ( d3dDeviceContext );

	LightGridBufferSRV = mLightGridBuffer->GetShaderResource ();

	return mLightBuffer->GetShaderResource ();
}


void D3DClass::SetAmbientColor ( XMFLOAT4& Color )
{
	Light->AmbientColor = Color;
}


void D3DClass::SetDiffuseColor ( XMFLOAT4& Color )
{
	Light->DiffuseColor = Color;
}


void D3DClass::SetDiffusePower ( float& Power )
{
	Light->DiffusePower = Power;
}


void D3DClass::SetDiffuseDirection ( XMFLOAT3& Direction )
{
	Light->DiffuseDirection = Direction;
}


void D3DClass::GetDiffuseDirection ( XMFLOAT3& Direction )
{
	Direction = Light->DiffuseDirection;
}


void D3DClass::SetSpecularColor ( XMFLOAT4& Color )
{
	Light->SpecularColor = Color;
}


void D3DClass::SetSpecularPower ( float& power )
{
	//	Light->SpecularColor.w = power;
	Light->SpecularPower = power;
}


void D3DClass::SetSpecularDirection ( XMFLOAT3& Direction )
{
	Light->SpecularDirection = Direction;
}


void D3DClass::GetSpecularDirection ( XMFLOAT3& Direction )
{
	Direction = Light->SpecularDirection;
}


void D3DClass::GetAmbientColor ( XMFLOAT4& AmbColor )
{
	AmbColor = Light->AmbientColor;
}


void D3DClass::GetDiffuseColor ( XMFLOAT4 &DiffColor )
{
	DiffColor = Light->DiffuseColor;
}


void D3DClass::GetSpecularColor ( XMFLOAT4& SpecColor )
{
	SpecColor = Light->SpecularColor;
}


void D3DClass::GetSpecularPower ( float& SpecPower )
{
	//	SpecPower = Light->SpecularColor.w;
	SpecPower = Light->SpecularPower;
}


float D3DClass::GetSpecularPower ()
{
	return Light->SpecularPower;
}


void D3DClass::Frame ()
{
	// Обновляем все PointLights
	Light->mUI.shadowsOn = D3DGC->ShadowsOn;
	Light->mUI.softshadowsOn = D3DGC->SoftShadowsOn;
	Light->mUI.PCF_Amount = D3DGC->PCF_Amount;
	Light->mUI.PCF_Step = D3DGC->PCF_Step;
	Light->mUI.ShadowCLAMP = D3DGC->ShadowCLAMP;
	Light->mUI.Shadow_Divider = D3DGC->Shadow_Divider;

	lightBufferSRV = FrameLights ( D3DGC->DX_deviceContext, &Light->mUI );

	PerFrameConstants_Debug->Update (Light);
	D3DGC->DX_deviceContext->PSSetConstantBuffers ( 1, 1, &PerFrameConstants_Debug->Buffer );
}


void D3DClass::SetAllLightDefault ()
{
	//	ZeroMemory(Light, sizeof(LightBufferType));
	Light->AmbientColor = XMFLOAT4 ( 1.0f, 1.0f, 1.0f, 1.0f );

	Light->DiffuseColor = XMFLOAT4 ( 1.0f, 1.0f, 1.0f, 1.0f );
	Light->DiffuseDirection = XMFLOAT3 ( 100.0f, 100.0f, 100.0f );
	Light->DiffusePower = 2.0f;

	Light->SpecularColor = XMFLOAT4 ( 1.0f, 1.0f, 1.0f, 1.0f );
	Light->SpecularDirection = Light->DiffuseDirection;//XMFLOAT3(0.0f, 0.0f, 1.0f);
	Light->SpecularPower = 0.002f;//0.002f;
}


int D3DClass::AddLightSource ( PointLight &NewLight )
{
	int Number;

	// Указатель на новый или старый свет который будет инициализирован
	PointLight* CreateLight = NULL;

	if (!FreeLightIndex.empty ())
	{
		Number = (int)FreeLightIndex.back ();
		FreeLightIndex.pop_back ();

		CreateLight = mPointLightParameters[Number];
		memcpy ( CreateLight, &NewLight, PointLightSize );
	}
	else
	{
		Number = (int)mPointLightParameters.size ();

		// Общее количестмо светов на сцене не должно привышать MAX_LIGHTS - 4096
		if (Number > MAX_LIGHTS)
			return -1;

		CreateLight = new PointLight;
		ZeroMemory ( CreateLight, PointLightSize );
		memcpy ( CreateLight, &NewLight, PointLightSize );
		// Создаём свет
		mPointLightParameters.emplace_back ( CreateLight );

		// Увеличиваем количество светов на сцене на 1
		mActiveLights = Number + 1;

		// Пересоздаём массив светов нужного размера
		ResetLightsBuffer ();
	}
	// Сохраняем индекс света для отрисовки.  Если свет не рисуется, то Dummy = -1
	CreateLight->Dummy = Number;

	// Если добавляется свет с тенью, то удаляем массив Shadow Maps 
	// чтобы создать новый + 1 для нового Spot Light света с тенью
	if (CreateLight->HaveShadow && mActiveLightsWithShadows < MAX_LIGHTS_WITH_SHADOW)
	{
		DeleteShadowMapsArray ( ShadowMap3D, SRV_ShadowMap3D );
		// Создаём новый массив Shadow Maps + 1 для нового света
		if (!CreatingShadowMapsArray (
			(int)LightViewPort.Width,
			(int)LightViewPort.Height,
			D3DGC->MSAAQualityCount,
			D3DGC->MSAAQualityChoosen,
			++mActiveLightsWithShadows // Количество светов с тенью + 1
		))
			return -2; // Не могу создать массив Shadow Maps для теней
					   // добавляем в список индексов SpotLight которым нужен ShadowMap
		SpotLightsWithShadowsIndexes.emplace_back ( Number );
	}

	return Number;
}


void D3DClass::ResetLightsBuffer ()
{
	// Удаляем буфер светов, чтобы добавить ещё один свет к существующим.
	RCUBE_DELETE ( mLightBuffer );
	// Создаём буфер светов передаваемый в шейдер
	mLightBuffer  = new StructuredBuffer<PointLight> ( D3DGC->DX_device, mActiveLights, D3D11_BIND_SHADER_RESOURCE, true );
}



int D3DClass::GetActiveLghtNumber ()
{
	return mActiveLights; ;//(int)FreeLightIndex.size();//mActiveLights;
}


void D3DClass::FreeLightSource ( int LightNumber )
{
	if (LightNumber < (int)mPointLightParameters.size ())
	{
		FreeLightIndex.emplace_back ( LightNumber );		// Помещаем индекс освободившегося света в массив индексов созданных, но не используемых светов
		mPointLightParameters[LightNumber]->Dummy = -1;	// Убираем свет из отрисовки
														// Проверяем, была ли у света тень для удаления тени из отрисовки ShadowMap
		if (mPointLightParameters[LightNumber]->ShadowMapSliceNumber != -1)
		{
			int Amount = (int)SpotLightsWithShadowsIndexes.size ();
			for (int i = 0; i < Amount; i++)
			{
				// Удаляем индекс света из списка светов для которых генеится ShadowMap
				if (SpotLightsWithShadowsIndexes[i] == LightNumber)
				{
					SpotLightsWithShadowsIndexes.erase ( SpotLightsWithShadowsIndexes.begin () + i );
					// Уменьшаем на 1 количество светов с тенью
					--mActiveLightsWithShadows;
					break;
				}
			}
		}

	}
}


bool D3DClass::DeleteLightSource ( int LightNumber )
{
	int i = (int)mPointLightParameters.size ();
	if (LightNumber < i)
	{
		delete mPointLightParameters[LightNumber];
		mPointLightParameters.erase ( mPointLightParameters.begin () + LightNumber );
		--mActiveLights;

		ResetLightsBuffer ();

		return true;
	}
	else
		return false;
}


void D3DClass::DeleteAllLights ()
{
	int Number = (int)mPointLightParameters.size ();

	//#pragma loop(ivdep)
	//#pragma loop(hint_parallel(0))
	for (int i = 0; i < Number; ++i)
	{
		delete mPointLightParameters[i];
	}

	RCUBE_DELETE ( mLightBuffer );
	// Очищаем буфер светов
	mPointLightParameters.clear ();
	// Очищаем буфер индексов светов с тенями
	SpotLightsWithShadowsIndexes.clear ();
	// Очищаем буфер свободных индексов для света
	FreeLightIndex.clear ();

	DeleteShadowMapsArray ( ShadowMap3D, SRV_ShadowMap3D );
}


bool D3DClass::ChangeLightDirection ( int LightNumber, XMFLOAT3& direction )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	mPointLightParameters[LightNumber]->direction = direction;

	return true;
}


bool D3DClass::ChangeLightDirection ( int LightNumber, XMVECTOR& direction )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	Temp.Vec = direction;
	mPointLightParameters[LightNumber]->direction = Temp.Fl3;

	return true;
}


bool D3DClass::ChangeLightAttnuationBegin ( int LightNumber, float& attBegin )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	mPointLightParameters[LightNumber]->attenuationBegin = attBegin;

	return true;
}


bool D3DClass::ChangeLightAttnuationEnd ( int LightNumber, float& attEnd )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	mPointLightParameters[LightNumber]->attenuationEnd = attEnd;

	return true;
}


bool D3DClass::ChangeLightColor ( int LightNumber, XMFLOAT3& color )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	mPointLightParameters[LightNumber]->color = color;

	return true;
}


bool D3DClass::ChangeLightDirection ( int LightNumber, XMVECTOR&Axis, float Angel )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	XMMATRIX Rot = XMMatrixRotationAxis ( Axis, Angel );
	Temp.Fl3 = mPointLightParameters[LightNumber]->direction;
	Temp2.Vec = XMVector3TransformCoord ( Temp.Vec, Rot );
	mPointLightParameters[LightNumber]->direction = Temp2.Fl3;

	return true;
}


bool D3DClass::GetLightDirection ( int LightNumber, XMFLOAT3& direction )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	direction = mPointLightParameters[LightNumber]->direction;

	return true;
}


bool D3DClass::GetLightDirection ( int LightNumber, XMVECTOR& direction )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	Temp.Fl3 = mPointLightParameters[LightNumber]->direction;
	direction = Temp.Vec;

	return true;
}


bool D3DClass::GetLightPos ( int LightNumber, XMFLOAT3& pos )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	pos = mPointLightParameters[LightNumber]->position;

	return true;
}


bool D3DClass::GetLightPos ( int LightNumber, XMVECTOR& pos )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;

	Temp.Fl3 = mPointLightParameters[LightNumber]->position;
	pos = Temp.Vec;

	return true;
}


void D3DClass::LightsOnly ()
{
	Light->mUI.lightingOnly = true;
}


void D3DClass::LightsOnlyOFF ()
{
	Light->mUI.lightingOnly = false;
}


void D3DClass::SetClustersAmount ( int amount )
{
	if (amount == 4 || amount == 8 || amount == 16 || amount == 24 || amount == 32)
		Light->mUI.clusteredGridScale = amount;
}


bool D3DClass::ChangeLightPos ( int LightNumber, XMFLOAT3& pos )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;
	mPointLightParameters[LightNumber]->position = pos;
	return true;
}


bool D3DClass::ChangeLightPos ( int LightNumber, XMVECTOR& pos )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return false;
	Temp.Vec = pos;
	mPointLightParameters[LightNumber]->position = Temp.Fl3;
	return true;

}


void D3DClass::RenderIndextedClustered ( ID3D11ShaderResourceView *ModelTexture,
	int indexDrawAmount, int InstancCout ) {

	D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &ModelTexture );
	D3DGC->DX_deviceContext->PSSetShaderResources ( 1, 1, &SRV_ShadowMap3D );
	D3DGC->DX_deviceContext->PSSetShaderResources ( 5, 1, &lightBufferSRV );
	D3DGC->DX_deviceContext->PSSetShaderResources ( 6, 1, &LightGridBufferSRV );
	// instance модель или нет ?  >1 = true
	if (InstancCout > 1)
		D3DGC->DX_deviceContext->DrawIndexedInstanced ( indexDrawAmount, InstancCout, 0, 0, 0 );
	else
		D3DGC->DX_deviceContext->DrawIndexed ( indexDrawAmount, 0, 0 );


}


void D3DClass::RenderClustered ( ID3D11ShaderResourceView *ModelTexture,
	int vertexDrawAmount, int InstancCout )
{

	D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &ModelTexture );
	D3DGC->DX_deviceContext->PSSetShaderResources ( 1, 1, &SRV_ShadowMap3D );
	D3DGC->DX_deviceContext->PSSetShaderResources ( 5, 1, &lightBufferSRV );
	D3DGC->DX_deviceContext->PSSetShaderResources ( 6, 1, &LightGridBufferSRV );
	// instance модель или нет ?  >1 = true
	if (InstancCout > 1)
		D3DGC->DX_deviceContext->DrawInstanced ( vertexDrawAmount, InstancCout, 0, 0 );
	else
		D3DGC->DX_deviceContext->Draw ( vertexDrawAmount, 0 );

}


void D3DClass::DeleteShadowMapsArray (
	ID3D11Texture2D* Texture,
	ID3D11ShaderResourceView* SRV
)
{
	RCUBE_RELEASE ( SRV );
	RCUBE_RELEASE ( Texture );
}


bool D3DClass::CreatingShadowMapsArray (

	int Width,
	int Height,
	UINT MsaaCount,
	UINT MsaaQuality,
	int ArraySize
)
{
	HRESULT res;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = ArraySize;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.SampleDesc.Count = MsaaCount;
	depthStencilDesc.SampleDesc.Quality = MsaaQuality;

	res = D3DGC->DX_device->CreateTexture2D ( &depthStencilDesc, NULL, &ShadowMap3D );
	if (FAILED ( res ))
	{
		return false;
	}

#if defined( DEBUG ) || defined( _DEBUG )
	const char c_szName [] = "ShadowMap3D";
	ShadowMap3D->SetPrivateData ( WKPDID_D3DDebugObjectName, sizeof ( c_szName ) - 1, c_szName );
#endif
 
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	// Если используется MSAA, то нужно иначе создать текстуру
	if (D3DGC->MSAAQualityCount > 1)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		srvDesc.Texture2DMSArray.ArraySize = depthStencilDesc.ArraySize;
		srvDesc.Texture2DMSArray.FirstArraySlice = 0;
	}
	else
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MipLevels = depthStencilDesc.MipLevels;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.ArraySize = depthStencilDesc.ArraySize;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
	}


	res = D3DGC->DX_device->CreateShaderResourceView ( ShadowMap3D, &srvDesc, &SRV_ShadowMap3D );
	if (FAILED ( res ))
	{
		return false;
	}

	return true;
}


void D3DClass::ShadowsON ( bool Value )
{
	Light->mUI.shadowsOn = Value;
}


void D3DClass::SoftShadowsON ( bool Value )
{
	Light->mUI.softshadowsOn = Value;
}


void D3DClass::LightRender ( int LightNumber, bool Yes )
{
	if (LightNumber > (int)mPointLightParameters.size ())
		return;
	else if (Yes)
		mPointLightParameters[LightNumber]->Dummy = LightNumber;
	else
		mPointLightParameters[LightNumber]->Dummy = -1;
}


// Отображение текстуры
/*
D3DGC->DX_deviceContext->CopyResource(D3DGC->ScreenShootTexture, D3DGC->backBufferPtr);
union _block
{
byte  *data;
unsigned int *udata;
} block;

D3D11_MAPPED_SUBRESOURCE  mapResource;
hr = D3DGC->DX_deviceContext->Map(D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource);

unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
block.data = new byte[amount];
memcpy(block.data, mapResource.pData, amount);
D3DGC->DX_deviceContext->Unmap(D3DGC->ScreenShootTexture, 0);
Image img;
D3D11_TEXTURE2D_DESC* TextureDesc = new D3D11_TEXTURE2D_DESC;
D3DGC->ScreenShootTexture->GetDesc(TextureDesc);
img.height = TextureDesc->Height;
img.width = TextureDesc->Width;
img.format = TextureDesc->Format;
img.rowPitch = mapResource.RowPitch;
img.slicePitch = mapResource.DepthPitch;
img.pixels = block.data;

hr = SaveToWICFile(img, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), L"2___BACKBUFFER.png");
if (FAILED(hr))
{
assert(L"Could not save 2___BACKBUFFER.png file.");
}

delete[] block.data;
delete TextureDesc;

*/

