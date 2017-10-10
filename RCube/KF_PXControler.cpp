#include "stdafx.h"
#include "KF_PXControler.h"


#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3GpuDEBUG_x64.lib")  
#pragma comment(lib, "PxCudaContextManagerDEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")    //PhysX extended library 
#pragma comment(lib, "PxCudaContextManager_x64.lib") //For PVD only 
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x64.lib")
#pragma comment(lib, "PxFoundationDEBUG_x64.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x64.lib")
#pragma comment(lib, "PxTaskDEBUG_x64.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib, "PhysX3_x64.lib ")
#pragma comment(lib, "PhysX3Gpu_x64.lib")  
#pragma comment(lib, "PxCudaContextManager_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3Cooking_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")    //PhysX extended library 
#pragma comment(lib, "PxCudaContextManager_x64.lib") //For PVD only 
#pragma comment(lib, "PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib, "PxFoundation_x64.lib")
#pragma comment(lib, "PxPvdSDK_x64.lib")
#pragma comment(lib, "PxTask_x64.lib")
#endif
#endif


KF_PXControler::KF_PXControler(){

	aHeightField = nullptr;
	g_ModelList = nullptr;
	DynamicDataArr = nullptr;
	DynamicDataArrCounter = 0;
	StaticNumObjects = 0;
	NumOfIndefinedCharacters = 0;

		gFoundation = nullptr;
		   gPhysics = nullptr;
gCudaContextManager = nullptr;
		gDispatcher = nullptr;
	  GPUDispatcher = nullptr;

#ifdef _DEBUG
		  transport = nullptr;
				pvd = nullptr;
		  pvdClient = nullptr;
#endif

}

KF_PXControler::~KF_PXControler() {
// +++++++++++++++++++++    Это ТЕСТ!!!    +++++++++++++++++++++++++++
// ++++++++++++++++   Для объединения кубов   ++++++++++++++++++++++++
	for ( UINT i = 0; i < 299; ++i )
	{
		PXSAFE_RELEASE( Joints[i] );
	}
// +++++++++++++++++++++    Это ТЕСТ!!!    +++++++++++++++++++++++++++

	if(DynamicDataArr != nullptr)
	delete[] DynamicDataArr;

	int c = 0;
	while (c < StaticRigitArray.size()) {

		PXSAFE_RELEASE(StaticRigitArray[c]);

		++c;
	}

	c = 0;
	while (c < DymanicRigitArray.size()) {

		PXSAFE_RELEASE(DymanicRigitArray[c]);

		++c;
	}

	g_ControllerManager->purgeControllers();

// !!! ПРИОРИТЕТ ВАЖЕН !!!
// пока не настроили до конца

	PXSAFE_RELEASE(g_ControllerManager);
	PXSAFE_RELEASE(aHeightField);
	PXSAFE_RELEASE(gScene);
	PXSAFE_RELEASE(gDispatcher);

#if PX_SUPPORT_GPU_PHYSX
	PXSAFE_RELEASE( gCudaContextManager );
#endif

	PXSAFE_RELEASE(theCooking);
	PXSAFE_RELEASE(gPhysics);
	PXSAFE_RELEASE(gFoundation);

//#ifdef _DEBUG	
//	PXSAFE_RELEASE( pvdClient );
	PXSAFE_RELEASE( pvd );
	PXSAFE_RELEASE( transport );
//#endif

}

KF_PXControler::FIZFileData::FIZFileData() {

	fullShapesCount = 0;
	DynamicFriction = 0.0f;
	Restitution = 0.0f;
	StaticFriction = 0.0f;
    mass = 0.0f;
	MassSpaceInertiaTensorX = 0.0f;
	MassSpaceInertiaTensorY = 0.0f;
	MassSpaceInertiaTensorZ = 0.0f;
	VelocityParam1 = 0.0f;
	VelocityParam2 = 0.0f;
	VelocityParam3 = 0.0f;
	AngularDampingParam = 0.0f;
	LinearDampingParam = 0.0f;

	Shapes->qx = 0.0f;
	Shapes->qy = 0.0f;
	Shapes->qz = 0.0f;
	Shapes->qw = 1.0f;

	Shapes->px = 0.0f;
	Shapes->py= 0.0f;
	Shapes->pz = 0.0f;

	lstrcpyW(ClimbingMode, L"EASY");

	UpVectorX = 0.0f;
	UpVectorY = 1.0f;
	UpVectorZ = 0.0f;

	stepOffset = 0.5f;

}

PxDefaultAllocator gDefaultAllocatorCallback;

PxErrorCallback& getSampleErrorCallback(  ) //PxErrorCode::Enum code, const char* message, const char* file,int line
{
	static PxDefaultErrorCallback gDefaultErrorCallback;

//	const size_t cSize = strlen( message ) + 1;
//	wchar_t* wc = new wchar_t[cSize];
//	size_t Ret;
//	mbstowcs_s( &Ret, wc, cSize, message, cSize );


//	MessageBox(0, wc, L"", 0 );

//	delete [] wc;
	return gDefaultErrorCallback;
}

void KF_PXControler::Init(HWND * g_hwnd , D3DGlobalContext *D3DGC, KFModelList * ActiveModelList)
{

//	PxErrorCode::Enum code = PxErrorCode::Enum::eABORT;
//	const char* message = "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
//	const char* file= "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
//	int line = 0;

	g_Hwnd = *g_hwnd;
	g_ModelList = ActiveModelList;

// !!! allocator должен быть объявлен именно так. Иначе неправильно инициализируется Foundation и не работает PhysX
	PxDefaultAllocator *allocator = &gDefaultAllocatorCallback;
	gFoundation = PxCreateFoundation( PX_FOUNDATION_VERSION, *allocator , getSampleErrorCallback() ); //code, message,  file, line
	if ( !gFoundation )
		MessageBox(0, L"PxCreateFoundation failed!", L"Error",0);
// ----------------------------------------------------------------------------------------------------------------

//	gProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(gFoundation);

#if PX_SUPPORT_GPU_PHYSX
	PxCudaContextManagerDesc cudaContextManagerDesc;
	ZeroMemory(&cudaContextManagerDesc, sizeof ( PxCudaContextManagerDesc ));
	cudaContextManagerDesc.graphicsDevice = D3DGC->D11_device;
	cudaContextManagerDesc.interopMode = PxCudaInteropMode::D3D11_INTEROP;
	cudaContextManagerDesc.ctx = NULL;
	cudaContextManagerDesc.appGUID = "RCUBE";

//	gCudaContextManager = PxCreateCudaContextManager( *gFoundation, cudaContextManagerDesc ); //, gProfileZoneManager

	if ( gCudaContextManager )
	{
		// Инфо об адаптере для инициализации GPU для физики
		char *Str = new char [128];
		strcpy_s( Str, 127, gCudaContextManager->getDeviceName() );
		int a = gCudaContextManager->getDriverVersion();
		size_t b = gCudaContextManager->getDeviceTotalMemBytes();
		int MultiprocessorCount = gCudaContextManager->getMultiprocessorCount();
		int _MAXThredsBlock = gCudaContextManager->getMaxThreadsPerBlock();
		int SharedMemPerBlock = gCudaContextManager->getSharedMemPerBlock();
		int PhyXGPU = gCudaContextManager->usingDedicatedGPU();
		UINT Speed = gCudaContextManager->getClockRate();
		int d = gCudaContextManager->getInteropMode();
		delete [] Str;

		if ( !gCudaContextManager->contextIsValid() )
		{
			PXSAFE_RELEASE( gCudaContextManager );
			MessageBox( 0, L"PxCreateCudaContextManager failed!", L"Error", 0 );
		}
	}
	else
	{
		printf( "create cuda context manager failed\n" );
	}
#endif
	
#ifdef _DEBUG
// file:///C:/PhysX/Documentation/PhysXGuide/Manual/VisualDebugger.html?highlight=pxvisualdebuggerconnection
	pvd = PxCreatePvd( *gFoundation );
	transport = PxDefaultPvdSocketTransportCreate( PVD_HOST, 5425, 10 );
	pvd->connect( *transport, PxPvdInstrumentationFlag::eALL );
	//  PxCreateBasePhysics
	gPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, pvd );
	// http://qubanshi.cc/questions/3383218/physx-simulate-never-ends-if-gpu-used
	// Включаем работу расширений, чтобы не глючил PVD
	PxInitExtensions( *gPhysics, pvd );
#else
	gPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true );
	PxInitExtensions( *gPhysics, NULL );
#endif


	PxCookingParams cp = PxCookingParams(gPhysics->getTolerancesScale());
	cp.buildGPUData = true;
	theCooking = PxCreateCooking(PX_PHYSICS_VERSION, gPhysics->getFoundation(), PxCookingParams(cp));
	// Включаем работу HeightFields для Terrain
		PxRegisterUnifiedHeightFields( *gPhysics );
	// Включаем работу частиц
		PxRegisterParticles( *gPhysics );

	PxSceneDesc desc(gPhysics->getTolerancesScale());

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	
	desc.cpuDispatcher = gDispatcher;
	desc.filterShader = PxDefaultSimulationFilterShader;
	desc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	
//	desc.gpuDynamicsConfig.constraintBufferCapacity = 64;
//	desc.gpuDynamicsConfig.contactBufferCapacity = 64;
//	desc.gpuDynamicsConfig.tempBufferCapacity = 64;

	if ( gCudaContextManager )	
		GPUDispatcher = gCudaContextManager->getGpuDispatcher();
	// Включаем расчёт RigidBoby на GPU	
	// file:///C:/PhysX-3.4-master/PhysX_3.4/Documentation/PhysXGuide/Manual/GPURigidBodies.html?highlight=pxcreatecudacontextmanager
	if ( GPUDispatcher )		
	{
		desc.gpuDispatcher = GPUDispatcher;
		// file:///C:/PhysX-3.4-master/PhysX_3.4/Documentation/PhysXGuide/Manual/BestPractices.html?highlight=eenable_gpu_dynamics
		desc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
		desc.flags |= PxSceneFlag::eENABLE_PCM;
		desc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
		desc.broadPhaseType = PxBroadPhaseType::eGPU;//eSAP;//eMBP;//
		desc.gpuMaxNumPartitions = 8;
/*		desc.gpuDynamicsConfig.constraintBufferCapacity = (32 * 1024 * 1024);
		desc.gpuDynamicsConfig.contactBufferCapacity = (24 * 1024 * 1024);
		desc.gpuDynamicsConfig.tempBufferCapacity = (16 * 1024 * 1024);
		desc.gpuDynamicsConfig.contactStreamSize = (6 * 1024 * 1024);
		desc.gpuDynamicsConfig.patchStreamSize = (5 * 1024 * 1024);
		desc.gpuDynamicsConfig.forceStreamCapacity = (1 * 1024 * 1024);
		desc.gpuDynamicsConfig.heapCapacity = (64 * 1024 * 1024);
		desc.gpuDynamicsConfig.foundLostPairsCapacity = (256 * 1024);
*/
	}

	gScene = gPhysics->createScene(desc);

#ifdef _DEBUG
//file:///C:/PhysX/Documentation/PhysXGuide/Manual/VisualDebugger.html?highlight=pxvisualdebuggerconnection
	pvdClient = gScene->getScenePvdClient();
	if ( pvdClient )
	{
		pvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
		pvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
		pvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
	}//#endif
#endif

	DynamicDataArr = new ObjectData[KF_MAX_MODEL_ARR_SIZE];

	PxDefaultMemoryOutputStream Stream;
	outStream = &Stream;

	g_ControllerManager = PxCreateControllerManager(*gScene);

}

void KF_PXControler::CreateHeildHield(int HFRows, int HFCollums , float VertixesInderect , Terrain::HeightMapType* terrainData ,
	std::wstring filename ) {

	FIZFileData FizData;
	ReadFIZFile(&FizData, filename);

	PxRigidStatic* PxTerrain;
	PxTerrain = gPhysics->createRigidStatic(PxTransform(PxIdentity));

	PxMaterial* gMaterial = gPhysics->createMaterial(FizData.StaticFriction, FizData.DynamicFriction, FizData.Restitution);
	PxReal minHeight = PX_MAX_F32;
	PxReal maxHeight = -PX_MAX_F32;
	PxReal deltaHeight = 1.0f;

	PxReal quantization = (PxReal)0x7fff;
	PxReal heightScale = PxMax(deltaHeight / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE);


	PxU32 numRows = HFRows, numCols = HFCollums;

	PxU32* hfSamples = new PxU32[HFRows * HFCollums];

	PxU32 index = 0;
	PxShape* shape; // PxSphereGeometry(3.0f)


	/*

	заполнение моего терайна и фиксворо отличается!

	мой                          физиксовый

	13 14 15 16                 4  8  12  16
	9  10 11 12                 3  7  11  15
	5  6  7  8                  2  6  10  14
	1  2  3  4                  1  5  9   13

	(пример терайн 4х4)

	*/

	for (PxU32 col = 0; col < numCols; col++)
	{
		for (PxU32 row = 0; row < numRows; row++)
		{

			PxI16 height;
			height = PxI16(terrainData[(col*numRows) + row].y);

			PxHeightFieldSample& smp = (PxHeightFieldSample&)(hfSamples[(row*numCols) + col]);
			smp.height = (PxI16)height; //height;
			smp.materialIndex0 = 0;
			smp.materialIndex1 = 0;

			//			if (userFlipEdge)
			//				smp.setTessFlag();

		}
	}

	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = numCols;
	hfDesc.nbRows = numRows;
	hfDesc.samples.data = hfSamples;
	hfDesc.thickness = -10.0f; // user-specified heightfield thickness
	hfDesc.flags = PxHeightFieldFlags();
	hfDesc.samples.stride = sizeof(PxU32);
	/*
	// http://www.ogre3d.org/forums/viewtopic.php?f=2&t=75965
	*/
	// приготавливаем форму нашей земли
	theCooking->cookHeightField(hfDesc, *outStream);
	aHeightField = theCooking->createHeightField(hfDesc, gPhysics->getPhysicsInsertionCallback());

	// Положение высот на земле можно удалить после приготовления
	delete[] hfSamples;
	// Создаём объект согласно форме нашей земли в PhysX
	PxHeightFieldGeometry hfGeom(aHeightField, PxMeshGeometryFlags(), 1.0f, VertixesInderect, VertixesInderect);
	shape = gPhysics->createShape(hfGeom, *gMaterial);
	// Прикрепляем форму к статическому объекту Земля
	PxTerrain->attachShape(*shape);

	// Изменяем позицию нашей земли в Физическом мире
	PxTransform localPose;
	localPose.p = PxVec3(-(((PxReal)HFRows / 2) * VertixesInderect) + 1, 0.0f, -((HFCollums / 2) * VertixesInderect) + 1); // make it so that the center of th eheightfield is at world (0,minHeight,0)
	localPose.q = PxQuat(PxIdentity);
	PxTerrain->setGlobalPose(localPose);
	// file:///C:/PhysX/Documentation/PhysXGuide/Manual/API.html?highlight=getconcretetype
	PxActor* Actor = PxTerrain->is<PxRigidStatic>();
	gScene->addActor(*Actor );
	StaticRigitArray.push_back(PxTerrain);


	PXSAFE_RELEASE(shape);
	
}

void KF_PXControler::ReadFIZFile(FIZFileData * Data , std::wstring filename ) {

	//Data->GeomData = L" ";
	wchar_t checkChar;
	int Index = 0;;
	bool CheckBool = true;

	std::wifstream fileIn(filename.c_str());
	
	char String[50];

	if (fileIn)
	{
		while (CheckBool)
		{
			checkChar = fileIn.get();

			Index = 0;
			while (checkChar != ' ' && checkChar != ';' && checkChar != '\n')
			{

				String[Index] = (char)checkChar;
				checkChar = fileIn.get();
				++Index;

			}

			String[Index] = '\0';

			if (strcmp(String, "End") == 0) {

				CheckBool = false;

			}

			if (strcmp(String, "Material") == 0) {

				fileIn >> Data->StaticFriction >> Data->DynamicFriction >> Data->Restitution;

			}

			if (strcmp(String, "Mass") == 0) {


				fileIn >> Data->mass;

			}

			if (strcmp(String, "MassSpaceInertia") == 0) {

				fileIn >> Data->MassSpaceInertiaTensorX >> Data->MassSpaceInertiaTensorY >> Data->MassSpaceInertiaTensorZ;

			}

			if (strcmp(String, "Shape") == 0) {

				int Cout = Data->fullShapesCount;
				++Data->fullShapesCount;
				fileIn >> Data->Shapes[Cout].GeomData;

				// Box
				if (Data->Shapes[Cout].GeomData[0] == 'B' && Data->Shapes[Cout].GeomData[1] == 'o' && Data->Shapes[Cout].GeomData[2] == 'x')

					fileIn >> Data->Shapes[Cout].GeomParam1 >> Data->Shapes[Cout].GeomParam2 >> Data->Shapes[Cout].GeomParam3
					>> Data->Shapes[Cout].px >> Data->Shapes[Cout].py >> Data->Shapes[Cout].pz >> Data->Shapes[Cout].qx >> Data->Shapes[Cout].qy
					>> Data->Shapes[Cout].qz >> Data->Shapes[Cout].qw;

				//Sphere
				if (Data->Shapes[Cout].GeomData[0] == 'S' && Data->Shapes[Cout].GeomData[1] == 'p' && Data->Shapes[Cout].GeomData[2] == 'h'
					&& Data->Shapes[Cout].GeomData[3] == 'e' && Data->Shapes[Cout].GeomData[4] == 'r' && Data->Shapes[Cout].GeomData[5] == 'e')

					fileIn >> Data->Shapes[Cout].GeomParam1 >> Data->Shapes[Cout].px >> Data->Shapes[Cout].py >> Data->Shapes[Cout].pz
					>> Data->Shapes[Cout].qx >> Data->Shapes[Cout].qy >> Data->Shapes[Cout].qz >> Data->Shapes[Cout].qw;

				//Capsule
				if (Data->Shapes[Cout].GeomData[0] == 'C' && Data->Shapes[Cout].GeomData[1] == 'a' && Data->Shapes[Cout].GeomData[2] == 'p'
					&& Data->Shapes[Cout].GeomData[3] == 's' && Data->Shapes[Cout].GeomData[4] == 'u' && Data->Shapes[Cout].GeomData[5] == 'l'
					&& Data->Shapes[Cout].GeomData[6] == 'e')

					fileIn >> Data->Shapes[Cout].GeomParam1 >> Data->Shapes[Cout].GeomParam2 >> Data->Shapes[Cout].px
					>> Data->Shapes[Cout].py >> Data->Shapes[Cout].pz >> Data->Shapes[Cout].qx >> Data->Shapes[Cout].qy
					>> Data->Shapes[Cout].qz >> Data->Shapes[Cout].qw;

			}


			// это для свойств формы персонажет для точного пониямания сфера это или куб
			// этот код в отличии от анокогичного кода для обычных персонажей должен выполняться только оди раз ведь форма для персонажа может быть только оодна
			if (strcmp(String, "CharacterShapeMode") == 0) {

				fileIn >> Data->Shapes[0].GeomData;
				// Box
				if (Data->Shapes[0].GeomData[0] == 'B' && Data->Shapes[0].GeomData[1] == 'o' && Data->Shapes[0].GeomData[2] == 'x')
					fileIn >> Data->Shapes[0].GeomParam1 >> Data->Shapes[0].GeomParam2 >> Data->Shapes[0].GeomParam3;


				//Capsule
				if (Data->Shapes[0].GeomData[0] == 'C' && Data->Shapes[0].GeomData[1] == 'a' && Data->Shapes[0].GeomData[2] == 'p'
					&& Data->Shapes[0].GeomData[3] == 's' && Data->Shapes[0].GeomData[4] == 'u' && Data->Shapes[0].GeomData[5] == 'l'
					&& Data->Shapes[0].GeomData[6] == 'e')

					fileIn >> Data->Shapes[0].GeomParam1 >> Data->Shapes[0].GeomParam2;

			}


			if (strcmp(String, "ClimbingMode") == 0) {

				fileIn >> Data->ClimbingMode;

			}


			if (strcmp(String, "AngularVelocity") == 0) {

				fileIn >> Data->VelocityParam1 >> Data->VelocityParam2 >> Data->VelocityParam3;

			}

			if (strcmp(String, "AngularDamping") == 0) {

				fileIn >> Data->AngularDampingParam;

			}

			if (strcmp(String, "LinearDamping") == 0) {

				fileIn >> Data->LinearDampingParam;

			}


			if (strcmp(String, "UpVector") == 0) {

				fileIn >> Data->UpVectorX >> Data->UpVectorY >> Data->UpVectorZ;

			}

			if (strcmp(String, "StepOffset") == 0) {

				fileIn >> Data->stepOffset;

			}

		}
	}
	else {

		MessageBox(g_Hwnd, L"не могу найти .fiz файл.KF_PXControler.cpp", L"Error", MB_OK);

	}

	fileIn.close();

}

int KF_PXControler::CreateObject(std::wstring filename, bool IsDynamic, ObjectData * Data) {

	int Result;
	FIZFileData FizData;
	PxShape** shapesArr;
	ReadFIZFile(&FizData, filename);
	int spIndex;
	// создаем материал
	WorkgMaterial = gPhysics->createMaterial(FizData.StaticFriction, FizData.DynamicFriction, FizData.Restitution);
	PositionType PosesData = g_ModelList->RCubeModelList[Data->ModelIndex]->ThisObjDesc.Positions[Data->InstanceIndex];
	// Создаём Кватернион для PhysX		
	WorkQuat = XMQuaternionRotationRollPitchYaw(PosesData.rotation.Fl3.x, PosesData.rotation.Fl3.y, PosesData.rotation.Fl3.z);
	WorkPxQ = { XMVectorGetX(WorkQuat), XMVectorGetY(WorkQuat), XMVectorGetZ(WorkQuat), XMVectorGetW(WorkQuat) };

	PxTransform TMlocalPose;

	// указываем позицию
	TMlocalPose.p = PxVec3(PosesData.position.Fl3.x, PosesData.position.Fl3.y, PosesData.position.Fl3.z); // make it so that the center of th eheightfield is at world (0,minHeight,0)
	TMlocalPose.q = PxQuat(WorkPxQ);


	shapesArr = new PxShape*[FizData.fullShapesCount];

	//******************** создаём геометрию обьекта********************
	int c = 0;
	while (c < FizData.fullShapesCount){

		shapesArr[c] = CreateShape(Data->ModelIndex, FizData.Shapes[c]);
		
	++c;
   }
	//******************** создаём геометрию обьекта********************

	// Инициализируем объект в PhysX

	if (!IsDynamic) {

		StaticNumObjects++;
		StaticRigitArray.push_back(gPhysics->createRigidStatic(PxTransform(PxIdentity)));

		Result = StaticNumObjects;
		spIndex = (int)StaticRigitArray.size() - 1;
		StaticRigitArray[spIndex]->setGlobalPose(TMlocalPose);
		

		c = 0;
		while (c < FizData.fullShapesCount) {

			StaticRigitArray[spIndex]->attachShape(*shapesArr[c]);

			++c;

		}
		// file:///C:/PhysX/Documentation/PhysXGuide/Manual/API.html?highlight=getconcretetype
		PxActor* Actor = StaticRigitArray[spIndex]->is<PxRigidStatic>();
		gScene->addActor( *Actor );
	} 
	else{


		DynamicDataArr[DynamicDataArrCounter] = *Data;
			++DynamicDataArrCounter;
			Result = DynamicDataArrCounter;
		DymanicRigitArray.push_back(gPhysics->createRigidDynamic(PxTransform(PxIdentity)));

		spIndex = (int)DymanicRigitArray.size() - 1;

		// указываем позицию
		DymanicRigitArray[spIndex]->setGlobalPose(TMlocalPose);

		int c = 0;

		while (c < FizData.fullShapesCount) {

			DymanicRigitArray[spIndex]->attachShape(*shapesArr[c]);
			++c;

		}

		DymanicRigitArray[spIndex]->setMass(FizData.mass);
		DymanicRigitArray[spIndex]->setMassSpaceInertiaTensor(PxVec3(FizData.MassSpaceInertiaTensorX , FizData.MassSpaceInertiaTensorY , FizData.MassSpaceInertiaTensorZ));
		PxRigidBodyExt::updateMassAndInertia(*DymanicRigitArray[spIndex], 0.0001f, NULL);
		DymanicRigitArray[spIndex]->setAngularVelocity(PxVec3(FizData.VelocityParam1, FizData.VelocityParam2, FizData.VelocityParam3));
		DymanicRigitArray[spIndex]->setAngularDamping(FizData.AngularDampingParam);
		DymanicRigitArray[spIndex]->setLinearDamping(FizData.LinearDampingParam);
		// file:///C:/PhysX/Documentation/PhysXGuide/Manual/API.html?highlight=getconcretetype
		PxActor* Actor = DymanicRigitArray[spIndex]->is<PxRigidDynamic>();
		gScene->addActor( *Actor );
	}

	c = 0;

	while (c < FizData.fullShapesCount) {
		PXSAFE_RELEASE(shapesArr[c]);

		++c;
	}

	delete[] shapesArr;
	return Result - 1;
}

void KF_PXControler::CreateInstancingObjects(std::wstring filename, bool IsDynamic , int ObjInex) {

	int InstNum =  g_ModelList->RCubeModelList[ObjInex]->ThisObjDesc.InstanceCount, c = 0;

	ObjectData Temp;

	Temp.ModelIndex = ObjInex;

	while (c < InstNum) {

		Temp.InstanceIndex = c;
		CreateObject(filename , IsDynamic , &Temp);

		++c;
	}

}

void KF_PXControler::Frame() {

	if (g_ModelList != nullptr) {

		PxTransform KinematicData;
		int c = 0;

		ObjectData * Temp = DynamicDataArr;
		PositionType ** Temp2 = g_ModelList->ObjPoses;

		while (c < DynamicDataArrCounter) {

			PositionType  * Temp3 = &Temp2[Temp->ModelIndex][Temp->InstanceIndex];

			KinematicData = DymanicRigitArray[c]->getGlobalPose();

			QuatXMfloat3.Position = &KinematicData.p;
			Temp3->position.Fl3 = *QuatXMfloat3.Temp;
			QuatXMfloat4.Rotation = &KinematicData.q;

			float W = QuatXMfloat4.Temp->w;
			*QuatXMfloat4.Vec = -*QuatXMfloat4.Vec;// * -1.0f;
			QuatXMfloat4.Temp->w = W;

//			QuatXMfloat4.Temp->y = -QuatXMfloat4.Temp->y;
//			QuatXMfloat4.Temp->z = -QuatXMfloat4.Temp->z;
//			QuatXMfloat4.Temp->x = -QuatXMfloat4.Temp->x;
			
			Temp3->rotation.Fl4 = *QuatXMfloat4.Temp;
			 
			g_ModelList->RCubeModelList[Temp->ModelIndex]->UpdateInstancingPos();

			++Temp;
			++c;
		}

		c = 0;

		PxExtendedVec3 position;

		while (c < NumOfIndefinedCharacters) {

			PositionType  * Temp4 = &Temp2[CharactersDataArr[c].ModelIndex][CharactersDataArr[c].InstanceIndex];

			position = ControllersArray[c]->getPosition ();
			Temp4->position.Fl3.x = (float)position.x;
			Temp4->position.Fl3.y = (float)position.y;
			Temp4->position.Fl3.z = (float)position.z;

			g_ModelList->RCubeModelList[CharactersDataArr[c].ModelIndex]->UpdateInstancingPos();
			++c;
		}

	}
}

PxShape* KF_PXControler::CreateShape(int ModelIndex, ShapeData  Shape) {

	PxGeometry * WorkGeometry = nullptr;
	KFModel * model = g_ModelList->RCubeModelList[ModelIndex];
	// Box
	
	if (Shape.GeomData[0] == 'B' && Shape.GeomData[1] == 'o' && Shape.GeomData[2] == 'x') {
		WorkGeometry = &PxBoxGeometry(Shape.GeomParam1, Shape.GeomParam2, Shape.GeomParam3);
		shape = gPhysics->createShape(*WorkGeometry, *WorkgMaterial);
	}
	//Sphere
	if (Shape.GeomData[0] == 'S' && Shape.GeomData[1] == 'p' && Shape.GeomData[2] == 'h' && Shape.GeomData[3] == 'e' && Shape.GeomData[4] == 'r' 
		&& Shape.GeomData[5] == 'e') {
		WorkGeometry = &PxSphereGeometry(Shape.GeomParam1);
		shape = gPhysics->createShape(*WorkGeometry, *WorkgMaterial);
	}
	//Capsule
	if (Shape.GeomData[0] == 'C' && Shape.GeomData[1] == 'a' && Shape.GeomData[2] == 'p' && Shape.GeomData[3] == 's' && Shape.GeomData[4] == 'u' 
		&& Shape.GeomData[5] == 'l' && Shape.GeomData[6] == 'e') {
		WorkGeometry = &PxCapsuleGeometry(Shape.GeomParam1, Shape.GeomParam2);
		shape = gPhysics->createShape(*WorkGeometry, *WorkgMaterial);
	}
	//TriangleMesh
	if (Shape.GeomData[0] == 'T' && Shape.GeomData[1] == 'r' && Shape.GeomData[2] == 'i' && Shape.GeomData[3] == 'a' && Shape.GeomData[4] == 'n' 
		&& Shape.GeomData[5] == 'g'&& Shape.GeomData[6] == 'l' && Shape.GeomData[7] == 'e' && Shape.GeomData[8] == 'M' && Shape.GeomData[9] == 'e'
		&& Shape.GeomData[10] == 's' && Shape.GeomData[11] == 'h') {

		int nbVerts, triCount, indexesCout;

		nbVerts = model->ThisObjDesc.ObjectVertexesCout;
		triCount = model->ThisObjDesc.ObjectVertexesCout / 3;
		indexesCout = model->ThisObjDesc.ObjectVertexesCout;


		PxVec3 * verts = new PxVec3[nbVerts];
		PxU32 * indices32 = new PxU32[indexesCout];
		int c = 0, counter = 0, fullCounter = 0, inv_c;

		while (counter < model->ThisObjDesc.MeshesCount ) {
			c = 0;
			inv_c = model->ThisObjDesc.Meshes[counter].VertexBufferSize;
			while (c < model->ThisObjDesc.Meshes[counter].VertexBufferSize) {

				verts[fullCounter].x = model->ThisObjDesc.Meshes[counter].VertArr[c].Position.x;
				verts[fullCounter].y = model->ThisObjDesc.Meshes[counter].VertArr[c].Position.y;
				verts[fullCounter].z = model->ThisObjDesc.Meshes[counter].VertArr[c].Position.z;

				indices32[fullCounter] = inv_c;

				--inv_c;
				++fullCounter;
				++c;
			}

			counter++;
		}



		PxTriangleMeshDesc meshDesc;

		meshDesc.setToDefault();

		meshDesc.points.count = nbVerts;
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = verts;

		meshDesc.triangles.count = triCount;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = indices32;

		meshDesc.flags = PxMeshFlags();

		bool Mesh = meshDesc.isValid();

#ifdef _DEBUG
		// mesh should be validated before cooked without the mesh cleaning
		bool bres = theCooking->validateTriangleMesh(meshDesc);
		PX_ASSERT(bres);
#endif

		MeshesArray.push_back(theCooking->createTriangleMesh(meshDesc,
			gPhysics->getPhysicsInsertionCallback()));


		PxMeshScale TMscale;
//		TMscale.createIdentity();

		PxTriangleMeshGeometry tmGeom;

		tmGeom.meshFlags = PxMeshGeometryFlags();
		tmGeom.scale = TMscale;
		tmGeom.triangleMesh = MeshesArray[MeshesArray.size() - 1];

		shape = gPhysics->createShape(tmGeom, *WorkgMaterial);
		RCUBE_ARR_DELETE(verts);
		RCUBE_ARR_DELETE(indices32);
	}
	

	PxTransform myTMlocalPose;
	WorkPxQ = { Shape.qx, Shape.qy, Shape.qz, Shape.qw };

	// указываем позицию
	myTMlocalPose.p = PxVec3(Shape.px, Shape.py, Shape.pz); // make it so that the center of th eheightfield is at world (0,minHeight,0)
	myTMlocalPose.q = PxQuat(WorkPxQ);

	shape->setLocalPose(myTMlocalPose);

	return shape;
}

int KF_PXControler::CreateCharacter(std::wstring filename , ObjectData * Data) {

	FIZFileData FizData;
	ReadFIZFile(&FizData, filename);

	PxMaterial* gMaterial = gPhysics->createMaterial(FizData.StaticFriction, FizData.DynamicFriction, FizData.Restitution);
	PositionType PosesData = g_ModelList->RCubeModelList[Data->ModelIndex]->ThisObjDesc.Positions[Data->InstanceIndex];

	// Box
	if (FizData.Shapes[0].GeomData[0] == 'B' && FizData.Shapes[0].GeomData[1] == 'o' && FizData.Shapes[0].GeomData[2] == 'x') {

		PxBoxControllerDesc boxDesc;

		boxDesc.reportCallback = &Evhp;
		boxDesc.halfSideExtent = FizData.Shapes[0].GeomParam1;
		boxDesc.halfHeight = FizData.Shapes[0].GeomParam2;
        boxDesc.halfForwardExtent = FizData.Shapes[0].GeomParam3;
		boxDesc.material = gMaterial;
        boxDesc.position = PxExtendedVec3(PosesData.position.Fl3.x, PosesData.position.Fl3.y, PosesData.position.Fl3.z);
		boxDesc.upDirection = PxVec3(FizData.UpVectorX, FizData.UpVectorY, FizData.UpVectorZ);
		boxDesc.stepOffset = FizData.stepOffset;


		ControllersArray.push_back(g_ControllerManager->createController(boxDesc));

	}


	// Capsule
	if (FizData.Shapes[0].GeomData[0] == 'C' && FizData.Shapes[0].GeomData[1] == 'a' && FizData.Shapes[0].GeomData[2] == 'p'
		&& FizData.Shapes[0].GeomData[3] == 's' && FizData.Shapes[0].GeomData[4] == 'u' && FizData.Shapes[0].GeomData[5] == 'l'
		&& FizData.Shapes[0].GeomData[6] == 'e') {

		PxCapsuleControllerDesc  CapsuleDesc;


		CapsuleDesc.reportCallback = &Evhp;
		CapsuleDesc.radius = FizData.Shapes[0].GeomParam1;
		CapsuleDesc.height = FizData.Shapes[0].GeomParam2;
		CapsuleDesc.material = gMaterial;
		CapsuleDesc.position = PxExtendedVec3(PosesData.position.Fl3.x, PosesData.position.Fl3.y, PosesData.position.Fl3.z);
		CapsuleDesc.upDirection = PxVec3(FizData.UpVectorX, FizData.UpVectorY, FizData.UpVectorZ);
		CapsuleDesc.stepOffset = FizData.stepOffset;

		if (FizData.ClimbingMode[0] == 'E' && FizData.ClimbingMode[1] == 'A' && FizData.ClimbingMode[2] == 'S' && FizData.ClimbingMode[3] == 'Y')
			CapsuleDesc.climbingMode = PxCapsuleClimbingMode::eEASY;

		if (FizData.ClimbingMode[0] == 'C' && FizData.ClimbingMode[1] == 'O' && FizData.ClimbingMode[2] == 'N' && FizData.ClimbingMode[3] == 'S' && 
			FizData.ClimbingMode[4] == 'T' && FizData.ClimbingMode[5] == 'R' && FizData.ClimbingMode[6] == 'A' && FizData.ClimbingMode[7] == 'I' && 
			FizData.ClimbingMode[8] == 'N' && FizData.ClimbingMode[9] == 'E' && FizData.ClimbingMode[10] == 'D')
			CapsuleDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;

		ControllersArray.push_back( g_ControllerManager->createController(CapsuleDesc));

	}

	//g_ControllerManager->createController();

	CharactersDataArr.push_back(*Data);
	++NumOfIndefinedCharacters;
	return NumOfIndefinedCharacters - 1;
}


void EventHelper::onShapeHit(const PxControllerShapeHit& hit) {

// file:///C:/PhysX/Documentation/PhysXGuide/Manual/API.html?highlight=getconcretetype
	PxRigidDynamic * dynam = NULL;
//	PxType dynam;
	dynam = hit.actor->is<PxRigidDynamic>();//actor->getConcreteType();//actor->isRigidDynamic();

	if (dynam == NULL) {
		return;
	}
	
	dynam->addForce(PxVec3(hit.dir.x / 5.0f, 0.0f , hit.dir.z / 5.0f));
	PxReal tx = (PxReal)((rand() % 1000) / 100) , ty = (PxReal)((rand() % 1000) / 100) , tz = (PxReal)((rand() % 1000) / 100);
	dynam->addTorque(PxVec3(tx , ty , tz));

}


int KF_PXControler::JoinAllCubes( )
{
	PxU32 nbActors = gScene->getNbActors( physx::PxActorTypeFlag::eRIGID_DYNAMIC ); 
	PxActor** actors = new PxActor*[nbActors];
	gScene->getActors( physx::PxActorTypeFlag::eRIGID_DYNAMIC, actors, nbActors );

	gScene->setVisualizationParameter( PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f );
	gScene->setVisualizationParameter( PxVisualizationParameter::eJOINT_LIMITS, 1.0f );

	UINT c = 0;

	while ( c < nbActors )
	{
		actors[c]->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, true );

		++c;
	}

	c = 0;

	while ( c < nbActors-1 )
	{

		// Создаём точку с ЛОКАЛЬНЫМИ координатами ВНУТРИ объекта и кватернионом поворота.
		// Кватернион копируем из поворота объекта ( ПРОСТО ДЛЯ ПРИМЕРА )
		PxTransform JointPos = PxTransform( -0.2f, -0.2f, -0.2f, PxQuat( PxIdentity ) );

		// Создаём точку у 2-го объекта с ЛОКАЛЬНЫМИ координатами ВНУТРИ объекта и кватернионом поворота.
		// Кватернион копируем из поворота объекта ( ПРОСТО ДЛЯ ПРИМЕРА )
		PxTransform JointPos2 = PxTransform( 0.2f, 0.2f, 0.2f, PxQuat( PxIdentity ) );

		// Создаём ЦЕПОЧНОЕ соединение наших кубов
		Joints[c] = PxDistanceJointCreate( *gPhysics, actors[c]->is<PxRigidDynamic>(), JointPos, actors[c + 1]->is<PxRigidDynamic>(), JointPos2 );
		Joints[c]->setConstraintFlag( PxConstraintFlag::eVISUALIZATION, true );
		// Устанавливаем максимальныю дистанцию между соединёнными точками в кубах
		Joints[c]->setMaxDistance( 1.0f );
		Joints[c]->setDistanceJointFlag( PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true );

		++c;
	}

		actors[112]->setActorFlag( PxActorFlag::eDISABLE_GRAVITY, false );
		actors[112]->is<PxRigidDynamic>()->setMass( 0.5f );
/*
			actors[0]->is<PxRigidDynamic>()->setAngularDamping(0.1f);
			actors[0]->is<PxRigidDynamic>()->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
			actors[0]->is<PxRigidDynamic>()->setMass( 50.0f );
			actors[0]->is<PxRigidDynamic>()->addForce( PxVec3( -0.1f, 0.0f, 0.0f ) );
*/
	delete [] actors;

	return nbActors;
}
