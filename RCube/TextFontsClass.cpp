#include "stdafx.h"

#include "TextFontsClass.h"

TextFonts::TextFonts(){

//	ScreenWidth = 800;
//	ScreenHeight = 600;

	printText = new wchar_t[1024];
	Numbertext = new wchar_t[64];
	FloatNumber = new char[64];

	ScreenWidth = 150;
	ScreenHeight = 600;

					VS = nullptr;
					PS = nullptr;
			 VS_Buffer = nullptr;
			 PS_Buffer = nullptr;
   g_pTextVertexLayout = nullptr;
		 g_pSampleType = nullptr;
		  d3d101Device = nullptr;
 m_ActiveDeviceContext = nullptr;
		  keyedMutex11 = nullptr;
		  keyedMutex10 = nullptr;
	   D2DRenderTarget = nullptr;
				 Brush = nullptr;
		   sharedTex11 = nullptr;
		 DWriteFactory = nullptr;
			TextFormat = nullptr;
			d2dTexture = nullptr;
		 d2dVertBuffer = nullptr;
		d2dIndexBuffer = nullptr;
	 cbPerObjectBuffer = nullptr;
		  Transparency = nullptr;
		   CCWcullMode = nullptr;
			CWcullMode = nullptr;
}

TextFonts::~TextFonts()
{
	delete [] printText;
	delete [] Numbertext;
	delete [] FloatNumber;
}

bool TextFonts::Initialize(HWND hwnd, D3DGlobalContext* D3DGC, wchar_t *FontName,UINT FontSize,XMVECTOR &color)
{
	bool result = false;
// Инициализация для Render
	stride = sizeof( Vertex );
	offset = 0;
	cbPerObj.WVP = XMMatrixTranspose(XMMatrixIdentity());
// перемещаем текст по экрану
	cbPerObj.WVP = XMMatrixTranslation(0.0f,0.0f,0.0f);
// -------------------------
	m_ActiveDeviceContext = D3DGC->m_deviceContext;
//	m_ActiveDevice = g_pd3dDevice;
	ScreenWidth = D3DGC->ScreenWidth;
	ScreenHeight = D3DGC->ScreenHeight;
	XMStoreFloat4A(&FontDrawColor,color);
	FontSize = FontSize;

	result = InitD2D_D3D101_DWrite(hwnd, D3DGC->Adapter, ScreenWidth, ScreenHeight, D3DGC->m_device, FontName, FontSize, FontDrawColor);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize InitD2D_D3D101_DWrite.", L"Error", MB_OK);
		return false;
	}
	result = InitD2DScreenTexture(D3DGC->m_device);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize InitD2DScreenTexture.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool TextFonts::InitD2D_D3D101_DWrite(HWND hwnd, IDXGIAdapter1 *Adapter, UINT width, UINT height, ID3D11Device *g_pd3dDevice,wchar_t *FontName, UINT FontSize, XMFLOAT4A &FontColor)
{

	//Create our Direc3D 10.1 Device
	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL,D3D10_CREATE_DEVICE_BGRA_SUPPORT,	D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &d3d101Device);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize D3D10CreateDevice1.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	//Create Shared Texture that Direct3D 10.1 will render on
	D3D11_TEXTURE2D_DESC sharedTexDesc;	

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = width;
	sharedTexDesc.Height = height;	
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;	
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;	
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;	

	hr = g_pd3dDevice->CreateTexture2D(&sharedTexDesc, NULL, &sharedTex11);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize CreateTexture2D.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedTex11->QueryInterfac.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;	

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedTex11->QueryInterface.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedResource10->GetSharedHandle.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1
	IDXGISurface1 *sharedSurface10;	

	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize d3d101Device->OpenSharedResource.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedSurface10->QueryInterface.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	// Create D2D factory
	ID2D1Factory *D2DFactory;	
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize D2D1CreateFactory.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);	

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize D2DFactory->CreateDxgiSurfaceRenderTarget.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	sharedSurface10->Release();
	D2DFactory->Release();	

	// Create a solid color brush to draw something with		
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(FontColor.x,FontColor.y,FontColor.z,FontColor.w), &Brush);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize D2DRenderTarget->CreateSolidColorBrush.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	// DirectWrite
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&DWriteFactory));
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize DWriteCreateFactory.", L"Error", MB_OK);
		Shutdown();
		return false;
	}

	hr = DWriteFactory->CreateTextFormat(
		FontName,
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		(float)FontSize,
		L"en-us",
		&TextFormat
		);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize DWriteFactory->CreateTextFormat.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize TextFormat->SetTextAlignment.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize TextFormat->SetParagraphAlignment.", L"Error", MB_OK);
		Shutdown();
		return false;
	}
	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);	

	return true;
}

bool TextFonts::InitD2DScreenTexture(ID3D11Device *g_pd3dDevice)
{
	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	};

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	g_pd3dDevice->CreateBuffer(&indexBufferDesc, &iinitData, &d2dIndexBuffer);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = v;
	hr = g_pd3dDevice->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);

	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	g_pd3dDevice->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);

	hr = D3DCompileFromFile(L"Shaders/Text_vs.hlsl", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, 0);
	hr = D3DCompileFromFile(L"Shaders/Text_ps.hlsl", 0, 0, "PS", "ps_5_0", 0, 0, &PS_Buffer, 0);

	//Create the Shader Objects
	hr = g_pd3dDevice->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);

// ++++++++++++++++++++++          Create the input layout          +++++++++++++++++++++++++++++
D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
};
	numElements = ARRAYSIZE(layout);

	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(),
                                          VS_Buffer->GetBufferSize(), &g_pTextVertexLayout );
    if( FAILED( hr ) ){

        return false;
	}
    // Set the input layout
	m_ActiveDeviceContext->IASetInputLayout( g_pTextVertexLayout );
// -----------------------------------------------------------------------------------------------

	hr = g_pd3dDevice->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

	//Set Vertex and Pixel Shaders
	m_ActiveDeviceContext->VSSetShader(VS, 0, 0);
	m_ActiveDeviceContext->PSSetShader(PS, 0, 0);

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSampleType );
	if(FAILED(hr))
	{
		return false;
	}

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );

	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	///////////////**************new**************////////////////////
	rtbd.DestBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
	///////////////**************new**************////////////////////
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	hr = g_pd3dDevice->CreateBlendState(&blendDesc, &Transparency);
	if(FAILED(hr))
	{
		return false;
	}

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	hr = g_pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);
	if(FAILED(hr))
	{
		return false;
	}

	cmdesc.FrontCounterClockwise = false;

	hr = g_pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);
	if(FAILED(hr))
	{
		return false;
	}

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = g_pd3dDevice->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
	if(FAILED(hr))
	{
		return false;
	}

	return true;
}

void TextFonts::RenderXMVector(wchar_t *text, XMVECTOR& Vector)
{
	XMFLOAT4A MyFloat;	
	  size_t cSize;
	  size_t size;

	XMStoreFloat4A(&MyFloat,Vector);
	
	wcscpy_s(printText,1024,text);
	wcscat_s ( printText,1024,L"Vector: x ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.x);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Vector: y ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.y);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Vector: z ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.z);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Vector: w ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.w);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	RenderText( *printText );
}

void TextFonts::RenderXMatrix(wchar_t *text, XMMATRIX& Matrix)
{
XMVECTOR Scaling = XMVectorSet(0.0f,0.0f,0.0f,0.0f),
		 Rotation = XMVectorSet(0.0f,0.0f,0.0f,0.0f),
		 Translation = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
XMFLOAT4A MyFloat;	
	  size_t cSize;
	  size_t size;


XMMatrixDecompose(&Scaling,&Rotation,&Translation, Matrix);

	XMStoreFloat4A(&MyFloat,Scaling);
	
	wcscpy_s(printText,1024,text);
	wcscat_s ( printText,1024,L"Scaling: x ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.x);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Scaling: y ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.y);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Scaling: z ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.z);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");
// =========================================
	XMStoreFloat4A(&MyFloat,Rotation);
	wcscat_s ( printText,1024,L"Rotation: x ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.x);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Rotation: y ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.y);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Rotation: z ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.z);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");
// --------------------------------------------------
	XMStoreFloat4A(&MyFloat,Translation);
	wcscat_s ( printText,1024,L"Translation: x ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.x);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Translation: y ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.y);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	wcscat_s ( printText,1024,L"Translation: z ");
	sprintf_s(FloatNumber,64,"%2.7lf",MyFloat.z);

	cSize = strlen(FloatNumber)+1;
    size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);
	wcscat_s ( printText,1024,Numbertext );
	wcscat_s ( printText,1024,L"\n");

	RenderText( *printText );

}

void TextFonts::RenderFloat (wchar_t *text, float Number)
{
// все равно максимальная точность на Intel платформе 17 знаков
	sprintf_s(FloatNumber,64,"%2.7lf",Number);
    const size_t cSize = strlen(FloatNumber)+1;
    size_t size = wcslen(Numbertext)+1;
	mbstowcs_s (&size,Numbertext, 64, FloatNumber, cSize);

	wcscpy_s(printText,1024,text);
	wcscat_s ( printText,1024,Numbertext );
	RenderText( *printText );
}

void TextFonts::RenderInt (wchar_t *text, int Number)
{
	wcscpy_s(printText,1024,text);
	_itow_s ( Number,Numbertext,64,10 );
	wcscat_s ( printText,1024,Numbertext );
	RenderText( *printText );
}

void TextFonts::SetRenderDimantions(UINT Width, UINT Height)
{
ScreenWidth = Width;
ScreenHeight = Height;
}

void TextFonts::Shutdown ()
{
	D3DSAFE_RELEASE(VS);
	D3DSAFE_RELEASE(PS);
	D3DSAFE_RELEASE(VS_Buffer);
	D3DSAFE_RELEASE(PS_Buffer);
	D3DSAFE_RELEASE(g_pTextVertexLayout);
	D3DSAFE_RELEASE(g_pSampleType);
	D3DSAFE_RELEASE(d3d101Device);
	D3DSAFE_RELEASE(m_ActiveDeviceContext);
	D3DSAFE_RELEASE(keyedMutex11);
	D3DSAFE_RELEASE(keyedMutex10);
	D3DSAFE_RELEASE(D2DRenderTarget);
	D3DSAFE_RELEASE(Brush);
	D3DSAFE_RELEASE(sharedTex11);
	D3DSAFE_RELEASE(DWriteFactory);
	D3DSAFE_RELEASE(TextFormat);
	D3DSAFE_RELEASE(d2dTexture);
	D3DSAFE_RELEASE(d2dVertBuffer);
	D3DSAFE_RELEASE(d2dIndexBuffer);
	D3DSAFE_RELEASE(cbPerObjectBuffer);
	D3DSAFE_RELEASE(Transparency);
	D3DSAFE_RELEASE(CCWcullMode);
	D3DSAFE_RELEASE(CWcullMode);
}
// bool нужен для очистки буфера
void TextFonts::RenderText(wchar_t &text)
	///////////////**************new**************////////////////////
{
	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync(0);

	//Use D3D10.1 device
	keyedMutex10->AcquireSync(0, 5);			

	//Draw D2D content		
	D2DRenderTarget->BeginDraw();	

	//Clear D2D Background
	D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(FontDrawColor.x,FontDrawColor.y,FontDrawColor.z,FontDrawColor.w);

	//Set the brush color D2D will use to draw with
	Brush->SetColor(FontColor);	

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, (float)ScreenWidth,(float)ScreenHeight);

	//Draw the Text
	D2DRenderTarget->DrawText(
		&text,
		(ULONG)wcslen(&text),
		TextFormat,
		layoutRect,
		Brush
		);

	D2DRenderTarget->EndDraw();	

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync(1);

	//Use the D3D11 Device
	keyedMutex11->AcquireSync(1,5);

	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	m_ActiveDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Set the blend state for D2D render target texture objects
	//m_ActiveDeviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);
	
	//Set the d2d Index buffer
	m_ActiveDeviceContext->IASetIndexBuffer( d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	
	m_ActiveDeviceContext->IASetVertexBuffers( 0, 1, &d2dVertBuffer, &stride, &offset );
	
//	WVP =  XMMatrixIdentity();
//	WVP *= XMMatrixScaling(0.5f,0.5f,0.5f);
//	WVP = XMMatrixRotationX(Trans_z);// * XMMatrixRotationX(Trans_z);
//	cbPerObj.WVP = XMMatrixTranspose(XMMatrixIdentity());
	m_ActiveDeviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
// Устанавливает матрицу к которой привязан объект
	m_ActiveDeviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
// Устанавливает текстуру которую нужно выводить
	m_ActiveDeviceContext->PSSetShaderResources( 0, 1, &d2dTexture );
	m_ActiveDeviceContext->IASetInputLayout( g_pTextVertexLayout );
	m_ActiveDeviceContext->PSSetSamplers( 0, 1, &g_pSampleType );
	m_ActiveDeviceContext->VSSetShader(VS, 0, 0);
	m_ActiveDeviceContext->PSSetShader(PS, 0, 0);

	m_ActiveDeviceContext->RSSetState(CWcullMode);
	
	m_ActiveDeviceContext->DrawIndexed( 6, 0, 0 );

	//m_ActiveDeviceContext->OMSetBlendState(0, 0, 0xffffffff);
}
