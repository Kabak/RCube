#include "stdafx.h"

// I have kept this class entirely empty for now as we are just building the framework for this tutorial.
////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsClass.h"

void __stdcall Fuck()
{
	int a = 1;
}

GraphicsClass::GraphicsClass()
{

	     ClusterMap = nullptr;
  cbPerObjectBuffer = nullptr;
		   MainMenu = nullptr;
		        Hud = nullptr;
			  m_D3D = nullptr;
		   m_Camera = nullptr;
 		  MyManager = nullptr;
		   m_Bitmap = nullptr;
			Mapping = nullptr;
		  ModelList = nullptr;
		 ShadowWork = nullptr;
// + Particel systems
	  FireParticles = nullptr;
		  TorchFire = nullptr;
	  SnowParticles = nullptr;
// - Particel systems
 		  m_Frustum = nullptr;
		  	Profile = nullptr;
			m_Light = nullptr;
			Terrain = nullptr;
		AnimTexture = nullptr;

			Global = new  char[100];

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

	delete [] Global;

	UINT i = 0;
	UINT j = (UINT) FontList.size();
	while ( i < j )
	{
		RCUBE_DELETE( FontList[i] );
		++i;
	};

	FontList.clear();

	Strings.clear();

	RCUBE_DELETE ( AnimTexture );
	RCUBE_DELETE ( Terrain );
	RCUBE_DELETE ( Profile );
	RCUBE_DELETE ( ShadowWork );
  RCUBE_SHUTDOWN ( m_Light );
  RCUBE_SHUTDOWN ( FireParticles );
  RCUBE_SHUTDOWN ( TorchFire );
  RCUBE_SHUTDOWN ( SnowParticles );
    RCUBE_DELETE ( ClusterMap );
   RCUBE_RELEASE ( cbPerObjectBuffer );
	RCUBE_DELETE ( ModelList );
    RCUBE_DELETE ( Mapping );
    RCUBE_DELETE ( MainMenu );
	RCUBE_DELETE ( Hud );
	RCUBE_DELETE ( m_Bitmap );
    RCUBE_DELETE ( MyManager );
  RCUBE_SHUTDOWN ( m_Text );
    RCUBE_DELETE ( m_Camera );
	RCUBE_DELETE ( m_Frustum );
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

// �������� ������������
// http://stackoverflow.com/questions/22133742/dx11-crash-when-accessing-xmmatrix
//	siz = __alignof(D3DClass);


	Profile = new RCubeProfiling;
	Profile->Initialize();

	m_D3D = new D3DClass;

	result = m_D3D->Initialize(hwnd, screenWidth, screenHeight, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize m_D3D Class", Error, MB_OK);
		return false;
	}
	// �������������� ���������� ������ �� InputClass 
	m_D3D->D3DGC->m_EngineInputClass = _Input;
	// �������������� ���������� ������� ���� � ��������� ���� �� ������
	m_D3D->D3DGC->ScreenWidth = _screenWidth;
	m_D3D->D3DGC->ScreenHeight = _screenHeight;
	m_D3D->D3DGC->WindowsPosX = WindowPosX;
	m_D3D->D3DGC->WindowsPosY = WindowPosY;

	// Create the camera object.
	m_Camera = new CameraClass;

	m_Frustum = new FrustumClass;

	m_Camera->GetViewMatrix(m_D3D->D3DGC->ViewMatrix);
//	m_Camera->Render();
//	m_Camera->SetPosition(0.0f, 30.0f, -30.0f);
	m_Camera->FirstPersonCam = false;
	m_Camera->SetPosition( -3.05f, 14.25f, -4.97f );
	m_Camera->SetLookAt( -3.16f, 13.35f, -5.35f );
	m_Camera->Render();
//	m_Camera->FirstPersonCam = true;


// + ������ ����������� �����
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof( KFModel::cbPerObject );
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = m_D3D->D3DGC->D11_device->CreateBuffer( &cbbd, NULL, &cbPerObjectBuffer );
	if ( FAILED( hr ) )
	{
		MessageBox( NULL, L"Create cbPerObjectBuffer failed!", Error, 0 );
		result = false;
	}

// - ������ ����������� �����

	MyManager = new KFResourceManager;
	MyManager->Init(m_D3D->D3DGC , hwnd);
//	m_D3D->D3DGC->ShaderManager = MyManager;
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
	// ������ ����������� ����� ����������
	// xcopy /Y "$(ProjectDir)\Shaders\*.cso" "$(OutDir)\Shaders\"
	//"$(ProjectDir)\Shaders\KafConvertor.exe" 
	//xcopy / Y "$(ProjectDir)\Shaders\*.kaf" "$(OutDir)\Shaders\"
//	MyManager->InitOneShader(L"Shaders/CubeMap_vs.cso");		// 0
//	MyManager->InitOneShader(L"Shaders/CubeMap_ps.cso");		// 0
//	MyManager->InitOneShader(L"Shaders/Font_vs.cso");			// 1
//	MyManager->InitOneShader(L"Shaders/Font_ps.cso");			// 1
//	MyManager->InitOneShader(L"Shaders/KF2DObj_vs.cso");		// 2
//	MyManager->InitOneShader(L"Shaders/KF2DObj_ps.cso");		// 2
//	MyManager->InitOneShader(L"Shaders/particle_vs.cso");		// 3
//	MyManager->InitOneShader(L"Shaders/particle_ps.cso");		// 3
//	MyManager->InitOneShader(L"Shaders/SkyDome_vs.cso");		// 4
//	MyManager->InitOneShader(L"Shaders/SkyDome_ps.cso");		// 4
//	MyManager->InitOneShader(L"Shaders/Model_vs.cso");			// 5
//	MyManager->InitOneShader(L"Shaders/Model_ps.cso");			// 5
//	MyManager->InitOneShader(L"Shaders/InstansingModel_vs.cso");// 6
//	MyManager->InitOneShader(L"Shaders/InstansingModel_ps.cso");// 6
//	MyManager->InitOneShader(L"Shaders/TerrainShader_vs.cso");	// 7
//	MyManager->InitOneShader(L"Shaders/TerrainShader_ps.cso");	// 7

	//	MyManager->InitOneShader(hwnd, L"Shaders/Clustered_ps.cso", m_D3D->D3DGC->D11_device, m_D3D->D3DGC->D11_deviceContext);		// 8
	//	MyManager->InitOneShader(hwnd, L"Shaders/Clustered_vs.cso", m_D3D->D3DGC->D11_device, m_D3D->D3DGC->D11_deviceContext);		// 8
//	MyManager->InitOneShader(L"Shaders/ClusteredSM_vs.cso");	// 8
//	MyManager->InitOneShader(L"Shaders/ClusteredSM_ps.cso");	// 8
// ���������� Clustering � FXAA
//	MyManager->InitOneShader(L"Shaders/FXAA_vs.cso");			// 9
//	MyManager->InitOneShader(L"Shaders/FXAA_ps.cso");			// 9
//	MyManager->InitOneShader(L"Shaders/LightRenderSM_vs.cso");	// 10
//	MyManager->InitOneShader(L"Shaders/LightRenderSM_ps.cso");	// 10
//	MyManager->InitOneShader(L"Shaders/FireAnimation_vs.cso");// 11
//	MyManager->InitOneShader(L"Shaders/FireAnimation_ps.cso");// 11
//	MyManager->InitOneShader(L"Shaders/FireParticle3D_vs.cso");// 12
//	MyManager->InitOneShader(L"Shaders/FireParticle3D_ps.cso");// 12
//	MyManager->InitOneShader(L"Shaders/TorchFire3D_vs.cso");// 13
//	MyManager->InitOneShader(L"Shaders/TorchFire3D_ps.cso");// 13 27
//	MyManager->InitOneShader(L"Shaders/Horiz_cs.cso");
//	MyManager->InitOneShader(L"Shaders/Vert_cs.cso");
//*/

// +++++++++++++++++++++     ������� ������     +++++++++++++++++++++++++++++++++++
// ������������ ������ ������ 74.0f 
// ������������ Outline 10
	FontOnTextureData					 *FontOnTexture;

// +++++++++++++++++++++     ������ 1 - � �����  0 - �   ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush1 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline1 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops1 = nullptr;
	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// �������� ��������
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 36.0f;			// ������ ������
	wchar_t * Font1 = L"Fonts/BuxtonSketch.ttf";// ���� ������ TTF
	FontOnTexture->FontName = Font1;

	FontOnTexture->Outline = true;				// ����� �� � ������ ���������
	FontOnTexture->OutlineWidth = 3.0f;			// ������� ��������� 0 < 10 

	const int GradientStopAmount1 = 3;
	// ��� ����� ����������
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
// ---------------------     ������ 1 - � �����      --------------------------------

// +++++++++++++++++++++     ������ 2 - � �����      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush2 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline2 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops2 = nullptr;
	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// �������� ��������
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 36.0f;
	wchar_t *Font2 = L"Fonts/8708_AA_Akashi.ttf"; // 8708_AA_Akashi.ttf // RosewoodStd-Regular.otf
	FontOnTexture->FontName = Font2;

	FontOnTexture->Outline = true;				// ����� �� � ������ ���������
	FontOnTexture->OutlineWidth = 6.0f;			// ������� ��������� 0 < 10 

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

// +++++++++++++++++++++     ������ 3 - � �����      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush3 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline3 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops3 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// �������� ��������
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font3 = L"Fonts/comic.ttf";
	FontOnTexture->FontName = Font3;

	FontOnTexture->Outline = true;				// ����� �� � ������ ���������
	FontOnTexture->OutlineWidth = 4.0f;			// ������� ��������� 0 < 10 

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

// +++++++++++++++++++++     ������ 4 - � �����      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush4 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline4 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops4 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// �������� ��������
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font4 = L"Fonts/BrushScriptStd.ttf"; //BrushScriptStd.otf
	FontOnTexture->FontName = Font4;
	FontOnTexture->Outline = true;				// ����� �� � ������ ���������
	FontOnTexture->OutlineWidth = 3.0f;			// ������� ��������� 0 < 10 

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

// +++++++++++++++++++++     ������ 5 - � �����      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush5 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline5 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops5 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// �������� ��������
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font5 = L"Fonts/RAVIE.TTF";
	FontOnTexture->FontName = Font5;

	FontOnTexture->Outline = true;				// ����� �� � ������ ���������
	FontOnTexture->OutlineWidth = 2.0f;			// ������� ��������� 0 < 10 

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

// +++++++++++++++++++++     ������ 6 - � �����      ++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush6 = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline6 = nullptr;
	ID2D1GradientStopCollection		  *GradientStops6 = nullptr;

	FontOnTexture = new FontOnTextureData;

	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// �������� ��������
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;
	wchar_t *Font6 = L"Fonts/times.ttf";
	FontOnTexture->FontName = Font6;
	FontOnTexture->Outline = true;				// ����� �� � ������ ���������
	FontOnTexture->OutlineWidth = 2.0f;			// ������� ��������� 0 < 10 

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

	// Create the text object.
	m_Text = new TextClass;
	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->D3DGC,
		FontList,
		MyManager
	);

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
	m_Bitmap = new SquareObjectClass;
	hr = m_Bitmap->Init(m_D3D->D3DGC,
	MouseCursorPos,
	MyManager->ShaderResourceArr[10],
	NO_FLIP,
	MyManager->BlobsArr[2]
	);
	
	if (FAILED( hr ))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", Error, MB_OK);
		return false;
	}

//	m_Bitmap = new BitmapClass;
	// Initialize the bitmap object.
//	result = m_Bitmap->Initialize(m_D3D->D3DGC, MyManager->ShaderResourceArr[10], 30, 30);
//	if(!result)
//	{
//		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
//		return false;
//	}
	// Initialize that the user has not clicked on the screen to try an intersection test yet.
//	m_beginCheck = false;
// ---------------------   Custom Cursor   ---------------------------------


// ++++++++++++++++++++++     ������ ������� �� ��������     +++++++++++++++++++++++++++++++
	ID2D1LinearGradientBrush	*LinearGradientBrush = nullptr;
	ID2D1SolidColorBrush		  *SolidBrushOutline = nullptr;
	ID2D1GradientStopCollection		  *GradientStops = nullptr;

	FontOnTexture = new FontOnTextureData;
	FontOnTexture->AliasingMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;// �������� ��������
	FontOnTexture->FontFaceType = DWRITE_FONT_FACE_TYPE_TRUETYPE;

	FontOnTexture->FontSize = 24.0f;								// ������ ������
	wchar_t * Path = L"Fonts/BuxtonSketch.ttf";//9825.otf";// C:\\Windows\\Fonts\\

	FontOnTexture->FontName = Path;

	FontOnTexture->Outline = true;				// ����� �� � ������ ���������
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
// ++++++++++++++++++++++++++++++++			������ ������� �� ��������        +++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	UpdateConstantBuffer();

//	m_D3D->TurnOnAlphaBlending();//TurnOnParticleBlending();
//	m_D3D->TurnOnParticleBlending();
	m_D3D->TurnOnTextOnTextureBlending();
	m_D3D->TurnZBufferOff();

	ID3D11RasterizerState* RSCullNone;
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory( &cmdesc, sizeof( D3D11_RASTERIZER_DESC ) );

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.FrontCounterClockwise = true;

	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = m_D3D->D3DGC->D11_device->CreateRasterizerState( &cmdesc, &RSCullNone );

	m_D3D->D3DGC->D11_deviceContext->RSSetState( RSCullNone );
	m_D3D->D3DGC->D11_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_D3D->D3DGC->D11_deviceContext->OMSetRenderTargets( 1, &m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, NULL ); //m_D3D->D3DGC->m_depthStencilView
	m_D3D->D3DGC->D11_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );



//	Profile->StartTimer();
	m_Text->m_Font[0]->RenderFontOnTexture ( MyManager->ShaderResourceArr[1],
											 Path,
											 L"�������� OFF",
											 FontOnTexture );
//	m_D3D->SaveTextureToPNG( m_D3D->D3DGC->BackBuffer_ProxyTextureSRV );

	m_D3D->D3DGC->D11_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );
	m_Text->m_Font[0]->RenderFontOnTexture ( MyManager->ShaderResourceArr[7],
											 Path,
											 L"FXAA ON",
											 FontOnTexture );

	m_D3D->D3DGC->D11_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );
	m_Text->m_Font[0]->RenderFontOnTexture ( MyManager->ShaderResourceArr[6],
											 Path,
											 L"Shadows ON",
											 FontOnTexture );

	m_D3D->D3DGC->D11_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );
	m_Text->m_Font[0]->RenderFontOnTexture ( MyManager->ShaderResourceArr[2],
											 Path,
											 L"�������� ON",
											 FontOnTexture );

	m_D3D->D3DGC->D11_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );
	m_Text->m_Font[0]->RenderFontOnTexture( MyManager->ShaderResourceArr[4],
											Path,
											L"Soft Shadows",
											FontOnTexture );
	//
	m_D3D->D3DGC->D11_deviceContext->ClearRenderTargetView(m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor);
	m_Text->m_Font[0]->RenderFontOnTexture(MyManager->ShaderResourceArr[3],
											Path,
											L"Text OFF",
											FontOnTexture);

//	Profile->StopTimer(Global);

	m_D3D->D3DGC->D11_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );

	RSCullNone->Release();

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();
// ---------------------			������ ������� �� ��������            -------------------------------


	RCUBE_RELEASE( LinearGradientBrush );
	RCUBE_RELEASE( SolidBrushOutline );
	RCUBE_RELEASE( GradientStops );

	delete FontOnTexture;
// ------------------------------    ������ ������� �� ���������    -------------------------------



	SENTENCE_INIT_DATA* Data = new SENTENCE_INIT_DATA;
	ZeroMemory(Data, sizeof(SENTENCE_INIT_DATA));
	int Number;

	Data->MaxLength = 128;
	Data->PosX = 100;
	Data->PosY = 90;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	Data->Level = INGAME_TEXT;
	Number = m_Text->AddSentence(Data, "W - �����, S - �����");

	Data->MaxLength = 128;
	Data->PosY = 130;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	Number = m_Text->AddSentence(Data, "R - �����, F - ����");

	Data->MaxLength = 128;
	Data->PosY = 160;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 4;
	Number = m_Text->AddSentence(Data, "A - �����, D - ������");

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
//	Data->Level = 0;
	Number = m_Text->AddSentence(Data, "��� ���� Scrolling ������.");

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
//	Data->Level = 0;
	Number = m_Text->AddSentence(Data, m_D3D->GetVideoCardString());

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
	//	Data->Level = 0;
	Number = m_Text->AddSentence( Data, m_D3D->GetVideoCardString() );

	Data->MaxLength = 128;
	Data->PosY = 190;
	Data->ShowType = SHOW;
	Data->Colour = { 0.0f, 1.0f, 0.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
	//	Data->Level = 0;
	Number = m_Text->AddSentence( Data, m_D3D->GetVideoCardString() );
	// 9
	Data->MaxLength = 128;
	Data->PosY = 900;
	Data->FontType = 3;
	Number = m_Text->AddSentence(Data, "Keys: W,S,A,D,R,F | 8,9 - Light Rotate | Num key -/+ Sound | P - Exit");

	// 10
	Data->FontType = 2;
	Data->MaxLength = 128;
	Data->PosY = 370;
	Data->ShowType = SHOW;
	Number = m_Text->AddSentence( Data, "         " );
	// 11
	Data->PosY = 400;
	Number = m_Text->AddSentence( Data, "         " );
	// 12
	Data->PosY = 430;
	Number = m_Text->AddSentence( Data, "         " );
	// 13
	Data->PosY = 400;
	Number = m_Text->AddSentence(Data, "         ");

	// 14
	Data->FontType = 0;
	Data->PosY = 470;
	Number = m_Text->AddSentence(Data, "         ");

	// 15 ����� � ����������
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
	Number = m_Text->AddSentence(Data, Str);

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
	Buttons[0].OsnTextureResource = MyManager->ResourceArr[1];
	Buttons[0].OsnTexture = MyManager->ShaderResourceArr[1];
	Buttons[0].IsClickTexture = MyManager->ShaderResourceArr[19];
	Buttons[0].IsMouseOnButtonTexture = MyManager->ShaderResourceArr[5];
	Buttons[0].IsNotEnalbledTexture = MyManager->ShaderResourceArr[5];

	Buttons[0].Data = NULL; // ��� ������ � Buttons[0].Label 
	Buttons[0].Label = "";
	Buttons[0].Type = BUTTON; // Button
	Buttons[0].EditType = NULL;
	Buttons[0].Checked = false;
	Buttons[0].Enabled = true;
	Buttons[0].WaitingForKeyPress = false;
	Buttons[0].VirtualKeyIndex = 0;
	Buttons[0].SecondSlot = true;
	Buttons[0].Blob = MyManager->BlobsArr[4];



	Buttons[1]._ObjParam = { 10.0f, Buttons[0]._ObjParam.y + y + 10.0f , x, y };
	Buttons[1].OsnTextureResource = MyManager->ResourceArr[2];
	Buttons[1].OsnTexture = MyManager->ShaderResourceArr[2];
	Buttons[1].IsClickTexture = MyManager->ShaderResourceArr[19];
	Buttons[1].IsMouseOnButtonTexture = MyManager->ShaderResourceArr[5];
	Buttons[1].IsNotEnalbledTexture = MyManager->ShaderResourceArr[5];
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
	Buttons[1].Data = NULL;  // ������ ��� ������������� ������ �������� � Buttons[1].Label �����
	Buttons[1].Label = "";
	Buttons[1].Type = BUTTON; // Button
	Buttons[1].EditType = NULL;
	Buttons[1].Checked = false;
	Buttons[1].Enabled = true;
	Buttons[1].WaitingForKeyPress = false;
	Buttons[1].VirtualKeyIndex = 0;
	Buttons[1].SecondSlot = true;
	Buttons[1].Blob = MyManager->BlobsArr[4];



	Buttons[2]._ObjParam = { 10.0f, Buttons[1]._ObjParam.y + y + 10.0f, x, y };
	Buttons[2].OsnTextureResource = MyManager->ResourceArr[3];
	Buttons[2].OsnTexture = MyManager->ShaderResourceArr[3];
	Buttons[2].IsClickTexture = MyManager->ShaderResourceArr[19];
	Buttons[2].IsMouseOnButtonTexture = NULL;
	Buttons[2].IsNotEnalbledTexture = MyManager->ShaderResourceArr[5];
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
	Buttons[2].Data = NULL; // ������ ��� ������������� ������ �������� � Buttons[2].Label �����
	Buttons[2].Label = "";
	Buttons[2].Type = BUTTON; // Button
	Buttons[2].EditType = NULL;
	Buttons[2].Checked = false;
	Buttons[2].Enabled = true;
	Buttons[2].WaitingForKeyPress = false;
	Buttons[2].VirtualKeyIndex = 0;
	Buttons[2].SecondSlot = true;
	Buttons[2].Blob = MyManager->BlobsArr[4];

	Buttons[3]._ObjParam = { 10.0f, Buttons[2]._ObjParam.y + y + 10.0f, x, y };
	Buttons[3].OsnTextureResource = MyManager->ResourceArr[4];
	Buttons[3].OsnTexture = MyManager->ShaderResourceArr[4];
	Buttons[3].IsClickTexture = MyManager->ShaderResourceArr[19];
	Buttons[3].IsMouseOnButtonTexture = NULL;
	Buttons[3].IsNotEnalbledTexture = MyManager->ShaderResourceArr[5];
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
	Buttons[3].Data = NULL; // ������ ��� ������������� ������ �������� � Buttons[3].Label �����
	Buttons[3].Label = "";
	Buttons[3].Type = BUTTON; // Button
	Buttons[3].EditType = NULL;
	Buttons[3].Checked = false;
	Buttons[3].Enabled = true;
	Buttons[3].WaitingForKeyPress = false;
	Buttons[3].VirtualKeyIndex = 0;
	Buttons[3].SecondSlot = true;
	Buttons[3].Blob = MyManager->BlobsArr[4];


	Buttons[4]._ObjParam = { 10.0f, Buttons[3]._ObjParam.y + y + 10.0f, x, y };
	Buttons[4].OsnTextureResource = MyManager->ResourceArr[5];
	Buttons[4].OsnTexture = MyManager->ShaderResourceArr[5];
	Buttons[4].IsClickTexture = MyManager->ShaderResourceArr[19];
	Buttons[4].IsMouseOnButtonTexture = MyManager->ShaderResourceArr[5];
	Buttons[4].IsNotEnalbledTexture = MyManager->ShaderResourceArr[5];

	Buttons[4].Data = NULL; // ��� ������ � Buttons[4].Label 
	Buttons[4].Label = "";
	Buttons[4].Type = BUTTON; // Button
	Buttons[4].EditType = NULL;
	Buttons[4].Checked = false;
	Buttons[4].Enabled = true;
	Buttons[4].WaitingForKeyPress = false;
	Buttons[4].VirtualKeyIndex = 0;
	Buttons[4].SecondSlot = true;
	Buttons[4].Blob = MyManager->BlobsArr[4];

	StringsList_Elements StringsList1[1];

	Strings.push_back( "���� }");
	Strings.push_back( "˸�� }" );
	Strings.push_back( "�����} �����������" );
	Strings.push_back( "�����} ������" );
	Strings.push_back( "�����-��} ������" );
	Strings.push_back( "�����-��} ���" );
	Strings.push_back( "��� ��� } �������" );
	Strings.push_back( "��� ��� } �������" );
	Strings.push_back( "��� ��� } ������?" );
	Strings.push_back( "!!!!!!!!!!!!!!!!!" );
	Strings.push_back( "Do not Enter ! MU" );
	Strings.push_back( "Eat this ) US !" );
	Strings.push_back( "Hi My litle Friend" );
	Strings.push_back( "What is the weather" );
	Strings.push_back( "���� }" );
	Strings.push_back( "˸�� }" );
	Strings.push_back( "�����} �����������" );
	Strings.push_back( "�����} ������" );
	Strings.push_back( "�����-��} ������" );
	Strings.push_back( "�����-��} ���" );
	Strings.push_back( "��� ��� } �������" );
	Strings.push_back( "��� ��� } �������" );
	Strings.push_back( "��� ��� } ������?" );
	Strings.push_back( "!!!!!!!!!!!!!!!!!" );

	// ����������� 3-� Level � ������ SENTENCE_INIT_DATA int Level ���������� ������� �����������
	StringsList1->FontIndex = 0;
	StringsList1->ObjParam = {20.0f + x, 10.0f, 200, 150 };
	StringsList1->SentencesIndex = STRINGLIST1;
	StringsList1->Strings = Strings;
	StringsList1->StringsMAXLength = 64;
	StringsList1->StringsSpacing = 0; // 2������� ����� ��������
	StringsList1->Scaling = 0.75f;		// ��������� ����� � ���� StringsList
	StringsList1->_ShowStringsList = true;// �������� StringsList
	StringsList1->Enabled = true;
	StringsList1->ScrollSpeed = 10;
	StringsList1->Blob = MyManager->BlobsArr[4];

	MainMenu = new MenuControrerClass;
	MainMenu->Init ( m_D3D->D3DGC,
					 Buttons, 5, // Buttons and Checkboxes
					 NULL, 0,	// ScrollBars
					 StringsList1, 1,	// StringsLists
					 NULL,		// ��� ��������
					 { 112.0f, 84.0f, 800.0f, 600.0f },
					 MyManager->ShaderResourceArr[18],
					 MyManager->BlobsArr[4],
					 m_Text
					 );

	KFScrollBar_Elements Bars[10];

	Bars[0].Blob = MyManager->BlobsArr[4];
	Bars[0].Horizontal = false;
	Bars[0].Values = { 0.0f, 1.0f, 0.0f, 0.01f };
	Bars[0].ShowButtons = true;
	Bars[0].ObjParam = { 200.0f, 50.0f, 0.0f, 200.0f };
	Bars[0].UpSideDown = true;

	Bars[0].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[0].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[0].BodyNotEnalbledTexture = NULL;
	Bars[0].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[0].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[0].ButtonNotEnalbledTexture = NULL;
	Bars[0].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[0].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[0].TravellerNotEnalbledTexture = NULL;
	Bars[0].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[0].MouseOnTravellerTexture = NULL;

// ��� ������� ������� �����
	Bars[1].Blob = MyManager->BlobsArr[4];
	Bars[1].Horizontal = false;
	Bars[1].Values = { 0.00001f, 0.03f, 0.00392f, 0.0001f };
	Bars[1].ShowButtons = true;
	Bars[1].ObjParam = { 10.0f, 450.0f, 0.0f, 100.0f };
	Bars[1].UpSideDown = true;

	Bars[1].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[1].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[1].BodyNotEnalbledTexture = NULL;
	Bars[1].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[1].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[1].ButtonNotEnalbledTexture = NULL;
	Bars[1].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[1].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[1].TravellerNotEnalbledTexture = NULL;
	Bars[1].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[1].MouseOnTravellerTexture = NULL;
	// DepthBias
	Bars[2].Blob = MyManager->BlobsArr[4];
	Bars[2].Horizontal = false;
	Bars[2].Values = { -1000.0f, 1000.0f, 1.0f, 1.0f };
	Bars[2].ShowButtons = true;
	Bars[2].ObjParam = { 40.0f, 450.0f, 0.0f, 100.0f };
	Bars[2].UpSideDown = true;

	Bars[2].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[2].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[2].BodyNotEnalbledTexture = NULL;
	Bars[2].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[2].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[2].ButtonNotEnalbledTexture = NULL;
	Bars[2].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[2].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[2].TravellerNotEnalbledTexture = NULL;
	Bars[2].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[2].MouseOnTravellerTexture = NULL;
	// SlopeScaledDepthBias
	Bars[3].Blob = MyManager->BlobsArr[4];
	Bars[3].Horizontal = false;
	Bars[3].Values = { -1.0f, 1000.0f, 4.0f, 1.0f };
	Bars[3].ShowButtons = true;
	Bars[3].ObjParam = { 70.0f, 450.0f, 0.0f, 100.0f };
	Bars[3].UpSideDown = true;

	Bars[3].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[3].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[3].BodyNotEnalbledTexture = NULL;
	Bars[3].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[3].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[3].ButtonNotEnalbledTexture = NULL;
	Bars[3].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[3].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[3].TravellerNotEnalbledTexture = NULL;
	Bars[3].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[3].MouseOnTravellerTexture = NULL;

	Bars[4].Blob = MyManager->BlobsArr[4];
	Bars[4].Horizontal = false;
	Bars[4].Values = { 0.5f, 15.0f, 2.5f, 0.01f };
	Bars[4].ShowButtons = true;
	Bars[4].ObjParam = { 100.0f, 450.0f, 0.0f, 100.0f };
	Bars[4].UpSideDown = true;

	Bars[4].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[4].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[4].BodyNotEnalbledTexture = NULL;
	Bars[4].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[4].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[4].ButtonNotEnalbledTexture = NULL;
	Bars[4].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[4].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[4].TravellerNotEnalbledTexture = NULL;
	Bars[4].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[4].MouseOnTravellerTexture = NULL;

	Bars[5].Blob = MyManager->BlobsArr[4];
	Bars[5].Horizontal = false;
	Bars[5].Values = { 0.1f, 10.0f, 1.24f, 0.01f };
	Bars[5].ShowButtons = true;
	Bars[5].ObjParam = { 130.0f, 450.0f, 0.0f, 100.0f };
	Bars[5].UpSideDown = true;

	Bars[5].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[5].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[5].BodyNotEnalbledTexture = NULL;
	Bars[5].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[5].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[5].ButtonNotEnalbledTexture = NULL;
	Bars[5].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[5].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[5].TravellerNotEnalbledTexture = NULL;
	Bars[5].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[5].MouseOnTravellerTexture = NULL;

	Bars[6].Blob = MyManager->BlobsArr[4];
	Bars[6].Horizontal = false;
	Bars[6].Values = { -1.0f, 3.0f, 0.003f, 0.0005f };
	Bars[6].ShowButtons = true;
	Bars[6].ObjParam = { 160.0f, 450.0f, 0.0f, 100.0f };
	Bars[6].UpSideDown = true;

	Bars[6].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[6].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[6].BodyNotEnalbledTexture = NULL;
	Bars[6].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[6].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[6].ButtonNotEnalbledTexture = NULL;
	Bars[6].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[6].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[6].TravellerNotEnalbledTexture = NULL;
	Bars[6].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[6].MouseOnTravellerTexture = NULL;
	// Shadow Divider
	Bars[7].Blob = MyManager->BlobsArr[4];
	Bars[7].Horizontal = false;
	Bars[7].Values = { 1.0f, 8128.0f, 3192.0f, 1.0f };
	Bars[7].ShowButtons = true;
	Bars[7].ObjParam = { 190.0f, 450.0f, 0.0f, 100.0f };
	Bars[7].UpSideDown = true;

	Bars[7].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[7].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[7].BodyNotEnalbledTexture = NULL;
	Bars[7].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[7].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[7].ButtonNotEnalbledTexture = NULL;
	Bars[7].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[7].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[7].TravellerNotEnalbledTexture = NULL;
	Bars[7].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[7].MouseOnTravellerTexture = NULL;

	// DyffuseY
	Bars[8].Blob = MyManager->BlobsArr[4];
	Bars[8].Horizontal = true;
	Bars[8].Values = { -1.0f, 1.0f, 0.005f, 0.001f };
	Bars[8].ShowButtons = true;
	Bars[8].ObjParam = { 35.0f, 700.0f, 100.0f, 25.0f };
	Bars[8].UpSideDown = false;

	Bars[8].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[8].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[8].BodyNotEnalbledTexture = NULL;
	Bars[8].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[8].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[8].ButtonNotEnalbledTexture = NULL;
	Bars[8].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[8].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[8].TravellerNotEnalbledTexture = NULL;
	Bars[8].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[8].MouseOnTravellerTexture = NULL;

	// Active LIghts on Screen
	Bars[9].Blob = MyManager->BlobsArr[4];
	Bars[9].Horizontal = true;
	Bars[9].Values = { 1.0f, 3000.0f , 150.0f, 1.0f };
	Bars[9].ShowButtons = true;
	Bars[9].ObjParam = { 35.0f, 730.0f, 100.0f, 25.0f };
	Bars[9].UpSideDown = false;

	Bars[9].OsnTextureResource = MyManager->ResourceArr[12];
	Bars[9].BodyTexture = MyManager->ShaderResourceArr[12];
	Bars[9].BodyNotEnalbledTexture = NULL;
	Bars[9].ButtonsTexture = MyManager->ShaderResourceArr[13];
	Bars[9].ButtonPressTexture = MyManager->ShaderResourceArr[17];
	Bars[9].ButtonNotEnalbledTexture = NULL;
	Bars[9].MouseOnButtonTexture = MyManager->ShaderResourceArr[16];
	Bars[9].TravellerTexture = MyManager->ShaderResourceArr[14];
	Bars[9].TravellerNotEnalbledTexture = NULL;
	Bars[9].TravellerPressTexture = MyManager->ShaderResourceArr[15];
	Bars[9].MouseOnTravellerTexture = NULL;


	Buttons[1].Label = "";
	Buttons[1].Data = NULL;

	Buttons[2].Type = CHECKBOX; // CheckBox
	Buttons[2].Checked = true;
	Buttons[2].Label = "";
	Buttons[2].OsnTextureResource = MyManager->ResourceArr[5];
	Buttons[2].OsnTexture = MyManager->ShaderResourceArr[5];
	Buttons[2].IsClickTexture = MyManager->ShaderResourceArr[6];
	Buttons[2].Data = NULL;


	Buttons[3].Type = CHECKBOX; // CheckBox
	Buttons[3].Checked = true;
	Buttons[3].Label = "";
	Buttons[3].OsnTextureResource = MyManager->ResourceArr[5];
	Buttons[3].OsnTexture = MyManager->ShaderResourceArr[5];
	Buttons[3].IsClickTexture = MyManager->ShaderResourceArr[7];
	Buttons[3].Data = NULL;


	Buttons[6]._ObjParam = { 10.0f, Buttons[3]._ObjParam.y + y + 10.0f, x, y };
	Buttons[6].OsnTextureResource = MyManager->ResourceArr[5];
	Buttons[6].OsnTexture = MyManager->ShaderResourceArr[5];
	Buttons[6].IsClickTexture = MyManager->ShaderResourceArr[4];
	Buttons[6].IsMouseOnButtonTexture = NULL;
	Buttons[6].IsNotEnalbledTexture = MyManager->ShaderResourceArr[5];

	Buttons[6].Data = NULL; // ��� ������ � Buttons[4].Label 
	Buttons[6].Label = "";
	Buttons[6].SentenceIndex = -1;
	Buttons[6].Type = CHECKBOX; // Button
	Buttons[6].EditType = NULL;
	Buttons[6].Checked = true;
	Buttons[6].Enabled = true;
	Buttons[6].WaitingForKeyPress = false;
	Buttons[6].VirtualKeyIndex = 0;
	Buttons[6].SecondSlot = false;
	Buttons[6].Blob = MyManager->BlobsArr[4];


	Data4->MaxLength = 16; // ��� Edit ��������� �������� ���������� ����� ��� �������� + 1
	Data4->PosX = 10;
	Data4->PosY = 330;
	Data4->ShowType = 1;
	Data4->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data4->Render = true;
	Data4->FontType = 0;
	Data4->Level = MENU;


	Buttons[4]._ObjParam = { 10.0f, 330.0f, 0.0f, 0.0f };
	Buttons[4].OsnTextureResource = MyManager->ResourceArr[11];
	Buttons[4].OsnTexture = MyManager->ShaderResourceArr[11];
	Buttons[4].IsClickTexture = NULL;
	Buttons[4].IsMouseOnButtonTexture = NULL;
	Buttons[4].IsNotEnalbledTexture = NULL;
	Buttons[4].Data = Data4; // ��� ������ � Buttons[4].Label 
	Buttons[4].Label = "��� �����";
	Buttons[4].Type = EDIT; // Edit
	Buttons[4].EditType = ANYSYMBOL; // Edit
	Buttons[4].Checked = false;
	Buttons[4].Enabled = true;
	Buttons[4].WaitingForKeyPress = false;
	Buttons[4].VirtualKeyIndex = 0;
	Buttons[4].SecondSlot = true;
	Buttons[4].Blob = MyManager->BlobsArr[4];

	Data5->MaxLength = 32;
	Data5->PosX = 10;
	Data5->PosY = 330;
	Data5->ShowType = 1;
	Data5->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data5->Render = true;
	Data5->FontType = 0;
	Data5->Level = MENU;

	Buttons[5]._ObjParam = { 10.0f, 380.0f, 0.0f, 0.0f };
	Buttons[5].OsnTextureResource = MyManager->ResourceArr[11];
	Buttons[5].OsnTexture = MyManager->ShaderResourceArr[11];
	Buttons[5].IsClickTexture = NULL;
	Buttons[5].IsMouseOnButtonTexture = NULL;
	Buttons[5].IsNotEnalbledTexture = NULL;
	Buttons[5].Data = Data5; // ��� ������ � Buttons[4].Label 
	Buttons[5].Label = "";
	Buttons[5].Type = LABEL; // Edit
	Buttons[5].ActiveStringsList = false;
	Buttons[5].EditType = NULL;
	Buttons[5].Checked = false;
	Buttons[5].Enabled = true;
	Buttons[5].WaitingForKeyPress = true;
	Buttons[5].VirtualKeyIndex = 0;
	Buttons[5].SecondSlot = true;
	Buttons[5].Blob = MyManager->BlobsArr[4];


	Buttons[2].Type = 1; // CheckBox
	Buttons[2].Checked = true;
	Buttons[3].Type = 1; // CheckBox
	Buttons[3].Checked = true;

	// Hide Text
	Buttons[7]._ObjParam = { 10.0f, 600.0f, 0.0f, 0.0f };
	Buttons[7].OsnTextureResource = MyManager->ResourceArr[3];
	Buttons[7].OsnTexture = MyManager->ShaderResourceArr[3];
	Buttons[7].IsClickTexture = MyManager->ShaderResourceArr[19];
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
	Buttons[7].Blob = MyManager->BlobsArr[4];


	// +++++++++++++++++++     ��������     ++++++++++++++++++++++++++++++

	AnimTexture = new KF2DTextureAnimation;
	XMFLOAT4 ObjData = { 100.0f, 800.0f, 100.0f, 100.0f };
	AnimTexture->Init(hwnd, m_D3D->D3DGC, 8, 8, MyManager->ShaderResourceArr[8],
	11, 2, MyManager, ObjData, MyManager->BlobsArr[4]);
	// -------------------     ��������     ------------------------------


	Hud = new MenuControrerClass;
	Hud->Init(m_D3D->D3DGC,
		Buttons, 8, 
		Bars, 10,
		NULL, 0,	// StringsLists
		AnimTexture,	// ������� ������ �������� � ����
		{ float(screenWidth - 250) , 0.0f, 250.0f, float(screenHeight) },
		MyManager->ShaderResourceArr[18],
		MyManager->BlobsArr[4],
		m_Text
		);

	// �������� ������� Label ������ ������
	Hud->PutLabelsInsideButtons();
	// �������� ��������� HUD
	Hud->IsMenuDraw = true;
//	Hud->SetBackground(MyManager->ShaderResourceArr[0], MyManager->BlobsArr[5]);
	// ���������� HUD ��� Hud->IsMenuActive = true;
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
	// --------------------- ��� �������
	Mapping = new KFCubeMapping;
	Mapping->Init(m_D3D->D3DGC->D11_deviceContext , m_D3D->D3DGC->D11_device, 
	//MyManager->ShaderResourceArr[22],
	MyManager->ShaderResourceArr[MyManager->InitOneTexture(L"Textures/skymap.dds")],
	MyManager->BlobsArr[0]) ;

// ++++++++++++++++++++++++++++++++++++++++     LIGHT     ++++++++++++++++++++++++++++++++++++++++++++	
	m_Light = new LightClass;
	result = m_Light->Init( hwnd, m_D3D->D3DGC, m_Frustum );
	if ( !result )
	{
		MessageBox( hwnd, L"Light object initialisation failure.", Error, MB_OK );
		return false;
	}
// -----------------------------------------     LIGHT    ---------------------------------------------

// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++
	// ����������� ��� ������ ������ !!!
	m_Camera->GetPosition( m_D3D->D3DGC->CameraPosition );

	// Create the particle system object.
	SnowParticles = new SnowFallParticles;
	if (!SnowParticles )
	{
		return false;
	}

	result = SnowParticles->Initialize
		(
		hwnd,
		m_D3D->D3DGC,
		MyManager->BlobsArr[6],				// ���� �� �������� ������� ��� Particles ( ����� ��� �������� Layout )
												//		MyManager->ShaderResourceArr[20],	// �������� ������
		MyManager->ShaderResourceArr[20], //8
		1,
		1,
		1,
		m_Light								// ��������� �� ����� ����� � ������
		);
		
		TorchFire = new TorchFireParticles;
		if ( !TorchFire )
		{
			return false;
		}

		// �������������� �����
		TorchFireSmoke TorchFireSmokeInit;
		TorchFireSmokeInit.TorchPosition = {125.0f, 1.0f, 20.0f };
		TorchFireSmokeInit.FireFramesAmount = 31;	// �������� � 0  ( ����� 32 ����� )
		TorchFireSmokeInit.FireFrameStart = 0;		// 1-� ���� �������� - 0-� ( ������ ��� � �++ �� � ���� )
		TorchFireSmokeInit.SmokeFramesAmount = 4;	// 4 ����� ��������
		TorchFireSmokeInit.SmokeFrameStart = 32;	// ������ � 32 ����� 
		TorchFireSmokeInit.FireFlyFrameNumber = 49;	// ���� �� �������� ���������� FireFly
		TorchFireSmokeInit.FireFlyCutWidth = 16;	// ������ ������� ����� ��� FireFly �� ������ � ������ ( �� ������� ������ ������ )
		TorchFireSmokeInit.FireFlyCutHeight = 8;	// ������ ������� ����� ��� FireFly �� ������ � ������ ( �� ������� ������ ������ )
		TorchFireSmokeInit.UX_Amount = 8;
		TorchFireSmokeInit.VY_Amount = 8;

		result = TorchFire->Initialize
			(
			hwnd,
			m_D3D->D3DGC,
			MyManager->BlobsArr[26],				// ���� �� �������� ������� ��� Particles ( ����� ��� �������� Layout )
													//		MyManager->ShaderResourceArr[20],	// �������� ������
			MyManager->ShaderResourceArr[22], //8 22
			TorchFireSmokeInit,
			m_Light								// ��������� �� ����� ����� � ������
			);

		if ( !result )
		{
			return false;
		}

	FireParticles = new FireParticleSystem;
		if ( !FireParticles )
		{
			return false;
		}

	result = FireParticles->Initialize
	(	
		hwnd, 
		m_D3D->D3DGC, 
		MyManager->BlobsArr[24],				// ���� �� �������� ������� ��� Particles ( ����� ��� �������� Layout )
//		MyManager->ShaderResourceArr[20],	// �������� ������
		MyManager->ShaderResourceArr[8], //8
		8,
		8,
		0,
		m_Light								// ��������� �� ����� ����� � ������
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


// +++++++++++++++++++     ������ ���� �� �����     +++++++++++++++++++++++
		LightClass::PointLight AddedLight;
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
		m_Light->AddLightSource(AddedLight);
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
		m_Light->AddLightSource(AddedLight);
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


// ������������ �������� � ���������� ������
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
// ������������ �������� � ���������� ������


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



// -------------------     ������ ���� �� �����     -----------------------
		XMFLOAT3 Lpos;
		m_Light->GetLightPos(0, Lpos);
//		m_Camera->SetPosition(Lpos.x + 20, Lpos.y, Lpos.z);
// ���� ������
//		m_Camera->SetPosition( -3.05f, 14.25f, -4.97f );
//		m_Camera->SetLookAt( -3.16f, 13.35f, -5.35f );
// ���� ����
//		m_Camera->SetPosition( 328.12f, 9.85f, 298.42f );
//		m_Camera->SetLookAt( 328.10f, 9.04f, 297.84f );
// ������ ����� . ������
		m_Camera->SetPosition( 20.98f, 37.81f, 54.40f );
		m_Camera->SetLookAt( 20.70f, 37.36f, 53.55f );

		//int Index = MyManager->InitOneTexture(hwnd, L"Textures/land.jpg", m_D3D->D3DGC->D11_device, m_D3D->D3DGC->D11_deviceContext);
		//MyManager->InitTextures(hwnd, L"Textures/RcubeTextures.kaf", m_D3D->D3DGC->D11_device, m_D3D->D3DGC->D11_deviceContext);

		KFTerrain::KFLandscapeParam Land;

		Land.LowlandsCout = 50;// ���������� ��
		Land.HeightesCout = 50;// ���������� ���
		Land.MaxHeight = 50;// ������������ ������ ���
		Land.MinHeight = 10;// ����������� ������ �����
		Land.MaxRadius = 80;// ������������ ������ �����/���
		Land.MinRadius = 1;// ����������� ������ �����/���
		Land.filename = "Textures/hm2.bmp";

		float Rows = 1000.0f , Columns = 1000.0f;

		//\//\/\/\/!!!!!����������� ����� ������ ����� �������������!!!!!!/\/\/\/\/\/\

		Terrain = new KFTerrain;
		Terrain->Initialize(hwnd, m_D3D->D3DGC->D11_device, hwnd, MyManager->BlobsArr[16], m_D3D->D3DGC->D11_deviceContext,
		MyManager->ShaderResourceArr[21], //MyManager->ShaderResourceArr[MyManager->InitOneTexture(hwnd , L"Textures/moonTexture.png" , m_D3D->D3DGC->D11_device , m_D3D->D3DGC->D11_deviceContext)] 
		Rows , Columns , &Land, 1.0f/* ���������� ����� ������� */, 100 /* ���������� ��������� �� ������� ������ ������ �� X ��� */,
			100/* ���������� ��������� �� ������� ������ ������ �� Z ���  */, 49 /* ��������� ���������� � ��������� */, m_Light);

		/*
		������

		������ ����� � ��������� ������ ������ �������� �� ���������� ��������� �� ������� ������ ������ �� X ���
		������ ����� � ��������� ������ ������ �������� ������������ ��������� �� ������� ������ ������ �� Z ���

		���������� ��������� �� ������� ������ ������ �� ����� ��� ������ ���� ������ ��� ��������� ���������� � ��� ���� + 1

		���������� ����� ������� ������ ���� ������ ���� ����� 1

		������ � ������� ������� ������������� ������������ ������������ �������

		���� �� ������� ������ ������� � ������ �� ���������� �� ������ ����� � ������� ���� (�������� ������ �� ������ ������������ �����)

		����� �������� �������� ��� ������� ���� ������ ������ ��� ������ ��������� �� 10 �������

		*/

		ModelList = new KFModelList;

		ModelList->Init(hwnd, m_D3D->D3DGC, MyManager, m_Light, m_Frustum);
		ModelList->IsClusteringUse = false;

		ClusterMap = new KFClusterMap(100, 100, 100, 50, 50, 50);

		ShadowWork = new KFShadowWork;

		KFShadowWork::ObjectUsingShadows  SWobjects;

		SWobjects.ModelList = ModelList;
		SWobjects.Terrain = Terrain;

		ShadowWork->Init(hwnd, m_D3D->D3DGC, MyManager, m_Light, m_Frustum, SWobjects);
		ShadowWork->UsingShaderIndex = 10;

		return true;
}

// ++++++++++++++++++++++++++++++     FRAME     +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GraphicsClass::Frame( FPSTimers &Counters, DXINPUTSTRUCT& InputStruct1)
{
	bool result = true;
	
	char Str1[128];

	++ShadowFrameCounts;
	fpstimers = Counters;
// ������ ������ � ��������� ���������� ��� ������. ����� ������������ ��� ��������� � FPS �������
	m_Text->fpstimers = Counters;

// ++++++++++++++++++++     ������� ����     ++++++++++++++++++++++++++++++++++++
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
// ++++++++++++++++++++     ������� ����     ++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++	
	// �������� ��������������
	// ���� 540 ns    ���  517 ns   
//	char Str[25];// = new char [25];
//	Profile->StartTimer();
	// �������� ��������������
	SnowParticles->Frame( Counters );


// ������
// https://msdn.microsoft.com/en-us/library/ms682516(v=vs.85).aspx
// https://stackoverflow.com/questions/10673585/start-thread-with-member-function
// http://www.cyberforum.ru/visual-cpp/thread12242.html
// ��� _beginthreadex vs CreateThread C++
// https://stackoverflow.com/questions/331536/windows-threading-beginthread-vs-beginthreadex-vs-createthread-c
// ��-������
// http://www.quizful.net/post/multithreading-cpp11
//	thread t( [this] { GraphicsClass::SnowParticles->Frame( fpstimers ); } );
//	thread t( &GraphicsClass::SnowThread, &fpstimers );
//	t.join();
//	DWORD   ThreadId;
//	unsigned  ThreadAdr;
//	CreateThread(NULL,0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&Fuck), &fpstimers , CREATE_SUSPENDED , &ThreadId ); 
//	CreateThread( NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>( GraphicsClass::SnowThread ), &fpstimers, CREATE_SUSPENDED, &ThreadId );
//	_beginthreadex( NULL, 0, reinterpret_cast<_beginthreadex_proc_type>( &Fuck ), &fpstimers, CREATE_SUSPENDED, &ThreadAdr );
//	_beginthreadex(NULL,0, reinterpret_cast<_beginthreadex_proc_type>(SnowThread), &fpstimers , CREATE_SUSPENDED , &ThreadAdr );

//	Profile->StopTimer( Str );
//	m_Text->UpdateSentence( 4, Str, 100, 160 );

	TorchFire->Frame( Counters );
	FireParticles->Frame( Counters );
	// �������� ��������������
//	Profile->StopTimer(Str);
//	m_Text->UpdateSentence(5, Str, 100, 160);
	// �������� ��������������
// --------------------------------------------------  PARTICLE SYSTEM  -----------------------------

	// �������� ��������������
	// ���� 540 ns    ���  517 ns   
//		char Str[128];// = new char [25];
//		Profile->StartTimer();
		// �������� ��������������

// ++++++++++++    ��� ����� ������ ����     +++++++++++++++++++
	if ( m_D3D->D3DGC->ShadowsOn && ShadowFrameCounts > 0 )//Counters.FpsRate / 25
	{
		ShadowWork->RenderSpotLightsSadowMaps( m_Light->SpotLightsWithShadowsIndexes );
		ShadowFrameCounts = 0;
	}
	// �������� ��������������
//		Profile->StopTimer(Str);
//		m_Text->UpdateSentence(5, Str, m_Text->GetPosX(5), m_Text->GetPosY(5));
	// �������� ��������������

// ++++++++++++++  FRUSTUM +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SHADOWMAP ���������� ������ FRUSTUM, ������� ����� ����� SHADOWMAP ���������� FRUSTUM �� �����

	// ������� ������ �������� �� ����������
	//m_Frustum->ConstructFrustum(SCREEN_DEPTH, m_D3D->D3DGC->ProjectionMatrix, m_D3D->D3DGC->ViewMatrix);

	// ����������� � ���������
	m_Frustum->ConstructFrustumNew( m_D3D->D3DGC->ViewProjection );

// ----------------------  FRUSTUM    --------------------------------------------------

// ++++++++++++    ��� ����� ����� ����     +++++++++++++++++++
// ���� 540 ns    ���  517 ns   
		char Str[25];// = new char [25];
		Profile->StartTimer();
// �������� ��������������

// ��������� ���� �����
// ����������� ����� ��������� ����� !!!
	m_Light->Frame();
// ------------------------------------------------------------

// �������� ��������������
	Profile->StopTimer(Str);
	strcpy_s(Str1, 128, "LightsFrame() : ");
	strcat_s(Str1, 128, Str);
	m_Text->UpdateSentence(13, Str1, m_Text->GetPosX( 13 ), m_Text->GetPosY( 13 ) );
// �������� ��������������


	result = Render( InputStruct1.m_mouseX, InputStruct1.m_mouseY);
	if(!result)
	{  
		return false;
	}

	return true;
}


bool GraphicsClass::Render(int& mouseX, int& mouseY )
{
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->SetBackBufferRenderTarget();
	m_D3D->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

// � ������ ������� ������������ ������ � �������� � �������
	UpdateCamera();

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + ������ CUBEMAP	
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	XMMATRIX Scale = XMMatrixScaling(1000.0f, 1000.0f, 1000.0f);
	XMMATRIX Translation = XMMatrixTranslation(m_D3D->D3DGC->CameraPosition.x, m_D3D->D3DGC->CameraPosition.y, m_D3D->D3DGC->CameraPosition.z);
	XMMATRIX mtr = Scale * Translation * m_D3D->D3DGC->ViewProjection;
	MyManager->SetActiveShadersInProgramm(0);
	Mapping->Render(mtr);
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// - ������ CUBEMAP	
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// �������������� ������ ������ ��� ���� �������� 
	UpdateConstantBuffer();

	char Str[128];
	char Str1[128];
//	sprintf_s(Str, 50, "Spec Power = %8.5f", m_Light->GetSpecularPower());
//	m_Text->UpdateSentence(4, Str, 100, 160);

//	sprintf_s(Str, 70, "Cam pos: X: %4.2f , Y: %4.2f , Z: %4.2f", m_D3D->D3DGC->CameraPosition.x, m_D3D->D3DGC->CameraPosition.y, m_D3D->D3DGC->CameraPosition.z );
//	m_Text->UpdateSentence(3, Str, 100, 130);
	float angel = Hud->GetScrollBarValue( 2 );
	sprintf_s( Str, 70, "DepthBias : %4.2f", angel );
	m_Text->UpdateSentence( 3, Str, 100, 130 );

//	float angel = atan2(0.0f - m_D3D->D3DGC->CameraPosition.x, 0.0f - m_D3D->D3DGC->CameraPosition.z) * 57.29578049f;

	m_Camera->GetLookAt( CamLook.Vec );
//	sprintf_s( Str, 70, "Cam LookAt: X: %4.2f , Y: %4.2f , Z: %4.2f , W: %4.2f", CamLook.Fl4.x, CamLook.Fl4.y, CamLook.Fl4.z, CamLook.Fl4.w );
//	m_Text->UpdateSentence( 2, Str, 100, 100 );
	angel = Hud->GetScrollBarValue( 3 );
	sprintf_s(Str, 70, "SlopeScaledDepthBias : %4.2f", angel );
	m_Text->UpdateSentence(2, Str, 100, 100);

	angel = Hud->GetScrollBarValue ( 1 );
	sprintf_s ( Str, 50, "DiffuseX = %6.5f", angel );
  //	sprintf_s( Str, 50, "Active P = %i", m_ParticleSystem->GetActiveParticleAmmount() );
	m_Text->UpdateSentence ( 4, Str, 100, 160 );

	angel = Hud->GetScrollBarValue( 4 );
	sprintf_s( Str, 50, "PCF_Amount = %4.2f", angel );
	m_Text->UpdateSentence( 5, Str, 100, 190 );

	angel = Hud->GetScrollBarValue( 5 );
	sprintf_s( Str, 50, "PCF_Step = %4.2f", angel );
	m_Text->UpdateSentence( 6, Str, 100, 220 );
//	sprintf_s( Str, 50, "Lights %i", m_Light->GetActiveLghtNumber() );
//	m_Text->UpdateSentence( 5, Str, 100, 160 );

	angel = Hud->GetScrollBarValue( 6 );
	sprintf_s( Str, 50, "ShadowCLAMP = %1.4f", angel );
	m_Text->UpdateSentence( 7, Str, 100, 250 );

	angel = Hud->GetScrollBarValue( 7 );
	sprintf_s( Str, 50, "Shadow Divider = %4.1f", angel );
	m_Text->UpdateSentence( 8, Str, 100, 280 );

// LightInfo
	sprintf_s( Str, 50, "Active Lights = %d", m_Light->mActiveLights );
	m_Text->UpdateSentence( 11, Str, 100, 310 );

	sprintf_s( Str, 50, "Visible Lights = %d", m_Light->mVisibleLights );
	m_Text->UpdateSentence( 12, Str, 100, 340 );

	angel = Hud->GetScrollBarValue(8);
	sprintf_s(Str, 50, "DiffuseY = %1.3f", angel);
	m_Text->UpdateSentence(14, Str, 100, 470);

	m_Text->UpdateSentence( 15, m_Text->GetSentenceText(15), m_Text->GetPosX(15), m_Text->GetPosY(15) );

// +++++++++++++++++++++++++++++++++ ���� �� ���������� ��������������� ++++++++++++++++++++++++++++++++
//	m_D3D->TurnOnAlphaBlending();

// +++++++++++++++++++++++++++++ ���� �� ���������� Z �������
//	m_D3D->TurnZBufferOn();
/*
// ������ ������ � ������� ����� �0 � ������� ��� ����
XMFLOAT3 Pos;
XMFLOAT3 Dir;
m_Light->GetLightPos(0, Pos);
m_Camera->SetPosition( Pos.x, Pos.y, Pos.z );
m_Light->GetLightDirection(0, Dir);
m_Camera->SetLookAt(Dir.x * 1000.0f, Dir.y * 1000.0f, Dir.z * 1000.0f );
m_Camera->FirstPersonCam = false;
m_Camera->Render();
// ---------------------------------------------------------------------------
*/
// +++++++++++++++++++++    ������ ������� �� �����    +++++++++++++++++++++++++++++++++++++++++++
	MyManager->SetActiveShadersInProgramm(8);
	// ������������� ����� �� ��� ����� ��� ��������� ���� ������� �� ������
	// ����� �� ����������� ��� � ����� ����� ���
	m_D3D->D3DGC->D11_deviceContext->PSSetSamplers(0, 1, &m_D3D->D3DGC->CLight_DiffuseSampler);
	m_D3D->D3DGC->D11_deviceContext->PSSetSamplers(1, 1, &m_D3D->D3DGC->CLight_SampleTypeClamp);
	m_D3D->D3DGC->D11_deviceContext->PSSetSamplers(2, 1, &m_D3D->D3DGC->CLight_cmpSampler);
	m_D3D->D3DGC->D11_deviceContext->OMSetDepthStencilState(m_D3D->D3DGC->m_depthStencilState, 0);
	m_D3D->D3DGC->D11_deviceContext->RSSetState(m_D3D->D3DGC->m_rasterState);
	m_D3D->D3DGC->D11_deviceContext->OMSetRenderTargets(1, &m_D3D->D3DGC->BackBuffer_RTV, m_D3D->D3DGC->m_depthStencilView);
	// �������� ��������������
	// ���� 540 ns    ���  517 ns   
//			char Str[25];// = new char [25];
//		Profile->StartTimer();
	// �������� ��������������
	Terrain->Frame(true, m_Camera->position);
	// �������� ��������������
//		Profile->StopTimer(Str);
//		m_Text->UpdateSentence(5, Str, 100, 160);
	// �������� ��������������
	Terrain->LightRender();

//	char Str[25];// = new char [25];
	Profile->StartTimer();

	ModelList->Frame();

	Profile->StopTimer(Str);

	strcpy_s(Str1, 128, "Frustum �������� : ");
	strcat_s(Str1, 128, Str);
	m_Text->UpdateSentence(10, Str1, m_Text->GetPosX( 10 ), m_Text->GetPosY( 10 ) );

	ModelList->LightRender();


	m_D3D->TurnOffAlphaBlending();

	m_D3D->SetDefaultResterizeState();

	// �������� ��������������
//	Profile->StopTimer(Str);
//	m_Text->UpdateSentence(4, Str, 100, 160);
	// �������� ��������������
	// ++++++++    FXAA    +++++++++++	
	// �����������
	m_D3D->SetCullNoneRState();
	// ������������� ������� FXAA
	MyManager->SetActiveShadersInProgramm(9);
	// ��������� ����� FXAA
	if ( m_D3D->D3DGC->EnableFXAA )
	{
		m_D3D->FXAAScene();
	}
	// ++++++++    FXAA    +++++++++++	
	// �������� ��������������
//			Profile->StopTimer(Str);
//			m_Text->UpdateSentence(4, Str, 100, 160);
	// �������� ��������������

//	m_D3D->TurnOnAlphaBlending();

	// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++
	m_D3D->TurnOnParticleBlending();
	m_D3D->TurnZBufferOff();
	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	
	// SNOW ������� ������
	MyManager->SetActiveShadersInProgramm( 3 );
	SnowParticles->Render();

//	Profile->StopTimer( Str );
//	m_Text->UpdateSentence( 5, Str, 100, 160 );


	// �������� ��������������
	// ���� 540 ns    ���  517 ns   
	//		char Str[25];// = new char [25];
//	Profile->StartTimer();
	// FIRE �������� ������������� �����
	MyManager->SetActiveShadersInProgramm( 12 );
	FireParticles->Render();
//	Profile->StopTimer( Str );
//	m_Text->UpdateSentence( 5, Str, 100, 160 );
	// TorchFire �����, �����

	// �������� ��������������
	// ���� 540 ns    ���  517 ns   
	//		char Str[25];// = new char [25];
//	Profile->StartTimer();
	// �������� ��������������	
	MyManager->SetActiveShadersInProgramm( 13 );
	TorchFire->Render();
//	Profile->StopTimer( Str );
//	m_Text->UpdateSentence( 4, Str, 100, 160 );

	// �������� ��������������
	m_D3D->TurnZBufferOn();
	// --------------------------------------------------  PARTICLE SYSTEM  ------------------------------

	// -------------------------------- ���� �� ���������� Z �������

	// ++++++++++++++++++++++++++++++++++���� � ���������� Z �������

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  2D TEXTURE + FONT  +++++++++++++++++++++++++++++++++++++++++++++
	// Get the world, view, and projection matrices from the camera and d3d objects.

	// ��� ����++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Turn the Z buffer back on now that all 2D rendering has completed
	//ed->Draw(m_D3D->D3DGC->OrthoMatrix , m_D3D->D3DGC->WorldMatrix);


	m_D3D->TurnOnAlphaBlending();
	m_D3D->TurnZBufferOff();

	// ++++++++++++++++++++++++     ������ ����� � ����     +++++++++++++++++++++++++++++
	MyManager->SetActiveShadersInProgramm(1);
	// �������� ��������������
	// ���� 540 ns    ���  517 ns   
	//	char Str[25];// = new char [25];
	//	Profile->StartTimer();
	// �������� ��������������
	if (!Hud->GetButtonState(7))
		result = m_Text->Render( 0 );
/*
	// ��������� ����
	m_Light->LightRender(0, 0);
	else
	m_Light->LightRender(0, 1);
*/
	// �������� ��������������
	//	Profile->StopTimer(Str);
	//	m_Text->UpdateSentence(4, Str, 100, 160);
	// �������� ��������������
/*	if (!result)
	{
		return false;
	}
*/	MyManager->SetActiveShadersInProgramm(2);
	// ++++++++++++++++++++++++     ������ ����� � ����     +++++++++++++++++++++++++++++

	// ++++++++++++++++++++++++     ������ ����� � HUD      +++++++++++++++++++++++++++++
//		MyManager->SetActiveShadersInProgramm(2);

	if ( Hud->IsMenuDraw )
	{
		// �������� ��������������
		// ���� 540 ns    ���  517 ns   
		//	char Str[25];// = new char [25];
		//	Profile->StartTimer();
		// �������� ��������������
		Hud->Draw();
		// �������� ��������������
		//	Profile->StopTimer(Str);
		//	m_Text->UpdateSentence(4, Str, 100, 160);
		// �������� ��������������
		MyManager->SetActiveShadersInProgramm(1);
		result = m_Text->Render( 2 ); // ������ ����� ��� HUD
		MyManager->SetActiveShadersInProgramm(2);
	}

	m_D3D->D3DGC->EnableFXAA = Hud->GetButtonState(3);
	m_D3D->D3DGC->ShadowsOn = Hud->GetButtonState(2);
	m_D3D->D3DGC->SoftShadowsOn = Hud->GetButtonState( 6 );
	m_D3D->D3DGC->PCF_Amount = Hud->GetScrollBarValue( 4 );
	m_D3D->D3DGC->PCF_Step = Hud->GetScrollBarValue( 5 );
	m_D3D->D3DGC->ShadowCLAMP = Hud->GetScrollBarValue( 6 );
	m_D3D->D3DGC->Shadow_Divider = Hud->GetScrollBarValue( 7 );

	// ����������� ������
	FireParticles->ChangeActiveParticleAmount((int)Hud->GetScrollBarValue(9));
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

// ++++++++++++++++++++++++     ������ ����� � HUD     +++++++++++++++++++++++++++++

// ++++++++++++++++++++++++     ������ ����� ��� ����     +++++++++++++++++++++++++++++
	if ( MainMenu->IsMenuDraw )
	{
		// �������� ��������������
		// ���� 540 ns    ���  517 ns   
//			char Str[25];// = new char [25];
//			Profile->StartTimer();
		// �������� ��������������
		m_D3D->BlurScene(MyManager->GetComputeShader(0), MyManager->GetComputeShader(1), 2);
		// �������� ��������������
//			Profile->StopTimer(Str);
//			m_Text->UpdateSentence(13, Str, m_Text->GetPosX( 13 ), m_Text->GetPosY( 13 ) );
		MainMenu->Draw();
		MyManager->SetActiveShadersInProgramm(1);
		result = m_Text->Render( 1 );	// ������ ����� ��� ����
		MyManager->SetActiveShadersInProgramm(2);
	}
// ++++++++++++++++++++++++     ������ ����� ��� ����     +++++++++++++++++++++++++++++

		// ������ ������ ������
		m_Bitmap->ChangeMousePosition( mouseX, mouseY );
		m_Bitmap->Draw();
		// ������ ������ ������

//	if (IsSettingsMenuOn)
		//			Settingsmenu->Draw(m_D3D->D3DGC->OrthoMatrix) ;

		m_D3D->SetBackBufferRenderTarget();


	// ++++++++++++++++++++++++   ����������� ������� ������   ++++++++++++++++++++++++++++++++



	m_D3D->TurnOffAlphaBlending();

	m_D3D->TurnZBufferOn();
	// -----------------------   ����������� ������� ������   --------------------------------


	//	m_D3D->TurnZBufferOff() ;

	//	m_D3D->TurnZBufferOn() ;

	// ----------------------------------���� � ���������� Z �������


	m_D3D->EndScene();

	return true;
}

void GraphicsClass::UpdateCamera()
{
	// � ������ ������� ������������ ������ � �������� � �������
	m_Camera->Render();
	// ������� ������
	m_Camera->GetPosition( m_D3D->D3DGC->CameraPosition );
	// ��������� ������� ������
	m_Camera->GetViewMatrix( m_D3D->D3DGC->ViewMatrix );
	// ������������ ������� ���������� � m_D3D->D3DGC->ProjectionMatrix; 
	// Identity ������� ���������� � m_D3D->D3DGC->WorldMatrix; 
	// Orthomatrix ���������� � m_D3D->D3DGC->OrthoMatrix
	m_D3D->D3DGC->ViewProjection = m_D3D->D3DGC->ViewMatrix * m_D3D->D3DGC->ProjectionMatrix;
}

void GraphicsClass::UpdateConstantBuffer()
{
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++     �������������� ������ ������ ��� ���� ��������     ++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	KFModel::cbPerObject cbPerObj;
	cbPerObj.World = XMMatrixTranspose( m_D3D->D3DGC->WorldMatrix );
	cbPerObj.View = XMMatrixTranspose( m_D3D->D3DGC->ViewMatrix );
	cbPerObj.Projection = XMMatrixTranspose( m_D3D->D3DGC->ProjectionMatrix );
	cbPerObj.ViewProjection = XMMatrixTranspose( m_D3D->D3DGC->ViewProjection );
	cbPerObj.Ortho = XMMatrixTranspose( m_D3D->D3DGC->OrthoMatrix );
	cbPerObj.cameraPosition = m_D3D->D3DGC->CameraPosition;
	RCube_VecFloat34 Temp;
	Temp.Vec = XMQuaternionRotationMatrix( m_D3D->D3DGC->ViewMatrix );
	cbPerObj.TransposedCameraRotation2 = Temp.Fl4;

	m_D3D->D3DGC->D11_deviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	m_D3D->D3DGC->D11_deviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
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