#include "stdafx.h"

// I have kept this class entirely empty for now as we are just building the framework for this tutorial.
////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsClass.h"

void __stdcall Duck()
{
	int a = 1;
}

GraphicsClass::GraphicsClass()
{

					   m_D3D = nullptr;
					 Profile = nullptr;
				   m_Frustum = nullptr;
					m_Camera = nullptr;
				   MyManager = nullptr;
					m_Bitmap = nullptr;
					 Mapping = nullptr;

					MainMenu = nullptr;
				 AnimTexture = nullptr;
						 Hud = nullptr;

// + Particel systems
			   SnowParticles = nullptr;
				   TorchFire = nullptr;
			   FireParticles = nullptr;
// - Particel systems


				   KFTerrain = nullptr;
				   ModelList = nullptr;
//				  ClusterMap = nullptr;
				  ShadowWork = nullptr;
				  RCubeRender = nullptr;

			Global = new  char[260];
			Global[0] = NULL;

			Str1   = new  char[STRMAXLENGTH];

	   CamLook.Vec = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0 );

		  TempVal1 = 0.0f;
		  TempVal2 = 0.0f;

		      time = 0.0f;
		  rotation = 0.0f;

 ShadowFrameCounts = 0;

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}


GraphicsClass::~GraphicsClass()
{

//	RCUBE_DELETE ( TempFont )

	delete [] Global;
	delete [] Str1;

	UINT i = 0;
	UINT j = (UINT) FontList.size();
	while ( i < j )
	{
		RCUBE_DELETE( FontList[i] );
		++i;
	};

	FontList.clear();

	Strings.clear();

	RCUBE_DELETE ( RCubeRender );
	RCUBE_DELETE ( ShadowWork );
//    RCUBE_DELETE ( ClusterMap );

	RCUBE_DELETE ( ModelList );
	RCUBE_DELETE ( KFTerrain );
  RCUBE_SHUTDOWN ( FireParticles );
  RCUBE_SHUTDOWN ( TorchFire );
  RCUBE_SHUTDOWN ( SnowParticles );
	RCUBE_DELETE ( Hud );
	RCUBE_DELETE ( AnimTexture );
	RCUBE_DELETE ( MainMenu );
	RCUBE_DELETE ( Mapping );
	RCUBE_DELETE ( m_Bitmap );
	RCUBE_DELETE ( MyManager );
    RCUBE_DELETE ( m_Camera );
	RCUBE_DELETE ( m_Frustum );
	RCUBE_DELETE ( Profile );
  RCUBE_SHUTDOWN ( m_D3D );

}


D3DGlobalContext *GraphicsClass::GetD3DGC()
{
	return m_D3D->D3DGC;
}


bool GraphicsClass::Initialize(HWND hwnd , int& _screenWidth, int& _screenHeight, int& WindowPosX, int& WindowPosY, InputClass* _Input )
{
	bool result;
//	XMMATRIX baseViewMatrix;

	const WCHAR *Error = L"Graphics Error";

	screenWidth = _screenWidth;
	screenHeight = _screenHeight;

	g_WindowPosX = WindowPosX ;
	g_WindowPosY = WindowPosY ;

// Проверка выравнивания
// http://stackoverflow.com/questions/22133742/dx11-crash-when-accessing-xmmatrix
//	siz = __alignof(D3DClass);

	Profile = new RCubeProfiling;
	Profile->Initialize ();

	m_Frustum = new FrustumClass;

	m_D3D = new D3DClass;

	result = m_D3D->Initialize(hwnd, screenWidth, screenHeight, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, m_Frustum );
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize m_D3D Class", Error, MB_OK);
		return false;
	}

	// Устанавливаем Samplers
	ID3D11SamplerState* AllSamplers[4] =
	{
		m_D3D->D3DGC->Wrap_Model_Texture,			// Most models sampler
		m_D3D->D3DGC->CLight_ShadowMap_Sampler,		// ShadowMap sampler
		m_D3D->D3DGC->CLight_SM_PCF_Sampler,		// ShadowMap PCF sampler , FXAA
		m_D3D->D3DGC->FlatObject_Sampler			// Flat Object sampler ( CubeMap, Text, Menu Items, All Particles  )
	};
	m_D3D->D3DGC->DX_deviceContext->PSSetSamplers ( 0, 4, AllSamplers );

	// Инициализируем глобальную ссылку на InputClass 
	m_D3D->D3DGC->m_EngineInputClass = _Input;
	// Инициализируем глобальные размеры окна и положение окна не экране
	m_D3D->D3DGC->ScreenWidth = _screenWidth;
	m_D3D->D3DGC->ScreenHeight = _screenHeight;
	m_D3D->D3DGC->WindowsPosX = WindowPosX;
	m_D3D->D3DGC->WindowsPosY = WindowPosY;

	// Create the camera object.
	m_Camera = new CameraClass;

	m_Camera->GetViewMatrix(m_D3D->D3DGC->ViewMatrix);
	m_Camera->FirstPersonCam = false;
	m_Camera->SetPosition( -3.05f, 14.25f, -4.97f );
	m_Camera->SetLookAt( -3.16f, 13.35f, -5.35f );
	m_Camera->Render();

	MyManager = new ResourceManager;
	MyManager->Init(m_D3D->D3DGC);

// Грузим ресурсы
	hr = MyManager->InitTextures(L"Textures/Result.kaf");
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"MyManager load \"Textures/Result.kaf\" failed!", Error, 0 );
		return false;
	}
	hr = MyManager->InitShaders(L"Shaders/Shaders.kaf");
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"MyManager load \"Shaders/Shaders.kaf\" failed!", Error, 0 );
		return false;
	}

	m_D3D->FXAAShaderIndex = MyManager->GetShaderIndexByName(L"FXAA");
	m_D3D->BlureHorizComputeShaderIndex = MyManager->GetShaderIndexByName(L"Horiz");
	m_D3D->BlureVertComputeShaderIndex = MyManager->GetShaderIndexByName(L"Vert");

// +++++++++++++++++++++     СОЗДАЁМ ШРИФТЫ     +++++++++++++++++++++++++++++++++++
// Максимальный размер шрифта 74.0f 
// Максимальный Outline 10
	FontOnTextureData					 *FontOnTexture;

// +++++++++++++++++++++     Создаём 1 - й шрифт  0 - Й   ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush1 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline1 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops1 = nullptr;
	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 36.0f;			// Размер шрифта
	wchar_t * Font1 = L"Fonts/BuxtonSketch.ttf";// Файл шрифта TTF
	FontOnTexture->FontName = Font1;

	FontOnTexture->Outline = true;				// Будет ли в шрифте окантовка
	FontOnTexture->OutlineWidth = 3.0f;			// Толщина окантовки 0 < 10 

	const int GradientStopAmount1 = 3;
	// Про кисти градиентов
	// https://msdn.microsoft.com/en-us/library/dd368041(v=vs.85).aspx
	D2D1_GRADIENT_STOP gradientStops1[GradientStopAmount1];
	gradientStops1[0].color = D2D1::ColorF( 0x00, 0x00, 0x00, 1.0f );
	gradientStops1[0].position = 0.0f;
	gradientStops1[1].color = D2D1::ColorF( 0xDC, 0x6A, 0x2E, 1.0f );
	gradientStops1[1].position = 0.8f;
	gradientStops1[2].color = D2D1::ColorF( 0x81, 0x20, 0x40, 1.0f );
	gradientStops1[2].position = 1.0f;

	// x,y - start point  z,w - end point
	XMFLOAT4 GradientPos = { 0.0f, -20.0f, 0.0f, 20.0f };
	XMFLOAT4 OutlineColor = { 0.0f, 1.0f, 0.0f, 1.0f };

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateGradientStopCollection(
		gradientStops1,
		GradientStopAmount1,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops1
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GradientStopCollection failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F( GradientPos.x, GradientPos.y ),
		D2D1::Point2F( GradientPos.z, GradientPos.w ) ),
		GradientStops1,
		&LinearGradientBrush1
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF(
		OutlineColor.x,
		OutlineColor.y,
		OutlineColor.z,
		OutlineColor.w ),
		&SolidBrushOutline1
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create SolidColorBrush failed!", Error, 0 );
		result = false;
	}

	FontOnTexture->OutlineBrush = SolidBrushOutline1;
	FontOnTexture->FillBrush = LinearGradientBrush1;

//0
	FontList.push_back( FontOnTexture );
// ---------------------     Создаём 1 - й шрифт      --------------------------------

// +++++++++++++++++++++     Создаём 2 - й шрифт      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush2 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline2 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops2 = nullptr;
	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 36.0f;
	wchar_t *Font2 = L"Fonts/8708_AA_Akashi.ttf"; // 8708_AA_Akashi.ttf // RosewoodStd-Regular.otf
	FontOnTexture->FontName = Font2;

	FontOnTexture->Outline = true;				// Будет ли в шрифте окантовка
	FontOnTexture->OutlineWidth = 6.0f;			// Толщина окантовки 0 < 10 

	const int GradientStopAmount2 = 3;

	D2D1_GRADIENT_STOP gradientStops2[GradientStopAmount2];
	gradientStops2[0].color = D2D1::ColorF( D2D1::ColorF::Turquoise, 1 );
	gradientStops2[0].position = 0.0f;
	gradientStops2[1].color = D2D1::ColorF( D2D1::ColorF::RosyBrown, 1 );
	gradientStops2[1].position = 0.8f;
	gradientStops2[2].color = D2D1::ColorF( D2D1::ColorF::White, 1 );
	gradientStops2[2].position = 1.0f;

	GradientPos = { 0.0f, -20.0f, 0.0f, 0.0f };
	OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateGradientStopCollection(
		gradientStops2,
		GradientStopAmount2,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops2
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GradientStopCollection failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F( GradientPos.x, GradientPos.y ),
		D2D1::Point2F( GradientPos.z, GradientPos.w ) ),
		GradientStops2,
		&LinearGradientBrush2
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF(
		OutlineColor.x,
		OutlineColor.y,
		OutlineColor.z,
		OutlineColor.w ),
		&SolidBrushOutline2
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create SolidColorBrush failed!", Error, 0 );
		result = false;
	}

	FontOnTexture->OutlineBrush = SolidBrushOutline2;
	FontOnTexture->FillBrush = LinearGradientBrush2;
//1
	FontList.push_back( FontOnTexture );

// +++++++++++++++++++++     Создаём 3 - й шрифт      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush3 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline3 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops3 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font3 = L"Fonts/comic.ttf";
	FontOnTexture->FontName = Font3;

	FontOnTexture->Outline = true;				// Будет ли в шрифте окантовка
	FontOnTexture->OutlineWidth = 4.0f;			// Толщина окантовки 0 < 10 

	const int GradientStopAmount3 = 3;
	D2D1_GRADIENT_STOP gradientStops3[GradientStopAmount3];
	gradientStops3[0].color = D2D1::ColorF( D2D1::ColorF::YellowGreen, 1 );
	gradientStops3[0].position = 0.0f;
	gradientStops3[1].color = D2D1::ColorF( D2D1::ColorF::Violet, 1 );
	gradientStops3[1].position = 0.8f;
	gradientStops3[2].color = D2D1::ColorF( D2D1::ColorF::White, 1 );
	gradientStops3[2].position = 1.0f;

	GradientPos = { 0.0f, -10.0f, 0.0f, 0.0f };
	OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateGradientStopCollection(
		gradientStops3,
		GradientStopAmount3,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops3
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GradientStopCollection failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F( GradientPos.x, GradientPos.y ),
		D2D1::Point2F( GradientPos.z, GradientPos.w ) ),
		GradientStops3,
		&LinearGradientBrush3
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF(
		OutlineColor.x,
		OutlineColor.y,
		OutlineColor.z,
		OutlineColor.w ),
		&SolidBrushOutline3
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create SolidColorBrush failed!", Error, 0 );
		result = false;
	}
	FontOnTexture->OutlineBrush = SolidBrushOutline3;
	FontOnTexture->FillBrush = LinearGradientBrush3;
//2
	FontList.push_back( FontOnTexture );

// +++++++++++++++++++++     Создаём 4 - й шрифт      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush4 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline4 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops4 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font4 = L"Fonts/BrushScriptStd.ttf"; //BrushScriptStd.otf
	FontOnTexture->FontName = Font4;
	FontOnTexture->Outline = true;				// Будет ли в шрифте окантовка
	FontOnTexture->OutlineWidth = 3.0f;			// Толщина окантовки 0 < 10 

	const int GradientStopAmount4 = 3;
	D2D1_GRADIENT_STOP gradientStops4[GradientStopAmount4];
	gradientStops4[0].color = D2D1::ColorF( D2D1::ColorF::Plum, 1 );
	gradientStops4[0].position = 0.0f;
	gradientStops4[1].color = D2D1::ColorF( D2D1::ColorF::Chocolate, 1 );
	gradientStops4[1].position = 0.8f;
	gradientStops4[2].color = D2D1::ColorF( D2D1::ColorF::Silver, 1 );
	gradientStops4[2].position = 1.0f;
	
	GradientPos = { 0.0f, -10.0f, 0.0f, 0.0f };
	OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateGradientStopCollection(
		gradientStops4,
		GradientStopAmount4,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops4
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GradientStopCollection failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F( GradientPos.x, GradientPos.y ),
		D2D1::Point2F( GradientPos.z, GradientPos.w ) ),
		GradientStops4,
		&LinearGradientBrush4
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF(
		OutlineColor.x,
		OutlineColor.y,
		OutlineColor.z,
		OutlineColor.w ),
		&SolidBrushOutline4
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create SolidColorBrush failed!", Error, 0 );
		result = false;
	}
	FontOnTexture->OutlineBrush = SolidBrushOutline4;
	FontOnTexture->FillBrush = LinearGradientBrush4;
//3
	FontList.push_back( FontOnTexture );

// +++++++++++++++++++++     Создаём 5 - й шрифт      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush5 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline5 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops5 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font5 = L"Fonts/RAVIE.TTF";
	FontOnTexture->FontName = Font5;

	FontOnTexture->Outline = true;				// Будет ли в шрифте окантовка
	FontOnTexture->OutlineWidth = 2.0f;			// Толщина окантовки 0 < 10 

	const int GradientStopAmount5 = 3;
	D2D1_GRADIENT_STOP gradientStops5[GradientStopAmount5];
	gradientStops5[0].color = D2D1::ColorF( D2D1::ColorF::Khaki, 1 );
	gradientStops5[0].position = 0.0f;
	gradientStops5[1].color = D2D1::ColorF( D2D1::ColorF::Yellow, 1 );
	gradientStops5[1].position = 0.8f;
	gradientStops5[2].color = D2D1::ColorF( D2D1::ColorF::Olive, 1 );
	gradientStops5[2].position = 1.0f;

	GradientPos = { 0.0f, -10.0f, 0.0f, 0.0f };
	OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateGradientStopCollection(
		gradientStops5,
		GradientStopAmount5,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops5
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GradientStopCollection failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F( GradientPos.x, GradientPos.y ),
		D2D1::Point2F( GradientPos.z, GradientPos.w ) ),
		GradientStops5,
		&LinearGradientBrush5
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF(
		OutlineColor.x,
		OutlineColor.y,
		OutlineColor.z,
		OutlineColor.w ),
		&SolidBrushOutline5
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create SolidColorBrush failed!", Error, 0 );
		result = false;
	}
	FontOnTexture->OutlineBrush = SolidBrushOutline5;
	FontOnTexture->FillBrush = LinearGradientBrush5;
//4
	FontList.push_back( FontOnTexture );

// +++++++++++++++++++++     Создаём 6 - й шрифт      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush6 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline6 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops6 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font6 = L"Fonts/times.ttf";
	FontOnTexture->FontName = Font6;
	FontOnTexture->Outline = true;				// Будет ли в шрифте окантовка
	FontOnTexture->OutlineWidth = 2.0f;			// Толщина окантовки 0 < 10 

	const int GradientStopAmount6 = 3;
	D2D1_GRADIENT_STOP gradientStops6[GradientStopAmount6];
	gradientStops6[0].color = D2D1::ColorF( D2D1::ColorF::Beige, 1 );
	gradientStops6[0].position = 0.0f;
	gradientStops6[1].color = D2D1::ColorF( D2D1::ColorF::Brown, 1 );
	gradientStops6[1].position = 0.8f;
	gradientStops6[2].color = D2D1::ColorF( D2D1::ColorF::WhiteSmoke, 1 );
	gradientStops6[2].position = 1.0f;

	GradientPos = { 0.0f, -10.0f, 0.0f, 0.0f };
	OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateGradientStopCollection(
		gradientStops6,
		GradientStopAmount6,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops6
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GradientStopCollection failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F( GradientPos.x, GradientPos.y ),
		D2D1::Point2F( GradientPos.z, GradientPos.w ) ),
		GradientStops6,
		&LinearGradientBrush6
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF(
		OutlineColor.x,
		OutlineColor.y,
		OutlineColor.z,
		OutlineColor.w ),
		&SolidBrushOutline6
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create SolidColorBrush failed!", Error, 0 );
		result = false;
	}
	FontOnTexture->OutlineBrush = SolidBrushOutline6;
	FontOnTexture->FillBrush = LinearGradientBrush6;
//5
	FontList.push_back( FontOnTexture );

	MyManager->TextShaderIndex = MyManager->GetShaderIndexByName ( L"Font" );
	// Инициализируем систему рисования 
	RCubeRender = new RenderClass ( m_D3D->D3DGC, MyManager, m_D3D);

	// Новая система шрифтов
	char* Text = new char [50];
	WCHAR* WText = new WCHAR [50];
	size_t FontsNumber = FontList.size ();
	for (size_t i = 0; i < FontsNumber; ++i)
	{
		MyManager->RCube_Font.push_back ( RCubeRender->CreateRFont ( FontList[i], CharStr ) );
/*
		sprintf_s ( Text, 50, "1MyFont%d.png", (int)i );
		size_t cSize = strlen ( Text );
		size_t Dest= 0;
		mbstowcs_s (&Dest, WText, cSize+1, Text, cSize+1 );
		MyManager->SaveTextureToFile ( MyManager->RCube_Font[i]->FontTextureIndex, WText );
*/
	}

	RCUBE_ARR_DELETE ( Text );
	RCUBE_ARR_DELETE ( WText );



	RCUBE_RELEASE( LinearGradientBrush6 );
	RCUBE_RELEASE( SolidBrushOutline6 );
	RCUBE_RELEASE( GradientStops6 );

	RCUBE_RELEASE( LinearGradientBrush5 );
	RCUBE_RELEASE( SolidBrushOutline5 );
	RCUBE_RELEASE( GradientStops5 );

	RCUBE_RELEASE( LinearGradientBrush4 );
	RCUBE_RELEASE( SolidBrushOutline4 );
	RCUBE_RELEASE( GradientStops4 );

	RCUBE_RELEASE( LinearGradientBrush3 );
	RCUBE_RELEASE( SolidBrushOutline3 );
	RCUBE_RELEASE( GradientStops3 );

	RCUBE_RELEASE( LinearGradientBrush2 );
	RCUBE_RELEASE( SolidBrushOutline2 );
	RCUBE_RELEASE( GradientStops2 );

	RCUBE_RELEASE( LinearGradientBrush1 );
	RCUBE_RELEASE( SolidBrushOutline1 );
	RCUBE_RELEASE( GradientStops1 );

	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", Error, MB_OK);
		return false;
	}

// +++++++++++++++++++++   Custom Cursor   ++++++++++++++++++++++++++++++++++
	XMFLOAT4 MouseCursorPos = {0.0f, 0.0f, 30.0f, 30.0f};
	{
		// Инициализируцем буферы для m_Bitmap
		int TempIndex = MyManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, MyManager->TexturesArr[10]->SRV );
		m_Bitmap = new FlatObjectClass;
		hr = m_Bitmap->Init ( m_D3D->D3DGC->ScreenWidth, m_D3D->D3DGC->ScreenHeight,
			MouseCursorPos,
			MyManager->TexturesArr[10]->SRV,
			NO_FLIP,
			MyManager->Get_Flat_ObjectBuffers_ByIndex ( TempIndex )
		);
		m_Bitmap->BuffersIndex = TempIndex;
	}


	m_Bitmap->ShaderIndex = MyManager->GetShaderIndexByName(L"KF2DObj");

	if (FAILED( hr ))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", Error, MB_OK);
		return false;
	}
// ---------------------   Custom Cursor   ---------------------------------


// ++++++++++++++++++++++     РИСУЕМ ТЕКСТОМ НА ТЕКСТУРЕ     +++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline = nullptr;
	ID2D1GradientStopCollection		  *GradientStops = nullptr;

	FontOnTexture = new FontOnTextureData;
	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// Включаем алиасинг
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;								// Размер шрифта
	wchar_t * Path = L"Fonts/BuxtonSketch.ttf";//9825.otf";// C:\\Windows\\Fonts\\

	FontOnTexture->FontName = Path;

	FontOnTexture->Outline = true;				// Будет ли в шрифте окантовка
	FontOnTexture->OutlineWidth = 3.0f;

	const int GradientStopAmount = 3;

	D2D1_GRADIENT_STOP gradientStops[GradientStopAmount];
	gradientStops[0].color = D2D1::ColorF( 0xf0, 0x00, 0x00, 1 );
	gradientStops[0].position = 0.0f;
	gradientStops[1].color = D2D1::ColorF( 0xDC, 0x6A, 0x2E, 1 );
	gradientStops[1].position = 0.8f;
	gradientStops[2].color = D2D1::ColorF( 0x81, 0x20, 0x40, 1 );
	gradientStops[2].position = 1.0f;

	GradientPos = { 0.0f, -10.0f, 0.0f, 10.0f };
	OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateGradientStopCollection(
		gradientStops,
		GradientStopAmount,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create GradientStopCollection failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F( GradientPos.x, GradientPos.y ),
		D2D1::Point2F( GradientPos.z, GradientPos.w ) ),
		GradientStops,
		&LinearGradientBrush
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create LinearGradientBrush failed!", Error, 0 );
		result = false;
	}

	hr = m_D3D->D3DGC->D2DRenderTarget->CreateSolidColorBrush( D2D1::ColorF(
		OutlineColor.x,
		OutlineColor.y,
		OutlineColor.z,
		OutlineColor.w ),
		&SolidBrushOutline
		);
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create SolidColorBrush failed!", Error, 0 );
		result = false;
	}
	FontOnTexture->OutlineBrush = SolidBrushOutline;
	FontOnTexture->FillBrush = LinearGradientBrush;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++			РИСУЕМ ТЕКСТОМ на ТЕКСТУРЕ        +++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	UpdateConstantBuffer();

	m_D3D->TurnOnTextOnTextureBlending();
	m_D3D->TurnZBufferOff();

	m_D3D->SetCullNoneResterizeState ();
	m_D3D->D3DGC->DX_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_D3D->D3DGC->DX_deviceContext->OMSetRenderTargets( 1, &m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, NULL ); //m_D3D->D3DGC->m_depthStencilView
	m_D3D->D3DGC->DX_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );

//	Profile->StartTimer();

	RCubeRender->RenderFontOnTexture ( MyManager->TexturesArr[1]->SRV,
											 Path,
											 L"Анимация OFF",
											 FontOnTexture
	);

	RCubeRender->RenderFontOnTexture ( MyManager->TexturesArr[7]->SRV,
											 Path,
											 L"FXAA ON",
											 FontOnTexture 
		);

	RCubeRender->RenderFontOnTexture ( MyManager->TexturesArr[6]->SRV,
											 Path,
											 L"Shadows ON",
											 FontOnTexture
		);

	RCubeRender->RenderFontOnTexture ( MyManager->TexturesArr[2]->SRV,
											 Path,
											 L"Анимация ON",
											 FontOnTexture 
	);

	RCubeRender->RenderFontOnTexture( MyManager->TexturesArr[4]->SRV,
											Path,
											L"Soft Shadows",
											FontOnTexture
	);

	RCubeRender->RenderFontOnTexture(MyManager->TexturesArr[3]->SRV,
											Path,
											L"Text OFF",
											FontOnTexture 
	);


//	Profile->StopTimer(Global);

	m_D3D->D3DGC->DX_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();
// ---------------------			РИСУЕМ ТЕКСТОМ на ТЕКСТУРЕ            -------------------------------


	RCUBE_RELEASE( LinearGradientBrush );
	RCUBE_RELEASE( SolidBrushOutline );
	RCUBE_RELEASE( GradientStops );


	delete FontOnTexture;
// ------------------------------    Рисуем текстом на текстурах    -------------------------------



	SENTENCE_INIT_DATA* Data = new SENTENCE_INIT_DATA;
	ZeroMemory(Data, sizeof(SENTENCE_INIT_DATA));
	int Number;
	Data->MaxLength = 128;
	Data->PosX = 100;
	Data->PosY = 90;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 1;
	Data->Level = INGAME_TEXT;
	Number = MyManager->AddSentence ( Data, "FPS" );

	Data->MaxLength = 128;
	Data->PosX = 100;
	Data->PosY = 90;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 5;
	Data->Level = INGAME_TEXT;
	Number = MyManager->AddSentence ( Data, "CPU" );


	Data->MaxLength = 128;
	Data->PosX = 100;
	Data->PosY = 90;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	Data->Level = INGAME_TEXT;
	Number = MyManager->AddSentence(Data, "W - Вперёд, S - Назад");

	Data->MaxLength = 128;
	Data->PosY = 130;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	Number = MyManager->AddSentence(Data, "R - Вверх, F - вниз");

	Data->MaxLength = 128;
	Data->PosY = 160;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 4;
	Number = MyManager->AddSentence(Data, "A - Влево, D - вправо");

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
//	Data->Level = 0;
	Number = MyManager->AddSentence(Data, "Это тест Scrolling строки.");

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
//	Data->Level = 0;
	Number = MyManager->AddSentence(Data, m_D3D->GetVideoCardString());

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
	//	Data->Level = 0;
	Number = MyManager->AddSentence( Data, m_D3D->GetVideoCardString() );

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
	//	Data->Level = 0;
	Number = MyManager->AddSentence( Data, m_D3D->GetVideoCardString() );
	// 9
	Data->MaxLength = 128;
	Data->PosY = 900;
	Data->FontType = 3;
	Number = MyManager->AddSentence(Data, "Keys: W,S,A,D,R,F | 8,9 - Light Rotate | Num key -/+ Sound | P - Exit");

	// 10
	Data->FontType = 2;
	Data->MaxLength = 128;
	Data->PosY = 370;
	Data->ShowType = SHOW;
	Number = MyManager->AddSentence( Data, "         " );
	// 11
	Data->PosY = 400;
	Number = MyManager->AddSentence( Data, "         " );
	// 12
	Data->PosY = 430;
	Number = MyManager->AddSentence( Data, "         " );
	// 13
	Data->PosY = 400;
	Number = MyManager->AddSentence(Data, "         ");

	// 14
	Data->FontType = 0;
	Data->PosY = 470;
	Number = MyManager->AddSentence(Data, "         ");

	// 15
	Data->FontType = 0;
	Data->PosY = 500;
	Number = MyManager->AddSentence(Data, "         ");

	// 15 Текст о Видеоплате
	Data->MaxLength = 128;
	Data->PosY = 310;
	Data->ShowType = SHOW_SCROLLING;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	//	Data->Level = 0;
	char Str[256];
	strcpy_s(Str, 249, m_D3D->GetVideoCardString());
	strcat_s(Str, 249, " | Clustered Shading with Spotlight shadows DEMO");
	Number = MyManager->AddSentence(Data, Str);

	delete Data;

	SENTENCE_INIT_DATA *Data1, *Data2, *Data3, *Data4, *Data5;

	Data1 = new SENTENCE_INIT_DATA;
	Data2 = new SENTENCE_INIT_DATA;
	Data3 = new SENTENCE_INIT_DATA;
	Data4 = new SENTENCE_INIT_DATA;
	Data5 = new SENTENCE_INIT_DATA;

	ZeroMemory ( Data1, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data2, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data3, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data4, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data5, sizeof ( SENTENCE_INIT_DATA ) );

	float x = 150.0f;
	float y = 50.0f;

	KFButton_Elements Buttons[8];
	Buttons[0]._ObjParam = { 10.0f, 10.0f, x, y };
	Buttons[0].OsnTextureResource = MyManager->TexturesArr[1]->Resource;
	Buttons[0].OsnTexture = MyManager->TexturesArr[1]->SRV;
	Buttons[0].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[0].IsMouseOnButtonTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[0].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;

	Buttons[0].Data = NULL; // Нет строки в Buttons[0].Label 
	Buttons[0].Label = "";
	Buttons[0].Type = BUTTON; // Button
	Buttons[0].EditType = NULL;
	Buttons[0].Checked = false;
	Buttons[0].Enabled = true;
	Buttons[0].WaitingForKeyPress = false;
	Buttons[0].VirtualKeyIndex = 0;
	Buttons[0].SecondSlot = true;


	Buttons[1]._ObjParam = { 10.0f, Buttons[0]._ObjParam.y + y + 10.0f , x, y };
	Buttons[1].OsnTextureResource = MyManager->TexturesArr[2]->Resource;
	Buttons[1].OsnTexture = MyManager->TexturesArr[2]->SRV;
	Buttons[1].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[1].IsMouseOnButtonTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[1].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;
/*
	Data1->MaxLength = 0;
	Data1->PosX = 150;
	Data1->PosY = 350;
	Data1->ShowType = 1;
	Data1->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data1->Render = true;
	Data1->FontType = 0;
	Data1->Level = 2;
*/
	Buttons[1].Data = NULL;  // данные для инициализации строки стекстом в Buttons[1].Label далее
	Buttons[1].Label = "";
	Buttons[1].Type = BUTTON; // Button
	Buttons[1].EditType = NULL;
	Buttons[1].Checked = false;
	Buttons[1].Enabled = true;
	Buttons[1].WaitingForKeyPress = false;
	Buttons[1].VirtualKeyIndex = 0;
	Buttons[1].SecondSlot = true;


	Buttons[2]._ObjParam = { 10.0f, Buttons[1]._ObjParam.y + y + 10.0f, x, y };
	Buttons[2].OsnTextureResource = MyManager->TexturesArr[3]->Resource;
	Buttons[2].OsnTexture = MyManager->TexturesArr[3]->SRV;
	Buttons[2].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[2].IsMouseOnButtonTexture = NULL;
	Buttons[2].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;
/*
	Data2->MaxLength = 0;
	Data2->PosX = 150;
	Data2->PosY = 380;
	Data2->ShowType = 1;
	Data2->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data2->Render = true;
	Data2->FontType = 0;
	Data2->Level = 2;
*/
	Buttons[2].Data = NULL; // данные для инициализации строки стекстом в Buttons[2].Label далее
	Buttons[2].Label = "";
	Buttons[2].Type = BUTTON; // Button
	Buttons[2].EditType = NULL;
	Buttons[2].Checked = false;
	Buttons[2].Enabled = true;
	Buttons[2].WaitingForKeyPress = false;
	Buttons[2].VirtualKeyIndex = 0;
	Buttons[2].SecondSlot = true;

	Buttons[3]._ObjParam = { 10.0f, Buttons[2]._ObjParam.y + y + 10.0f, x, y };
	Buttons[3].OsnTextureResource = MyManager->TexturesArr[4]->Resource;
	Buttons[3].OsnTexture = MyManager->TexturesArr[4]->SRV;
	Buttons[3].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[3].IsMouseOnButtonTexture = NULL;
	Buttons[3].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;
/*
	Data3->MaxLength = 0;
	Data3->PosX = 150;
	Data3->PosY = 400;
	Data3->ShowType = 1;
	Data3->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data3->Render = true;
	Data3->FontType = 0;
	Data3->Level = 2;
*/
	Buttons[3].Data = NULL; // данные для инициализации строки стекстом в Buttons[3].Label далее
	Buttons[3].Label = "";
	Buttons[3].Type = BUTTON; // Button
	Buttons[3].EditType = NULL;
	Buttons[3].Checked = false;
	Buttons[3].Enabled = true;
	Buttons[3].WaitingForKeyPress = false;
	Buttons[3].VirtualKeyIndex = 0;
	Buttons[3].SecondSlot = true;

	Buttons[4]._ObjParam = { 10.0f, Buttons[3]._ObjParam.y + y + 10.0f, x, y };
	Buttons[4].OsnTextureResource = MyManager->TexturesArr[5]->Resource;
	Buttons[4].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[4].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[4].IsMouseOnButtonTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[4].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;

	Buttons[4].Data = NULL; // Нет строки в Buttons[4].Label 
	Buttons[4].Label = "";
	Buttons[4].Type = BUTTON; // Button
	Buttons[4].EditType = NULL;
	Buttons[4].Checked = false;
	Buttons[4].Enabled = true;
	Buttons[4].WaitingForKeyPress = false;
	Buttons[4].VirtualKeyIndex = 0;
	Buttons[4].SecondSlot = true;

	StringsList_Elements StringsList1[1];

	Strings.push_back( "Федя }");
	Strings.push_back( "Лёша }" );
	Strings.push_back( "Кодла} инопланетян" );
	Strings.push_back( "Много} пьяных" );
	Strings.push_back( "Какой-то} дядька" );
	Strings.push_back( "Какая-то} тётя" );
	Strings.push_back( "Все мня } Достали" );
	Strings.push_back( "Где мня } Достали" );
	Strings.push_back( "Кто мня } Достал?" );
	Strings.push_back( "!!!!!!!!!!!!!!!!!" );
	Strings.push_back( "Do not Enter ! MU" );
	Strings.push_back( "Eat this ) US !" );
	Strings.push_back( "Hi My litle Friend" );
	Strings.push_back( "What is the weather" );
	Strings.push_back( "Федя }" );
	Strings.push_back( "Лёша }" );
	Strings.push_back( "Кодла} инопланетян" );
	Strings.push_back( "Много} пьяных" );
	Strings.push_back( "Какой-то} дядька" );
	Strings.push_back( "Какая-то} тётя" );
	Strings.push_back( "Все мня } Достали" );
	Strings.push_back( "Где мня } Достали" );
	Strings.push_back( "Кто мня } Достал?" );
	Strings.push_back( "!!!!!!!!!!!!!!!!!" );

	// Резервируем 3-й Level в списке SENTENCE_INIT_DATA int Level глобальных номеров предложений
	StringsList1->FontIndex = 0;
	StringsList1->ObjParam = {20.0f + x, 10.0f, 200, 150 };
	StringsList1->SentencesIndex = STRINGLIST1;
	StringsList1->Strings = Strings;
	StringsList1->StringsMAXLength = 64;
	StringsList1->StringsSpacing = 0; // 2пикселя между строками
	StringsList1->Scaling = 0.75f;		// Подгоняем текст в окно StringsList
	StringsList1->_ShowStringsList = true;// рисовать StringsList
	StringsList1->Enabled = true;
	StringsList1->ScrollSpeed = 10;

	MainMenu = new MenuControrerClass;
	MainMenu->ShaderForDraw = MyManager->GetShaderIndexByName ( L"KF2DObj" );

	MainMenu->Init ( m_D3D->D3DGC,
					 Buttons, 5, // Buttons and Checkboxes
					 NULL, 0,	// ScrollBars
					 StringsList1, 1,	// StringsLists
					 NULL,		// Нет анимации
					 { 112.0f, 84.0f, 800.0f, 600.0f },
					 MyManager->TexturesArr[18]->SRV,
					 MyManager
					 //m_Text
					 );

	MyManager->AddMenu ( dynamic_cast<Menu*> (MainMenu) );	// Добавляем меню в список существующих
	KFScrollBar_Elements Bars[11];
	
	Bars[0].Horizontal = false;
	Bars[0].Values = { 0.0f, 1.0f, 0.0f, 0.01f };
	Bars[0].ShowButtons = true;
	Bars[0].ObjParam = { 200.0f, 50.0f, 0.0f, 200.0f };
	Bars[0].UpSideDown = true;

	Bars[0].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[0].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[0].BodyNotEnalbledTexture = NULL;
	Bars[0].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[0].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[0].ButtonNotEnalbledTexture = NULL;
	Bars[0].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[0].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[0].TravellerNotEnalbledTexture = NULL;
	Bars[0].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[0].MouseOnTravellerTexture = NULL;

// Для отладки шейдера теней
	Bars[1].Horizontal = false;
	Bars[1].Values = { 0.00001f, 0.03f, 0.00392f, 0.0001f };
	Bars[1].ShowButtons = true;
	Bars[1].ObjParam = { 10.0f, 450.0f, 0.0f, 100.0f };
	Bars[1].UpSideDown = true;

	Bars[1].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[1].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[1].BodyNotEnalbledTexture = NULL;
	Bars[1].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[1].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[1].ButtonNotEnalbledTexture = NULL;
	Bars[1].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[1].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[1].TravellerNotEnalbledTexture = NULL;
	Bars[1].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[1].MouseOnTravellerTexture = NULL;
	// DepthBias
	Bars[2].Horizontal = false;
	Bars[2].Values = { -1000.0f, 1000.0f, 1.0f, 1.0f };
	Bars[2].ShowButtons = true;
	Bars[2].ObjParam = { 40.0f, 450.0f, 0.0f, 100.0f };
	Bars[2].UpSideDown = true;

	Bars[2].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[2].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[2].BodyNotEnalbledTexture = NULL;
	Bars[2].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[2].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[2].ButtonNotEnalbledTexture = NULL;
	Bars[2].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[2].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[2].TravellerNotEnalbledTexture = NULL;
	Bars[2].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[2].MouseOnTravellerTexture = NULL;
	// SlopeScaledDepthBias
	Bars[3].Horizontal = false;
	Bars[3].Values = { -1.0f, 1000.0f, 4.0f, 1.0f };
	Bars[3].ShowButtons = true;
	Bars[3].ObjParam = { 70.0f, 450.0f, 0.0f, 100.0f };
	Bars[3].UpSideDown = true;

	Bars[3].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[3].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[3].BodyNotEnalbledTexture = NULL;
	Bars[3].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[3].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[3].ButtonNotEnalbledTexture = NULL;
	Bars[3].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[3].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[3].TravellerNotEnalbledTexture = NULL;
	Bars[3].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[3].MouseOnTravellerTexture = NULL;
	// PCF_AMOUNT
	Bars[4].Horizontal = false;
	Bars[4].Values = { 0.5f, 15.0f, 2.5f/*3.3f*/, 0.01f };
	Bars[4].ShowButtons = true;
	Bars[4].ObjParam = { 100.0f, 450.0f, 0.0f, 100.0f };
	Bars[4].UpSideDown = true;

	Bars[4].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[4].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[4].BodyNotEnalbledTexture = NULL;
	Bars[4].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[4].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[4].ButtonNotEnalbledTexture = NULL;
	Bars[4].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[4].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[4].TravellerNotEnalbledTexture = NULL;
	Bars[4].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[4].MouseOnTravellerTexture = NULL;
	// PCF_Step
	Bars[5].Horizontal = false;
	Bars[5].Values = { 0.1f, 10.0f, 1.24f/*1.29f*/, 0.01f };
	Bars[5].ShowButtons = true;
	Bars[5].ObjParam = { 130.0f, 450.0f, 0.0f, 100.0f };
	Bars[5].UpSideDown = true;

	Bars[5].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[5].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[5].BodyNotEnalbledTexture = NULL;
	Bars[5].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[5].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[5].ButtonNotEnalbledTexture = NULL;
	Bars[5].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[5].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[5].TravellerNotEnalbledTexture = NULL;
	Bars[5].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[5].MouseOnTravellerTexture = NULL;

	Bars[6].Horizontal = false;
	Bars[6].Values = { -1.0f, 3.0f, 0.003f, 0.0005f };
	Bars[6].ShowButtons = true;
	Bars[6].ObjParam = { 160.0f, 450.0f, 0.0f, 100.0f };
	Bars[6].UpSideDown = true;

	Bars[6].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[6].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[6].BodyNotEnalbledTexture = NULL;
	Bars[6].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[6].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[6].ButtonNotEnalbledTexture = NULL;
	Bars[6].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[6].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[6].TravellerNotEnalbledTexture = NULL;
	Bars[6].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[6].MouseOnTravellerTexture = NULL;
	// Shadow Divider
	Bars[7].Horizontal = false;
	Bars[7].Values = { 1.0f, 8128.0f, 3192.0f, 1.0f };
	Bars[7].ShowButtons = true;
	Bars[7].ObjParam = { 190.0f, 450.0f, 0.0f, 100.0f };
	Bars[7].UpSideDown = true;

	Bars[7].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[7].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[7].BodyNotEnalbledTexture = NULL;
	Bars[7].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[7].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[7].ButtonNotEnalbledTexture = NULL;
	Bars[7].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[7].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[7].TravellerNotEnalbledTexture = NULL;
	Bars[7].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[7].MouseOnTravellerTexture = NULL;

	// DyffuseY
	Bars[8].Horizontal = true;
	Bars[8].Values = { -1.0f, 1.0f, 0.005f, 0.001f };
	Bars[8].ShowButtons = true;
	Bars[8].ObjParam = { 35.0f, 700.0f, 100.0f, 25.0f };
	Bars[8].UpSideDown = false;

	Bars[8].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[8].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[8].BodyNotEnalbledTexture = NULL;
	Bars[8].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[8].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[8].ButtonNotEnalbledTexture = NULL;
	Bars[8].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[8].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[8].TravellerNotEnalbledTexture = NULL;
	Bars[8].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[8].MouseOnTravellerTexture = NULL;

	// Active LIghts on Screen
	Bars[9].Horizontal = true;
	Bars[9].Values = { 1.0f, 3000.0f , 150.0f, 1.0f };
	Bars[9].ShowButtons = true;
	Bars[9].ObjParam = { 35.0f, 730.0f, 100.0f, 25.0f };
	Bars[9].UpSideDown = false;

	Bars[9].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[9].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[9].BodyNotEnalbledTexture = NULL;
	Bars[9].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[9].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[9].ButtonNotEnalbledTexture = NULL;
	Bars[9].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[9].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[9].TravellerNotEnalbledTexture = NULL;
	Bars[9].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[9].MouseOnTravellerTexture = NULL;

	// Active Lights on Screen
	Bars[10].Horizontal = true;
	Bars[10].Values = { 1.0f, 3000.0f , 150.0f, 1.0f };
	Bars[10].ShowButtons = true;
	Bars[10].ObjParam = { 35.0f, 760.0f, 100.0f, 25.0f };
	Bars[10].UpSideDown = false;

	Bars[10].OsnTextureResource = MyManager->TexturesArr[12]->Resource;
	Bars[10].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[10].BodyNotEnalbledTexture = NULL;
	Bars[10].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[10].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[10].ButtonNotEnalbledTexture = NULL;
	Bars[10].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[10].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[10].TravellerNotEnalbledTexture = NULL;
	Bars[10].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[10].MouseOnTravellerTexture = NULL;


	Buttons[1].Label = "";
	Buttons[1].Data = NULL;

	Buttons[2].Type = CHECKBOX; // CheckBox
	Buttons[2].Checked = m_D3D->D3DGC->ShadowsOn;
	Buttons[2].Label = "";
	Buttons[2].OsnTextureResource = MyManager->TexturesArr[5]->Resource;
	Buttons[2].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[2].IsClickTexture = MyManager->TexturesArr[6]->SRV;
	Buttons[2].Data = NULL;


	Buttons[3].Type = CHECKBOX; // CheckBox
	Buttons[3].Checked = m_D3D->D3DGC->EnableFXAA;
	Buttons[3].Label = "";
	Buttons[3].OsnTextureResource = MyManager->TexturesArr[5]->Resource;
	Buttons[3].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[3].IsClickTexture = MyManager->TexturesArr[7]->SRV;
	Buttons[3].Data = NULL;


	Buttons[6]._ObjParam = { 10.0f, Buttons[3]._ObjParam.y + y + 10.0f, x, y };
	Buttons[6].OsnTextureResource = MyManager->TexturesArr[5]->Resource;
	Buttons[6].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[6].IsClickTexture = MyManager->TexturesArr[4]->SRV;
	Buttons[6].IsMouseOnButtonTexture = NULL;
	Buttons[6].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;

	Buttons[6].Data = NULL; // Нет строки в Buttons[4].Label 
	Buttons[6].Label = "";
	Buttons[6].SentenceIndex = -1;
	Buttons[6].Type = CHECKBOX; // Button
	Buttons[6].EditType = NULL;
	Buttons[6].Checked = m_D3D->D3DGC->SoftShadowsOn;
	Buttons[6].Enabled = true;
	Buttons[6].WaitingForKeyPress = false;
	Buttons[6].VirtualKeyIndex = 0;
	Buttons[6].SecondSlot = false;


	Data4->MaxLength = 16; // Для Edit указывать желаемое количество вводи мых символов + 1
	Data4->PosX = 10;
	Data4->PosY = 330;
	Data4->ShowType = 1;
	Data4->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data4->Render = true;
	Data4->FontType = 0;
	Data4->Level = MENU;


	Buttons[4]._ObjParam = { 10.0f, 330.0f, 0.0f, 0.0f };
	Buttons[4].OsnTextureResource = MyManager->TexturesArr[11]->Resource;
	Buttons[4].OsnTexture = MyManager->TexturesArr[11]->SRV;
	Buttons[4].IsClickTexture = NULL;
	Buttons[4].IsMouseOnButtonTexture = NULL;
	Buttons[4].IsNotEnalbledTexture = NULL;
	Buttons[4].Data = Data4; // Нет строки в Buttons[4].Label 
	Buttons[4].Label = "Мой текст";
	Buttons[4].Type = EDIT; // Edit
	Buttons[4].EditType = ANYSYMBOL; // Edit
	Buttons[4].Checked = false;
	Buttons[4].Enabled = true;
	Buttons[4].WaitingForKeyPress = false;
	Buttons[4].VirtualKeyIndex = 0;
	Buttons[4].SecondSlot = true;

	Data5->MaxLength = 32;
	Data5->PosX = 10;
	Data5->PosY = 330;
	Data5->ShowType = 1;
	Data5->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data5->Render = true;
	Data5->FontType = 0;
	Data5->Level = MENU;

	Buttons[5]._ObjParam = { 10.0f, 380.0f, 0.0f, 0.0f };
	Buttons[5].OsnTextureResource = MyManager->TexturesArr[11]->Resource;
	Buttons[5].OsnTexture = MyManager->TexturesArr[11]->SRV;
	Buttons[5].IsClickTexture = NULL;
	Buttons[5].IsMouseOnButtonTexture = NULL;
	Buttons[5].IsNotEnalbledTexture = NULL;
	Buttons[5].Data = Data5; // Нет строки в Buttons[4].Label 
	Buttons[5].Label = "";
	Buttons[5].Type = LABEL; // Edit
	Buttons[5].ActiveStringsList = false;
	Buttons[5].EditType = NULL;
	Buttons[5].Checked = false;
	Buttons[5].Enabled = true;
	Buttons[5].WaitingForKeyPress = true;
	Buttons[5].VirtualKeyIndex = 0;
	Buttons[5].SecondSlot = true;

	// Hide Text
	Buttons[7]._ObjParam = { 10.0f, 600.0f, 0.0f, 0.0f };
	Buttons[7].OsnTextureResource = MyManager->TexturesArr[3]->Resource;
	Buttons[7].OsnTexture = MyManager->TexturesArr[3]->SRV;
	Buttons[7].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[7].IsMouseOnButtonTexture = NULL;
	Buttons[7].IsNotEnalbledTexture = NULL;
	Buttons[7].Data = NULL;
	Buttons[7].Label = "";
	Buttons[7].Type = CHECKBOX; // CheckBox
	Buttons[7].ActiveStringsList = false;
	Buttons[7].EditType = NULL;
	Buttons[7].Checked = false;
	Buttons[7].Enabled = true;
	Buttons[7].WaitingForKeyPress = false;
	Buttons[7].VirtualKeyIndex = 0;
	Buttons[7].SecondSlot = false;


	// +++++++++++++++++++     Анимация     ++++++++++++++++++++++++++++++

	AnimTexture = new KF2DTextureAnimation;
	XMFLOAT4 ObjData = { 100.0f, 800.0f, 100.0f, 100.0f };
	{
		int TempIndex = MyManager->Create_Flat_Obj_Buffers ( D3D11_USAGE_DEFAULT, 4, 6, MyManager->TexturesArr[10]->SRV );
		AnimTexture->Init ( hwnd, m_D3D->D3DGC, 8, 8, MyManager->TexturesArr[8]->SRV,
			MyManager->GetShaderIndexByName ( L"FireAnimation" ),	// анимация в меню FireAnimation_vs
			MyManager->GetShaderIndexByName ( L"KF2DObj" ),			// возвращаем KF2DObj_vs
			MyManager->Get_Flat_ObjectBuffers_ByIndex ( TempIndex ),
			ObjData );
	}
	// -------------------     Анимация     ------------------------------


	Hud = new MenuControrerClass;
	Hud->ShaderForDraw = MyManager->GetShaderIndexByName ( L"KF2DObj" );
	Hud->Init(m_D3D->D3DGC,
		Buttons, 8, 
		Bars, 11,
		NULL, 0,	// StringsLists
		AnimTexture,	// Передаём объект анимации в меню
		{ float(screenWidth - 250) , 0.0f, 250.0f, float(screenHeight) },
		MyManager->TexturesArr[18]->SRV,
		MyManager
		//m_Text
		);
	
	MyManager->AddMenu ( dynamic_cast<Menu*> (Hud) );	// Добавляем меню в список существующих

	// Помещаем надписи Label внутрь кнопок
	Hud->PutLabelsInsideButtons();
	// включаем рисование HUD
	Hud->IsMenuDraw = true;

	// Активируем HUD или Hud->IsMenuActive = true;
	Hud->IsMenuActive = true;
	
//	Hud->SetScrollBarParametors(0, 0.02f, 0.05f, 3.0f, 0.3f, 10);
//	Hud->SetScrollBarParametors(1, 0.02f, 0.1f, 3.0f, 0.3f, 10);
//	Hud->SetScrollBarParametors(2, 0.02f, 0.15f, 3.0f, 0.3f, 10);
//	Hud->SetScrollBarParametors(3, 0.02f, 0.2f, 3.0f, 0.3f, 10);

	delete Data1;
	delete Data2;
	delete Data3;
	delete Data4;
	delete Data5;
	// --------------------- это коробка
	Mapping = new CubeMapping;
	Mapping->ShaderForDraw = MyManager->GetShaderIndexByName ( L"CubeMap" );
	Mapping->Init(m_D3D->D3DGC, 
	MyManager->TexturesArr[MyManager->InitOneTexture(L"Textures/skymap.dds")]->SRV) ;

// ++++++++++++++++++++++++++++++++++++++++     LIGHT & ShadowMap    ++++++++++++++++++++++++++++++++++++++++++++	
//	m_Light = new LightClass;
	m_D3D->LightShaderForDraw = MyManager->GetShaderIndexByName ( L"ClusteredSM" );
//	m_D3D->ShadowMapShaderFordraw = MyManager->GetShaderIndexByName ( L"LightRenderSM" );
//	m_D3D->ShadowMapShader = MyManager->GetVertexShader ( m_D3D->ShadowMapShaderFordraw );
// ----------------------------------------     LIGHT & ShadowMap    ---------------------------------------------

// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++
	// Обязательно для систем частиц !!!
	m_Camera->GetPosition( m_D3D->D3DGC->CameraPosition );

	// Create the particle system object.
	SnowParticles = new SnowFallParticles;
	SnowParticles->ShaderForDraw = MyManager->GetShaderIndexByName ( L"particle" );

	if (!SnowParticles )
	{
		return false;
	}

	result = SnowParticles->Initialize
		(
		hwnd,
		m_D3D->D3DGC,
		MyManager->TexturesArr[20]->SRV,	// Текстура звезды
		1,
		1,
		1
		);
		
		TorchFire = new TorchFireParticles;
		TorchFire->ShaderForDraw = MyManager->GetShaderIndexByName ( L"TorchFire3D" );
		if ( !TorchFire )
		{
			return false;
		}

		// Инициализируем Факел
		TorchFireSmoke TorchFireSmokeInit;
		TorchFireSmokeInit.TorchPosition = {125.0f, 1.0f, 20.0f };
		TorchFireSmokeInit.FireFramesAmount = 31;	// начсиная с 0  ( всего 32 кадра )
		TorchFireSmokeInit.FireFrameStart = 0;		// 1-й кадр анимации - 0-й ( потому что в С++ всё с нуля )
		TorchFireSmokeInit.SmokeFramesAmount = 4;	// 4 кадра анимации
		TorchFireSmokeInit.SmokeFrameStart = 32;	// начало с 32 кадра 
		TorchFireSmokeInit.FireFlyFrameNumber = 49;	// Кадр из которого нарезяются FireFly
		TorchFireSmokeInit.FireFlyCutWidth = 16;	// Размер нарезки куска для FireFly по ширине в кусках ( на сколько кусков резать )
		TorchFireSmokeInit.FireFlyCutHeight = 8;	// Размер нарезки куска для FireFly по высоте в кусках ( на сколько кусков резать )
		TorchFireSmokeInit.UX_Amount = 8;
		TorchFireSmokeInit.VY_Amount = 8;

		result = TorchFire->Initialize
			(
			hwnd,
			m_D3D->D3DGC,
			MyManager->TexturesArr[22]->SRV, //8 22 //		MyManager->ShaderResourceArr[20],	// Текстура звезды
			TorchFireSmokeInit,
			m_D3D								// Указатель на класс света в движке
			);

		if ( !result )
		{
			return false;
		}

	FireParticles = new FireParticleSystem;
	FireParticles->ShaderForDraw = MyManager->GetShaderIndexByName ( L"FireParticle3D" );
		if ( !FireParticles )
		{
			return false;
		}

	result = FireParticles->Initialize
	(	
		hwnd, 
		m_D3D->D3DGC, 
		MyManager->TexturesArr[8]->SRV, //8
		8,
		8,
		0,
		m_D3D								// Указатель на класс света в движке
	);

	if (!result)
	{
		return false;
	}

// --------------------------------------------------  PARTICLE SYSTEM  ---------------------------------------------


		XMFLOAT3 CamPos;
		m_Camera->GetPosition(CamPos);

		//Map->IsClastersDarw = true;
		//Map->IsClastersDarw = true;


// +++++++++++++++++++     Создаём свет на сцене     +++++++++++++++++++++++
		PointLight AddedLight;
		// R
		AddedLight.attenuationBegin = 0.1f;
		AddedLight.attenuationEnd = 1900.0f;
		AddedLight.color = { 1.0f, 0.7f, 1.0f };
		AddedLight.position = { 400.0f, 50.0f, 450.0f };//;{ 0.0f, 10.0f, 00.0f }
		AddedLight.direction = { -50.0f, -10.0f, -50.0f };//;{ 0.0f, -0.3f, -1.0f }
		AddedLight.angel = 180.0f;
		AddedLight.HaveShadow = true;
		AddedLight.LightID = -2;
		AddedLight.ShadowMapSliceNumber = -1;
		AddedLight.Dummy = 1;
		m_D3D->AddLightSource(AddedLight);
		// G
		AddedLight.attenuationBegin = 1.0f;
		AddedLight.attenuationEnd = 500.0f;
		AddedLight.color = { 1.0f, 1.0f, 0.7f };
		AddedLight.position = { 20.98f, 37.f, 54.40f };//{ 30.0f, 10.0f, 70.0f };
		AddedLight.direction = { -200.0f, -300.0f, -900.0f };//{ -200.0f, -300.0f, -900.0f };
		AddedLight.angel = 10.0f;
		AddedLight.HaveShadow = true;
		AddedLight.LightID = -2;
		AddedLight.ShadowMapSliceNumber = -1;
		AddedLight.Dummy = 1;
		m_D3D->AddLightSource(AddedLight);
		// B
/*		AddedLight.attenuationBegin = 1.0f;
		AddedLight.attenuationEnd = 500.0f;
		AddedLight.color = { 1.0f, 1.0f, 1.0f };
		AddedLight.position = { 30.0f, 30.0f, -20.0f };
		AddedLight.direction = { 0.0f, -500.0f, -1000.0f };
		AddedLight.angel = 10.0f;
		AddedLight.HaveShadow = true;
		AddedLight.LightID = -2;
		AddedLight.ShadowMapSliceNumber = -1;
		AddedLight.Dummy = 1;
		m_Light->AddLightSource(AddedLight);
*/


// Тестирование удаления и добавления светов
/*
		m_Light->FreeLightSource( 0 );

		AddedLight.attenuationBegin = 1.0f;
		AddedLight.attenuationEnd = 100.0f;
		AddedLight.color = { 0.0f, 0.0f, 0.7f };
		AddedLight.position = { 30.0f, 10.0f, 70.0f };
		AddedLight.direction = { -200.0f, -300.0f, -900.0f };
		AddedLight.angel = 0.0f;
		AddedLight.HaveShadow = false;
		m_Light->AddLightSource ( AddedLight );

		AddedLight.attenuationBegin = 1.0f;
		AddedLight.attenuationEnd = 1900.0f;
		AddedLight.color = { 1.0f, 0.7f, 1.0f };
		AddedLight.position = { 400.0f, 50.0f, 450.0f };//;{ 0.0f, 10.0f, 00.0f }
		AddedLight.direction = { -50.0f, -10.0f, -50.0f };//;{ 0.0f, -0.3f, -1.0f }
		AddedLight.angel = 180.0f;
		AddedLight.HaveShadow = true;
		m_Light->AddLightSource ( AddedLight );
*/
// Тестирование удаления и добавления светов


/*
		float ValueX = 0, ValueY = 0, ValueZ = 0;

		int MaxX = 1000;
		int MinX = -0;
		int MaxY = 20;
		int MinY = 0;
		int MaxZ = 180;
		int MinZ = -0;
		float LO = 0.0f;
		float HI = 1.0f;

		float HI1 = 100;
		float LO1 = 30;
		float r = 0;
		float g = 0;
		float b = 0;
		AddedLight.angel = 0.0f;

		int LightsWanted = 3;

		
		for ( int i = m_Light->mActiveLights; i < LightsWanted; ++i )
		{
			//http://stackoverflow.com/questions/686353/c-random-float-number-generation Random float

			int RandInt = static_cast <int> ( rand () ) % MaxX;
			int RandInty = static_cast <int> ( rand () ) % MaxX;
			int RandIntz = static_cast <int> ( rand () ) % 2;
			if ( RandIntz )
				RandInt += RandInty;
			else
				RandInt -= RandInty;
			if ( RandInt == 0 )
				++RandInt;

			r = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( HI - LO ) ) );
			g = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( HI - LO ) ) );
			b = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( HI - LO ) ) );
			ValueX = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( RandInt ) ) );
			ValueY = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( 20 ) ) );
			ValueZ = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( RandInt ) ) );
			if ( RandIntz )
				ValueX = -ValueX;



			AddedLight.angel = static_cast <float> ( rand () / ( static_cast <float> ( RAND_MAX / 45 - 5 ) ) );
			AddedLight.direction = { r / -g , -0.1f, -g / b };
			float inv_Len = 1.0f / sqrt ( ( AddedLight.direction.x * AddedLight.direction.x )
										  + ( AddedLight.direction.y * AddedLight.direction.y ) + ( AddedLight.direction.z * AddedLight.direction.z ) );
			AddedLight.direction.x = AddedLight.direction.x * inv_Len;
			AddedLight.direction.y = AddedLight.direction.y * inv_Len;
			AddedLight.direction.z = AddedLight.direction.z * inv_Len;
			//AddedLight.direction = {1.0f , -0.1f, 1.0f };



			AddedLight.position = XMFLOAT3 ( ValueX, ValueY, ValueZ );

			if ( i == 0 )
			{
				r = 1.0f;
				g = 0.0f;
				b = 0.0f;
			}
			if ( i == 1 )
			{
				r = 0.0f;
				g = 1.0f;
				b = 0.0f;
			}
			if ( i == 2 )
			{
				r = 0.0f;
				g = 0.0f;
				b = 1.0f;
			}

			AddedLight.color = XMFLOAT3 ( r, g, b );

			float Min = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( HI1 - LO1 ) ) );
			float Max = static_cast <float> ( rand () ) / ( static_cast <float> ( RAND_MAX / ( HI1 - LO1 ) ) );
			AddedLight.attenuationBegin = Min;
			AddedLight.attenuationEnd = Max;

			AddedLight.HaveShadow = false;
			AddedLight.Dummy = 1;

			m_Light->AddLightSource ( AddedLight );
		}

*/



// -------------------     Создаём свет на сцене     -----------------------
		XMFLOAT3 Lpos;
		m_D3D->GetLightPos(0, Lpos);
//		m_Camera->SetPosition(Lpos.x + 20, Lpos.y, Lpos.z);
// Тень ракеты
//		m_Camera->SetPosition( -3.05f, 14.25f, -4.97f );
//		m_Camera->SetLookAt( -3.16f, 13.35f, -5.35f );
// Тень куба
//		m_Camera->SetPosition( 328.12f, 9.85f, 298.42f );
//		m_Camera->SetLookAt( 328.10f, 9.04f, 297.84f );
// Ракета сбоку . сверху
		m_Camera->SetPosition( 20.98f, 37.81f, 54.40f );
		m_Camera->SetLookAt( 20.70f, 37.36f, 53.55f );

		//int Index = MyManager->InitOneTexture(hwnd, L"Textures/land.jpg", m_D3D->D3DGC->DX_device, m_D3D->D3DGC->DX_deviceContext);
		//MyManager->InitTextures(hwnd, L"Textures/RcubeTextures.kaf", m_D3D->D3DGC->DX_device, m_D3D->D3DGC->DX_deviceContext);

		Terrain::KFLandscapeParam Land;

		Land.LowlandsCout = 50;// количество ям
		Land.HeightesCout = 50;// количество гор
		Land.MaxHeight = 50;// максимальная высота гор
		Land.MinHeight = 10;// минимальная низота низин
		Land.MaxRadius = 80;// максимальный радиус низин/гор
		Land.MinRadius = 1;// минимальный радиус низин/гор
		Land.filename = "Textures/hm2.bmp";

		float Rows = 1000.0f , Columns = 1000.0f;

		//\//\/\/\/!!!!!ОБЯЗАТЕЛЬНО ЧИТАЙ НЮАНСЫ ПОСЛЕ ИНИЦИАЛИЗАЦИИ!!!!!!/\/\/\/\/\/\

		KFTerrain = new Terrain;
		KFTerrain->Initialize(hwnd, m_D3D->D3DGC,
		MyManager->TexturesArr[21]->SRV, //MyManager->ShaderResourceArr[MyManager->InitOneTexture(hwnd , L"Textures/moonTexture.png" , m_D3D->D3DGC->DX_device , m_D3D->D3DGC->DX_deviceContext)] 
		Rows , Columns , &Land, 1.0f/* расстояние между точками */, 100 /* количество кластеров на которые разбит тераин по X оси */,
			100/* количество кластеров на которые разбит тераин по Z оси  */, 49 /* дальность прорисовки в кластерах */, m_D3D);

		/*
		НЮАНСЫ

		ширина земли в вертексах должна нацело делиться на количество кластеров на которые разбит тераин по X оси
		длинна земли в вертексах должна нацело делиться наколичество кластеров на которые разбит тераин по Z оси

		количество кластеров на которые разбит тераин по любой оси должно быть больше чем дальность прорисовки в два раза + 1

		расстояние между точками должно быть больше либо равно 1

		Физикс и разброс кубиков автоматически подхватывают новыеразмеры терайна

		если ты изменил размер терайна и ничего не изменилось то сделай килин и запусти зано (почемуто вижуал не всегда подхватывает файлы)

		чтобы грамотно замерять фпс подожди пока кубики упадут фпс должен подняться на 10 пунктов

		*/

		ModelList = new KFModelList;

		ModelList->Init(m_D3D->D3DGC, m_D3D, m_Frustum);
//		ModelList->IsClusteringUse = false;

//		ClusterMap = new KFClusterMap(100, 100, 100, 50, 50, 50);

		ShadowWork = new KFShadowWork;

		KFShadowWork::ObjectUsingShadows  SWobjects;

		SWobjects.ModelList = ModelList;
		SWobjects.Terrain = KFTerrain;

		ShadowWork->Init(hwnd, m_D3D->D3DGC, m_D3D, m_Frustum, SWobjects);
		ShadowWork->ShaderFordraw = MyManager->GetShaderIndexByName ( L"LightRenderSM" );

		return true;
}

// ++++++++++++++++++++++++++++++     FRAME     +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GraphicsClass::Frame( FPSTimers &Counters, DXINPUTSTRUCT& InputStruct1)
{
	bool result = true;
	
//	char Str1[STRMAXLENGTH];

	++ShadowFrameCounts;
	fpstimers = Counters;
// делаем доступ к скоросити компьютера для текста. Чтобы отрисовывать его синхронно с FPS системы
//	m_Text->fpstimers = Counters;
	RCubeRender->fpstimers = Counters;

	sprintf_s ( Str1, STRMAXLENGTH, "Fps : %d", Counters.FpsRate );
	MyManager->UpdateSentence ( 0, Str1, 20, 20 );

	sprintf_s ( Str1, STRMAXLENGTH, "ИСПОЛЬЗОВАНИЕ ПРОЦЕССОРА: %d %%", Counters.CpuVal );
	MyManager->UpdateSentence ( 1, Str1, 20, 50 );

// ++++++++++++++++++++     Вращаем свет     ++++++++++++++++++++++++++++++++++++
/*
	XMVECTOR Dir;
	XMVECTOR rotaxis = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

//	rotation = (XM_2PI / 360);
//	XMMATRIX Rotation = XMMatrixRotationAxis( rotaxis, -0.01745329251944444444444444444444 );
	m_Light->GetLightDirection( 2, Dir );
//	XMVectorSetW(Dir, 1.0f);
//	XMVECTOR a = XMVector3TransformCoord( Dir, XMMatrixRotationAxis( rotaxis, -0.0174532925 ) );
	m_Light->ChangeLightDirection( 2, XMVector3TransformCoord( Dir, XMMatrixRotationAxis( rotaxis, 0.0174532925 /2)));
*/
// ++++++++++++++++++++     Вращаем свет     ++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++	
	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
//	char Str[25];// = new char [25];
//	Profile->StartTimer();
	// Измеряем быстродействие
	SnowParticles->Frame( Counters );


// Потоки
// https://msdn.microsoft.com/en-us/library/ms682516(v=vs.85).aspx
// https://stackoverflow.com/questions/10673585/start-thread-with-member-function
// http://www.cyberforum.ru/visual-cpp/thread12242.html
// Про _beginthreadex vs CreateThread C++
// https://stackoverflow.com/questions/331536/windows-threading-beginthread-vs-beginthreadex-vs-createthread-c
// по-русски
// http://www.quizful.net/post/multithreading-cpp11
//	thread t( [this] { GraphicsClass::SnowParticles->Frame( fpstimers ); } );
//	thread t( &GraphicsClass::SnowThread, &fpstimers );
//	t.join();
//	DWORD   ThreadId;
//	unsigned  ThreadAdr;
//	CreateThread(NULL,0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&Duck), &fpstimers , CREATE_SUSPENDED , &ThreadId ); 
//	CreateThread( NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>( GraphicsClass::SnowThread ), &fpstimers, CREATE_SUSPENDED, &ThreadId );
//	_beginthreadex( NULL, 0, reinterpret_cast<_beginthreadex_proc_type>( &Duck ), &fpstimers, CREATE_SUSPENDED, &ThreadAdr );
//	_beginthreadex(NULL,0, reinterpret_cast<_beginthreadex_proc_type>(SnowThread), &fpstimers , CREATE_SUSPENDED , &ThreadAdr );

//	Profile->StopTimer( Str );
//	MyManager->->UpdateSentence( 4, Str, 100, 160 );

	TorchFire->Frame( Counters );
	FireParticles->Frame( Counters );
	// Измеряем быстродействие
//	Profile->StopTimer(Str);
//	MyManager->->UpdateSentence(5, Str, 100, 160);
	// Измеряем быстродействие
// --------------------------------------------------  PARTICLE SYSTEM  -----------------------------

	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
//		char Str[128];// = new char [25];
//		Profile->StartTimer();
		// Измеряем быстродействие

// ++++++++++++    Как часто рисуем Тень     +++++++++++++++++++
	if ( m_D3D->D3DGC->ShadowsOn && ShadowFrameCounts > 0 )//Counters.FpsRate / 25
	{
		MyManager->SetActiveShadersInProgramm ( ShadowWork->ShaderFordraw );
		ShadowWork->RenderSpotLightsSadowMaps( m_D3D->SpotLightsWithShadowsIndexes );
		ShadowFrameCounts = 0;
	}
	// Измеряем быстродействие
//		Profile->StopTimer(Str);
//		MyManager->->UpdateSentence(5, Str, MyManager->GetPosX(5), MyManager->GetPosY(5));
	// Измеряем быстродействие

// ++++++++++++++  FRUSTUM +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SHADOWMAP использует другой FRUSTUM, поэтому сразу после SHADOWMAP возвращаем FRUSTUM на место

	// Страрая версия фрастума от Растертека
	//m_Frustum->ConstructFrustum(SCREEN_DEPTH, m_D3D->D3DGC->ProjectionMatrix, m_D3D->D3DGC->ViewMatrix);

	// Подсмотрели у Брайнзара
	m_Frustum->ConstructFrustumNew( m_D3D->D3DGC->ViewProjection );

// ----------------------  FRUSTUM    --------------------------------------------------

// ++++++++++++    Как часто рисем Тень     +++++++++++++++++++
// Федя 540 ns    Мой  517 ns   
		char Str[25];// = new char [25];
		Profile->StartTimer();
// Измеряем быстродействие

// Обновляем свет сцены
// Обязательно после отрисовки теней !!!
		m_D3D->Frame();
// ------------------------------------------------------------

// Измеряем быстродействие
	Profile->StopTimer(Str);
	strcpy_s(Str1, STRMAXLENGTH, "LightsFrame() : ");
	strcat_s(Str1, STRMAXLENGTH, Str);
	MyManager->UpdateSentence(13, Str1, MyManager->GetPosX( 13 ), MyManager->GetPosY( 13 ) );
// Измеряем быстродействие


	result = Render( InputStruct1.m_mouseX, InputStruct1.m_mouseY);
	if(!result)
	{  
		return false;
	}

	return true;
}


bool GraphicsClass::Render(int& mouseX, int& mouseY )
{

	// Clear the buffers to begin the scene.
	m_D3D->SetBackBufferRenderTarget();
	XMFLOAT4 begin = { 0.2f, 0.2f, 0.2f, 1.0f };
	m_D3D->BeginScene( begin );

// В первую очередь обрабатываем камеру и получаем её позицию
	UpdateCamera();

// Инициализируем массив матриц для всех шейдеров 
	UpdateConstantBuffer();

	char Str[STRMAXLENGTH];
	char Str1[STRMAXLENGTH];
//	sprintf_s(Str, 50, "Spec Power = %8.5f", m_Light->GetSpecularPower());
//	MyManager->UpdateSentence(4, Str, 100, 160);

//	sprintf_s(Str, 70, "Cam pos: X: %4.2f , Y: %4.2f , Z: %4.2f", m_D3D->D3DGC->CameraPosition.x, m_D3D->D3DGC->CameraPosition.y, m_D3D->D3DGC->CameraPosition.z );
//	MyManager->UpdateSentence(3, Str, 100, 130);
	float angel = Hud->GetScrollBarValue( 2 );
	sprintf_s( Str, 70, "DepthBias : %4.2f", angel );
	MyManager->UpdateSentence( 3, Str, 100, 130 );

//	float angel = atan2(0.0f - m_D3D->D3DGC->CameraPosition.x, 0.0f - m_D3D->D3DGC->CameraPosition.z) * 57.29578049f;

	m_Camera->GetLookAt( CamLook.Vec );
//	sprintf_s( Str, 70, "Cam LookAt: X: %4.2f , Y: %4.2f , Z: %4.2f , W: %4.2f", CamLook.Fl4.x, CamLook.Fl4.y, CamLook.Fl4.z, CamLook.Fl4.w );
//	MyManager->UpdateSentence( 2, Str, 100, 100 );
	angel = Hud->GetScrollBarValue( 3 );
	sprintf_s(Str, 70, "SlopeScaledDepthBias : %4.2f", angel );
	MyManager->UpdateSentence(2, Str, 100, 100);

	angel = Hud->GetScrollBarValue ( 1 );
	sprintf_s ( Str, 50, "DiffuseX = %6.5f", angel );
  //	sprintf_s( Str, 50, "Active P = %i", m_ParticleSystem->GetActiveParticleAmmount() );
	MyManager->UpdateSentence ( 4, Str, 100, 160 );

	angel = Hud->GetScrollBarValue( 4 );
	sprintf_s( Str, 50, "PCF_Amount = %4.2f", angel );
	MyManager->UpdateSentence( 5, Str, 100, 190 );

	angel = Hud->GetScrollBarValue( 5 );
	sprintf_s( Str, 50, "PCF_Step = %4.2f", angel );
	MyManager->UpdateSentence( 6, Str, 100, 220 );
//	sprintf_s( Str, 50, "Lights %i", m_Light->GetActiveLghtNumber() );
//	MyManager->UpdateSentence( 5, Str, 100, 160 );

//	angel = Hud->GetScrollBarValue( 6 );
//	sprintf_s( Str, 50, "ShadowCLAMP = %1.4f", angel );
//	MyManager->UpdateSentence( 7, Str, 100, 250 );
	sprintf_s ( Str1, STRMAXLENGTH - 1, "Время рисования всех строк текста : %s", Global );
	MyManager->UpdateSentence ( 7, Str1, 100, 250 );

	angel = Hud->GetScrollBarValue( 7 );
	sprintf_s( Str, 50, "Shadow Divider = %4.1f", angel );
	MyManager->UpdateSentence( 8, Str, 100, 280 );

// LightInfo
	sprintf_s( Str, 50, "Active Lights = %d", m_D3D->mActiveLights );
	MyManager->UpdateSentence( 11, Str, 100, 310 );

	sprintf_s( Str, 50, "Visible Lights = %d", m_D3D->mVisibleLights );
	MyManager->UpdateSentence( 12, Str, 100, 340 );

	angel = Hud->GetScrollBarValue(8);
	sprintf_s(Str, 50, "DiffuseY = %1.5f", angel);
	MyManager->UpdateSentence(14, Str, 100, 470);

	angel = Hud->GetScrollBarValue(9);
	sprintf_s(Str, 50, "DiffuseZ = %1.5f", angel);
	MyManager->UpdateSentence(15, Str, 100, 500);

	MyManager->UpdateSentence(16, MyManager->GetSentenceText(16), MyManager->GetPosX(16), MyManager->GetPosY(16));

// +++++++++++++++++++++    РИСУЕМ ОБЪЕКТЫ НА СЦЕНЕ    +++++++++++++++++++++++++++++++++++++++++++
	MyManager->SetActiveShadersInProgramm( m_D3D->LightShaderForDraw );
	// Устанавливаем сразу всё что нужно для отрисовки всех моделей со светом
	// Чтобы не устанаваить это в цикле много раз
	m_D3D->D3DGC->DX_deviceContext->OMSetDepthStencilState(m_D3D->D3DGC->m_depthStencilState, 0);
	m_D3D->SetDefaultResterizeState ();
	m_D3D->D3DGC->DX_deviceContext->OMSetRenderTargets(1, &m_D3D->D3DGC->BackBuffer_RTV, m_D3D->D3DGC->m_depthStencilView);
	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
//			char Str[25];// = new char [25];
//		Profile->StartTimer();
	// Измеряем быстродействие
	KFTerrain->Frame(true, m_Camera->position);
	// Измеряем быстродействие
//		Profile->StopTimer(Str);
//		MyManager->UpdateSentence(5, Str, 100, 160);
	// Измеряем быстродействие

	KFTerrain->LightRender();

//	char Str[25];// = new char [25];
	Profile->StartTimer();

	ModelList->Frame();

	Profile->StopTimer(Str);

	strcpy_s(Str1, STRMAXLENGTH, "Frustum объектам : ");
	strcat_s(Str1, STRMAXLENGTH, Str);
	MyManager->UpdateSentence(10, Str1, MyManager->GetPosX( 10 ), MyManager->GetPosY( 10 ) );
	
	ModelList->LightRender();

	MyManager->Frustum_3D_Objects ( m_Frustum );
	RCubeRender->RenderScene ();

	m_D3D->TurnOffAlphaBlending();

//	m_D3D->SetDefaultResterizeState();

	// Измеряем быстродействие
//	Profile->StopTimer(Str);
//	MyManager->UpdateSentence(4, Str, 100, 160);
	// Измеряем быстродействие

	// Обязательно для систем частиц
	m_D3D->SetCullNoneResterizeState ();
	// Устанавливаем шейдеры FXAA

	// Фильтруем сцену FXAA
	if ( m_D3D->D3DGC->EnableFXAA )
	{
		// ++++++++    FXAA    +++++++++++	
		MyManager->SetActiveShadersInProgramm( m_D3D->FXAAShaderIndex );
		m_D3D->FXAAScene();
	}
	// ++++++++    FXAA    +++++++++++	
	// Измеряем быстродействие
//			Profile->StopTimer(Str);
//			MyManager->UpdateSentence(4, Str, 100, 160);
	// Измеряем быстродействие

//	m_D3D->TurnOnAlphaBlending();

	// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++
	m_D3D->TurnOnParticleBlending();
	m_D3D->TurnZBufferOff();
	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	
	// SNOW система частиц
	MyManager->SetActiveShadersInProgramm( SnowParticles->ShaderForDraw );
	SnowParticles->Render();

//	Profile->StopTimer( Str );
//	MyManager->UpdateSentence( 5, Str, 100, 160 );


	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
	//		char Str[25];// = new char [25];
//	Profile->StartTimer();
	// FIRE Летающий анимированный огонь
	MyManager->SetActiveShadersInProgramm( FireParticles->ShaderForDraw );
	FireParticles->Render();
//	Profile->StopTimer( Str );
//	MyManager->UpdateSentence( 5, Str, 100, 160 );
	// TorchFire Факел, костёр

	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
	//		char Str[25];// = new char [25];
//	Profile->StartTimer();
	// Измеряем быстродействие	
	MyManager->SetActiveShadersInProgramm( TorchFire->ShaderForDraw );
	TorchFire->Render();
//	Profile->StopTimer( Str );
//	MyManager->UpdateSentence( 4, Str, 100, 160 );

	// Измеряем быстродействие
	m_D3D->TurnZBufferOn();
	// --------------------------------------------------  PARTICLE SYSTEM  ------------------------------

	// -------------------------------- блок со включонным Z буфером

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// + Рисуем CUBEMAP	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	MyManager->SetActiveShadersInProgramm( Mapping->ShaderForDraw );
	Mapping->Render();
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// - Рисуем CUBEMAP	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	m_D3D->TurnOnAlphaBlending();
	m_D3D->TurnZBufferOff();

	// ++++++++++++++++++++++++     Рисуем Текст в игре     +++++++++++++++++++++++++++++


	if (!Hud->GetButtonState(7))
	{
		MyManager->SetActiveShadersInProgramm( MyManager->TextShaderIndex );
// Измеряем быстродействие
// Федя 540 ns    Мой  517 ns   
//	char Str[25];// = new char [25];
	Profile->StartTimer();
// Измеряем быстродействие
		RCubeRender->RenderText ( 0 );
// Измеряем быстродействие
	Profile->StopTimer( Global );
// Измеряем быстродействие
	}

/*
	// Выключаем свет
	m_Light->LightRender(0, 0);
	else
	m_Light->LightRender(0, 1);
*/

/*	if (!result)
	{
		return false;
	}*/
//	MyManager->SetActiveShadersInProgramm(2);
	// ++++++++++++++++++++++++     Рисуем Текст в игре     +++++++++++++++++++++++++++++

	// ++++++++++++++++++++++++     Рисуем текст в HUD      +++++++++++++++++++++++++++++
//		MyManager->SetActiveShadersInProgramm(2);

	if ( Hud->IsMenuDraw )
	{
		// Измеряем быстродействие
		// Федя 540 ns    Мой  517 ns   
		//	char Str[25];// = new char [25];
		//	Profile->StartTimer();
		// Измеряем быстродействие
		MyManager->SetActiveShadersInProgramm ( Hud->ShaderForDraw );
		RCubeRender->RenderMenu ( 1 );
//		Hud->Draw();
		// Измеряем быстродействие
		//	Profile->StopTimer(Str);
		//	MyManager->UpdateSentence(4, Str, 100, 160);
		// Измеряем быстродействие
		MyManager->SetActiveShadersInProgramm( MyManager->TextShaderIndex );
		RCubeRender->RenderText( 2 ); // Рисуем текст для HUD
//		MyManager->SetActiveShadersInProgramm(2);
	}

	m_D3D->D3DGC->EnableFXAA = Hud->GetButtonState(3);
	m_D3D->D3DGC->ShadowsOn = Hud->GetButtonState(2);
	m_D3D->D3DGC->SoftShadowsOn = Hud->GetButtonState( 6 );
	m_D3D->D3DGC->PCF_Amount = Hud->GetScrollBarValue( 4 );
	m_D3D->D3DGC->PCF_Step = Hud->GetScrollBarValue( 5 );
	m_D3D->D3DGC->ShadowCLAMP = Hud->GetScrollBarValue( 6 );
	m_D3D->D3DGC->Shadow_Divider = Hud->GetScrollBarValue( 7 );

	// Регулировка частиц
	FireParticles->ChangeActiveParticleAmount((int)Hud->GetScrollBarValue(10));
/*
	if ( !m_D3D->D3DGC->SoftShadowsOn )
	{
//		TempVal1 = Hud->GetScrollBarValue( 2 );
//		TempVal2 = Hud->GetScrollBarValue( 3 );

		float Val = 1.0f;
		Hud->SetScrollBarValue( 3, Val );
		Hud->SetScrollBarValue( 2, Val );
	}
	else
	{
		TempVal1 = -583.0f;
		TempVal2 = 4.0f;
		Hud->SetScrollBarValue( 2, TempVal1 );
		Hud->SetScrollBarValue( 3, TempVal2 );
	}
*/

// ++++++++++++++++++++++++     Рисуем текст в HUD     +++++++++++++++++++++++++++++

// ++++++++++++++++++++++++     Рисуем текст для меню     +++++++++++++++++++++++++++++
	if ( MainMenu->IsMenuDraw )
	{
		// Измеряем быстродействие
		// Федя 540 ns    Мой  517 ns   
//			char Str[25];// = new char [25];
//			Profile->StartTimer();
		// Измеряем быстродействие
		m_D3D->BlurScene(MyManager->GetComputeShader( m_D3D->BlureHorizComputeShaderIndex ), MyManager->GetComputeShader( m_D3D->BlureVertComputeShaderIndex ), 2);
		// Измеряем быстродействие
//			Profile->StopTimer(Str);
//			MyManager->UpdateSentence(13, Str, MyManager->GetPosX( 13 ), MyManager->GetPosY( 13 ) );
		MyManager->SetActiveShadersInProgramm ( MainMenu->ShaderForDraw );
		RCubeRender->RenderMenu ( 0 );
//		MainMenu->Draw();
		MyManager->SetActiveShadersInProgramm( MyManager->TextShaderIndex );
		RCubeRender->RenderText ( 1 );	// Рисуем текст для меню

	}
// ++++++++++++++++++++++++     Рисуем текст для меню     +++++++++++++++++++++++++++++

		// Рисуем мышкин курсор
		MyManager->SetActiveShadersInProgramm( m_Bitmap->ShaderIndex );
		m_Bitmap->ChangeMousePosition( mouseX, mouseY );
		RCubeRender->RenderFlatObject ( MyManager->Get_Flat_ObjectBuffers_ByIndex ( m_Bitmap->BuffersIndex ) );
		// Рисуем мышкин курсор

//	if (IsSettingsMenuOn)
		//			Settingsmenu->Draw(m_D3D->D3DGC->OrthoMatrix) ;

		m_D3D->SetBackBufferRenderTarget();


	// ++++++++++++++++++++++++   Собственный мышиный курсор   ++++++++++++++++++++++++++++++++



	m_D3D->TurnOffAlphaBlending();

	m_D3D->TurnZBufferOn();
	// -----------------------   Собственный мышиный курсор   --------------------------------


	//	m_D3D->TurnZBufferOff() ;

	//	m_D3D->TurnZBufferOn() ;

	// ----------------------------------блок с выключеным Z буфером


	m_D3D->EndScene();

	return true;
}

void GraphicsClass::UpdateCamera()
{
	// В первую очередь обрабатываем камеру и получаем её позицию
	m_Camera->Render();
	// Позиция камеры
	m_Camera->GetPosition( m_D3D->D3DGC->CameraPosition );
	// Считываем матрицу камеры
	m_Camera->GetViewMatrix( m_D3D->D3DGC->ViewMatrix );
	// Проекционная матрица изначально в m_D3D->D3DGC->ProjectionMatrix; 
	// Identity матрица изначально в m_D3D->D3DGC->WorldMatrix; 
	// Orthomatrix изначально в m_D3D->D3DGC->OrthoMatrix
	m_D3D->D3DGC->ViewProjection = m_D3D->D3DGC->ViewMatrix * m_D3D->D3DGC->ProjectionMatrix;
}

void GraphicsClass::UpdateConstantBuffer()
{
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++     Инициализируем массив матриц для всех шейдеров     ++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ConstantBufferData cbPerObj;
	// Расчёт матрицы для CubeMap
	XMMATRIX Scale = XMMatrixScaling(5000.0f, 5000.0f, 5000.0f);
	XMMATRIX Translation = XMMatrixTranslation(m_D3D->D3DGC->CameraPosition.x, m_D3D->D3DGC->CameraPosition.y, m_D3D->D3DGC->CameraPosition.z);
	XMMATRIX mtr = Scale * Translation * m_D3D->D3DGC->ViewProjection;

	cbPerObj.World = XMMatrixTranspose(m_D3D->D3DGC->WorldMatrix);
	cbPerObj.View = XMMatrixTranspose(m_D3D->D3DGC->ViewMatrix);
	cbPerObj.Projection = XMMatrixTranspose(m_D3D->D3DGC->ProjectionMatrix);
	cbPerObj.ViewProjection = XMMatrixTranspose(m_D3D->D3DGC->ViewProjection);
	cbPerObj.ScaleMatrix = XMMatrixTranspose(mtr);	// CubeMapMatrix
	cbPerObj.Ortho = XMMatrixTranspose(m_D3D->D3DGC->OrthoMatrix);
	cbPerObj.cameraPosition = m_D3D->D3DGC->CameraPosition;
	RCube_VecFloat34 Temp;
	Temp.Vec = XMQuaternionRotationMatrix(m_D3D->D3DGC->ViewMatrix);
	cbPerObj.TransposedCameraRotation2 = Temp.Fl4;

	m_D3D->D3DGC->Global_VS_ConstantsBuffer->Update( &cbPerObj );
	m_D3D->D3DGC->DX_deviceContext->VSSetConstantBuffers(0, 1, &m_D3D->D3DGC->Global_VS_ConstantsBuffer->Buffer );
}


void GraphicsClass::GetWindowPos ( HWND hWnd, int &x, int &y )
{
	HWND hWndParent = GetParent ( hWnd );
	POINT p = { 0 };

	MapWindowPoints ( hWnd, hWndParent, &p, 1 );

	x = p.x;
	y = p.y;
}


void GraphicsClass::SnowThread( FPSTimers& fpstimers )
{
	SnowParticles->Frame( fpstimers );
}