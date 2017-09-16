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

	D3DGC->MSAAQualityCount = 1;//1 2 4 8
	D3DGC->MSAAQualityChoosen = 0; //0 2 16 32
	
	D3DGC->hwnd							= nullptr;
	D3DGC->Adapter						= nullptr;
	D3DGC->D11_device					= nullptr;
	D3DGC->D11_deviceContext			= nullptr;
	D3DGC->D11_swapChain				= nullptr;
	D3DGC->D10_device					= nullptr;
	D3DGC->BackBuffer2DT				= nullptr;
	D3DGC->BackBuffer_SRV				= nullptr;
	D3DGC->BackBuffer_RTV				= nullptr;
	D3DGC->BackBuffer_UAV				= nullptr;
	D3DGC->DWriteFactory				= nullptr;
	D3DGC->m_alphaEnableBlendingState	= nullptr;
	D3DGC->m_alphaDisableBlendingState	= nullptr;
	D3DGC->m_alpha_TOnT_BlendingState	= nullptr;	// Текст на текстуре
	D3DGC->m_depthStencilState			= nullptr;
	D3DGC->m_depthStencilView			= nullptr;
	D3DGC->mGeometryBlendState			= nullptr;
	D3DGC->m_alphaParticleBlendingState = nullptr;
	D3DGC->m_depthStencilBuffer			= nullptr;
	D3DGC->m_rasterState				= nullptr;
	D3DGC->m_EngineInputClass			= nullptr;
	m_rasterStateNone					= nullptr;
	m_depthDisabledStencilState			= nullptr;

#if defined( DEBUG ) || defined( _DEBUG )
	D3DGC->DebugDevice = nullptr;
#endif

// FXAA
	D3DGC->g_pcbFXAA					= nullptr;
	D3DGC->BackBuffer_ProxyTexture		= nullptr;
	D3DGC->BackBuffer_ProxyTextureSRV	= nullptr;
	D3DGC->BackBuffer_ProxyTextureRTV	= nullptr;
	D3DGC->BackBuffer_CopyResolveTexture= nullptr;
	D3DGC->BackBuffer_CopyResolveTextureSRV = nullptr;
	D3DGC->BackBuffer_CopyResolveTextureUAV = nullptr;
//	D3DGC->g_pSamPointMirror			= nullptr;
//	D3DGC->g_pSamLinearWrap				= nullptr;
//	D3DGC->g_pSamPointCmpClamp			= nullptr;
	D3DGC->g_pSamBilinear				= nullptr;
	D3DGC->ScreenShootTexture			= nullptr;
//	D3DGC->SRV_ScreenShootTexture		= nullptr;

// Shadow Maps & Lights
	D3DGC->CLight_DiffuseSampler		= nullptr;
	D3DGC->CLight_SampleTypeClamp		= nullptr;
	D3DGC->CLight_cmpSampler			= nullptr;

// Direct Write Fonts
	D3DGC->sharedTex11					= nullptr;
	D3DGC->sharedTex11_SRV				= nullptr;
	D3DGC->sharedTex11_MAPED			= nullptr;
	D3DGC->keyedMutex11					= nullptr;
	D3DGC->keyedMutex10					= nullptr;
	D3DGC->D2DRenderTarget				= nullptr;
	D3DGC->D2DFactory					= nullptr;
	// Размеры текстуры для отрисовки шрифтов 
	// достаточные для размера шрифта 74.0f с Outline 10
	// Максимальная длинна 8192 для DX10
	D3DGC->sharedTex11_Width			= 8192;
	D3DGC->sharedTex11_Height			= 128;

}


D3DClass::~D3DClass()
{
	delete D3DGC;
}

bool D3DClass::Initialize(HWND hwnd, int screenWidth, int screenHeight, bool vsync, bool fullscreen,
	float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory1*			 Factory1	= nullptr;
	IDXGIOutput*        AdapterOutput	= nullptr;

	D3DGC->hwnd							= hwnd;

/*
	HRESULT hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED |
								 COINIT_DISABLE_OLE1DDE );
	if ( SUCCEEDED( hr ) )
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
							   IID_IFileOpenDialog, reinterpret_cast<void**>( &pFileOpen ) );

		if ( SUCCEEDED( hr ) )
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show( NULL );

			// Get the file name from the dialog box.
			if ( SUCCEEDED( hr ) )
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult( &pItem );
				if ( SUCCEEDED( hr ) )
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );

					// Display the file name to the user.
					if ( SUCCEEDED( hr ) )
					{
						MessageBox( NULL, pszFilePath, L"File Path", MB_OK );
						CoTaskMemFree( pszFilePath );
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

*/







	D3DGC->ScreenWidth					= screenWidth;
	D3DGC->ScreenHeight					= screenHeight;
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
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
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
		MessageBox(hwnd, L"Could not initialize CreateDXGIFactory1.", L"Error", MB_OK);
		return false;
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
					MessageBox(hwnd, L"Could not copy VideoAdapter name.", L"Error", MB_OK);
					RCUBE_RELEASE( Factory1 );
					Shutdown();
					return false;
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
		MessageBox(hwnd, L"Could not get Adapters[0]->EnumOutputs.", L"Error", MB_OK);
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
	}

	result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not get AdapterOutput->GetDisplayModeList.", L"Error", MB_OK);
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		MessageBox(hwnd, L"No DisplayMode in List.", L"Error", MB_OK);
		delete[] displayModeList;
		RCUBE_RELEASE( AdapterOutput );
//		AdaptersRelease();
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
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
	result = D3D11CreateDevice(D3DGC->Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, nDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &D3DGC->D11_device, &featureLevelSuccess, &D3DGC->D11_deviceContext);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"D3D11CreateDevice Failed.", 0, 0);
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
	}
	if (featureLevelSuccess != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(hwnd, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
	}

#if defined( DEBUG ) || defined( _DEBUG )

	const char c_szName[] = "MyDeViCe KAF";
	D3DGC->D11_device->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);

	const char c_szName1[] = "FIRST CONTEXT KAF";
	D3DGC->D11_deviceContext->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName1) - 1, c_szName1);

	result = D3DGC->D11_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&D3DGC->DebugDevice));
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize DebugDevice.", L"Error", MB_OK);
//		AdaptersRelease();
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
	}
#endif
	UINT MSAAQualityNumber;
	// Check MSAA Quality Levels supported
	result = D3DGC->D11_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, D3DGC->MSAAQualityCount, &MSAAQualityNumber);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CheckMultisampleQualityLevels.", 0, 0);
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
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

	result = Factory1->CreateSwapChain(D3DGC->D11_device, &swapChainDesc, &D3DGC->D11_swapChain);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateSwapChain.", 0, 0);
		RCUBE_RELEASE( Factory1 );
		Shutdown();
		return false;
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
	result = D3DGC->D11_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&D3DGC->BackBuffer2DT );
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D m_swapChain->GetBuffer.", 0, 0);
		Shutdown();
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = D3DGC->D11_device->CreateRenderTargetView(D3DGC->BackBuffer2DT, NULL, &D3DGC->BackBuffer_RTV );
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateRenderTargetView.", 0, 0);
		Shutdown();
		return false;
	}

	result = D3DGC->D11_device->CreateShaderResourceView(D3DGC->BackBuffer2DT, NULL, &D3DGC->BackBuffer_SRV );
	if ( FAILED( result ) )
	{
		MessageBox( hwnd, L"Error CreateShaderResourceView BackBuffer_SRV.", 0, 0 );
		Shutdown();
		return false;
	}
/*
	result = D3DGC->D11_device->CreateUnorderedAccessView( D3DGC->BackBuffer2DT, NULL, &D3DGC->BackBuffer_UAV);
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
	result = D3DGC->D11_device->CreateTexture2D(&depthBufferDesc, NULL, &D3DGC->m_depthStencilBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateTexture2D.", 0, 0);
		Shutdown();
		return false;
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
	result = D3DGC->D11_device->CreateDepthStencilState(&depthStencilDesc, &D3DGC->m_depthStencilState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateDepthStencilState.", 0, 0);
		Shutdown();
		return false;
	}

	// With the created depth stencil state we can now set it so that it takes effect. Notice we use the device context to set it.

	// Set the depth stencil state.
	D3DGC->D11_deviceContext->OMSetDepthStencilState(D3DGC->m_depthStencilState, 1);

	// The next thing we need to create is the description of the view of the depth stencil buffer. We do this so that Direct3D knows to use the depth buffer as a depth stencil texture.
	// After filling out the description we then call the function CreateDepthStencilView to create it.

	// Initailze the depth stencil view.
	CD3D11_DEPTH_STENCIL_VIEW_DESC DSD(
		// Для выключеного MSAA МОЖНО ставить D3D11_DSV_DIMENSION_TEXTURE2D
		// Для ВКЛ MSAA нужно D3D11_DSV_DIMENSION_TEXTURE2DMS
		D3D11_DSV_DIMENSION_TEXTURE2DMS,
		//D3D11_DSV_DIMENSION_TEXTURE2D,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
		0,
		0,
		1);
	// Create the depth stencil view.
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2-й параметр  поставил NULL вместо &depthStencilViewDesc и заработало сглаживание
	result = D3DGC->D11_device->CreateDepthStencilView(D3DGC->m_depthStencilBuffer, &DSD, &D3DGC->m_depthStencilView);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateDepthStencilView.", 0, 0);
		Shutdown();
		return false;
	}

	// With that created we can now call OMSetRenderTargets. This will bind the render target view and the depth stencil buffer to the output render pipeline.
	// This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created. With the graphics written to the back buffer we can then swap it to the front
	// and display our graphics on the user's screen.

	// Можно удалить следующую строку
	D3DGC->D11_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, D3DGC->m_depthStencilView);

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
	result = D3DGC->D11_device->CreateRasterizerState(&rasterDesc, &D3DGC->m_rasterState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateRasterizerState.", 0, 0);
		Shutdown();
		return false;
	}
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthClipEnable = false;
	result = D3DGC->D11_device->CreateRasterizerState(&rasterDesc, &m_rasterStateNone);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateRasterizerState CULLNONE.", 0, 0);
		Shutdown();
		return false;
	}

	// Now set the rasterizer state.
	D3DGC->D11_deviceContext->RSSetState(D3DGC->m_rasterState);

	// The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. Set this to be the entire size of the window.

	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	D3DGC->D11_deviceContext->RSSetViewports(1, &viewport);

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

	// Here we setup the description of the depth stencil. Notice the only difference between this new depth stencil and the old one is the DepthEnable is set to false here for 2D drawing.

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = true;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;//D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = false;
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

	// Now create the new depth stencil.

	// Create the state using the device.
	result = D3DGC->D11_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateDepthStencilState.", 0, 0);
		Shutdown();
		return false;
	}

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
	result = D3DGC->D11_device->CreateBlendState(&blendStateDescription, &D3DGC->m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateBlendState.", 0, 0);
		Shutdown();
		return false;
	}

	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	result = D3DGC->D11_device->CreateBlendState(&blendStateDescription, &D3DGC->m_alphaParticleBlendingState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D Create m_alphaParticleBlendingState.", 0, 0);
		Shutdown();
		return false;
	}	// Now to create an alpha disabled state we change the description we just made to set BlendEnable to FALSE. The rest of the settings can be left as they are.


	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//D3D11_BLEND_ONE; //D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f; // ВОТ ЭТО РАБОТАЕТ   НО НЕМНОГО  НЕ ТАК КАК НУЖНО НАМ  МОЖЕТ ПОТОМУ ЧТО В ТЕКСТУРАХ БЛЕНДИНГ УКАЗАН НЕ ТАК КАК НУЖНО НАМ
	result = D3DGC->D11_device->CreateBlendState( &blendStateDescription, &D3DGC->m_alpha_TOnT_BlendingState);
	if ( FAILED( result ) )
	{
		MessageBox( hwnd, L"Direct3D Create m_alpha_TOnT_BlendingState.", 0, 0 );
		Shutdown();
		return false;
	}	// Now to create an alpha disabled state we change the description we just made to set BlendEnable to FALSE. The rest of the settings can be left as they are.



	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	// We then create an alpha disabled blending state using the modified blend state description. We now have two blending states we can switch between to turn on and off alpha blending.

	// Create the blend state using the description.
	result = D3DGC->D11_device->CreateBlendState(&blendStateDescription, &D3DGC->m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateBlendState for Particles.", 0, 0);
		Shutdown();
		return false;
	}
// Для кластеринга
	CD3D11_BLEND_DESC desc(D3D11_DEFAULT);
	result = D3DGC->D11_device->CreateBlendState(&desc, &D3DGC->mGeometryBlendState);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Direct3D CreateBlendState for Clustering Shading.", 0, 0);
		Shutdown();
		return false;
	}

	// DirectWrite
	result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&D3DGC->DWriteFactory));
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize DWrite CreateFactory.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	// +++++++++++++++++++++++++++     FXAA     ++++++++++++++++++++++++++++++++++
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_FXAA);
	result = D3DGC->D11_device->CreateBuffer(&cbDesc, NULL, &D3DGC->g_pcbFXAA);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize g_pcbFXAA.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE MappedResource;

	hr = D3DGC->D11_deviceContext->Map( D3DGC->g_pcbFXAA, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	CB_FXAA* pFXAA = ( CB_FXAA* ) MappedResource.pData;

	pFXAA->m_fxaa = XMVECTOR { 1.0f / D3DGC->ScreenWidth, 1.0f / D3DGC->ScreenHeight, 0.0f, 0.0f };
	D3DGC->D11_deviceContext->Unmap( D3DGC->g_pcbFXAA, 0 );


	DXGI_SAMPLE_DESC SDesc;
	ZeroMemory(&SDesc, sizeof(DXGI_SAMPLE_DESC));
	SDesc.Count = swapChainDesc.SampleDesc.Count;
	SDesc.Quality = swapChainDesc.SampleDesc.Quality;

	DXGI_SURFACE_DESC pBackBufferSurfaceDesc;
	ZeroMemory(&pBackBufferSurfaceDesc, sizeof(DXGI_SURFACE_DESC));

	pBackBufferSurfaceDesc.Format = swapChainDesc.BufferDesc.Format;
	pBackBufferSurfaceDesc.Width = swapChainDesc.BufferDesc.Width;
	pBackBufferSurfaceDesc.Height = swapChainDesc.BufferDesc.Height;
	pBackBufferSurfaceDesc.SampleDesc = SDesc;

	if ( !FxaaIntegrateResource(D3DGC->D11_device, &pBackBufferSurfaceDesc ) )
	{
		MessageBox( hwnd, L"Could not initialize FXAA.", L"Error", MB_OK );
		Shutdown();
		return false;
	}
	
	if ( !InitD2D_D3D101_DWrite() )
	{
		MessageBox( hwnd, L"Could not initialize DWrite.", L"Error", MB_OK );
		Shutdown();
		return false;
	}

	D3D11_SAMPLER_DESC samDesc;

	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; //D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.MipLODBias = 0.0f;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.BorderColor[0] = samDesc.BorderColor[1] = samDesc.BorderColor[2] = samDesc.BorderColor[3] = 0;
	samDesc.MinLOD = 0;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = D3DGC->D11_device->CreateSamplerState( &samDesc, &D3DGC->g_pSamBilinear );

/*
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;//D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	result = D3DGC->D11_device->CreateSamplerState(&samDesc, &D3DGC->g_pSamPointMirror);

	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = D3DGC->D11_device->CreateSamplerState(&samDesc, &D3DGC->g_pSamLinearWrap);

	samDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	result = D3DGC->D11_device->CreateSamplerState(&samDesc, &D3DGC->g_pSamPointCmpClamp);
*/
	// ---------------------------     FXAA     ----------------------------------

	//  ++++++++++++++++++   Для LightClass и Shadows   ++++++++++++++++++++++++
	// Create sampler state
	{
		CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MaxAnisotropy = 16;
		D3DGC->D11_device->CreateSamplerState(&desc, &D3DGC->CLight_DiffuseSampler);
	}
	{
		// Если делать не CLAMP то появляются дублирующие тени
		CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MaxAnisotropy = 16;
		D3DGC->D11_device->CreateSamplerState(&desc, &D3DGC->CLight_SampleTypeClamp);
	}
	{
		CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; //D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;//D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;//
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;//D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS;//D3D11_COMPARISON_LESS_EQUAL;
		D3DGC->D11_device->CreateSamplerState(&desc, &D3DGC->CLight_cmpSampler);
	}
	//  ------------------   Для Shadows   -------------------------

	return true;
}

// The Shutdown function will release and clean up all the pointers used in the Initialize function, its pretty straight forward. However before doing that I put in a call to force the swap
// chain to go into windowed mode first before releasing any pointers. If this is not done and you try to release the swap chain in full screen mode it will throw some exceptions.
// So to avoid that happening we just always force windowed mode before shutting down Direct3D.

void D3DClass::Shutdown()
{
	for(int i=0, j = (int)ActualDisplayModes.size();  i < j; i++)
	{
		delete ActualDisplayModes[i];
	}
	ActualDisplayModes.clear();

// Fonts
	RCUBE_RELEASE( D3DGC->sharedTex11 );
	RCUBE_RELEASE( D3DGC->sharedTex11_SRV );
	RCUBE_RELEASE( D3DGC->sharedTex11_MAPED );
	RCUBE_RELEASE( D3DGC->keyedMutex11 );
	RCUBE_RELEASE( D3DGC->keyedMutex10 );
	RCUBE_RELEASE( D3DGC->D2DRenderTarget );
	RCUBE_RELEASE( D3DGC->D2DFactory );

// FXAA	
	RCUBE_RELEASE( D3DGC->g_pcbFXAA );
	RCUBE_RELEASE( D3DGC->BackBuffer_ProxyTexture );
	RCUBE_RELEASE( D3DGC->BackBuffer_ProxyTextureSRV );
	RCUBE_RELEASE( D3DGC->BackBuffer_ProxyTextureRTV );
	RCUBE_RELEASE( D3DGC->BackBuffer_CopyResolveTexture );
	RCUBE_RELEASE( D3DGC->BackBuffer_CopyResolveTextureSRV );
	RCUBE_RELEASE( D3DGC->BackBuffer_CopyResolveTextureUAV );
//	RCUBE_RELEASE( D3DGC->g_pSamPointMirror );
//	RCUBE_RELEASE( D3DGC->g_pSamLinearWrap );
//	RCUBE_RELEASE( D3DGC->g_pSamPointCmpClamp );
	RCUBE_RELEASE( D3DGC->g_pSamBilinear );
// FXAA	

// ShadowMap
	RCUBE_RELEASE( D3DGC->CLight_DiffuseSampler );
	RCUBE_RELEASE( D3DGC->CLight_SampleTypeClamp );
	RCUBE_RELEASE( D3DGC->CLight_cmpSampler );
//  ShadowMap
	RCUBE_RELEASE( D3DGC->DWriteFactory );
	RCUBE_RELEASE( D3DGC->m_alphaDisableBlendingState );
	RCUBE_RELEASE( D3DGC->m_alphaParticleBlendingState );
	RCUBE_RELEASE( D3DGC->m_alpha_TOnT_BlendingState );
	RCUBE_RELEASE( D3DGC->m_alphaEnableBlendingState );
	RCUBE_RELEASE( m_depthDisabledStencilState );
	RCUBE_RELEASE( D3DGC->m_rasterState );
	RCUBE_RELEASE( m_rasterStateNone );
	RCUBE_RELEASE( D3DGC->m_depthStencilView );
	RCUBE_RELEASE( D3DGC->m_depthStencilState );
	RCUBE_RELEASE( D3DGC->mGeometryBlendState );
	RCUBE_RELEASE( D3DGC->m_depthStencilBuffer );
	RCUBE_RELEASE( D3DGC->BackBuffer_RTV );
	RCUBE_RELEASE( D3DGC->BackBuffer2DT );
	RCUBE_RELEASE( D3DGC->BackBuffer_SRV );
	RCUBE_RELEASE( D3DGC->BackBuffer_UAV );
	RCUBE_RELEASE( D3DGC->ScreenShootTexture );
//	RCUBE_RELEASE( D3DGC->SRV_ScreenShootTexture );

	if (D3DGC->D11_swapChain){
		D3DGC->D11_swapChain->SetFullscreenState(false, NULL);
		RCUBE_RELEASE(D3DGC->D11_swapChain);
	}

	RCUBE_RELEASE( D3DGC->D10_device );
	RCUBE_RELEASE(D3DGC->D11_deviceContext);
	AdaptersRelease();

#if defined( DEBUG ) || defined( _DEBUG )
	D3DGC->DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	D3DGC->DebugDevice->Release();
#endif

	RCUBE_RELEASE(D3DGC->D11_device);
	//    if (D3DGC->Adapter)                        D3DGC->Adapter->Release();

	RCUBE_ARR_DELETE ( m_videoCardDescription );

	return;
}

// In the D3DClass I have a couple helper functions. The first two are BeginScene and EndScene. BeginScene will be called whenever we are going to draw a new 3D scene at the beginning of each frame.
// All it does is initializes the buffers so they are blank and ready to be drawn to. The other function is Endscene, it tells the swap chain to display our 3D scene once all the drawing has completed
// at the end of each frame.

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	D3DGC->D11_deviceContext->ClearRenderTargetView(D3DGC->BackBuffer_RTV, color);

	// Clear the depth buffer.
	D3DGC->D11_deviceContext->ClearDepthStencilView(D3DGC->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		D3DGC->D11_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		D3DGC->D11_swapChain->Present(0, 0);
	}

	return;
}

// The next three helper functions give copies of the projection, world, and orthographic matrices to calling functions. Most shaders will need these matrices for rendering so there needed
// to be an easy way for outside objects to get a copy of them. We won't call these functions in this tutorial but I'm just explaining why they are in the code.

void D3DClass::GetProjectionMatrix(XMMATRIX &projectionMatrix)
{
	projectionMatrix = D3DGC->ProjectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX &worldMatrix)
{
	worldMatrix = D3DGC->WorldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX &orthoMatrix)
{
	orthoMatrix = D3DGC->OrthoMatrix;
	return;
}


// These are the new functions for enabling and disabling the Z buffer. To turn Z buffering on we set the original depth stencil. To turn Z buffering off we set the new depth stencil that has
// depthEnable set to false. Generally the best way to use these functions is first do all your 3D rendering, then turn the Z buffer off and do your 2D rendering, and then turn the Z buffer on again.

void D3DClass::TurnZBufferOn()
{
	D3DGC->D11_deviceContext->OMSetDepthStencilState(D3DGC->m_depthStencilState, 1);
	return;
}


void D3DClass::TurnZBufferOff()
{
	D3DGC->D11_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
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
	D3DGC->D11_deviceContext->OMSetBlendState(D3DGC->m_alphaEnableBlendingState, blendFactor, 0xffffffff);

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
	D3DGC->D11_deviceContext->OMSetBlendState(D3DGC->m_alphaDisableBlendingState, blendFactor, 0xffffffff);

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
	D3DGC->D11_deviceContext->OMSetBlendState(D3DGC->m_alphaParticleBlendingState, blendFactor, 0xffffffff);

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
	D3DGC->D11_deviceContext->OMSetBlendState( D3DGC->m_alpha_TOnT_BlendingState, blendFactor, 0xffffffff );

	return;
}


ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
{
	return D3DGC->m_depthStencilView;
}


void D3DClass::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	D3DGC->D11_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, D3DGC->m_depthStencilView);

	return;
}


ID3D11RenderTargetView* D3DClass::GetRenderTargetView(){
	return D3DGC->BackBuffer_RTV;
}


void D3DClass::GetDisplaySupportedModes(UINT resolution_min)
{
	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);
	//std::ofstream out("out.txt");

	for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; ++iModeNum) {
		//    out << "Mode #" << iModeNum << " = " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << " Bits PP " << dm.dmBitsPerPel <<
		//        " color " << dm.dmColor << " Freq. " << dm.dmDisplayFrequency << endl;
	}
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

	//    FoundAdapter1->Release();
	Adapters.clear();
}

char* D3DClass::GetVideoCardString()
{
	return &m_videoCardDescription[0];
}


void D3DClass::SetDefaultResterizeState()
{
	D3DGC->D11_deviceContext->RSSetState(D3DGC->m_rasterState);
}


void D3DClass::SetCullNoneRState()
{
	D3DGC->D11_deviceContext->RSSetState(m_rasterStateNone);
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
		MessageBox( NULL , L"FXAA can't create g_pCopyResolveTexture", 0, 0 );
		return false;
	}

	desc.SampleDesc.Count = pBackBufferSurfaceDesc->SampleDesc.Count;
	desc.SampleDesc.Quality = pBackBufferSurfaceDesc->SampleDesc.Quality;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //| D3D11_BIND_UNORDERED_ACCESS;  Если только без MSAA

	result = pd3dDevice->CreateTexture2D(&desc, 0, &D3DGC->BackBuffer_ProxyTexture );
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create g_pProxyTexture", 0, 0 );
		return false;
	}
	result = pd3dDevice->CreateShaderResourceView(D3DGC->BackBuffer_ProxyTexture, NULL, &D3DGC->BackBuffer_ProxyTextureSRV);
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create BackBuffer_ProxyTextureSRV", 0, 0 );
		return false;
	}
	result = pd3dDevice->CreateRenderTargetView(D3DGC->BackBuffer_ProxyTexture, 0, &D3DGC->BackBuffer_ProxyTextureRTV );
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create BackBuffer_ProxyTextureRTV", 0, 0 );
		return false;
	}
	result = pd3dDevice->CreateShaderResourceView(D3DGC->BackBuffer_CopyResolveTexture, NULL, &D3DGC->BackBuffer_CopyResolveTextureSRV);
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create BackBuffer_CopyResolveTextureSRV", 0, 0 );
		return false;
	}

	result = pd3dDevice->CreateUnorderedAccessView( D3DGC->BackBuffer_CopyResolveTexture, NULL, &D3DGC->BackBuffer_CopyResolveTextureUAV );
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"Error CreateUnorderedAccessView BackBuffer_CopyResolveTextureUAV.", 0, 0 );
		Shutdown();
		return false;
	}

/*
	desc.BindFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	// Сохранение возможно только для текстуры без MSAA 
	// Нужно делать ResolveSubresource
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	result = pd3dDevice->CreateTexture2D(&desc, 0, &D3DGC->ScreenShootTexture);
	if ( FAILED( result ) )
	{
		MessageBox( NULL , L"FXAA can't create ScreenShotTexture", 0, 0 );
		return false;
	}
*/
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

//		D3DGC->D11_deviceContext->VSSetConstantBuffers(12, 1, &D3DGC->g_pcbFXAA);
		D3DGC->D11_deviceContext->PSSetConstantBuffers(12, 1, &D3DGC->g_pcbFXAA);
/*
		ID3D11SamplerState * ppSamplerStates[4] = { D3DGC->g_pSamPointMirror,
			D3DGC->g_pSamLinearWrap,
			D3DGC->g_pSamPointCmpClamp,
			D3DGC->g_pSamBilinear };
*/
		D3DGC->D11_deviceContext->PSSetSamplers(0, 1, &D3DGC->g_pSamBilinear );

		D3DGC->D11_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, 0);

		ID3D11Resource* MyresDest = nullptr;

		if (D3DGC->EnableMSAA && D3DGC->MSAAQualityCount > 1)
		{
			// resolve first
			D3DGC->D11_deviceContext->ResolveSubresource(D3DGC->BackBuffer_CopyResolveTexture, 0, D3DGC->BackBuffer2DT, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
			D3DGC->D11_deviceContext->PSSetShaderResources(0, 1, &D3DGC->BackBuffer_CopyResolveTextureSRV);
		}
		else
		{
			D3DGC->BackBuffer_ProxyTextureSRV->GetResource( &MyresDest );
			D3DGC->D11_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer2DT );
			D3DGC->D11_deviceContext->PSSetShaderResources(0, 1, &D3DGC->BackBuffer_ProxyTextureSRV);
		}
//		D3DGC->D11_deviceContext->IASetVertexBuffers( NULL, NULL,NULL,NULL,NULL);
		D3DGC->D11_deviceContext->IASetInputLayout(NULL);
		D3DGC->D11_deviceContext->Draw(3, 0);
/*
		D3DGC->D11_deviceContext->CopyResource(D3DGC->ScreenShootTexture, D3DGC->backBufferPtr);
		union _block
		{
			byte  *data;
			unsigned int *udata;
		} block;

		D3D11_MAPPED_SUBRESOURCE  mapResource;
		hr = D3DGC->D11_deviceContext->Map(D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource);

		unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
		block.data = new byte[amount];
		memcpy(block.data, mapResource.pData, amount);
		D3DGC->D11_deviceContext->Unmap(D3DGC->ScreenShootTexture, 0);
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
		D3DGC->D11_deviceContext->OMSetRenderTargets(1, &D3DGC->BackBuffer_RTV, D3DGC->m_depthStencilView);
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

	hr = D3DGC->D11_device->CreateTexture2D( desc, 0, &D3DGC->ScreenShootTexture );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Can't create ScreenShotTexture", 0, 0 );
		Result = false;
		goto END;
	}

	// Если текстура MSAA, то делаем ResolveSubresurces
	if ( InputTextureDesc->SampleDesc.Count > 1 )
	{
		D3DGC->D11_deviceContext->ResolveSubresource( D3DGC->BackBuffer_CopyResolveTexture, 0, tempResource, 0, DXGI_FORMAT_R8G8B8A8_UNORM );
	// D3DGC->BackBuffer_CopyResolveTextureSRV
		ID3D11Resource* Resource;
		D3DGC->BackBuffer_CopyResolveTextureSRV->GetResource( &Resource );
		D3DGC->D11_deviceContext->CopyResource( D3DGC->ScreenShootTexture, Resource );
		Resource->Release();
	}
	else
	{
		D3DGC->D11_deviceContext->CopyResource( D3DGC->ScreenShootTexture, tempResource );
	}

	union _block
	{
		byte  *data;
		unsigned int *udata;
	} block;

	D3D11_MAPPED_SUBRESOURCE  mapResource;
	hr = D3DGC->D11_deviceContext->Map( D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource );

	unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
	block.data = new byte[amount];
	memcpy( block.data, mapResource.pData, amount );
	D3DGC->D11_deviceContext->Unmap( D3DGC->ScreenShootTexture, 0 );

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
		MessageBox( NULL, L"Could not initialize D3D10CreateDevice1.", L"Error", MB_OK );
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

	hr = D3DGC->D11_device->CreateTexture2D( &sharedTexDesc, NULL, &D3DGC->sharedTex11 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11.", L"Error", MB_OK );
		return false;
	}

	hr = D3DGC->D11_device->CreateShaderResourceView( D3DGC->sharedTex11, NULL, &D3DGC->sharedTex11_SRV );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize ShaderResourceView sharedTex11.", L"Error", MB_OK );
		return false;
	}

	// Создаём копию текстуры sharedTex11 с которой может работать CPU
	sharedTexDesc.BindFlags = 0;
	sharedTexDesc.MiscFlags = 0;
	sharedTexDesc.Usage = D3D11_USAGE_STAGING;
	sharedTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	hr = D3DGC->D11_device->CreateTexture2D( &sharedTexDesc, NULL, &D3DGC->sharedTex11_MAPED );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11_MAPED.", L"Error", MB_OK );
		return false;
	}

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = D3DGC->sharedTex11->QueryInterface( __uuidof( IDXGIKeyedMutex ), ( void** ) &D3DGC->keyedMutex11 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11->QueryInterfac.", L"Error", MB_OK );
		return false;
	}

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	hr = D3DGC->sharedTex11->QueryInterface( __uuidof( IDXGIResource ), ( void** ) &sharedResource10 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedTex11->QueryInterface.", L"Error", MB_OK );
		return false;
	}

	hr = sharedResource10->GetSharedHandle( &sharedHandle10 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedResource10->GetSharedHandle.", L"Error", MB_OK );
		return false;
	}
	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1
	IDXGISurface1 *sharedSurface10;

	hr = D3DGC->D10_device->OpenSharedResource( sharedHandle10, __uuidof( IDXGISurface1 ), ( void** ) ( &sharedSurface10 ) );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize d3d101Device->OpenSharedResource.", L"Error", MB_OK );
		return false;
	}

	hr = sharedSurface10->QueryInterface( __uuidof( IDXGIKeyedMutex ), ( void** ) &D3DGC->keyedMutex10 );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize sharedSurface10->QueryInterface.", L"Error", MB_OK );
		return false;
	}

	// Create D2D factory
	hr = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof( ID2D1Factory ), ( void** ) &D3DGC->D2DFactory );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not initialize D2D1CreateFactory.", L"Error", MB_OK );
		return false;
	}

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory( &renderTargetProperties, sizeof( renderTargetProperties ) );

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat( DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED );

	hr = D3DGC->D2DFactory->CreateDxgiSurfaceRenderTarget( sharedSurface10, &renderTargetProperties, &D3DGC->D2DRenderTarget );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Could not Create DxgiSurfaceRenderTarget.", L"Error", MB_OK );
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
//		D3DGC->D11_deviceContext->ResolveSubresource(D3DGC->BackBuffer_CopyResolveTexture, 0, D3DGC->BackBuffer2DT, 0, DXGI_FORMAT_R8G8B8A8_UNORM );
//		D3DGC->D11_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer_CopyResolveTexture);

		D3DGC->D11_deviceContext->CopyResource( MyresDest, D3DGC->BackBuffer2DT );

		D3DGC->D11_deviceContext->CSSetShaderResources(0, 1, &D3DGC->BackBuffer_ProxyTextureSRV);
		D3DGC->D11_deviceContext->CSSetUnorderedAccessViews(0, 1, &D3DGC->BackBuffer_CopyResolveTextureUAV, 0);

		int c = 0;
		//	int BlurAmont = 1; // Сила Blur
		do
		{
			D3DGC->D11_deviceContext->CSSetShader(Horizont, NULL, 0);
			D3DGC->D11_deviceContext->Dispatch(D3DGC->ScreenWidth / 96, D3DGC->ScreenHeight, 1);

			D3DGC->D11_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer_CopyResolveTexture);

			D3DGC->D11_deviceContext->CSSetShader(Vertical, NULL, 0);
			D3DGC->D11_deviceContext->Dispatch(D3DGC->ScreenWidth, D3DGC->ScreenHeight / 96, 1);

			D3DGC->D11_deviceContext->CopyResource(MyresDest, D3DGC->BackBuffer_CopyResolveTexture);
			++c;
		} while (c < BlurAmont);

		D3DGC->D11_deviceContext->CopyResource(D3DGC->BackBuffer2DT, D3DGC->BackBuffer_CopyResolveTexture);

		MyresDest->Release();
	}

	
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	D3DGC->D11_deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	D3DGC->D11_deviceContext->CSSetShaderResources(0, 1, nullSRV);
	//  --------------------------    BLUR      ----------------------------------
}


// Отображение текстуры
/*
D3DGC->D11_deviceContext->CopyResource(D3DGC->ScreenShootTexture, D3DGC->backBufferPtr);
union _block
{
byte  *data;
unsigned int *udata;
} block;

D3D11_MAPPED_SUBRESOURCE  mapResource;
hr = D3DGC->D11_deviceContext->Map(D3DGC->ScreenShootTexture, 0, D3D11_MAP_READ, NULL, &mapResource);

unsigned int amount = mapResource.DepthPitch; // RowPitch * Height;
block.data = new byte[amount];
memcpy(block.data, mapResource.pData, amount);
D3DGC->D11_deviceContext->Unmap(D3DGC->ScreenShootTexture, 0);
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

