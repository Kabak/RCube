#include "stdafx.h"
#include "KFTextureManeger.h"

KFTextureManeger::KFTextureManeger(){
	TextInit = false ;
	g_device = nullptr;
	g_deviceContext = nullptr;
}

HRESULT KFTextureManeger::Init(HWND hwnd , float TextureWidth , float TextureHaigth, ID3D11Device* device, 
		float ScreenWidth , float ScreenHeigth , int ScreenPosX , int ScreenPosY , ID3D11DeviceContext* deviceContext
		, ID3D11RenderTargetView *ActiveRenderTargetView , ID3D11BlendState * ActiveBlendState
		, ID3D11ShaderResourceView * texture){

			TextcordWidht = TextureWidth ;
			TextcordHeight = TextureHaigth ;

	g_ActiveRenderTargetView = ActiveRenderTargetView ;
	stride = sizeof( Vertex );
	offset = 0;
	g_BlendState = ActiveBlendState ;

			g_device = device ;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_BLEND_DESC blendStateDescription;

	g_hwnd = hwnd ;
	g_ScereenWidth = ScreenWidth ;
	g_ScreenHeight = ScreenHeigth ;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = (UINT)TextureWidth;
	textureDesc.Height = (UINT)TextureHaigth;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"KFTextureManegerClass. ошибка в создании m_renderTargetTexture", L"Error", MB_OK);
		return E_FAIL;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"KFTextureManegerClass. ошибка в создании рендер таргета", L"Error", MB_OK);
		return E_FAIL;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"KFTextureManegerClass. ошибка в создании шейдер ресурсов", L"Error", MB_OK);
		return E_FAIL;
	}


	BacroundTexture = new SquareObjectClass ;
    OnTexture = new SquareObjectClass ;
	DrawingTexture = new SquareObjectClass ;
	AnimeTextures = new SquareObjectClass ;

	BacroundTexture->Init(hwnd , (int)ScreenWidth , (int)ScreenHeigth , ScreenPosX , ScreenPosY , 0.0f , 0.0f , TextureWidth , TextureHaigth , device 
		, texture, deviceContext);
	OnTexture->Init(hwnd , (int)ScreenWidth , (int)ScreenHeigth , ScreenPosX , ScreenPosY , 0.0f , 0.0f , TextureWidth , TextureHaigth , device 
		, texture, deviceContext);
	DrawingTexture->Init(hwnd , (int)ScreenWidth , (int)ScreenHeigth , ScreenPosX , ScreenPosY , 0.0f , 0.0f , TextureWidth , TextureHaigth , device 
		, texture, deviceContext);
	AnimeTextures->Init(hwnd , (int)ScreenWidth , (int)ScreenHeigth , ScreenPosX , ScreenPosY , 0.0f , 0.0f , TextureWidth , TextureHaigth , device 
		, texture, deviceContext);

	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

// To create an alpha enabled blend state description change BlendEnable to TRUE and DestBlend to D3D11_BLEND_INV_SRC_ALPHA. The other settings are set to their default values which can be 
// looked up in the Windows DirectX Graphics Documentation.

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

// We then create an alpha enabled blending state using the description we just setup.

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if(FAILED(result))
	{
		return E_FAIL;
	}

// Now to create an alpha disabled state we change the description we just made to set BlendEnable to FALSE. The rest of the settings can be left as they are.

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

// We then create an alpha disabled blending state using the modified blend state description. We now have two blending states we can switch between to turn on and off alpha blending.

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if(FAILED(result))
	{
		return E_FAIL;
	}


	g_deviceContext = deviceContext ;


	return S_OK ;

}

void KFTextureManeger::DrawTextureOnTextute(ID3D11ShaderResourceView * FirstTexture , ID3D11ShaderResourceView * SecondTexture 
											, float SecondTexturePosX , float SecondTexturePosY , float Width , float Height
											, XMMATRIX & OrthoMatix){
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	g_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);


	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 0.0f;

	g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	// Clear the back buffer.
	g_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	BacroundTexture->SetObjectTexture(FirstTexture) ;

    OnTexture->SetObjectTexture(SecondTexture) ;
	

	OnTexture->x = SecondTexturePosX ;
	OnTexture->y = SecondTexturePosY ;
	OnTexture->_Width = Width ;
	OnTexture->_Heigth = Height ;

	BacroundTexture->Draw( OrthoMatix) ;
	OnTexture->Draw(OrthoMatix) ;

	g_deviceContext->OMSetRenderTargets(1, &g_ActiveRenderTargetView, NULL);

	
	// Turn off the alpha blending.
	g_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, NULL, 0xffffffff);

}

KFTextureManeger::~KFTextureManeger(){

	if (g_device != nullptr) g_device->Release();
	if (g_deviceContext = nullptr) g_deviceContext->Release();


		if(m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

	if (BacroundTexture) delete BacroundTexture ;
	if (OnTexture)delete OnTexture ;
	if (DrawingTexture) delete DrawingTexture ;
	if(TextInit == true ){
	D2DRenderTarget->Release();	
	Brush->Release();
	TextFormat->Release();
	Transparency->Release();
	DWriteFactory->Release();
	d2dVertBuffer->Release();
	d2dIndexBuffer->Release();
	cbPerObjectBuffer->Release();
	d2dTexture->Release();
	g_pTextVertexLayout->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	g_pSampleType->Release();
	CCWcullMode->Release();
	CWcullMode->Release();
	d3d101Device->Release();
	sharedTex11->Release();
	keyedMutex11->Release();
	keyedMutex10->Release();
	}
	if (TextcordTop) 
		delete [] TextcordTop ;
	if (TextcordLeft) 
		delete [] TextcordLeft ;
	if(m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if(m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}
}

void KFTextureManeger::Render(float PosX , float PosY, XMMATRIX & OrthoMatix){
	if (!DrawingTexture)return ;
	g_deviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);
	DrawingTexture->SetObjectTexture(d2dTexture) ;

	DrawingTexture->x = PosX ;
	DrawingTexture->y = PosY ;

	DrawingTexture->SetObjectTexture(m_shaderResourceView) ;
	DrawingTexture->Draw( OrthoMatix) ;

	g_deviceContext->OMSetBlendState(g_BlendState, 0, 0xffffffff);
    
}

void KFTextureManeger::PreparationToAnimate(int NumVerse , int NumColumniation , ID3D11ShaderResourceView * animTexture ){
	float OneFrameWidht , OneFrameHeight , Schot ;

	OneFrameWidht = 1.0f / NumColumniation ;
	OneFrameHeight = 1.0f / NumVerse ;

	TextcordWidht = OneFrameWidht ;
	TextcordHeight = OneFrameHeight ;

	NumOfAnimeFrams = NumVerse * NumColumniation ;

//	AnimeTextures->SetObjectTexture(TextureFileName ) ;

	TextcordTop = new float[NumOfAnimeFrams ] ;
	TextcordLeft = new float[NumOfAnimeFrams ] ;

	int c = 0 , a = 0 , NumOfZapolnElem = 0  ;

	while (c < NumVerse)
	{
		Schot = c * OneFrameHeight ;
		while (a < NumColumniation){
          TextcordLeft[NumOfZapolnElem] = a * OneFrameWidht ;
		  TextcordTop[NumOfZapolnElem] = Schot ;
		  NumOfZapolnElem++ ;
			a++ ;
		}
		a = 0 ;
		c++ ;
	}

	float h = TextcordTop[0] , i =  TextcordLeft[0] ;
	AnimeTextures->SetObjectTexture(animTexture);
}

void KFTextureManeger::DrawAnimate(int NumOfDrawingFrame, XMMATRIX & OrthoMatix){
    float color[4];

	// Setup the color to clear the buffer to.
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 0.0f;

	g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	// Clear the back buffer.
	g_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	AnimeTextures->ResetObjectTexcord(TextcordTop[NumOfDrawingFrame] , TextcordLeft[NumOfDrawingFrame] , TextcordTop[NumOfDrawingFrame] + TextcordHeight , TextcordLeft[NumOfDrawingFrame] + TextcordWidht) ;
	AnimeTextures->Draw( OrthoMatix) ;

	g_deviceContext->OMSetRenderTargets(1, &g_ActiveRenderTargetView, NULL);
}

void KFTextureManeger::DrawTextOnTexture(ID3D11ShaderResourceView * texture, float TextPosX , float TextPosY , wchar_t &Text, XMMATRIX & OrthoMatix){
	float color[4];

	Render(20 , 30 , OrthoMatix) ;


	// Setup the color to clear the buffer to.
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 0.0f;

	//	ID3D11ShaderResourceView *const pSRV[1] = { NULL };
	//	g_deviceContext->PSSetShaderResources(0, 1, pSRV);
/*	g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL);

	// Clear the back buffer.
	g_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	
	if (BacroundTexture){
		BacroundTexture->SetObjectTexture(texture);
	}

	if (BacroundTexture){

	BacroundTexture->Draw( OrthoMatix ) ;
	}
	
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
	
	int x = (int)(TextPosX * TextcordWidht) ;
	int y = (int)(TextPosX * TextcordHeight) ;

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(x , y , g_ScereenWidth, g_ScreenHeight);
	//Draw the Text
	D2DRenderTarget->DrawText(
		&Text,
		wcslen(&Text),
		TextFormat,
		layoutRect,
		Brush
		);

	D2DRenderTarget->EndDraw();	

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync(1);

	//Use the D3D11 Device
	keyedMutex11->AcquireSync(1, 5);

	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Set the blend state for D2D render target texture objects
	g_deviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	g_deviceContext->IASetIndexBuffer( d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer

	g_deviceContext->IASetVertexBuffers( 0, 1, &d2dVertBuffer, &stride, &offset );

//	WVP =  XMMatrixIdentity();
//	WVP *= XMMatrixScaling(0.5f,0.5f,0.5f);
//	WVP = XMMatrixRotationX(Trans_z);// * XMMatrixRotationX(Trans_z);
//	cbPerObj.WVP = XMMatrixTranspose(XMMatrixIdentity());	
	g_deviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
// Устанавливает матрицу к которой привязан объект
	g_deviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
// Устанавливает текстуру которую нужно выводить
	g_deviceContext->PSSetShaderResources( 0, 1, &d2dTexture );
	g_deviceContext->IASetInputLayout( g_pTextVertexLayout );
	g_deviceContext->PSSetSamplers( 0, 1, &g_pSampleType );
	g_deviceContext->VSSetShader(VS, 0, 0);
	g_deviceContext->PSSetShader(PS, 0, 0);

	g_deviceContext->RSSetState(CWcullMode);

	g_deviceContext->DrawIndexed( 6, 0, 0 );

	g_deviceContext->OMSetBlendState(g_BlendState, 0, 0xffffffff);
	
	g_deviceContext->OMSetRenderTargets(1, &g_ActiveRenderTargetView, NULL);*/
}


void KFTextureManeger::PreparationToTextRendering(IDXGIAdapter1 *Adapter,wchar_t *FontName,UINT FontSize,XMFLOAT4 &color){
	TextInit = true ;
		bool result = false;
// Инициализация для Render
	stride = sizeof( Vertex );
	offset = 0;
	cbPerObj.WVP = XMMatrixTranspose(XMMatrixIdentity());
// перемещаем текст по экрану
	cbPerObj.WVP = XMMatrixTranslation(0.0f,0.0f,0.0f);
// -------------------------
//	m_ActiveDevice = g_pd3dDevice;

	FontDrawColor.x = color.x ;
	FontDrawColor.y = color.y ;
	FontDrawColor.z = color.z ;
	FontDrawColor.w = color.w ;

	//XMStoreFloat4A(&FontDrawColor,color);
	FontSize = FontSize;

	result = InitD2D_D3D101_DWrite(g_hwnd , Adapter, (UINT)g_ScereenWidth , (UINT)g_ScreenHeight, g_device, FontName, FontSize, FontDrawColor);
	if(!result)
	{
		MessageBox(g_hwnd, L"Could not initialize InitD2D_D3D101_DWrite.", L"Error", MB_OK);
		return ;
	}
	result = InitD2DScreenTexture(g_device);
	if(!result)
	{
		MessageBox(g_hwnd, L"Could not initialize InitD2DScreenTexture.", L"Error", MB_OK);
		return ;
	}

}

bool KFTextureManeger::InitD2D_D3D101_DWrite(HWND hwnd, IDXGIAdapter1 *Adapter, UINT width, UINT height, ID3D11Device *g_pd3dDevice,wchar_t *FontName, UINT FontSize, XMFLOAT4A &FontColor)
{

	//Create our Direc3D 10.1 Device
	HRESULT hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL,D3D10_CREATE_DEVICE_BGRA_SUPPORT,	D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &d3d101Device);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize D3D10CreateDevice1.", L"Error", MB_OK);
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
		return false;
	}
	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedTex11->QueryInterfac.", L"Error", MB_OK);
		return false;
	}
	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;	

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedTex11->QueryInterface.", L"Error", MB_OK);
		return false;
	}
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedResource10->GetSharedHandle.", L"Error", MB_OK);
		return false;
	}
	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1
	IDXGISurface1 *sharedSurface10;	

	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize d3d101Device->OpenSharedResource.", L"Error", MB_OK);
		return false;
	}
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize sharedSurface10->QueryInterface.", L"Error", MB_OK);
		return false;
	}

	// Create D2D factory
	ID2D1Factory *D2DFactory;	
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);	
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize D2D1CreateFactory.", L"Error", MB_OK);
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
		return false;
	}

	sharedSurface10->Release();
	D2DFactory->Release();	

	// Create a solid color brush to draw something with		
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(FontColor.x,FontColor.y,FontColor.z,FontColor.w), &Brush);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize D2DRenderTarget->CreateSolidColorBrush.", L"Error", MB_OK);
		return false;
	}

	// DirectWrite
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&DWriteFactory));
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize DWriteCreateFactory.", L"Error", MB_OK);
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
		return false;
	}
	hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize TextFormat->SetTextAlignment.", L"Error", MB_OK);
		return false;
	}
	hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	if(FAILED(hr))
	{
		MessageBox(hwnd, L"Could not initialize TextFormat->SetParagraphAlignment.", L"Error", MB_OK);
		return false;
	}
	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);	

	return true;
}

bool KFTextureManeger::InitD2DScreenTexture(ID3D11Device *g_pd3dDevice)
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
	HRESULT hr = g_pd3dDevice->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);

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
	g_deviceContext->IASetInputLayout( g_pTextVertexLayout );
// -----------------------------------------------------------------------------------------------

	hr = g_pd3dDevice->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

	//Set Vertex and Pixel Shaders
	g_deviceContext->VSSetShader(VS, 0, 0);
	g_deviceContext->PSSetShader(PS, 0, 0);

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

	blendDesc.AlphaToCoverageEnable = true;
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

ID3D11ShaderResourceView* KFTextureManeger::GetTexture(){
	return m_shaderResourceView ;
}

HRESULT KFTextureManeger::SetTextureSize(float width , float height){


	BacroundTexture->_Width = width ;
	BacroundTexture->_Heigth = height ;
	OnTexture->_Width = width ;
	OnTexture->_Heigth = height ;
	DrawingTexture->_Width = width ;
	DrawingTexture->_Heigth = height ;
	AnimeTextures->_Width = width ;
	AnimeTextures->_Heigth = height ;

	return S_OK ;
}

void KFTextureManeger::ResSetWindowParam(int ScreenWidth , int ScreenHeigth , int WindowPosX , int WindowPosY){

	BacroundTexture->ResetWindowParam(WindowPosX , WindowPosY , ScreenWidth ,ScreenHeigth) ;
	OnTexture->ResetWindowParam(WindowPosX , WindowPosY , ScreenWidth ,ScreenHeigth) ;
	DrawingTexture->ResetWindowParam(WindowPosX , WindowPosY , ScreenWidth ,ScreenHeigth) ;
	AnimeTextures->ResetWindowParam(WindowPosX , WindowPosY , ScreenWidth ,ScreenHeigth) ;

}