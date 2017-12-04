#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "EngineKernel.h"
#include "D3DGlobalContext.h"

// UTF16 код нажатой клавиши
int UTF16_KeyCode = -1;

// ПОТОК
// https://msdn.microsoft.com/query/dev14.query?appId=Dev14IDEF1&l=EN-US&k=k%28process%2F_beginthread%29;k%28_beginthread%29;k%28DevLang-C%2B%2B%29;k%28TargetOS-Windows%29&rd=true
// http://www.cyberforum.ru/visual-cpp/thread3803.html
CRITICAL_SECTION cs;
int a[5];

void Thread(void* pParams)
{
	int i, num = 0;

	while (TRUE)
	{
		EnterCriticalSection(&cs);
		for (i = 0; i < 5; i++) a[i] = num;
		LeaveCriticalSection(&cs);
		++num;
	}
}
// ПОТОК
// In the class constructor I initialize the object pointers to null. This is important because if the initialization of these objects fail then the Shutdown function further on will attempt to clean up those objects. If the objects are not null then it assumes they were valid created objects and that they need to be cleaned up. It is also good practice to initialize all pointers and variables to null in your applications. Some release builds will fail if you do not do so.
EngineKernel::EngineKernel()
{
		m_Input = nullptr;
	 m_Graphics = nullptr;
		  m_Cpu = nullptr;
		m_Timer = nullptr;
		m_Sound = nullptr;
			Pic = nullptr;
	   RNetWork = nullptr;

	ZeroMemory(&Counters, sizeof(FPSTimers));
	ZeroMemory(&InputStruct, sizeof(DXINPUTSTRUCT));
	DZ = 0.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;//-20.0f;

	CamX = 0.0f;
	CamY = 0.0f;
	CamZ = 0.0f;

// PhysX objects
//	CubicObject		= new PxTransform [MAX_OBJ];
//	CubeRotations	= new XMMATRIX [MAX_OBJ];
//	gCube = new PxRigidDynamic[500];

	rot = 0.00f;
	rot2 = 0.00f;
	Speed2 = 20.0f; // Об/мин
	Speed = 50.0f;  // Об/мин
	
	MouseSpeed = 20.0f;
	KeySpeed = 1000.0f;
	EngineFrameTime = 0.000001f;

	Start = true;
	
	IsEscapeDown = false;
	IsMenuOn = false;
//	IsOptionsOn = false;

// Используем для отладки теней в и шейдера
	DiffuseX = 0.0f;
	DiffuseY = 0.0f;
	DiffuseZ = 0.0f;

	DepthBias = 0.0f;
	SlopeScaledDepthBias = 0.0f;

}

// Here I create an empty copy constructor and empty class destructor. In this class I don't have need of them but if not defined some compilers will generate them for you, and in which case I'd rather they be empty.
//You will also notice I don't do any object clean up in the class destructor. I instead do all my object clean up in the Shutdown function you will see further down. The reason being is that I don't trust it to be called. Certain windows functions like ExitThread() are known for not calling your class destructors resulting in memory leaks. You can of course call safer versions of these functions now but I'm just being careful when programming on windows.
EngineKernel::EngineKernel(const EngineKernel& other)
{
}


EngineKernel::~EngineKernel()
{
}

// The following Initialize function does all the setup for the application. It first calls InitializeWindows which will create the window for our application to use. It also creates and initializes both the input and graphics objects that the application will use for handling user input and rendering graphics to the screen.
bool EngineKernel::Initialize()
{
	//	int screenWidth, screenHeight;
	bool result;
	HRESULT res;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;
	WindowPosX = 0;
	WindowPosY = 0;
	IsMenuOn = false;


	// Initialize the windows api.
	InitializeWindows( screenWidth, screenHeight, WindowPosX, WindowPosY );

	// Create the input object.  This object will be used to handle reading the input devices from the user.
	m_Input = new InputClass;
	// Initialize the input object.
	result = m_Input->Initialize( m_hinstance, m_hwnd, screenWidth, screenHeight );
	if (!result)
	{
		MessageBox( m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK );
		Shutdown();
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	// Initialize the graphics object.
	result = m_Graphics->Initialize( m_hwnd, screenWidth, screenHeight, WindowPosX, WindowPosY, m_Input );
	if (!result)
	{
		Shutdown();
		return false;
	}
	/*
	********************************************ОПИСАНИЕ KF_PXControler класса********************************************
	пока наш контролер достатосно прост это не глюк а скорее не использование физикса в полной менре.Я не супер человек а физика достаточно кастомная так что так или 
	иначе мне придеттся чтото добавлять т.е если чего то нет это не значит что я это специально не сделал  прост я проэто скорее всего не знал , это может касаться 
	некоторых доп функции физикса или различны его настроек. Прост если чтото необходимо добавить (скорре всего это понадобиться в реализации конкретного обьекта 
	придется это доделать).
	
	создание физики! это минимальный набор функции неообходим для работы физики
	***************************************************************************

	обьявляется он как и все мои предыдущие классы - ст 141 EngineKernel.h

	затем происходит его инициализация - ст 175 - 184

	симуляция и фрэйм - ст 604 - 624

	резил - ст 510
	***************************************************************************
	
	добасление обьектов! это функции которые позволяют добавлять обьекты как токовые т.е если ты не добавижэтот тот или иной обьект по 
	сути ничего не изменьться просто он ге будет задейстован в физике
	***************************************************************************

	создание отдельного обьекта - ст 245 - 257

	созание инстансинг обьектов - ст 217, 218

	добавление карты высот - ст 489 - 495

	***************************************************************************
	
	для того чтобы понять как создается Fiz файл смотри фйл пример- описатние Models/Пример Fiz файла.txt


	********************************************ОПИСАНИЕ KF_PXControler класса********************************************
	*/



	KF_PXControler::ObjectData data; // эта структура дает функции полное понимае о индексах обьета в моделЛисте

	//++++++++++++++++++++инит физикса++++++++++++++++++++++++++++++

	PxControl = new KF_PXControler; // выделение памяти под его
	PxControl->Init( &m_hwnd/*это параметры окна в котором находиться выполнение движка (необходимо для вывода сообщении об ошибках)*/, 
	m_Graphics->GetD3DGC(),
	m_Graphics->MyManager
		/*глобальный моделлист в котором обьекты которые добавляются в физику*/); //  инит его

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   С ЭТИМ МЕСТОМ МОГУТ БЫТЬ ВОПРОСЫ !!!

//	m_Graphics->m_Camera->SetPosition(0.0f, 0.0f, 0.0f);

	//я не знаю на счет этих строк я пробовал вынести их в PXControl->init но ничего не вышло  ошибка происходит
	PxDefaultMemoryOutputStream Stream;
	PxControl->outStream = &Stream;
	//*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*1*
	//++++++++++++++++++++инит физикса++++++++++++++++++++++++++++++

	// ++++++++++++++++++++++++++++++   Init Objects   +++++++++++++++++++++++++++++++++++
//	int BlobIndex = m_Graphics->MyManager->GetShaderIndexByName ( L"ClusteredSM" ) * 2;
	int BunchShaderIndex = m_Graphics->MyManager->GetShaderIndexByName ( L"ClusteredSM" );


// + NEW RENDER !!!!!!!!!!!!
	int index = m_Graphics->MyManager->Add_3D_Object ( L"Models/Cube.kfo", MAX_OBJ );
	m_Graphics->MyManager->InitRandInstansingPoses ( index, ( m_Graphics->KFTerrain->g_Rows / 2 ) * m_Graphics->KFTerrain->g_VertixesIndent
										 , -( ( m_Graphics->KFTerrain->g_Rows / 2 ) * m_Graphics->KFTerrain->g_VertixesIndent ), 20.0f, 19.0f
										 , ( m_Graphics->KFTerrain->g_Columns / 2 ) * m_Graphics->KFTerrain->g_VertixesIndent, -( ( m_Graphics->KFTerrain->g_Columns / 2 ) * m_Graphics->KFTerrain->g_VertixesIndent ) );
	m_Graphics->MyManager->InitRandInstansingRots ( index );

	index = m_Graphics->MyManager->Add_3D_Object ( L"Models/Ship.kfo", 1 );
	m_Graphics->MyManager->Set_3D_Object_Positon ( index, 0, 150.0f, 0.0f, 0.0f );

	index = m_Graphics->MyManager->Add_3D_Object ( L"Models/Dragon.kfo", 1 );//2
	m_Graphics->MyManager->Set_3D_Object_Positon ( index, 0, XMFLOAT3 ( 100.0f, 4.0f, -20.0f ) );
// - NEW RENDER !!!!!!!!!!!!

	m_Graphics->ModelList->AddObject( L"Models/Cube.kfo", BunchShaderIndex, MAX_OBJ);//0

	m_Graphics->ModelList->InitRandInstansingPoses( 0, (m_Graphics->KFTerrain->g_Rows / 2) * m_Graphics->KFTerrain->g_VertixesIndent
		, -((m_Graphics->KFTerrain->g_Rows / 2) * m_Graphics->KFTerrain->g_VertixesIndent), 20.0f, 19.0f
		, (m_Graphics->KFTerrain->g_Columns / 2) * m_Graphics->KFTerrain->g_VertixesIndent, -((m_Graphics->KFTerrain->g_Columns / 2) * m_Graphics->KFTerrain->g_VertixesIndent) );

	m_Graphics->ModelList->InitRandInstansingRots( 0 );

	///****************Добавление инстнсинг обьектов***************************************

	//вот все просто одна функция
	PxControl->CreateInstancingObjects(
	L"Models/Cube.fiz.txt",	// имя файла с данными о физике 
	true,					// это значит динамическии обьект или статическии
	0						// это номер обьекта в индексах ModelList который используется в движке
	);

	// Joints
	PxControl->JoinAllCubes();
//	PxActor* Actor->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, true );

	///****************Добавление инстнсинг обьектов***************************************

	m_Graphics->ModelList->AddObject( L"Models/Ship.kfo", BunchShaderIndex, 1);//1
	m_Graphics->ModelList->SetPositon( 1, 0, 0.0f, 5.0f, 0.0f );


	m_Graphics->ModelList->AddObject(L"Models/Dragon.kfo", BunchShaderIndex, 1);//2
	m_Graphics->ModelList->SetPositon(2 , 0 , XMFLOAT3(-100.0f , 4.0f , 20.0f ));
	//m_Graphics->ModelList->SetRotation(2, 0, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	// ********************** ДОБАВЛЕНИЕ ОДНОГО ОБЬЕКТА ОБЬЕКТА  ********************************************
	//========================
	data.InstanceIndex = 0;//индекс инстэнса
	data.ModelIndex = 2;//индекс обькта
	//=======================
	// вот это непосредственно создание
/*	PxControl->CreateObject(
	L"Models/stones.fiz.txt",//имя файла описывающего физ свойства
	false, //статическии или динамическии обьект
	&data  //указатель на структуру описанную выше
	);
	data.InstanceIndex = 0;//индекс инстэнса
	data.ModelIndex = 2;//индекс обькта
*/
//	PxControl->CreateCharacter(L"Models/Character.fiz.txt" , &data);
	///********************** ДОБАВЛЕНИЕ ОДНОГО ОБЬЕКТА ОБЬЕКТА  ********************************************


	// ------------------------------   Init Objects   -----------------------------------
	// Create and initialize the CpuClass.

	// Create the cpu object.
	m_Cpu = new CpuClass;
	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create and initialize the TimerClass.

	// Create the timer object.
	m_Timer = new TimerClass;
	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox( m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK );
		Shutdown();
		return false;
	}

	/*
	Force the main thread to always run on CPU 0.
	This is done because on some systems QueryPerformanceCounter returns a bit different counter values
	on the different CPUs (contrary to what it's supposed to do), which can cause negative frame times
	if the thread is scheduled on the other CPU in the next frame. This can cause very jerky behavior and
	appear as if frames return out of order.
	// http://humus.name/index.php?page=3D
	*/
	SetThreadAffinityMask( GetCurrentThread(), 1 );


	//	int fhf = m_Timer->GenerateRandNym(-100, 100);

	m_Sound = new SoundClass;
	// Initialize the sound object.
	result = m_Sound->Initialize( m_hwnd );
	if (!result)
	{
		MessageBox( m_hwnd, L"Could not initialize Direct Sound.", L"Error", MB_OK );
		Shutdown();
		return false;
	}

//	m_Graphics->MyManager->InitSounds ( L"Sounds/sounds.kaf", m_Sound );

	KF3DObjectData * Data = new KF3DObjectData[26];
	int c = 0;
	/*while (c < 26){
	for (int j = 0; j < 24; ++j)
	{
	Data[c].ObjVertPosArray.push_back(m_Graphics->m_CubeRubic->Vertices[c][j].Vertex_Pos);
	}
	c++;
	}*/
	c = 0;
	/*while (c < 26){
	for (int j = 0; j < 36; ++j)
	{
	Data[c].ObjVertIndexArray.push_back(m_Graphics->m_CubeRubic->indices3x3[j]);
	}
	c++;
	}*/


	c = 0;
	/*while (c < 26){
	//	Data[c].ObjWorld = &m_Graphics->m_CubeRubic->g_World[c];
	c++;
	}*/

	Pic = new KFPicking;
	res = Pic->Init( m_hwnd, WindowPosX, WindowPosY, screenWidth, screenHeight, Data, 26 );
	if (FAILED( res ))
	{
		MessageBox( m_hwnd, L"Could not initialize Picking.", L"Error", MB_OK );
		Shutdown();
		return false;
	}
	//	delete [] SphereVertexType ;
	delete[] Data;

	//  СЕТЬ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/*	RNetWork = new RCubeNetwork;

	if ( !RNetWork->InitRcubeNetwork(m_hwnd) )
	{
	Shutdown();
	return false;
	}

	/*	if ( !RNetWork->InitRcubeServer(m_hwnd) )
	{
	Shutdown();
	return false;
	}
	*/
	/*	if (!RNetWork->InitRcubeClient(m_hwnd))
	{
	Shutdown();
	return false;
	}

	RNetWork->ConnectToServer();
	// ЗАПУСК СЕТЕВОГО ПОТОКА
	//	InitializeCriticalSection(&cs);
	//	_beginthread(Thread, 0, NULL);
	//	_endthread();
	/*
	char *Str  = new char[30];
	char *Str1 = nullptr;

	Str1 = RNetWork->GetCurrentIp();

	SENTENCE_INIT_DATA* Data1 = new SENTENCE_INIT_DATA;
	ZeroMemory(Data1, sizeof(SENTENCE_INIT_DATA));
	int Number;

	//	Number = enet_address_get_host_ip(&address,Str2,20);
	sprintf_s(Str, 30, "Host IP : %s", Str1);

	Data1->MaxLength = 30;
	Data1->PosX = 100;
	Data1->PosY = 800;
	Data1->ShowType = 1;
	Data1->Blue = 0.0f;
	Data1->Red = 0.0f;
	Data1->Green = 1.0f;
	Data1->Alpha = 1.0f;
	Data1->Render = true;
	Data1->FontType = 0;
	Number = m_Graphics->m_Text->AddSentence(Data1, Str);

	delete [] Data1;

	delete [] Str;
	*/

	//  СЕТЬ ------------------------------------------------------------------------------------


	//	++++++++++++++++++++++++++++++   PhysX Initialisation   ++++++++++++++++++++++++++++++++++++++++++

	/// *************** добавление карты высот (террэйна******************************
	// одна функция
	PxControl->CreateHeildHield( m_Graphics->KFTerrain->g_Rows			/*количество точек по оси X*/, 
								 m_Graphics->KFTerrain->g_Columns		/*количество точек по оси Y*/,
								 m_Graphics->KFTerrain->g_VertixesIndent/*отступ между точками*/, 
								 m_Graphics->KFTerrain->m_heightMap		/*сам вот этот буфер вершин*/, 
								 L"Models/Terrain.fiz.txt"				/*а это файл физики котрый описывает свойства*/ );

	// если юзается все время террэйн моего типа по прсто можно имя заменить Terrain на произвольно созданный Terrain или же если наробка или нейкии другой обьект использующии карту вершин можно просто грамотно воспользоватья оргументами для создания
	/// *************** добавление карты высот (террэйна******************************


	// Создаём Кватернион для PhysX		


	//PXSAFE_RELEASE(shape);
	//	------------------------------	 PhysX Initialisation   -------------------------------------------

	return true;
}


//The Shutdown function does the clean up. It shuts down and releases everything associated with the graphics and input object. As well it also shuts down the window and cleans up the handles associated with it.
void EngineKernel::Shutdown()

{

	RCUBE_DELETE	(PxControl); /// релиз
	RCUBE_SHUTDOWN	(m_Sound);
	RCUBE_DELETE	(m_Timer);
	RCUBE_SHUTDOWN	(m_Cpu);
	RCUBE_SHUTDOWN	( m_Input );
	RCUBE_DELETE	(m_Graphics);
	RCUBE_DELETE	(Pic);

	// Shutdown the window.
	ShutdownWindows();

	return;
}

/* The Run function is where our application will loop and do all the application processing until we decide to quit. The application processing is done in the Frame function which is called each loop. This is an important concept to understand as now the rest of our application must be written with this in mind. The pseudo code looks like the following:

while not done
check for windows system messages
process system messages
process application loop
check if user wanted to quit during the frame processing
*/
void EngineKernel::Run()
{
	MSG msg;
	bool  result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			// Запускаем глобальный таймер времени кадра нашего движкка
			m_Timer->StartTimer(StartCounter);
			result = Frame();
			m_Timer->ComputeTimePassed(EngineFrameTime, StartCounter, StopCounter);
// СЕТЬ
// Проверка событий в сети	
//			RNetWork->ConnectToServer();
//			RNetWork->RcubeSendPacket();
//			RNetWork->CheckRcubeClientEvent();

			if (!result)
			{
				//	MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
		}
		// Check if the user pressed escape and wants to quit.
		/*	if(m_Input->IsEscapePressed() == true)
		{
		done = true;
		}*/
	}

	return;

}

//The following Frame function is where all the processing for our application is done. So far it is fairly simple, we check the input object to see if the user has pressed escape and wants to quit. If they don't want to quit then we call the graphics object to do its frame processing which will render the graphics for that frame. As the application grows we'll place more code inside here.
bool EngineKernel::Frame()
{
	bool result;

//	++++++++++++++++++++++++++++++   PhysX Initialisation   ++++++++++++++++++++++++++++++++++++++++++
	if (Start)
	{
//		gCube[0]->setAngularDamping(0.1f);
//		gCube[0]->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
//		PxRigidBodyExt::updateMassAndInertia(*gCube[0], 50.0f);
		Start = false;
	}
	bool Start2 = true;
	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
//	char Str[25];// = new char [25];
//	m_Timer->StartTimer();
	// Измеряем быстродействие

	// симуляцию я не выносил ведь это важная часть внешнеей работы с физикой делается она по средству как ниже
	PxControl->gScene->simulate(1.0f / 60.0f);
	while (!PxControl->gScene->fetchResults(false))
	{
		//  Тут можно что угодно делать пока не завершится симуляция
		if ( Start2 )
		{
			result = m_Graphics->Frame ( Counters, InputStruct );
			Start2 = false;
		}

	}
	// Измеряем быстродействие
//	m_Timer->StopTimer ( Str );
//	m_Graphics->m_Text->UpdateSentence ( 5, Str, 100, 160 );
	// Измеряем быстродействие

// ++++++++++++++++++     Обрабатываем изменения SсrollBars     ++++++++++++++++++++
	Ambi = m_Graphics->Hud->GetScrollBarValue ( 0 );
	m_Graphics->m_D3D->SetAmbientColor ( XMFLOAT4 { Ambi,Ambi,Ambi,Ambi } );

	DiffuseX = m_Graphics->Hud->GetScrollBarValue( 1 );
	DiffuseY = m_Graphics->Hud->GetScrollBarValue( 8 );
	DiffuseZ = m_Graphics->Hud->GetScrollBarValue( 9 );
	m_Graphics->m_D3D->SetDiffuseColor( XMFLOAT4 { DiffuseX,DiffuseY,DiffuseZ,0.0f } );

	float Val = m_Graphics->Hud->GetScrollBarValue( 2 );
	bool Need = false;
	if ( DepthBias != Val )
	{
		Need = true;
		DepthBias = Val;
	}

	float Val2 = m_Graphics->Hud->GetScrollBarValue( 3 );
	
	if ( SlopeScaledDepthBias != Val2 )
	{
		Need = true;
		SlopeScaledDepthBias = Val2;
	}

	if ( Need )
		m_Graphics->ShadowWork->InitRasterizerState((int) DepthBias, SlopeScaledDepthBias );


// ++++++++++++++++++     Обрабатываем изменения SсrollBars     ++++++++++++++++++++

	///*********************************Симуляция************************************


	// Измеряем быстродействие
	// Федя 540 ns    Мой  517 ns   
//		char Str[25];// = new char [25];
//		m_Timer->StartTimer ();
	// Измеряем быстродействие
	/// Frame*********************************************
	PxControl->Frame();
	/// Frame*********************************************
	// Измеряем быстродействие
//		m_Timer->StopTimer(Str);
//		m_Graphics->m_Text->UpdateSentence(5, Str, 100, 160);
	// Измеряем быстродействие
//	------------------------------	 PhysX Initialisation   -------------------------------------------

	static float x = 0.0f;
	static float y = 0.0f;
	static float z = 0.0f;//-20.0f;

	// Update the system stats.
	m_Timer->Frame();
	m_Cpu->Frame();

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Get the location of the mouse from the input object,
	m_Input->GetDXInput(InputStruct);

	m_Cpu->GetCpuPercentage(Counters.CpuVal);
	m_Timer->GetFps(Counters.FpsRate);
	Counters.FrameTime = EngineFrameTime;// * 100;
	Counters.Seed += Counters.FpsRate;
//	m_Timer->GetFrameTime(Counters.FrameTime);
//	m_Timer->GetSpeed(Counters.ComputerSpeed);
	Counters.ComputerSpeed = (int)(( 1 / EngineFrameTime ) / (KeySpeed / 10));

	// если меню выключено
	if ( !IsMenuOn ){

			KeysPressed();
			DIMOUSESTATE2 State;
			m_Input->GetMouseState(State);

			// обновляем HUD
			m_Graphics->Hud->Frame( m_Input, UTF16_KeyCode, m_Graphics->fpstimers);
			// рисуем HUD
//			m_Graphics->Hud->IsMenuDraw = true;
			if ( m_Graphics->Hud->GetButtonState(0) )
			{
//				m_Graphics->MainMenu->SetMenuPos(XMFLOAT2{ 500.0f, 200.0f });
//				m_Graphics->MainMenu->SetButtonParam( 2 , XMFLOAT4{ 100.0f, 10.0f, 100.0f, 20.0f } );
//				m_Graphics->MainMenu->SetButtonSize( 2, 1.2f );
//				m_Graphics->MainMenu->SetButtonPos( 2, 1.2f );
				m_Graphics->AnimTexture->Enabled = false;
			}		
	if ( m_Graphics->Hud->GetButtonState(1) )
	{
//				m_Graphics->MainMenu->SetMenuPos(XMFLOAT2{ 112.0f, 84.0f });
//				m_Graphics->MainMenu->SetButtonToOrigin( 2 );
//				m_Graphics->MainMenu->SetButtonSize( 2 , 1.0f );
//				m_Graphics->MainMenu->SetButtonPos( 2, 1.0f );
				m_Graphics->AnimTexture->Enabled = true;
	}
		// По нажатих ESCAPE выводим меню или закрываем его, если уже выведино
		if ( m_Input->ActionKey[23].Pressed )
		{
			IsEscapeDown = true;
		}
		else if ( IsEscapeDown )
		{
			// перестаём обрабатывать нажатия клавиш на HUD, но продолжаем его рисовать
			m_Graphics->Hud->IsMenuActive = false;
			// Начинаем обрабатывать нажатия клавиш в меню
			m_Graphics->MainMenu->IsMenuActive = true;
			m_Graphics->MainMenu->IsMenuDraw = true;
			IsEscapeDown = false;
			IsMenuOn = true;
		}


		// Update the rotation variable each frame.
		//	m_Graphics->rotation += (float)XM_PI * 0.0005f;
		//	if(m_Graphics->rotation > 360.0f)
		//	{
		//		m_Graphics->rotation -= 360.0f;
		//	}																			
		static bool IsShoot = false;
		// Если нажали мышкой по кубику, то мониторим где отпустили мышку, чтобы понять нужно ли вращять грань
		static bool CubeClick = false;

		// +++++++++++++++++++++++++++++++++ PICKING ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Если не вращаются грани кубика можно проверять нажатие мышки на гранях
		/*if (!m_Graphics->m_CubeRubic->RotationRuning) // && m_Graphics->m_CubeRubic->RotateIterations > 1 
		{
			if (InputStruct.m_mouseState.rgbButtons[0] & 0x80){
				// действуящая версия пикинга
				if (IsShoot == false)
				{
					XMMATRIX Proj, View;
					m_Graphics->m_D3D->GetProjectionMatrix(Proj);
					m_Graphics->m_Camera->GetViewMatrix(View);
					int TriAngle = 0xffffffff;
					//		GetCursorPos(&mousePos);	
					int a = Pic->ApproofObjs(InputStruct.m_mouseX, InputStruct.m_mouseY, Proj, View, TriAngle);

					m_Graphics->Exp = TriAngle;
					//		if (a != -1)
					//		{
					if (a == 26){
						m_Graphics->IsSphereStartRender = false;
						Pic->AproofObjectsDesc[a] = false;
					}
					if (a > -1 && a < 26 && !CubeClick)
					{
						// признак что был клик по любому кубику - для включения мониторинга отпускания кнопки мышки
						CubeClick = true;
						//m_Graphics->m_CubeRubic->CubTriangels->Cubs[0] = a;
						//m_Graphics->m_CubeRubic->CubTriangels->Triangels[0] = TriAngle;
						// ---------------------------------------------------------
						Counters.Triangel1 = TriAngle;
						Counters.Cube1 = a;
						// ---------------------------------------------------------
					}
					//		}
				}
				IsShoot = true;
			}
			if (!InputStruct.m_mouseState.rgbButtons[0]){
				if (CubeClick)
				{
					XMMATRIX Proj, View;
					m_Graphics->m_D3D->GetProjectionMatrix(Proj);
					m_Graphics->m_Camera->GetViewMatrix(View);
					int TriAngle = 0xffffffff;
					int a = Pic->ApproofObjs(InputStruct.m_mouseX, InputStruct.m_mouseY, Proj, View, TriAngle);
					m_Graphics->Exp = TriAngle;

					//m_Graphics->m_CubeRubic->CubTriangels->Cubs[1] = a;
					//m_Graphics->m_CubeRubic->CubTriangels->Triangels[1] = TriAngle;
					// ---------------------------------------------------------
					Counters.Triangel2 = TriAngle;
					Counters.Cube2 = a;
					// ---------------------------------------------------------
					// Кубики дожны быть разными
					/*if (m_Graphics->m_CubeRubic->CubTriangels->Cubs[0] != m_Graphics->m_CubeRubic->CubTriangels->Cubs[1])
						// Проверяем какую грань нужнго повернуть и куда
						if (m_Graphics->m_CubeRubic->CheckCubTri())
						{
							m_Graphics->m_CubeRubic->Cubics_Positions_Change(m_Graphics->m_CubeRubic->Rotate_dir, m_Graphics->m_CubeRubic->Gran_Number);
							// ---------------------------------------------------------
							Counters.Gran = m_Graphics->m_CubeRubic->Gran_Number;
							// --------------------------------------------------------
							m_Graphics->m_CubeRubic->NeedRotation = true;
							m_Graphics->m_CubeRubic->RotationRuning = true;
							if (!m_Sound->_3DSoundPlaying(1))
								m_Sound->PlayWaveFile3D(1, 0, 0, 0, 40);
						}
					// Очищаем массив входных данных от старых значений
					m_Graphics->m_CubeRubic->ClearInputCubTri();
					CubeClick = false;
				}

				IsShoot = false;
			}
		}*/


		// ++++++++++++++++++++++++++++++++++    TEST ВРАЩАЕМ СВЕТ    ++++++++++++++++++++++++++++++
		//Keep the cubes rotating
//		rot = 0.05;//1.0f / ( fpstimers.FpsRate );
		rot = (XM_2PI * Speed2) * EngineFrameTime;
//		if (rot > XM_2PI)
//			rot = 0.0f;
//		rot2 += XM_PI / 360;
		
		rot2 += ( XM_2PI * Speed ) * EngineFrameTime ;
		if (rot2 > XM_2PI)
			rot2 = 0.0f;

		float X, Y, Z;

		X = 0;
		//	X = rot * 80;

		Y = 0;
		//	Y = rot * 40;

		Z = 0;
		Z = rot * 80;

		// Ворочаем направление SpotLight
//		XMVECTOR Source;

		XMVECTOR Axis = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		XMMATRIX Rot = XMMatrixRotationAxis(XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f }, rot);
		// Измеряем быстродействие
		// Федя 540 ns    Мой  517 ns   
//			char Str[25];// = new char [25];
//			m_Timer->StartTimer();
		// Измеряем быстродействие
/*	
			m_Graphics->m_D3D->GetLightDirection(0, Source);
			m_Graphics->m_D3D->ChangeLightDirection(0, XMVector3TransformCoord(Source, Rot));
//		m_Graphics->m_D3D->ChangeLightDirection(0, Axis, rot );
/*
Камеру в позицию света и поворот лицом к свету
		XMFLOAT3 Lpos;
		m_Graphics->m_D3D->GetLightPos(0, Lpos);
		m_Graphics->m_Camera->SetPosition(Lpos.x, Lpos.y, Lpos.z);
		m_Graphics->m_Camera->SetLookAt(Direction.x * 1000, Direction.y * 1000, Direction.z * 1000);
		m_Graphics->m_Camera->FirstPersonCam = false;
*/
/*
			m_Graphics->m_D3D->GetLightDirection(1, Source);
			m_Graphics->m_D3D->ChangeLightDirection(1, XMVector3TransformCoord(Source, Rot));
//		m_Graphics->m_D3D->ChangeLightDirection(1, Axis, rot);

			m_Graphics->m_D3D->GetLightDirection(2, Source);
			m_Graphics->m_D3D->ChangeLightDirection(2, XMVector3TransformCoord(Source, Rot));
//		m_Graphics->m_D3D->ChangeLightDirection(2, Axis, rot);
*/
		// Измеряем быстродействие
//			m_Timer->StopTimer(Str);
//			m_Graphics->m_Text->UpdateSentence(4, Str, 100, 160);
		// Измеряем быстродействие

// ++++++++++++++++++++++++++++++++++    TEST ВРАЩАЕМ СВЕТ    ++++++++++++++++++++++++++++++
//		result = m_Graphics->Frame(Counters, InputStruct);

		//	m_Graphics->MKButton->Frame(InputStruct.m_mouseState) ;
/*
		m_Graphics->MainMenu->g_MenuPosY = 0.5f;
		m_Graphics->MainMenu->g_Heigth = 0;
		m_Graphics->MainMenu->g_MenuPosX = 0.5f;
		m_Graphics->MainMenu->g_Width = 0;
*/
		// ------------------------------------------------------------------------------------
	}
	// если меню включено
	else {
		// По нажатих ESCAPE закрываем меню, если уже выведино
		if (m_Input->ActionKey[23].Pressed)
		{
			IsEscapeDown = true;
		}
		else if ( IsEscapeDown )
		{
			m_Graphics->MainMenu->IsMenuActive = false;
			m_Graphics->MainMenu->IsMenuDraw = false;
			IsMenuOn = false;
			// включаем обработку нажатия клавиш на HUD
			m_Graphics->Hud->IsMenuActive = true;
			IsEscapeDown = false;
		}

			// провекка нажатия кнопки выхода
			if ( m_Graphics->MainMenu->GetButtonState ( 4 ) )
				done = true;

			// кнопка продолжить
			if ( m_Graphics->MainMenu->GetButtonState ( 0 ) ) 
			{
				m_Graphics->MainMenu->IsMenuActive = false;
				m_Graphics->MainMenu->IsMenuDraw = false;
				IsMenuOn = false;
				// включаем обработку нажатия клавиш на HUD
				m_Graphics->Hud->IsMenuActive = true;
				IsEscapeDown = false;
				m_Graphics->MainMenu->ClearButtonState( 0 );
			}
			//кнопка новая игра
			if ( m_Graphics->MainMenu->GetButtonState ( 1 ) ) 
			{

			}
			// кнопка меню настроек
			if ( m_Graphics->MainMenu->GetButtonState ( 2 ) ) 
			{

			}

			// естли включено меню настроек
			if (m_Graphics->IsSettingsMenuOn)
			{
			}


//		m_Graphics->Frame(Counters, InputStruct);
	}

	if ( IsMenuOn )
	{
		m_Graphics->MainMenu->Frame( m_Input, UTF16_KeyCode, m_Graphics->fpstimers);
	}

	if (InputStruct.m_keyboardState[DIK_P] & 0x80) {
		done = true;
	}

	if (InputStruct.m_keyboardState[DIK_A] & 0x80) {
		y -= 0.01f;
		
	}

	return true;
}

//The MessageHandler function is where we direct the windows system messages into. This way we can listen for certain information that we are interested in. Currently we will just read if a key is pressed or if a key is released and pass that information on to the input object. All other information we will pass back to the windows default message handler.
LRESULT CALLBACK EngineKernel::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	/*	switch(umsg)
	{
	// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
	// If a key is pressed send it to the input object so it can record that state.
	m_Input->KeyDown((unsigned int)wparam);
	return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
	// If a key is released then send it to the input object so it can unset the state for that key.
	m_Input->KeyUp((unsigned int)wparam);
	return 0;
	}

	// Any other messages send to the default message handler as our application won't make use of them.
	default:
	{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
	*/
	return DefWindowProc(hwnd, umsg, wparam, lparam);

}

// The InitializeWindows function is where we put the code to build the window we will use to render to. It returns screenWidth and screenHeight back to the calling function
// so we can make use of them throughout the application. We create the window using some default settings to initialize a plain black window with no borders. 
// The function will make either a small window or make a full screen window depending on a global variable called FULL_SCREEN. 
// If this is set to true then we make the screen cover the entire users desktop window. If it is set to false we just make a 800x600 window in the middle of the screen. 
// I placed the FULL_SCREEN global variable at the top of the graphicsclass.h file in case you want to modify it. It will make sense later why I placed the global in that file
// instead of the header for this file.
void EngineKernel::InitializeWindows(int& screenWidth, int& screenHeight, int& WindowPosX, int& WindowPosY)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;
	UINT WindowInitParam;

	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
		// Окно на полный экран
		WindowInitParam = WS_POPUP;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.====================================================================
		//screenWidth  = 800;
		//screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
		// Окно в окно
		WindowInitParam = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	}

	WindowPosX = posX;
	WindowPosY = posY;

	// Create the window with the screen settings and get the handle to it.
	// https://msdn.microsoft.com/en-us/library/ms632600%28v=vs.85%29.aspx
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WindowInitParam,//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, //WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP    WS_THICKFRAME |
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// +++++     Нужно для того, чтобы курсор мышки в Windows совпадал с курсором D3D в оконном режиме
	LPRECT MyRect = new tagRECT;
	MyRect->bottom = 0;
	MyRect->top = 0;
	MyRect->left = 0;
	MyRect->right = 0;

	GetClientRect ( m_hwnd, MyRect );
	screenWidth = MyRect->right - MyRect->left;
	screenHeight = MyRect->bottom - MyRect->top;

	delete MyRect;
	// +++++     Нужно для того, чтобы курсор мышки в Windows совпадал с курсором D3D в оконном режиме

	// Hide the mouse cursor.
	//	ShowCursor(true);
	//	ShowCursor(false);

	return;
}

//ShutdownWindows does just that. It returns the screen settings back to normal and releases the window and the handles associated with it.
void EngineKernel::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

// The WndProc function is where windows sends its messages to. You'll notice we tell windows the name of it when we initialize the window class with wc.lpfnWndProc = WndProc in the InitializeWindows function above. I included it in this class file since we tie it directly into the system class by having it send all the messages to the MessageHandler function defined inside SystemClass. This allows us to hook the messaging functionality straight into our class and keep the code clean.
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms645530%28v=vs.85%29.aspx
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	UTF16_KeyCode = -1;

	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CHAR:
	{
		UTF16_KeyCode = (int)wparam;
		break;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
	return S_OK;
}

void EngineKernel::KeysPressed()
{
	float MouseCorrection = 0.0043633231;//0.00872664;// 0.01745329;
	float Speed = EngineFrameTime;

//		MouseCorrection = MouseSpeed * Speed;

	m_Graphics->m_Camera->moveBackForward = 0;
	m_Graphics->m_Camera->moveLeftRight = 0;


	//PxControl->DymanicRigitArray[MenPXIndex]->setLinearDamping(5.0f);
	// +++++++++++++++++++++++++++++++   Обработка мыши    ++++++++++++++++++++++++++++++
	if (InputStruct.m_mouseDX != 0 && InputStruct.m_mouseState.rgbButtons[1])
	{
		m_Graphics->m_Camera->GetRPY( CamX , CamY , CamZ );
		CamY += (InputStruct.m_mouseDX * MouseCorrection );
		m_Graphics->m_Camera->SetRotationFirstPerson(CamX, CamY, CamZ);

	}
	if (InputStruct.m_mouseDY != 0 && InputStruct.m_mouseState.rgbButtons[1])
	{
		m_Graphics->m_Camera->GetRPY( CamX, CamY, CamZ );
		CamX += (InputStruct.m_mouseDY * MouseCorrection );
		m_Graphics->m_Camera->SetRotationFirstPerson(CamX, CamY, CamZ);

	}

		XMFLOAT3 Position;
		XMVECTOR CamLook = m_Graphics->m_Camera->lookAt;
		m_Graphics->m_Camera->GetPosition(Position);
		XMFLOAT3 ComeputerVector, NormiliseVector;

			ComeputerVector.x = Position.x - XMVectorGetX(CamLook);
			ComeputerVector.y = Position.y - XMVectorGetY(CamLook);
			ComeputerVector.z = Position.z - XMVectorGetZ(CamLook);

			//if (!IsPreviosFrameWSADPressed) {

			float inv_length = 1.0f / sqrt(ComeputerVector.x*ComeputerVector.x + ComeputerVector.y*ComeputerVector.y + ComeputerVector.z*ComeputerVector.z);
			NormiliseVector.x = inv_length * ComeputerVector.x;
			NormiliseVector.y = inv_length * ComeputerVector.y;
			NormiliseVector.z = inv_length * ComeputerVector.z;
		//	m_Graphics->m_Camera->SetPosition(m_Graphics->ModelList->ObjDescs[2]->Positions[0].position.x + (NormiliseVector.x * 10)
		//		, m_Graphics->ModelList->ObjDescs[2]->Positions[0].position.y + NormiliseVector.y
		//		, m_Graphics->ModelList->ObjDescs[2]->Positions[0].position.z + (NormiliseVector.z * 10));
	


	if (InputStruct.m_mouseDZ != 0)
	{
		m_Graphics->m_Camera->FirstPersonCam = true;
		DZ = 0;
		DZ += InputStruct.m_mouseDZ * 0.01f;
		m_Graphics->m_Camera->moveBackForward = DZ;
	}
	// -------------------------------   Обработка мыши    --------------------------------
	//Y
	if (m_Input->ActionKey[16].Pressed)
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (Ambi >= 1.0f)
			Ambi = 1.0f;
		else
			Ambi += 0.01f;
		m_Graphics->m_D3D->SetAmbientColor(XMFLOAT4{Ambi,Ambi,Ambi,Ambi});
	}
	//H
	if (m_Input->ActionKey[17].Pressed)
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (Ambi <= 0.0f)
			Ambi = 0.0f;
		else
			Ambi -= 0.01f;
		m_Graphics->m_D3D->SetAmbientColor(XMFLOAT4{ Ambi,Ambi,Ambi,Ambi });
	}
	//Y
	if (m_Input->ActionKey[13].Pressed)
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (DifPower >= 1.0f)
			DifPower = 1.0f;
		else
			DifPower += 0.01f;
		m_Graphics->m_D3D->SetDiffusePower(DifPower);
	}
	//H
	if (m_Input->ActionKey[15].Pressed)
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (DifPower <= 0.0f)
			DifPower = 0.0f;
		else
			DifPower -= 0.01f;
		m_Graphics->m_D3D->SetDiffusePower(DifPower);
	}
	//T
	if (m_Input->ActionKey[12].Pressed)
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		char Str[50];// = new char [25];
		
		if ( SpecPower >= 0.01f ) //1.0f
			SpecPower = 0.01f;
		else
			SpecPower += 0.00001f;
		m_Graphics->m_D3D->SetSpecularPower(SpecPower);

		sprintf_s(Str, 50, "Spec. Pow = %8.4f", SpecPower);
		m_Graphics->MyManager->UpdateSentence(4, Str, 100, 160);
	}
	//G
	if (m_Input->ActionKey[14].Pressed)
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		char Str[50];// = new char [25];
		if (SpecPower <= 0.00001f)//0.011f
			SpecPower = 0.00001f;
		else
			SpecPower -= 0.00001f;
		m_Graphics->m_D3D->SetSpecularPower(SpecPower);

		sprintf_s(Str, 50, "Spec. Pow = %8.4f", SpecPower);
		m_Graphics->MyManager->UpdateSentence(4, Str, 100, 160);
	}
	//Z
	if (m_Input->ActionKey[4].Pressed)
	{
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		XMFLOAT4 Specular = { 0.0f, 1.0f, 0.0f, 1.0f };
		XMFLOAT4 Dif = {1.0f, 0.0f, 1.0f, 1.0f };
		XMFLOAT3 DifDir = { 0.0f, 0.0f, 1.0f };
		float DifPower = 1.0f;
		float SpecPower = 0.0f;
		m_Graphics->m_D3D->SetDiffuseDirection(DifDir);
		m_Graphics->m_D3D->SetSpecularDirection(DifDir);
		m_Graphics->m_D3D->SetDiffuseColor(Dif);
		m_Graphics->m_D3D->SetSpecularColor(Specular);
		m_Graphics->m_D3D->SetSpecularPower(SpecPower);
		m_Graphics->m_D3D->SetDiffusePower(DifPower);
	}
	//X
	if (m_Input->ActionKey[5].Pressed)
	{
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		XMFLOAT4 Specular = { 1.0f, 0.0f, 0.0f, 1.0f };
		XMFLOAT4 Dif = {1.0f, 0.0f, 1.0f, 1.0f};
		XMFLOAT3 DifDir = { 0.0f, 0.0f, 1.0f };
		float DifPower = 0.1f;
		float SpecPower = 0.05f;
		m_Graphics->m_D3D->SetDiffuseDirection(DifDir);
		m_Graphics->m_D3D->SetSpecularDirection(DifDir);
		m_Graphics->m_D3D->SetDiffuseColor(Dif);
		m_Graphics->m_D3D->SetSpecularColor(Specular);
		m_Graphics->m_D3D->SetSpecularPower(SpecPower);
		m_Graphics->m_D3D->SetDiffusePower(DifPower);
	}


	static float CharacterFrameCounter = 1.0f; // переменные пошагового смещения человека

	float MenMoveBackForvard = 0, MenmoveRightLeft = 0;

	// W
	if (m_Input->ActionKey[1].Pressed)
	{

		//PxControl->ControllersArray[0]->move(PxVec3(0.2f, 0.0f, 0.0f), 0.0003f, 100.0f, Filter);
//		MenMoveBackForvard = -(1000.0f * Speed);
		m_Graphics->m_Camera->moveBackForward += 0.5;
	}
	// S
	if (m_Input->ActionKey[3].Pressed)
	{

		//PxControl->ControllersArray[0]->move(PxVec3(-0.2f, 0.0f, 0.0f), 0.0003f, 100.0f, Filter);
//		MenMoveBackForvard = (1000.0f * Speed);
		m_Graphics->m_Camera->moveBackForward -= 0.5;
	}
	// D
	if (m_Input->ActionKey[9].Pressed)
	{

		//PxControl->ControllersArray[0]->move(PxVec3(0.0f, 0.0f, 0.2f), 0.0003f, 100.0f, Filter);

//		MenmoveRightLeft = (1000.0f * Speed);
		m_Graphics->m_Camera->moveLeftRight += 0.5;
	}
	// A
	if (m_Input->ActionKey[2].Pressed)
	{

		//PxControl->ControllersArray[0]->move(PxVec3(0.0f, 0.0f, -0.2f), 0.0003f, 100.0f, Filter);

//		MenmoveRightLeft = -(1000.0f * Speed);
		m_Graphics->m_Camera->moveLeftRight -= 0.5;
	}

	// Spase
	if (m_Input->ActionKey[26].Pressed)
	{
		//PxControl->DymanicRigitArray[MenPXIndex]->addForce(PxVec3(0.0f, 0.1f, 0.0f));
	}
/*
	PxExtendedVec3 menPos;
	XMVECTOR WorkQuat;

	CharacterFrameCounter -= (MenMoveBackForvard * 2);

	if (CharacterFrameCounter > 19.0f)
		CharacterFrameCounter = 1.0f;
	if (CharacterFrameCounter < 1.0f)
		CharacterFrameCounter = 19.0f;


	//m_Graphics->ModelList->FrameIndexes[2] = (int)CharacterFrameCounter;
	WorkQuat = XMQuaternionRotationRollPitchYaw(0.0f , CamY + 1.5f, 0.0f);
    XMMATRIX MenRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, CamY + 1.5f, 0.0f); //  матрица поворота человека


	XMVECTOR MenRight = XMVector3TransformCoord( m_Graphics->m_Camera->DefaultForward, MenRotationMatrix), 
		MenForward = XMVector3TransformCoord(m_Graphics->m_Camera->DefaultRight , MenRotationMatrix);


	XMVECTOR Mposition = MenMoveBackForvard*MenForward;
	Mposition += MenmoveRightLeft*MenRight ;

	XMFLOAT3 Men_position;
	XMStoreFloat3(&Men_position, Mposition);

	PxControllerFilters  Filter;

//	PxControl->ControllersArray[0]->move(PxVec3(Men_position.x, Men_position.y - 0.5f , Men_position.z), -1.0f, 1000.0f, Filter);

	// расчет позиции камеры то третего лица

    Mposition = 10.0f*MenForward;
	XMStoreFloat3(&Men_position, Mposition);

	menPos = PxControl->ControllersArray[0]->getPosition();
*/
	// R
	if (m_Input->ActionKey[6].Pressed)
	{
		m_Graphics->m_Camera->moveUp = .5f;// Speed * KeySpeed;
	}
	// F
	if (m_Input->ActionKey[8].Pressed)
	{
		m_Graphics->m_Camera->moveUp = -.5f;// -Speed * 1000;
	}
	if (m_Input->ActionKey[7].Pressed)
	{
		XMFLOAT3 Temp;
		Temp.x = 0.9f;
		Temp.y = 0.9f;
		Temp.z = 0.0f;
		m_Graphics->m_Camera->CameraRotate(Temp, 10) ;
	}
	if (m_Input->ActionKey[0].Pressed)
	{
		XMFLOAT3 Temp;
		Temp.x = 0.0f;
		Temp.y = 1.0f;
		Temp.z = 1.0f;
		m_Graphics->m_Camera->CameraMove(Temp);
	}


	if (m_Input->ActionKey[19].Pressed)
	{
		if (!m_Sound->_3DSoundPlaying(0))
			m_Sound->PlayWaveFile3D(0, 0, 0, 0, 40);
		CamY += 0.01f;//0.00005f;
		CamZ = 0.0f;
		CamX = 0.0f;
		m_Graphics->m_Camera->SetRotationFirstPerson(CamX, CamY, CamZ);
	}

	if (m_Input->ActionKey[20].Pressed)
	{
		if (!m_Sound->StereoSoundPlaying(1))
			m_Sound->PlayWaveFile(1);
		CamY -= 0.01f;
		CamZ = 0.0f;
		CamX = 0.0f;
		m_Graphics->m_Camera->SetRotationFirstPerson(CamX, CamY, CamZ);
	}

	if (m_Input->ActionKey[21].Pressed)
	{
		//		z = -0.001f;
//		x += -0.01f;
//		y = 0.0f;
//		m_Graphics->m_Camera->SetRotation(x, y, z);
		XMFLOAT3 a;// {  x, -0.3f, -0.9f };
		m_Graphics->m_D3D->GetLightDirection(0, a);
		a.x -= 1.0f;
		m_Graphics->m_D3D->ChangeLightDirection(0, a);
//		XMFLOAT3 a{ 30.0f + x, 10.0f, 70.0f };
//		m_Graphics->m_D3D->ChangeLightPos(0, a);
	}

	if (m_Input->ActionKey[22].Pressed)
	{
		//		z = 0.001f;
//		x += 0.01f;
//		y = 0.0f;
//		m_Graphics->m_Camera->SetRotation(x, y, z);
		XMFLOAT3 a;// {  x, -0.3f, -0.9f };
		m_Graphics->m_D3D->GetLightDirection(0, a);
		a.x += 1.0f;
		m_Graphics->m_D3D->ChangeLightDirection(0, a);
//		XMFLOAT3 a{ 30.0f + x , 10.0f, 70.0f };
//		m_Graphics->m_D3D->ChangeLightPos(0, a);
	}
	if (m_Input->ActionKey[24].Pressed)
	{
//		m_Graphics->m_D3D->LightsOnly();
//		m_Graphics->m_D3D->D3DGC->EnableFXAA = true;
// ++++++++++++++++++++     Вращаем свет     ++++++++++++++++++++++++++++++++++++

		XMVECTOR Dir;
		XMVECTOR rotaxis = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

		//	rotation = (XM_2PI / 360);
		//	XMMATRIX Rotation = XMMatrixRotationAxis( rotaxis, -0.01745329251944444444444444444444 );
		m_Graphics->m_D3D->GetLightDirection( 2, Dir );
//		float a = XMVectorGetX( Dir );
//		a += 0.3f;//		Dir = XMVectorSetX( Dir, a );
////		Dir = XMVector3Normalize(Dir);
//		m_Graphics->m_D3D->ChangeLightDirection( 2, Dir );
		//	XMVectorSetW(Dir, 1.0f);
		//	XMVECTOR a = XMVector3TransformCoord( Dir, XMMatrixRotationAxis( rotaxis, -0.0174532925 ) );
		m_Graphics->m_D3D->ChangeLightDirection( 2, XMVector3TransformCoord( Dir, XMMatrixRotationAxis( rotaxis, 0.0174532925 / 1.0f ) ) );

/*
// Изменяем положение света на сцене
		m_Graphics->m_D3D->GetLightPos(2, Dir);
		float a = XMVectorGetX( Dir );
		a +=0.5f;
		Dir = XMVectorSetX( Dir, a );
		m_Graphics->m_D3D->ChangeLightPos(2, Dir);
*/

		// ++++++++++++++++++++     Вращаем свет     ++++++++++++++++++++++++++++++++++++
	}
	if (m_Input->ActionKey[25].Pressed)
	{
//		m_Graphics->m_D3D->LightsOnlyOFF();
//		m_Graphics->m_D3D->D3DGC->EnableFXAA = false;
// ++++++++++++++++++++     Вращаем свет     ++++++++++++++++++++++++++++++++++++

		XMVECTOR Dir;
		XMVECTOR rotaxis = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

		//	rotation = (XM_2PI / 360);
		//	XMMATRIX Rotation = XMMatrixRotationAxis( rotaxis, -0.01745329251944444444444444444444 );
		m_Graphics->m_D3D->GetLightDirection( 2, Dir );
//		float a = XMVectorGetX( Dir );
//		a -=0.3f;
//		Dir = XMVectorSetX( Dir, a );
////		Dir = XMVector3Normalize( Dir );
//		m_Graphics->m_D3D->ChangeLightDirection( 2, Dir );
		//	XMVectorSetW(Dir, 1.0f);
		//	XMVECTOR a = XMVector3TransformCoord( Dir, XMMatrixRotationAxis( rotaxis, -0.0174532925 ) );
		m_Graphics->m_D3D->ChangeLightDirection( 2, XMVector3TransformCoord( Dir, XMMatrixRotationAxis( rotaxis, -0.0174532925 / 1.0f ) ) );

/*
// Изменяем положение света на сцене
		m_Graphics->m_D3D->GetLightPos( 2, Dir );
		float a = XMVectorGetX( Dir );
		a -= 0.5f;
		Dir = XMVectorSetX( Dir, a );
		m_Graphics->m_D3D->ChangeLightPos( 2, Dir );
*/
		// ++++++++++++++++++++     Вращаем свет     ++++++++++++++++++++++++++++++++++++
	}
	/*
	if(InputStruct.m_keyboardState[DIK_COMMA] & 0x80)
	{
	m_Graphics->m_Camera->moveLeftRight += 0.01f;
	}

	if(InputStruct.m_keyboardState[DIK_M] & 0x80)
	{
	m_Graphics->m_Camera->moveLeftRight -= 0.01f;
	}
	*/
}