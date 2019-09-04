#include "stdafx.h"

// I have kept this class entirely empty for now as we are just building the framework for this tutorial.
////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
			   TheradStarted = false;
				  ThreadDone = false;

					   m_D3D = nullptr;
					 Profile = nullptr;
				   m_Frustum = nullptr;
					m_Camera = nullptr;
				   MyManager = nullptr;
					m_Bitmap = nullptr;

					MainMenu = nullptr;
				 AnimTexture = nullptr;
						 Hud = nullptr;
			 FontSettingsHud = nullptr;

// + Particel systems
				   TorchFire = nullptr;
// - Particel systems

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
	  CubeMapIndex = -1;

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}


GraphicsClass::~GraphicsClass()
{

	delete Snow;

	delete [] Global;
	delete [] Str1;

	Strings.clear();

	RCUBE_DELETE ( RCubeRender );
  RCUBE_SHUTDOWN ( TorchFire );
    RCUBE_DELETE ( FontSettingsHud );
	RCUBE_DELETE ( Hud );
	RCUBE_DELETE ( AnimTexture );
	RCUBE_DELETE ( MainMenu );
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


bool GraphicsClass::Initialize(HWND hwnd , XMFLOAT4& _SCR_SCALE, int& WindowPosX, int& WindowPosY, InputClass* _Input, TimerClass* _Timer, PhysXControler* PhysX )
{
	bool result;
//	XMMATRIX baseViewMatrix;

	const WCHAR *Error = L"Graphics Error";

//	screenWidth = _screenWidth;
//	screenHeight = _screenHeight;

//	g_WindowPosX = WindowPosX ;
//	g_WindowPosY = WindowPosY ;

// Проверка выравнивания
// http://stackoverflow.com/questions/22133742/dx11-crash-when-accessing-xmmatrix
//	siz = __alignof(D3DClass);

	Profile = new RCubeProfiling;
	Profile->Initialize ();

	m_Frustum = new FrustumClass;

	m_D3D = new D3DClass;

	result = m_D3D->Initialize(hwnd, _SCR_SCALE, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, m_Frustum);
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
	_PhysX = PhysX;
	// Инициализируем глобальные размеры окна и положение окна не экране
//	m_D3D->D3DGC->ScreenScale = _SCR_SCALE;
//	m_D3D->D3DGC->ScreenWidth = ( int ) _SCR_SCALE.x;
//	m_D3D->D3DGC->ScreenHeight = ( int ) _SCR_SCALE.y;
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
	MyManager->Init(m_D3D->D3DGC, _Timer, PhysX );

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

	vector < RCube_Font_Style* > FontList;
	
	// Setting Shader for rendering fonts
	MyManager->TextShaderIndex = MyManager->GetShaderIndexByName ( L"Font" );
	// Инициализируем систему рисования 
	RCubeRender = new RenderClass ( m_D3D->D3DGC, MyManager, m_D3D, m_Frustum );


	Font_Param* Fparam = new Font_Param;

// Font TYPE 0
	Fparam->Font_Path_Name = L"Fonts/BuxtonSketch.ttf";
	Fparam->FontSize = 36.0f;
	Fparam->Outlined = true;
	Fparam->OutlineWidth = 3;

	// Про кисти градиентов
	// https://msdn.microsoft.com/en-us/library/dd368041(v=vs.85).aspx
	Fparam->gradientStops[0].color = D2D1::ColorF ( 0x00, 0x00, 0x00, 1.0f );
	Fparam->gradientStops[0].position = 0.0f;
	Fparam->gradientStops[1].color = D2D1::ColorF ( 0xDC, 0x6A, 0x2E, 1.0f );
	Fparam->gradientStops[1].position = 0.8f;
	Fparam->gradientStops[2].color = D2D1::ColorF ( 0x81, 0x20, 0x40, 1.0f );
	Fparam->gradientStops[2].position = 1.0f;
	Fparam->GradientPos = { 0.0f, -20.0f, 0.0f, 20.0f };
	Fparam->OutlineColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	FontList.push_back ( RCubeRender->Create_RCube_FontStyle ( Fparam ) );

// Font TYPE 1
	Fparam->Font_Path_Name = L"Fonts/8708_AA_Akashi.ttf";
	Fparam->FontSize = 36.0f;
	Fparam->Outlined = true;
	Fparam->OutlineWidth = 6;

	Fparam->gradientStops[0].color = D2D1::ColorF ( D2D1::ColorF::Turquoise, 1 );
	Fparam->gradientStops[0].position = 0.0f;
	Fparam->gradientStops[1].color = D2D1::ColorF ( D2D1::ColorF::RosyBrown, 1 );
	Fparam->gradientStops[1].position = 0.8f;
	Fparam->gradientStops[2].color = D2D1::ColorF ( D2D1::ColorF::White, 1 );
	Fparam->gradientStops[2].position = 1.0f;
	Fparam->GradientPos = { 0.0f, -20.0f, 0.0f, 0.0f };
	Fparam->OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	FontList.push_back ( RCubeRender->Create_RCube_FontStyle ( Fparam ) );


// Font TYPE 2
	Fparam->Font_Path_Name = L"Fonts/comic.ttf";
	Fparam->FontSize = 28.0f;
	Fparam->Outlined = true;
	Fparam->OutlineWidth = 4;

	Fparam->gradientStops[0].color = D2D1::ColorF ( D2D1::ColorF::YellowGreen, 1 );
	Fparam->gradientStops[0].position = 0.0f;
	Fparam->gradientStops[1].color = D2D1::ColorF ( D2D1::ColorF::Violet, 1 );
	Fparam->gradientStops[1].position = 0.8f;
	Fparam->gradientStops[2].color = D2D1::ColorF ( D2D1::ColorF::White, 1 );
	Fparam->gradientStops[2].position = 1.0f;
	Fparam->GradientPos = { 0.0f, -10.0f, 0.0f, 0.0f };
	Fparam->OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	FontList.push_back ( RCubeRender->Create_RCube_FontStyle ( Fparam ) );


// Font TYPE 3
	Fparam->Font_Path_Name = L"Fonts/BrushScriptStd.ttf";
	Fparam->FontSize = 24.0f;
	Fparam->Outlined = true;
	Fparam->OutlineWidth = 3;

	Fparam->gradientStops[0].color = D2D1::ColorF ( D2D1::ColorF::Plum, 1 );
	Fparam->gradientStops[0].position = 0.0f;
	Fparam->gradientStops[1].color = D2D1::ColorF ( D2D1::ColorF::Chocolate, 1 );
	Fparam->gradientStops[1].position = 0.8f;
	Fparam->gradientStops[2].color = D2D1::ColorF ( D2D1::ColorF::Silver, 1 );
	Fparam->gradientStops[2].position = 1.0f;
	Fparam->GradientPos = { 0.0f, -10.0f, 0.0f, 0.0f };
	Fparam->OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	FontList.push_back ( RCubeRender->Create_RCube_FontStyle ( Fparam ) );

// Font TYPE 4
	Fparam->Font_Path_Name = L"Fonts/RAVIE.TTF";
	Fparam->FontSize = 24.0f;
	Fparam->Outlined = true;
	Fparam->OutlineWidth = 2;

	Fparam->gradientStops[0].color = D2D1::ColorF ( D2D1::ColorF::Khaki, 1 );
	Fparam->gradientStops[0].position = 0.0f;
	Fparam->gradientStops[1].color = D2D1::ColorF ( D2D1::ColorF::Yellow, 1 );
	Fparam->gradientStops[1].position = 0.8f;
	Fparam->gradientStops[2].color = D2D1::ColorF ( D2D1::ColorF::Olive, 1 );
	Fparam->gradientStops[2].position = 1.0f;
	Fparam->GradientPos = { 0.0f, -20.0f, 0.0f, 0.0f };
	Fparam->OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	FontList.push_back ( RCubeRender->Create_RCube_FontStyle ( Fparam ) );

// Font TYPE 5
	Fparam->Font_Path_Name = L"Fonts/Tahoma.ttf";
	Fparam->FontSize = 24.0f;
	Fparam->Outlined = true;
	Fparam->OutlineWidth = 2;

	Fparam->gradientStops[0].color = D2D1::ColorF ( D2D1::ColorF::Beige, 1 );
	Fparam->gradientStops[0].position = 0.0f;
	Fparam->gradientStops[1].color = D2D1::ColorF ( D2D1::ColorF::Brown, 1 );
	Fparam->gradientStops[1].position = 0.8f;
	Fparam->gradientStops[2].color = D2D1::ColorF ( D2D1::ColorF::WhiteSmoke, 1 );
	Fparam->gradientStops[2].position = 1.0f;
	Fparam->GradientPos = { 0.0f, -50.0f, 0.0f, -5.0f };
	Fparam->OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	FontList.push_back ( RCubeRender->Create_RCube_FontStyle ( Fparam ) );

	delete Fparam;

	// Новая система шрифтов
	char* Text = new char [50];
	WCHAR* WText = new WCHAR [50];
	size_t FontsNumber = FontList.size ();
	for (size_t i = 0; i < FontsNumber; ++i)
	{
		MyManager->AddFont ( RCubeRender->Create_RCube_Font ( FontList[i], CharStr ) );
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

	UINT i = 0;
	UINT j = ( UINT ) FontList.size ();
	while ( i < j )
	{
		RCUBE_DELETE ( FontList[i] );
		++i;
	};

	FontList.clear ();


// +++++++++++++++++++++   Custom Cursor   ++++++++++++++++++++++++++++++++++
	XMFLOAT4 MouseCursorPos = {0.0f, 0.0f, 30.0f, 30.0f};
	{
		// Инициализируцем буферы для m_Bitmap
		int TempIndex = MyManager->Create_Flat_Obj_Buffers ( NO_CPU_ACCESS_BUFFER, 4, 6, MyManager->TexturesArr[10]->SRV );
		m_Bitmap = new FlatObjectClass;
		hr = m_Bitmap->Init ( m_D3D->D3DGC->ScreenWidth, m_D3D->D3DGC->ScreenHeight,
			MouseCursorPos,
			MyManager->TexturesArr[10]->SRV,
			NO_FLIP,
			MyManager->Get_Flat_ObjectBuffers_ByIndex ( TempIndex )
		);
		m_Bitmap->BuffersIndex = TempIndex;
	}


	m_Bitmap->ShaderIndex = MyManager->Temp_Font_StyleShaderIndex = MyManager->GetShaderIndexByName(L"KF2DObj");

	if (FAILED( hr ))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", Error, MB_OK);
		return false;
	}
// ---------------------   Custom Cursor   ---------------------------------

	UpdateConstantBuffer ();

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++			РИСУЕМ ТЕКСТОМ на ТЕКСТУРЕ        +++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Fparam = new Font_Param;
	Fparam->Font_Path_Name = L"Fonts/BuxtonSketch.ttf";
	Fparam->FontSize = 24.0f;
	Fparam->Outlined = true;
	Fparam->OutlineWidth = 3;

	Fparam->gradientStops[0].color = D2D1::ColorF ( 0xf0, 0x00, 0x00, 1 );
	Fparam->gradientStops[0].position = 0.0f;
	Fparam->gradientStops[1].color = D2D1::ColorF ( 0xDC, 0x6A, 0x2E, 1 );
	Fparam->gradientStops[1].position = 0.8f;
	Fparam->gradientStops[2].color = D2D1::ColorF ( 0x81, 0x20, 0x40, 1 );
	Fparam->gradientStops[2].position = 1.0f;
	Fparam->GradientPos = { 0.0f, -10.0f, 0.0f, 10.0f };
	Fparam->OutlineColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	RCube_Font_Style* Temp_Font_Style = RCubeRender->Create_RCube_FontStyle ( Fparam );

	m_D3D->TurnOnTextOnTextureBlending();
	m_D3D->TurnZBufferOff();
	m_D3D->SetCullNoneResterizeState ();
	m_D3D->D3DGC->DX_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_D3D->D3DGC->DX_deviceContext->OMSetRenderTargets( 1, &m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, NULL ); //m_D3D->D3DGC->m_depthStencilView
	m_D3D->D3DGC->DX_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );

//	Profile->StartTimer();

	RCubeRender->Render_Font_OnTexture ( MyManager->TexturesArr[1]->SRV,
										 Fparam->Font_Path_Name,
											 L"Анимация OFF",
											 Temp_Font_Style
	);

	RCubeRender->Render_Font_OnTexture ( MyManager->TexturesArr[7]->SRV,
										 Fparam->Font_Path_Name,
											 L"FXAA ON",
											 Temp_Font_Style 
		);

	RCubeRender->Render_Font_OnTexture ( MyManager->TexturesArr[6]->SRV,
										 Fparam->Font_Path_Name,
											 L"Shadows ON",
											 Temp_Font_Style
		);

	RCubeRender->Render_Font_OnTexture ( MyManager->TexturesArr[2]->SRV,
										 Fparam->Font_Path_Name,
											 L"Анимация ON",
											 Temp_Font_Style 
	);

	RCubeRender->Render_Font_OnTexture( MyManager->TexturesArr[4]->SRV,
										Fparam->Font_Path_Name,
											L"Soft Shadows",
											Temp_Font_Style
	);

	RCubeRender->Render_Font_OnTexture(MyManager->TexturesArr[3]->SRV,
										Fparam->Font_Path_Name,
											L"Text OFF",
											Temp_Font_Style 
	);


//	Profile->StopTimer(Global);

	m_D3D->D3DGC->DX_deviceContext->ClearRenderTargetView( m_D3D->D3DGC->BackBuffer_ProxyTextureRTV, m_D3D->D3DGC->ZeroColor );

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();
	RCUBE_DELETE ( Fparam );
	RCUBE_DELETE ( Temp_Font_Style );

// ------------------------			  РИСУЕМ ТЕКСТОМ на ТЕКСТУРЕ            -------------------------------



	SENTENCE_INIT_DATA* Data = new SENTENCE_INIT_DATA;
	ZeroMemory(Data, sizeof(SENTENCE_INIT_DATA));
	int Number;
	Data->MaxLength = 128;
	Data->PosX = 100;
	Data->PosY = 10;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 1;
	Data->Level = INGAME_TEXT;
	Number = MyManager->AddSentence ( Data, "FPS" );

	Data->MaxLength = 128;
	Data->PosX = 100;
	Data->PosY = 60;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	Data->Level = INGAME_TEXT;
	Number = MyManager->AddSentence ( Data, "CPU" );


	Data->MaxLength = 128;
	Data->PosX = 100;
	Data->PosY = 110;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	Data->Level = INGAME_TEXT;
	Number = MyManager->AddSentence(Data, "W - Вперёд, S - Назад");

	Data->MaxLength = 128;
	Data->PosY = 160;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	Number = MyManager->AddSentence(Data, "R - Вверх, F - вниз");

	Data->MaxLength = 128;
	Data->PosY = 210;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 4;
	Number = MyManager->AddSentence(Data, "A - Влево, D - вправо");

	Data->MaxLength = 128;
	Data->PosY = 260;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
//	Data->Level = 0;
	Number = MyManager->AddSentence(Data, "Это тест Scrolling строки.");

	Data->MaxLength = 128;
	Data->PosY = 310;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
//	Data->Level = 0;
	Number = MyManager->AddSentence(Data, m_D3D->GetVideoCardString());

	Data->MaxLength = 128;
	Data->PosY = 360;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
	//	Data->Level = 0;
	Number = MyManager->AddSentence( Data, m_D3D->GetVideoCardString() );

	Data->MaxLength = 128;
	Data->PosY = 410;
	Data->ShowType = SHOW;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 0;
	//	Data->Level = 0;
	Number = MyManager->AddSentence( Data, m_D3D->GetVideoCardString() );
	// 9
	Data->MaxLength = 128;
	Data->PosY = 1000;
	Data->FontType = 3;
//	Data->ShowType = SHOW_FROM_DIM;
//	Data->HideType = HIDE_TO_DIM;
	Number = MyManager->AddSentence(Data, "Keys: W,S,A,D,R,F | 8,9 - Light Rotate | Num key -/+ Sound | P - Exit");

	// 10
	Data->FontType = 2;
	Data->MaxLength = 128;
	Data->PosY = 460;
	Data->ShowType = SHOW;
	Number = MyManager->AddSentence( Data, "         " );
	// 11
	Data->PosY = 510;
	Number = MyManager->AddSentence( Data, "         " );
	// 12
	Data->PosY = 560;
	Number = MyManager->AddSentence( Data, "         " );
	// 13
	Data->PosY = 610;
	Number = MyManager->AddSentence(Data, "         ");

	// 14
	Data->FontType = 0;
	Data->PosY = 660;
	Number = MyManager->AddSentence(Data, "         ");

	// 15
	Data->FontType = 0;
	Data->PosY = 710;
	Number = MyManager->AddSentence(Data, "         ");

	// 16 Текст о Видеоплате
	Data->MaxLength = 128;
	Data->PosY = 760;
	Data->ShowType = SHOW;// SHOW_SCROLLING;// | SHOW_GLOWING;
	Data->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->FontType = 2;
	//	Data->Level = 0;
	char Str[256];
	strcpy_s(Str, 249, m_D3D->GetVideoCardString());
	strcat_s(Str, 249, " | Clustered Shading with Spotlight shadows DEMO");
	Number = MyManager->AddSentence(Data, Str);

	// 17
	Data->ShowType = SHOW;
	Data->FontType = 5;
	Data->PosY = 810;
	Number = MyManager->AddSentence ( Data, "1234567890АаБбВвГгДдЕеЁёЖжЗзИиКкЛлМмНнФф{}ЩщЖжЮю" );
	// 18
	Data->FontType = 5;
	Data->PosY = 860;
	Number = MyManager->AddSentence ( Data, "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz" );


	delete Data;

	SENTENCE_INIT_DATA *Data1, *Data2, *Data3, *Data4, *Data5, *Data6;

	Data1 = new SENTENCE_INIT_DATA;
	Data2 = new SENTENCE_INIT_DATA;
	Data3 = new SENTENCE_INIT_DATA;
	Data4 = new SENTENCE_INIT_DATA;
	Data5 = new SENTENCE_INIT_DATA;
	Data6 = new SENTENCE_INIT_DATA;

	ZeroMemory ( Data1, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data2, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data3, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data4, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data5, sizeof ( SENTENCE_INIT_DATA ) );
	ZeroMemory ( Data6, sizeof ( SENTENCE_INIT_DATA ) );

	float x = 150.0f;
	float y = 50.0f;

	KFButton_Elements Buttons[14];

	Buttons[0]._ObjParam = { 10.0f, 10.0f, x, y };
	Buttons[0].Colour = { 0.0f, 0.0f, 0.0f, 0.0f };	// If button - COLOR
	Buttons[0].OsnTexture = MyManager->TexturesArr[1]->SRV;
	Buttons[0].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[0].IsMouseOnButtonTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[0].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[0].Data = NULL;
	Buttons[0].TextLabel = nullptr;
	Buttons[0].Label = "";
	Buttons[0].Type = BUTTON;
	Buttons[0].EditType = NULL;
	Buttons[0].Checked = false;
	Buttons[0].Enabled = true;
	Buttons[0].WaitingForKeyPress = false;
	Buttons[0].VirtualKeyIndex = 0;
	Buttons[0].SecondSlot = true;


	Buttons[1]._ObjParam = { 10.0f, Buttons[0]._ObjParam.y + y + 10.0f , x, y };
	Buttons[1].OsnTexture = MyManager->TexturesArr[2]->SRV;
	Buttons[1].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[1].IsMouseOnButtonTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[1].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;

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

	Buttons[2].OsnTexture = MyManager->TexturesArr[3]->SRV;
	Buttons[2].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[2].IsMouseOnButtonTexture = NULL;
	Buttons[2].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[2].Data = NULL; 
	Buttons[2].Label = "";
	Buttons[2].Type = BUTTON;
	Buttons[2].EditType = NULL;
	Buttons[2].Checked = false;
	Buttons[2].Enabled = true;
	Buttons[2].WaitingForKeyPress = false;
	Buttons[2].VirtualKeyIndex = 0;
	Buttons[2].SecondSlot = true;

	Buttons[3]._ObjParam = { 10.0f, Buttons[2]._ObjParam.y + y + 10.0f, x, y };
	Buttons[3].OsnTexture = MyManager->TexturesArr[4]->SRV;
	Buttons[3].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[3].IsMouseOnButtonTexture = NULL;
	Buttons[3].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;

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
	Buttons[4].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[4].IsClickTexture = MyManager->TexturesArr[19]->SRV;
	Buttons[4].IsMouseOnButtonTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[4].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;

	Buttons[4].Data = NULL; 
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

	MainMenu = new MenuControllerClass;
	MainMenu->ShaderForDraw = MyManager->GetShaderIndexByName ( L"KF2DObj" );

	MainMenu->Init ( m_D3D->D3DGC,
					 Buttons, 5,		// Buttons and Checkboxes
					 NULL, 0,			// ScrollBars
					 StringsList1, 1,	// StringsLists
					 NULL, 0,			// ColorPickers
					 NULL,				// Нет анимации
					 { 112.0f, 84.0f, 800.0f, 600.0f },
					 MyManager->TexturesArr[18]->SRV,	// 18 
					 MyManager
					 //m_Text
					 );

	MyManager->AddMenu ( dynamic_cast<Menu*> (MainMenu) );	// Добавляем меню в список существующих
	ScrollBar_Elements Bars[18];
	

	Buttons[0]._ObjParam = { 10.0f, 10.0f, x, y };
	Buttons[1]._ObjParam = { 10.0f, Buttons[0]._ObjParam.y + y + 10.0f , x, y };
	Buttons[2]._ObjParam = { 10.0f, Buttons[1]._ObjParam.y + y + 10.0f, x, y };
	Buttons[3]._ObjParam = { 10.0f, Buttons[2]._ObjParam.y + y + 10.0f, x, y };

	Bars[0].Horizontal = false;
	Bars[0].Values = { 0.0f, 1.0f, 0.5f, 0.01f };
	Bars[0].ShowButtons = true;
	Bars[0].ObjParam = { 200.0f, 50.0f, 24.0f, 240.0f };
	Bars[0].UpSideDown = true;

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
	Bars[1].ObjParam = { 10.0f, 430.0f, 24.0f, 140.0f };
	Bars[1].UpSideDown = true;

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
	Bars[2].ObjParam = { 40.0f, 430.0f, 24.0f, 140.0f };
	Bars[2].UpSideDown = true;

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
	Bars[3].ObjParam = { 70.0f, 430.0f, 24.0f, 140.0f };
	Bars[3].UpSideDown = true;

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
	Bars[4].ObjParam = { 100.0f, 430.0f, 24.0f, 140.0f };
	Bars[4].UpSideDown = true;

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
	Bars[5].ObjParam = { 130.0f, 430.0f, 24.0f, 140.0f };
	Bars[5].UpSideDown = true;

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
	Bars[6].ObjParam = { 160.0f, 430.0f, 24.0f, 140.0f };
	Bars[6].UpSideDown = true;

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
	Bars[7].ObjParam = { 190.0f, 430.0f, 24.0f, 140.0f };
	Bars[7].UpSideDown = true;

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
	Bars[8].ObjParam = { 10.0f, 650.0f, 140.0f, 24.0f };
	Bars[8].UpSideDown = false;

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

	// DyffuseZ
	Bars[9].Horizontal = true;
	Bars[9].Values = { 1.0f, 3000.0f , 150.0f, 1.0f };
	Bars[9].ShowButtons = true;
	Bars[9].ObjParam = { 10.0f, 680.0f, 140.0f, 24.0f };
	Bars[9].UpSideDown = false;

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
	Bars[10].ObjParam = { 10.0f, 710.0f, 140.0f, 24.0f };
	Bars[10].UpSideDown = false;

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
	Buttons[2].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[2].IsClickTexture = MyManager->TexturesArr[6]->SRV;
	Buttons[2].Data = NULL;


	Buttons[3].Type = CHECKBOX; // CheckBox
	Buttons[3].Checked = m_D3D->D3DGC->EnableFXAA;
	Buttons[3].Label = "";
	Buttons[3].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[3].IsClickTexture = MyManager->TexturesArr[7]->SRV;
	Buttons[3].Data = NULL;


	Buttons[6]._ObjParam = { 10.0f, Buttons[3]._ObjParam.y + y + 10.0f, x, y };
	Buttons[6].OsnTexture = MyManager->TexturesArr[5]->SRV;
	Buttons[6].IsClickTexture = MyManager->TexturesArr[4]->SRV;
	Buttons[6].IsMouseOnButtonTexture = NULL;
	Buttons[6].IsNotEnalbledTexture = MyManager->TexturesArr[5]->SRV;

	Buttons[6].Data = NULL;
	Buttons[6].Label = "";
	Buttons[6].SentenceIndex = -1;
	Buttons[6].Type = CHECKBOX; // Button
	Buttons[6].EditType = NULL;
	Buttons[6].Checked = m_D3D->D3DGC->SoftShadowsOn;
	Buttons[6].Enabled = true;
	Buttons[6].WaitingForKeyPress = false;
	Buttons[6].VirtualKeyIndex = 0;
	Buttons[6].SecondSlot = false;

	// EDIT TEXT
	Data4->MaxLength = 16; // Для Edit указывать желаемое количество вводимых символов + 1
	Data4->PosX = 10;
	Data4->PosY = 330;
	Data4->ShowType = 1;
	Data4->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data4->Render = true;
	Data4->FontType = 0;
	Data4->Level = MENU;

	Buttons[4]._ObjParam = { 10.0f, 330.0f, 200.0f, 43.0f };
	Buttons[4].OsnTexture = MyManager->TexturesArr[11]->SRV;
	Buttons[4].IsClickTexture = NULL;
	Buttons[4].IsMouseOnButtonTexture = NULL;
	Buttons[4].IsNotEnalbledTexture = NULL;
	Buttons[4].Data = Data4; // Нет строки в Buttons[4].Label 
	Buttons[4].Label = "56";
	Buttons[4].Type = EDIT; // Edit
	Buttons[4].EditType = AS_FLOAT; // Edit
	Buttons[4].Checked = false;
	Buttons[4].Enabled = true;
	Buttons[4].WaitingForKeyPress = false;
	Buttons[4].VirtualKeyIndex = 0;
	Buttons[4].SecondSlot = true;


	// LABEL  KeyPressed
	Data5->MaxLength = 32;
	Data5->PosX = 10;
	Data5->PosY = 330;
	Data5->ShowType = 1;
	Data5->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data5->Render = true;
	Data5->FontType = 0;
	Data5->Level = MENU;

	Buttons[5]._ObjParam = { 10.0f, 380.0f, 200.0f, 43.0f };
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
	Buttons[7]._ObjParam = { 10.0f, 580.0f, x, y };
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

	// Gradient Stops 1
	Buttons[8]._ObjParam = { 10.0f, 850.0f, 30.0f, 20.0f };
	Buttons[8].Colour= { 1.0f, 0.0f, 0.0f, 0.0f };
	Buttons[8].OsnTexture = nullptr;
	Buttons[8].IsClickTexture = nullptr;
	Buttons[8].IsMouseOnButtonTexture = NULL;
	Buttons[8].IsNotEnalbledTexture = NULL;
	Buttons[8].Data = NULL;
	Buttons[8].Label = "";
	Buttons[8].Type = COLOR_PICK;
	Buttons[8].ActiveStringsList = false;
	Buttons[8].EditType = NULL;
	Buttons[8].Checked = false;
	Buttons[8].Enabled = true;
	Buttons[8].WaitingForKeyPress = false;
	Buttons[8].VirtualKeyIndex = 0;
	Buttons[8].SecondSlot = false;
	

	// Gradient Stops 1 POSITION
	Bars[11].Horizontal = true;
	Bars[11].Values = { -1.0f, 1.0f , 0.0f, 0.02f };
	Bars[11].ShowButtons = true;
	Bars[11].ObjParam = { 70.0f, 848.0f, 140.0f, 24.0f };
	Bars[11].UpSideDown = false;

	Bars[11].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[11].BodyNotEnalbledTexture = NULL;
	Bars[11].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[11].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[11].ButtonNotEnalbledTexture = NULL;
	Bars[11].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[11].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[11].TravellerNotEnalbledTexture = NULL;
	Bars[11].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[11].MouseOnTravellerTexture = NULL;

	// Gradient Stops 2
	Buttons[9]._ObjParam = { 10.0f, 880.0f, 30.0f, 20.0f };
	Buttons[9].Colour = { 0.0f, 1.0f, 0.0f, 0.0f };
	Buttons[9].OsnTexture = nullptr;
	Buttons[9].IsClickTexture = nullptr;
	Buttons[9].IsMouseOnButtonTexture = NULL;
	Buttons[9].IsNotEnalbledTexture = NULL;
	Buttons[9].Data = NULL;
	Buttons[9].Label = "";
	Buttons[9].Type = COLOR_PICK;
	Buttons[9].ActiveStringsList = false;
	Buttons[9].EditType = NULL;
	Buttons[9].Checked = false;
	Buttons[9].Enabled = true;
	Buttons[9].WaitingForKeyPress = false;
	Buttons[9].VirtualKeyIndex = 0;
	Buttons[9].SecondSlot = false;

	// Gradient Stops 2 POSITION
	Bars[12].Horizontal = true;
	Bars[12].Values = { -1.0f, 1.0f , 0.8f, 0.02f };
	Bars[12].ShowButtons = true;
	Bars[12].ObjParam = { 70.0f, 878.0f, 140.0f, 24.0f };
	Bars[12].UpSideDown = false;

	Bars[12].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[12].BodyNotEnalbledTexture = NULL;
	Bars[12].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[12].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[12].ButtonNotEnalbledTexture = NULL;
	Bars[12].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[12].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[12].TravellerNotEnalbledTexture = NULL;
	Bars[12].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[12].MouseOnTravellerTexture = NULL;

	// Gradient Stops 3
	Buttons[10]._ObjParam = { 10.0f, 910.0f, 30.0f, 20.0f };
	Buttons[10].Colour = { 0.0f, 0.0f, 1.0f, 0.0f };
	Buttons[10].OsnTexture = nullptr;
	Buttons[10].IsClickTexture = nullptr;
	Buttons[10].IsMouseOnButtonTexture = NULL;
	Buttons[10].IsNotEnalbledTexture = NULL;
	Buttons[10].Data = NULL;
	Buttons[10].Label = "";
	Buttons[10].Type = COLOR_PICK;
	Buttons[10].ActiveStringsList = false;
	Buttons[10].EditType = NULL;
	Buttons[10].Checked = false;
	Buttons[10].Enabled = true;
	Buttons[10].WaitingForKeyPress = false;
	Buttons[10].VirtualKeyIndex = 0;
	Buttons[10].SecondSlot = false;

	// Gradient Stops 3 POSITION
	Bars[13].Horizontal = true;
	Bars[13].Values = { -1.0f, 1.0f , 1.0f, 0.02f };
	Bars[13].ShowButtons = true;
	Bars[13].ObjParam = { 70.0f, 908.0f, 140.0f, 24.0f };
	Bars[13].UpSideDown = false;

	Bars[13].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[13].BodyNotEnalbledTexture = NULL;
	Bars[13].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[13].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[13].ButtonNotEnalbledTexture = NULL;
	Bars[13].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[13].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[13].TravellerNotEnalbledTexture = NULL;
	Bars[13].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[13].MouseOnTravellerTexture = NULL;


	// OUTLINE WIDTH
	Data6->MaxLength = 4; // Для Edit указывать желаемое количество вводимых символов + 2
	Data6->PosX = 10;
	Data6->PosY = 330;
	Data6->ShowType = 1;
	Data6->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Data6->Render = true;
	Data6->FontType = 1;
	Data6->Level = MENU;

	Buttons[11]._ObjParam = { 10.0f, 940.0f, 50.0f, 45.0f };
	Buttons[11].OsnTexture = MyManager->TexturesArr[26]->SRV;
	Buttons[11].IsClickTexture = NULL;
	Buttons[11].IsMouseOnButtonTexture = NULL;
	Buttons[11].IsNotEnalbledTexture = NULL;
	Buttons[11].Data = Data6;
	Buttons[11].Label = "1";
	Buttons[11].Type = EDIT; // Edit
	Buttons[11].EditType = AS_INTEGER; // Edit
	Buttons[11].Checked = false;
	Buttons[11].Enabled = true;
	Buttons[11].WaitingForKeyPress = false;
	Buttons[11].VirtualKeyIndex = 0;
	Buttons[11].SecondSlot = false;

	// OUTLINED
	Buttons[12]._ObjParam = { 10.0f, 1000.0f, 20.0f, 20.0f };
	Buttons[12].OsnTexture = MyManager->TexturesArr[27]->SRV;
	Buttons[12].IsClickTexture = MyManager->TexturesArr[28]->SRV;
	Buttons[12].IsMouseOnButtonTexture = NULL;
	Buttons[12].IsNotEnalbledTexture = NULL;
	Buttons[12].Data = NULL; // Нет строки в Buttons[4].Label 
	Buttons[12].Label = "";
	Buttons[12].SentenceIndex = -1;
	Buttons[12].Type = CHECKBOX; // Button
	Buttons[12].EditType = NULL;
	Buttons[12].Checked = false;
	Buttons[12].Enabled = true;
	Buttons[12].WaitingForKeyPress = false;
	Buttons[12].VirtualKeyIndex = 0;
	Buttons[12].SecondSlot = false;


	// OUTLINE COLOR
	Buttons[13].Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
	Buttons[13]._ObjParam = { 35.0f, 1000.0f, 30.0f, 20.0f };
	Buttons[13].OsnTexture = NULL;
	Buttons[13].IsClickTexture = NULL;
	Buttons[13].IsMouseOnButtonTexture = NULL;
	Buttons[13].IsNotEnalbledTexture = NULL;
	Buttons[13].Data = NULL; // Нет строки в Buttons[4].Label 
	Buttons[13].Label = "";
	Buttons[13].SentenceIndex = -1;
	Buttons[13].Type = COLOR_PICK; // Button
	Buttons[13].EditType = NULL;
	Buttons[13].Checked = false;
	Buttons[13].Enabled = true;
	Buttons[13].WaitingForKeyPress = false;
	Buttons[13].VirtualKeyIndex = 0;
	Buttons[13].SecondSlot = false;

/*
	// APPLY
	Buttons[14]._ObjParam = { 210.0f, 870.0f, 20.0f, 20.0f };
	Buttons[14].OsnTextureResource = MyManager->TexturesArr[27]->Resource;;
	Buttons[14].OsnTexture = MyManager->TexturesArr[27]->SRV;
	Buttons[14].IsClickTexture = MyManager->TexturesArr[28]->SRV;
	Buttons[14].IsMouseOnButtonTexture = NULL;
	Buttons[14].IsNotEnalbledTexture = NULL;
	Buttons[14].Data = NULL; // Нет строки в Buttons[4].Label 
	Buttons[14].Label = "";
	Buttons[14].SentenceIndex = -1;
	Buttons[14].Type = BUTTON; // Button
	Buttons[14].EditType = NULL;
	Buttons[14].Checked = false;
	Buttons[14].Enabled = true;
	Buttons[14].WaitingForKeyPress = false;
	Buttons[14].VirtualKeyIndex = 0;
	Buttons[14].SecondSlot = false;
*/

	// Gradient START X POSITION
	Bars[14].Horizontal = true;
	Bars[14].Values = { -1.0f, 1.0f , 0.0f, 0.02f };
	Bars[14].ShowButtons = true;
	Bars[14].ObjParam = { 100.0f, 940.0f, 140.0f, 22.0f };
	Bars[14].UpSideDown = false;

	Bars[14].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[14].BodyNotEnalbledTexture = NULL;
	Bars[14].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[14].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[14].ButtonNotEnalbledTexture = NULL;
	Bars[14].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[14].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[14].TravellerNotEnalbledTexture = NULL;
	Bars[14].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[14].MouseOnTravellerTexture = NULL;

	// Gradient START Y POSITION
	Bars[15].Horizontal = true;
	Bars[15].Values = { -100.0f, 100.0f , -50.0f, 1.0f };
	Bars[15].ShowButtons = true;
	Bars[15].ObjParam = { 100.0f, 965.0f, 140.0f, 22.0f };
	Bars[15].UpSideDown = false;

	Bars[15].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[15].BodyNotEnalbledTexture = NULL;
	Bars[15].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[15].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[15].ButtonNotEnalbledTexture = NULL;
	Bars[15].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[15].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[15].TravellerNotEnalbledTexture = NULL;
	Bars[15].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[15].MouseOnTravellerTexture = NULL;


	// Gradient STOP X POSITION
	Bars[16].Horizontal = true;
	Bars[16].Values = { -1.0f, 1.0f , 0.0f, 0.02f };
	Bars[16].ShowButtons = true;
	Bars[16].ObjParam = { 100.0f, 990.0f, 140.0f, 22.0f };
	Bars[16].UpSideDown = false;

	Bars[16].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[16].BodyNotEnalbledTexture = NULL;
	Bars[16].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[16].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[16].ButtonNotEnalbledTexture = NULL;
	Bars[16].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[16].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[16].TravellerNotEnalbledTexture = NULL;
	Bars[16].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[16].MouseOnTravellerTexture = NULL;


	// Gradient STOP Y POSITION
	Bars[17].Horizontal = true;
	Bars[17].Values = { -100.0f, 100.0f , -5.0f, 1.0f };
	Bars[17].ShowButtons = true;
	Bars[17].ObjParam = { 100.0f, 1015.0f, 140.0f, 22.0f };
	Bars[17].UpSideDown = false;

	Bars[17].BodyTexture = MyManager->TexturesArr[12]->SRV;
	Bars[17].BodyNotEnalbledTexture = NULL;
	Bars[17].ButtonsTexture = MyManager->TexturesArr[13]->SRV;
	Bars[17].ButtonPressTexture = MyManager->TexturesArr[17]->SRV;
	Bars[17].ButtonNotEnalbledTexture = NULL;
	Bars[17].MouseOnButtonTexture = MyManager->TexturesArr[16]->SRV;
	Bars[17].TravellerTexture = MyManager->TexturesArr[14]->SRV;
	Bars[17].TravellerNotEnalbledTexture = NULL;
	Bars[17].TravellerPressTexture = MyManager->TexturesArr[15]->SRV;
	Bars[17].MouseOnTravellerTexture = NULL;


	ColorPicker_Elements ColorPicker[1];

	ColorPicker[0].ActiveColorValue = { 0.0f, 1.0f, 0.0f, 1.0f };
	ColorPicker[0].Horizontal = false;
	ColorPicker[0].ObjParam = { 10.0f, 740.0f, 160.0f, 100.0f };
	ColorPicker[0]._Panthon_Alpha_Thickness = 20.0f;
	ColorPicker[0].PanthonTexture = MyManager->TexturesArr[24]->SRV; // 24
//	ColorPicker[0].SelectorTexture = MyManager->TexturesArr[26]->SRV; // 26

	// +++++++++++++++++++     Анимация     ++++++++++++++++++++++++++++++
	AnimTexture = new KF2DTextureAnimation;
	XMFLOAT4 ObjData = { 100.0f, 800.0f, 100.0f, 100.0f };
	{
		AnimTexture->Init ( hwnd, m_D3D->D3DGC, 8, 8, MyManager->TexturesArr[8]->SRV,
			MyManager->GetShaderIndexByName ( L"KF2DObj" ),	// FireAnimation анимация в меню FireAnimation_vs
			ObjData,
			MyManager 
		);
	}
	// -------------------     Анимация     ------------------------------


	Hud = new MenuControllerClass;
	Hud->ShaderForDraw = MyManager->GetShaderIndexByName ( L"KF2DObj" );
	Hud->Init(m_D3D->D3DGC,
		Buttons, 14,	// Buttons
		Bars, 18,		// ScrollBars
		NULL, 0,		// StringsLists
		ColorPicker, 1,	// ColorPickers
		AnimTexture,	// Animation
		{1670.0f, 0.0f, 250.0f, 1080.0f },
		MyManager->TexturesArr[18]->SRV,
		MyManager
		);
	
	MyManager->AddMenu ( dynamic_cast<Menu*> (Hud) );	// Добавляем меню в список существующих

	// Помещаем надписи Label внутрь кнопок
	Hud->PutLabelsInsideButtons();
	// включаем рисование HUD
	Hud->IsMenuDraw = true;

	// Активируем HUD или Hud->IsMenuActive = true;
	Hud->IsMenuActive = true;
	
	delete Data1;
	delete Data2;
	delete Data3;
	delete Data4;
	delete Data5;
	delete Data6;
	// --------------------- это коробка
	
	// Создаю CubeMap
	CubeMapIndex = MyManager->Add_CubeMap ( L"Textures/skymap.dds" );
	if ( CubeMapIndex < 0 )
	{
		MessageBox ( hwnd, L"Could not initialize CubeMap", Error, MB_OK );
		return false;
	}

// ++++++++++++++++++++++++++++++++++++++++     LIGHT & ShadowMap    ++++++++++++++++++++++++++++++++++++++++++++	
//	m_Light = new LightClass;
	m_D3D->LightShaderForDraw = MyManager->GetShaderIndexByName ( L"ClusteredSM" );
//	m_D3D->ShadowMapShaderFordraw = MyManager->GetShaderIndexByName ( L"LightRenderSM" );
//	m_D3D->ShadowMapShader = MyManager->GetVertexShader ( m_D3D->ShadowMapShaderFordraw );
// ----------------------------------------     LIGHT & ShadowMap    ---------------------------------------------

// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++
	// Обязательно для систем частиц !!!
	m_Camera->GetPosition( m_D3D->D3DGC->CameraPosition );

	ParticleSysInitData* _InitData = new ParticleSysInitData;

	_InitData->ReserveEmittersCount = 1;
	_InitData->ParticleSystemType = PS_BASIC;
	_InitData->Position.Fl3 = XMFLOAT3 ( 0.0f, 35.0f, 0.0f );

	Snow = new ParticleSystemController ( &fpstimers, m_D3D, PhysX, MyManager, _InitData );

	Emitter_Init_Data* Emitter_Data = new Emitter_Init_Data;
	memset ( Emitter_Data, 0, sizeof ( Emitter_Init_Data ) );

	Emitter_Data->InitPosition.Fl3 = _InitData->Position.Fl3;	// Set Particle System Position Offset in 3D World
	Emitter_Data->Animated = EM_NO_ANIMATION_RANDOM;
	Emitter_Data->EmittFunction  = E_SNOWFALL;
	Emitter_Data->UpdateFunction = U_SNOWFALL;
	Emitter_Data->InitSize = 0.05f;
	Emitter_Data->InitVelocity = 1.95f;
	Emitter_Data->InitVelocityVariation = -1.0f;

	Emitter_Data->KillXPosition = NO_KILL;
	Emitter_Data->KillYPosition = KILL_UNDER;
	Emitter_Data->KillZPosition = NO_KILL;
	Emitter_Data->KillPosition.Fl3 = XMFLOAT3 ( 0.0f, 0.0f, 0.0f );
	
	Emitter_Data->KillBySize = 0;
	Emitter_Data->KillSize.Fl3 = XMFLOAT3 ( 0.0f, 0.0f, 0.0f );

	Emitter_Data->KillByVelocity = 0;
	Emitter_Data->KillVelocity.Fl3 = XMFLOAT3 ( 0.0f, 0.0f, 0.0f );

	Emitter_Data->ApplyLightSourceToParticlses = false;
	Emitter_Data->LifeTime = -1.0f;
	Emitter_Data->MaxParticles = 5000;
	Emitter_Data->MaxActiveParticles = 5000;
	Emitter_Data->ParticlesPerSecond = 500.0f;
	Emitter_Data->Init3DParticleVelocity.Fl3 = XMFLOAT3 ( 10.95f, 0.95f, 0.95f );
	Emitter_Data->PlayFPS = 30;
	Emitter_Data->InitPositionDeviation.Fl3 = XMFLOAT3 ( 0.5f, 0.0f, 0.0f );

	Emitter_Data->TextureIndex = 23;// Snow texture 20; // Star Texture
	Emitter_Data->UX_Amount = 16;
	Emitter_Data->VY_Amount = 16;

	// Snow
	Snow->AddEmitter ( Emitter_Data );
	Snow->Render = true;

	Emitter_Data->TextureIndex = 20;
	Emitter_Data->Animated = EM_NO_ANIMATION;
	Emitter_Data->InitVelocity = 5.95f;
	Emitter_Data->InitVelocityVariation = -5.0f;
	Emitter_Data->InitPosition.Fl3 = XMFLOAT3 ( 100.0f, 50.0f, 10.0f );
	Emitter_Data->InitPositionDeviation.Fl3 = XMFLOAT3 ( 30.5f, 0.0f, 30.0f );
	Emitter_Data->InitSize = 0.5f;
	Emitter_Data->KillPosition.Fl3 = XMFLOAT3 ( 0.0f, 35.0f, 0.0f );
	Emitter_Data->MaxParticles = 500;
	Emitter_Data->MaxActiveParticles = 500;
	Emitter_Data->ParticlesPerSecond = 500.0f;
	// Big Snow
	Snow->AddEmitter ( Emitter_Data );


	Emitter_Data->Animated = EM_NO_ANIMATION_RANDOM;
	Emitter_Data->InitPositionDeviation.Fl3 = XMFLOAT3 ( 10.5f, 0.0f, 10.0f );
	Emitter_Data->InitVelocity = 15.9f;
	Emitter_Data->InitVelocityVariation = -15.0f;
	Emitter_Data->KillPosition.Fl3 = XMFLOAT3 ( 0.0f, 0.0f, 0.0f );
	Emitter_Data->InitPosition.Fl3 = XMFLOAT3 ( 100.0f, 30.0f, -100.0f );
	Emitter_Data->ApplyLightSourceToParticlses = false;
	Emitter_Data->InitSize = 1.0f;
	Emitter_Data->TextureIndex = 23;
	Emitter_Data->UX_Amount = 16;
	Emitter_Data->VY_Amount = 16;
	Emitter_Data->MaxParticles = 50;
	Emitter_Data->MaxActiveParticles = 50;
	Emitter_Data->ParticlesPerSecond = 50.0f;

	Snow->AddEmitter ( Emitter_Data );


	// All over map
	// Flying Animated Fire
	Emitter_Data->Animated = EM_ANIMATED_RANDOM;
	Emitter_Data->InitPositionDeviation.Fl3 = XMFLOAT3 ( 250.0f, 0.1f, 250.0f );
	Emitter_Data->InitVelocity = 1.0f;
	Emitter_Data->InitVelocityVariation = -0.9f;
	Emitter_Data->KillPosition.Fl3 = XMFLOAT3 ( 0.0f, 0.0f, 0.0f );
	Emitter_Data->InitPosition.Fl3 = XMFLOAT3 ( 1.0f, 30.0f, 1.0f );
	Emitter_Data->LightAttenuationBegin = 0.1f;
	Emitter_Data->LightAttenuationEnd = 10.5f;
	Emitter_Data->LightColour.Fl4 = XMFLOAT4 ( 1.0f, 1.0f, 1.0f, 1.0f );
	Emitter_Data->ApplyLightSourceToParticlses = true;
	Emitter_Data->InitSize = 1.0f;
	Emitter_Data->TextureIndex = 8;	//MyManager->TexturesArr[8]->SRV
	Emitter_Data->UX_Amount = 8;
	Emitter_Data->VY_Amount = 8;
	Emitter_Data->MaxParticles = 3000;
	Emitter_Data->MaxActiveParticles = 150;
	Emitter_Data->ParticlesPerSecond = 50.0f;

	Snow->AddEmitter ( Emitter_Data );

	delete Emitter_Data;

	delete _InitData;


	// Create the particle system object.
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
		m_D3D->AddLightSource(AddedLight);
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
//		XMFLOAT3 Lpos;
//		m_D3D->GetLightPos(0, Lpos);
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

		TerrainInitData NewTerrainData;
		NewTerrainData._QuadVertexStep = 10.0f;
		NewTerrainData._QuadsPerCluster = 63;
		NewTerrainData._ClustersX_ROW = 32;
		NewTerrainData._ClustersZ_COL = 32;
		NewTerrainData._CastShadow = true;
		NewTerrainData._ClusterRender = true;
		NewTerrainData._StaticBuffers = false;
		NewTerrainData.HightFileName = nullptr;

		Terrain_GenInit TerrainData;
		TerrainData.HeightNum = 600;
		TerrainData.LowlandNum = 600;
		TerrainData.MaxRadius = 200;
		TerrainData.MinRadius = 70;
		TerrainData.Max_Height = 200.0f;
		TerrainData.Min_Height = 50.0f;
		TerrainData.Max_Low = -200.0f;
		TerrainData.Min_Low = -50.0f;

		int TerrainNumber = MyManager->AddTerrain ( 21, &NewTerrainData, &TerrainData); // 21  9
//		MyManager->Delete_Terrain ( TerrainNumber );

		m_D3D->ShadowMapShaderIndex = MyManager->GetShaderIndexByName ( L"LightRenderSM" );

		// Create one Hight in the middle of terrain
		Terrain* TerrainObj = MyManager->Get_Terrain_Object_Pointer (TerrainNumber);
		XMFLOAT2 Position = { (float)TerrainObj->VertexInX_Row / 2, ( float ) TerrainObj->VertexInZ_Col / 2};
		XMFLOAT4 HRMaxMin = {100.0f, 200.0f, 45.0f, 50.0f};
//		MyManager->CreateLandScapeBending (TerrainObj, Position, HRMaxMin);
//		MyManager->GenerateLandScape (MyManager->Get_Terrain_Object_Pointer (TerrainNumber), &TerrainData);

		return true;
}

// ++++++++++++++++++++++++++++++     FRAME     +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GraphicsClass::Frame( FPSTimers &Counters, DXINPUTSTRUCT& InputStruct1)
{
	bool result = true;
	
	++ShadowFrameCounts;
	fpstimers = Counters;
// делаем доступ к скоросити компьютера для текста. Чтобы отрисовывать его синхронно с FPS системы
//	m_Text->fpstimers = Counters;
	RCubeRender->fpstimers = Counters;

	sprintf_s ( Str1, STRMAXLENGTH, "Fps : %d", Counters.FpsRate );
	
	MyManager->UpdateSentence ( 0, Str1);

	sprintf_s ( Str1, STRMAXLENGTH, "ИСПОЛЬЗОВАНИЕ ПРОЦЕССОРА: %d %%", Counters.CpuVal );
	MyManager->UpdateSentence ( 1, Str1);

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
	char Str[25];// = new char [25];
	Profile->StartTimer();
	// Измеряем быстродействие
	Snow->Frame ();
	Profile->StopTimer( Str );

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


//	MyManager->->UpdateSentence( 4, Str, MyManager->GetSentencePosX (4), MyManager->GetSentencePosY (4) );

	TorchFire->Frame( Counters );
	// Измеряем быстродействие
//	Profile->StopTimer(Str);
//	MyManager->->UpdateSentence(5, Str, MyManager->GetSentencePosX (5), MyManager->GetSentencePosY (5));
	// Измеряем быстродействие
// --------------------------------------------------  PARTICLE SYSTEM  -----------------------------

	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
//		char Str[128];// = new char [25];
//		Profile->StartTimer();
		// Измеряем быстродействие

// ++++++++++++    Как часто рисуем Тень     +++++++++++++++++++
	if ( m_D3D->D3DGC->ShadowsOn )// && ShadowFrameCounts > 0
	{
		MyManager->SetActiveShadersInProgramm ( m_D3D->ShadowMapShaderIndex );
		RCubeRender->RenderSpotLightsSadowMaps ( &m_D3D->SpotLightsWithShadowsIndexes );

		ShadowFrameCounts = 0;
	}
	// Измеряем быстродействие
//		Profile->StopTimer(Str);
//		MyManager->->UpdateSentence(5, Str, MyManager->GetSentencePosX(5), MyManager->GetSentencePosY(5));
	// Измеряем быстродействие

// ++++++++++++++  FRUSTUM +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SHADOWMAP использует другой FRUSTUM, поэтому сразу после SHADOWMAP возвращаем FRUSTUM на место

	// Страрая версия фрастума от Растертека
	//m_Frustum->ConstructFrustum(SCREEN_DEPTH, m_D3D->D3DGC->ProjectionMatrix, m_D3D->D3DGC->ViewMatrix);

	// Подсмотрели у Брайнзара
	m_Frustum->ConstructFrustumNew( m_D3D->D3DGC->ViewProjection );

	// Федя 540 ns    Мой  517 ns   
//	char Str[32];// = new char [25];
//	Profile->StartTimer ();
	// Измеряем быстродействие
/*	int Objects_Amount = ( int ) MyManager->Terrains.size ();
	for ( int i = 0; i < Objects_Amount; ++i )
	{
	if ( MyManager->Terrains[i]->ClusterRender )
		MyManager->FrustumTerrain ( m_Frustum, MyManager->Terrains[i] );
	}
*/	// Измеряем быстродействие
//	Profile->StopTimer ( Str );
// ----------------------  FRUSTUM    --------------------------------------------------

// ++++++++++++    Как часто рисем Тень     +++++++++++++++++++


// Обновляем свет сцены
// Обязательно после отрисовки теней !!!
// Frame свету
		m_D3D->Frame();
// ------------------------------------------------------------


	strcpy_s(Str1, STRMAXLENGTH, "Particles Frame : ");
	strcat_s(Str1, STRMAXLENGTH, Str);
	MyManager->UpdateSentence(13, Str1);
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
//	MyManager->UpdateSentence(4, Str, MyManager->GetSentencePosX (4), MyManager->GetSentencePosY (4));

//	sprintf_s(Str, 70, "Cam pos: X: %4.2f , Y: %4.2f , Z: %4.2f", m_D3D->D3DGC->CameraPosition.x, m_D3D->D3DGC->CameraPosition.y, m_D3D->D3DGC->CameraPosition.z );
//	MyManager->UpdateSentence(3, Str, 100, 130);
	float angel = Hud->GetScrollBarValue( 2 );
	sprintf_s( Str, 70, "DepthBias : %4.2f", angel );
	MyManager->UpdateSentence( 3, Str);

//	float angel = atan2(0.0f - m_D3D->D3DGC->CameraPosition.x, 0.0f - m_D3D->D3DGC->CameraPosition.z) * 57.29578049f;

	m_Camera->GetLookAt( CamLook.Vec );
//	sprintf_s( Str, 70, "Cam LookAt: X: %4.2f , Y: %4.2f , Z: %4.2f , W: %4.2f", CamLook.Fl4.x, CamLook.Fl4.y, CamLook.Fl4.z, CamLook.Fl4.w );
//	MyManager->UpdateSentence( 2, Str, 100, 100 );
	angel = Hud->GetScrollBarValue( 3 );
	sprintf_s(Str, 70, "SlopeScaledDepthBias : %4.2f", angel );
	MyManager->UpdateSentence(2, Str);

	angel = Hud->GetScrollBarValue ( 1 );
	sprintf_s ( Str, 50, "DiffuseX = %6.5f", angel );
  //	sprintf_s( Str, 50, "Active P = %i", m_ParticleSystem->GetActiveParticleAmmount() );
	MyManager->UpdateSentence ( 4, Str);

	angel = Hud->GetScrollBarValue( 4 );
	sprintf_s( Str, 50, "PCF_Amount = %4.2f", angel );
	MyManager->UpdateSentence( 5, Str);

	angel = Hud->GetScrollBarValue( 5 );
	sprintf_s( Str, 50, "PCF_Step = %4.2f", angel );
	MyManager->UpdateSentence( 6, Str, 100, 220 );
//	sprintf_s( Str, 50, "Lights %i", m_Light->GetActiveLghtNumber() );
//	MyManager->UpdateSentence( 5, Str, 100, 160 );

//	angel = Hud->GetScrollBarValue( 6 );
//	sprintf_s( Str, 50, "ShadowCLAMP = %1.4f", angel );
//	MyManager->UpdateSentence( 7, Str, 100, 250 );
	sprintf_s ( Str1, STRMAXLENGTH - 1, "Время рисования всех строк текста : %s", Global );
	MyManager->UpdateSentence ( 7, Str1);

	angel = Hud->GetScrollBarValue( 7 );
	sprintf_s( Str, 50, "Shadow Divider = %4.1f", angel );
	MyManager->UpdateSentence( 8, Str);

// LightInfo
	sprintf_s( Str, 50, "Active Lights = %d", m_D3D->mActiveLights );
	MyManager->UpdateSentence( 11, Str);

	sprintf_s( Str, 50, "Visible Lights = %d", m_D3D->mVisibleLights );
	MyManager->UpdateSentence( 12, Str);

	angel = Hud->GetScrollBarValue(8);
	sprintf_s(Str, 50, "DiffuseY = %1.5f", angel);
	MyManager->UpdateSentence(14, Str);

	angel = Hud->GetScrollBarValue(9);
	sprintf_s(Str, 50, "DiffuseZ = %1.5f", angel);
	MyManager->UpdateSentence(15, Str);



//	sprintf_s ( Str, 256, "Color = %x", Hud->ColorPickers[0]->GetSelectedColorCOLREF () );
	MyManager->UpdateSentence ( 16, MyManager->GetSentenceText(16));
//	MyManager->UpdateSentence(16, MyManager->GetSentenceText(16), MyManager->GetSentencePosX(16), MyManager->GetSentencePosY(16));
//	MyManager->UpdateSentence ( 16, MyManager->GetSentenceText ( 16 ), 100, 760 );



// +++++++++++++++++++++    РИСУЕМ ОБЪЕКТЫ НА СЦЕНЕ    +++++++++++++++++++++++++++++++++++++++++++
	MyManager->SetActiveShadersInProgramm( m_D3D->LightShaderForDraw );
	// Устанавливаем сразу всё что нужно для отрисовки всех моделей со светом
	// Чтобы не устанаваить это в цикле много раз
	m_D3D->D3DGC->DX_deviceContext->OMSetDepthStencilState(m_D3D->D3DGC->depthStencil_State, 0);
	m_D3D->SetDefaultResterizeState ();
	m_D3D->D3DGC->DX_deviceContext->OMSetRenderTargets(1, &m_D3D->D3DGC->BackBuffer_RTV, m_D3D->D3DGC->m_depthStencilView);

	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
	//	char Str[25];// = new char [25];
	Profile->StartTimer();

	MyManager->Frustum_3D_Objects ( m_Frustum );

	Profile->StopTimer ( Str );
//	m_D3D->TurnOnAlphaBlending ();
//	m_D3D->TurnOnParticleBlending ();
	RCubeRender->RenderScene ();
	
	strcpy_s(Str1, STRMAXLENGTH, "Frustum объектам : ");
	strcat_s(Str1, STRMAXLENGTH, Str);
	MyManager->UpdateSentence(10, Str1);
	
	m_D3D->TurnOffAlphaBlending();

//	m_D3D->SetDefaultResterizeState();

	// Измеряем быстродействие
//	Profile->StopTimer(Str);
//	MyManager->UpdateSentence(4, Str,MyManager->GetSentencePosX (4), MyManager->GetSentencePosY (4));
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
//			MyManager->UpdateSentence(4, Str, MyManager->GetSentencePosX (4), MyManager->GetSentencePosY (4));
	// Измеряем быстродействие

//	m_D3D->TurnOnAlphaBlending();

	// +++++++++++++++++++++++++++++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++++++++++++++++++++
	m_D3D->TurnOnParticleBlending();
	m_D3D->ZBufferNoWrite ();// TurnZBufferOff ();
	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.

	// NEW PARTICLE SYSTEM
	// SNOW система частиц Render
	RCubeRender->RenderParticleSystem ( 0 );

//	Profile->StopTimer( Str );
//	MyManager->UpdateSentence( 5, Str, MyManager->GetSentencePosX (5), MyManager->GetSentencePosY (5));

	// TorchFire Факел, костёр
	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
	//		char Str[25];// = new char [25];
//	Profile->StartTimer();
	// Измеряем быстродействие	
	MyManager->SetActiveShadersInProgramm( TorchFire->ShaderForDraw );
	TorchFire->Render();
//	Profile->StopTimer( Str );
//	MyManager->UpdateSentence( 4, Str, MyManager->GetSentencePosX (4), MyManager->GetSentencePosY (4) );

	// Измеряем быстродействие
	m_D3D->TurnZBufferOn();
	// --------------------------------------------------  PARTICLE SYSTEM  ------------------------------

	// -------------------------------- блок со включонным Z буфером

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// + Рисуем CUBEMAP	
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	m_D3D->RenderWireFrameResterizeState ();
	RCubeRender->RenderCubeMap ( CubeMapIndex );
//	m_D3D->SetCullNoneResterizeState ();
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
		//	MyManager->UpdateSentence(4, Str, MyManager->GetSentencePosX (4), MyManager->GetSentencePosY (4));
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
	Snow->ChangeActiveParticleAmount(3, ( int ) Hud->GetScrollBarValue ( 10 ) );


/*
	if ( ThreadDone )
	{
		ThreadDone = false;
		TheradStarted = false;
		MyManager->DeleteFont ( 5 );
		MyManager->RCube_Font.erase ( MyManager->RCube_Font.begin () + 5 );

		MyManager->UpdateSentence ( 17, MyManager->GetSentenceText ( 17 ) );
		MyManager->UpdateSentence ( 18, MyManager->GetSentenceText ( 18 ) );

	}
*/
	if ( Hud->GetEditFinished(  4 )	||		// Font size
		 Hud->GetEditFinished( 11 ) ||		// Outline size
		 Hud->GetButtonChanged( 12 )	||		// OUTLINED
		 Hud->GetButtonState ( 13 ) ||		// OUTLINE Colour
		 Hud->GetButtonState (  8 ) ||		// GR1 Colour
		 Hud->GetButtonState (  9 ) ||		// GR2 Colour
		 Hud->GetButtonState ( 10 ) ||		// GR3 Colour
		 Hud->GetScrollBarChanged ( 17 ) || // Gradient STOP Y POSITION
		 Hud->GetScrollBarChanged ( 16 ) || // Gradient STOP X POSITION
		 Hud->GetScrollBarChanged ( 15 ) || // Gradient START Y POSITION
		 Hud->GetScrollBarChanged ( 14 ) || // Gradient START X POSITION
		 Hud->GetScrollBarChanged ( 13 ) || // Gradient Stops 3 POSITION
		 Hud->GetScrollBarChanged ( 12 ) || // Gradient Stops 2 POSITION
		 Hud->GetScrollBarChanged ( 11 )    // Gradient Stops 1 POSITION
		 
//		 Hud->GetButtonState ( 14 )
		 )
	{
//		if ( !TheradStarted )
//			RunThread ();


//		MyManager->DeleteLastFont ();
		MyManager->DeleteFont ( 5 );

		RCube_Font_Style* Temp_Font_Style;
		Font_Param* Fparam = new Font_Param;

		Fparam->Font_Path_Name = L"Fonts/Tahoma.ttf";
		
		float Size = Hud->GetButtonTextAsFloat ( 4 );

		Fparam->FontSize = Size;
		Fparam->Outlined = Hud->GetButtonState ( 12 );
		Fparam->OutlineWidth = Hud->GetButtonTextAsInt ( 11 );

		XMFLOAT4 TempColor = Hud->GetButtonColour ( 8 );
		Fparam->gradientStops[0].color = D2D1::ColorF ( TempColor.x, TempColor.y, TempColor.z, 1 );
		Fparam->gradientStops[0].position = Hud->GetScrollBarValue ( 11 );
		
		TempColor = Hud->GetButtonColour ( 9 );
		Fparam->gradientStops[1].color = D2D1::ColorF ( TempColor.x, TempColor.y, TempColor.z, 1 );
		Fparam->gradientStops[1].position = Hud->GetScrollBarValue ( 12 );

		TempColor = Hud->GetButtonColour ( 10 );
		Fparam->gradientStops[2].color = D2D1::ColorF ( TempColor.x, TempColor.y, TempColor.z, 1 );
		Fparam->gradientStops[2].position = Hud->GetScrollBarValue ( 13 );

		Fparam->GradientPos = {
			Hud->GetScrollBarValue ( 14 ),
			Hud->GetScrollBarValue ( 15 ),
			Hud->GetScrollBarValue ( 16 ),
			Hud->GetScrollBarValue ( 17 )
		};

		Fparam->OutlineColor = Hud->GetButtonColour ( 13 );

		Temp_Font_Style = RCubeRender->Create_RCube_FontStyle ( Fparam );
		delete Fparam;
		MyManager->AddFont ( RCubeRender->Create_RCube_Font ( Temp_Font_Style, CharStr ) );
		delete Temp_Font_Style;
		MyManager->UpdateSentence ( 17, MyManager->GetSentenceText ( 17 ));
		MyManager->UpdateSentence ( 18, MyManager->GetSentenceText ( 18 ));


	}

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
//			MyManager->UpdateSentence(13, Str);
		MyManager->SetActiveShadersInProgramm ( MainMenu->ShaderForDraw );
		RCubeRender->RenderMenu ( 0 );
//		MainMenu->Draw();
		MyManager->SetActiveShadersInProgramm( MyManager->TextShaderIndex );
		RCubeRender->RenderText ( 1 );	// Рисуем текст для меню

	}
// ++++++++++++++++++++++++     Рисуем текст для меню     +++++++++++++++++++++++++++++

		// Рисуем мышкин курсор
//		MyManager->SetActiveShadersInProgramm( m_Bitmap->ShaderIndex );
//		m_Bitmap->ChangeMousePosition( mouseX, mouseY );
//		RCubeRender->RenderFlatObject ( MyManager->Get_Flat_ObjectBuffers_ByIndex ( m_Bitmap->BuffersIndex ) );
		// Рисуем мышкин курсор

	m_D3D->SetBackBufferRenderTarget();
	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();
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

	m_D3D->D3DGC->ViewProjection = m_D3D->D3DGC->ViewMatrix * m_D3D->D3DGC->ProjectionMatrix;
}

void GraphicsClass::UpdateConstantBuffer()
{
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++     Инициализируем массив матриц для всех шейдеров     ++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ConstantBufferData cbPerObj;
	// Расчёт матрицы для CubeMap
	XMMATRIX Scale = XMMatrixScaling( MATRIX_SCALING, MATRIX_SCALING, MATRIX_SCALING );
	XMMATRIX Translation = XMMatrixTranslation(m_D3D->D3DGC->CameraPosition.x, m_D3D->D3DGC->CameraPosition.y, m_D3D->D3DGC->CameraPosition.z);
	XMMATRIX mtr = Scale * Translation * m_D3D->D3DGC->ViewProjection;

	//No XMMatrixTranspose needed
	// https://softwaretrickery.com/2014/03/25/directx-why-examples-call-xmmatrixtranspose/
	cbPerObj.World = m_D3D->D3DGC->WorldMatrix;
	cbPerObj.View = m_D3D->D3DGC->ViewMatrix;
	cbPerObj.Projection = m_D3D->D3DGC->ProjectionMatrix;
	cbPerObj.ViewProjection = m_D3D->D3DGC->ViewProjection;
	cbPerObj.ScaleMatrix = mtr;	// CubeMapMatrix
	cbPerObj.Ortho = m_D3D->D3DGC->OrthoMatrix; 
	cbPerObj.cameraPosition = m_D3D->D3DGC->CameraPosition;
	RCube_VecFloat234 Temp;
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


DWORD WINAPI GraphicsClass::ThreadFunc ( LPVOID me )
{
	reinterpret_cast< GraphicsClass* >( me )->RunThread2();

	return 0;
}


void GraphicsClass::RunThread2 ()
{
	RCube_Font_Style* Temp_Font_Style;
	Font_Param* Fparam = new Font_Param;

	Fparam->Font_Path_Name = L"Fonts/Tahoma.ttf";

	float Size = Hud->GetButtonTextAsFloat ( 4 );

	Fparam->FontSize = Size;
	Fparam->Outlined = Hud->GetButtonState ( 12 );
	Fparam->OutlineWidth = Hud->GetButtonTextAsInt ( 11 );

	XMFLOAT4 TempColor = Hud->GetButtonColour ( 8 );
	Fparam->gradientStops[0].color = D2D1::ColorF ( TempColor.x, TempColor.y, TempColor.z, 1 );
	Fparam->gradientStops[0].position = Hud->GetScrollBarValue ( 11 );

	TempColor = Hud->GetButtonColour ( 9 );
	Fparam->gradientStops[1].color = D2D1::ColorF ( TempColor.x, TempColor.y, TempColor.z, 1 );
	Fparam->gradientStops[1].position = Hud->GetScrollBarValue ( 12 );

	TempColor = Hud->GetButtonColour ( 10 );
	Fparam->gradientStops[2].color = D2D1::ColorF ( TempColor.x, TempColor.y, TempColor.z, 1 );
	Fparam->gradientStops[2].position = Hud->GetScrollBarValue ( 13 );

	Fparam->GradientPos = {
		Hud->GetScrollBarValue ( 14 ),
		Hud->GetScrollBarValue ( 15 ),
		Hud->GetScrollBarValue ( 16 ),
		Hud->GetScrollBarValue ( 17 )
	};

	Fparam->OutlineColor = Hud->GetButtonColour ( 13 );

	Temp_Font_Style = RCubeRender->Create_RCube_FontStyle ( Fparam );
	delete Fparam;
	MyManager->AddFont ( RCubeRender->Create_RCube_Font ( Temp_Font_Style, CharStr ) );
	delete Temp_Font_Style;

	ThreadDone = true;
	TheradStarted = false;
}


void  GraphicsClass::RunThread ()
{
	HANDLE hThread;
	DWORD idThread;

	hThread = CreateThread ( NULL, 0, ThreadFunc, this, 0, &idThread );

	TheradStarted = true;
	ThreadDone = false;
}