#pragma once
#include "stdafx.h"

#ifndef PHYSXCONTROLER_H_
#define PHYSXCONTROLER_H_

#include <PxPhysicsAPI.h>
//#include <PxDistanceJoint.h>
#include "characterkinematic/PxControllerManager.h"
//#include "gpu/PxParticleGpu.h"
//#include "pxtask/PxGpuDispatcher.h"
//#include "pxtask/PxGpuTask.h"
#include <vector>
#include "D3DGlobalContext.h"
#include "Buffers_def.h"
#include <string.h>
//#include "ResourceManager.h"
#include "Terrain.h"
#include "3DModel.h"
#include <fstream>

#define EASY_CAPSULE_CLIMBING_MODE 1
#define CONSTRAINED_CAPSULE_CLIMBING_MODE 2

using namespace physx;

class EventHelper : public PxUserControllerHitReport
{

	// Implements PxUserControllerHitReport
	virtual void							onShapeHit(const PxControllerShapeHit& hit);
	virtual void							onControllerHit(const PxControllersHit& hit) {}
	virtual void							onObstacleHit(const PxControllerObstacleHit& hit) {}

	bool lastInteraction;
};


class PhysXControler {
public:

	// +++++++++++++++++++++    Это ТЕСТ!!!    +++++++++++++++++++++++++++
	// ++++++++++++++++   Для объединения кубов   ++++++++++++++++++++++++
	PxDistanceJoint* Joints[MAX_OBJ-1];
	// +++++++++++++++++++++    Это ТЕСТ!!!    +++++++++++++++++++++++++++

	// +++++++++++++++++++++++   Для быстрого копирования данных из PhysX   +++++++++++++++++++++++++
	union _QuatXMfloat4
	{
		XMFLOAT4* Temp;
		PxQuat* Rotation;
		XMVECTOR* Vec;
	} QuatXMfloat4;

	union _QuatXMfloat3
	{
		XMFLOAT3* Temp;
		PxVec3* Position;
	} QuatXMfloat3;
	// -----------------------   Для быстрого копирования данных из PhysX   --------------------------

	// структура назначена для четкого понимания к какомуобьекту идет назначение
	struct ObjectData
	{

		int ModelIndex;// обозначает номер модели
		int InstanceIndex;// обозначает номер инстэнса

	};

	//так же определяет вид геометрии персонажа если загружен токовой может исспользовать  Box(коробка) , Capsule(капсула)
	struct ShapeData {

		//******************************параметры формы обьекта*********************
		PxReal GeomParam1,//для Box Geom параметр определяет X данные размера
			GeomParam2,//для Box Geom параметр определяет Y данные размера
			GeomParam3;//для Box Geom параметр определяет Z данные размера
	     //******************************параметры формы обьекта*********************

		 //******************************параметры позиции обьекта*********************
		PxReal px,py,pz;
		//******************************параметры позиции обьекта*********************

		//******************************параметры поворота обьекта*********************
		PxReal qx,qy,qz,qw;
	    //******************************параметры поворота обьекта*********************

		// ******************строка определяет вид геометрии обьекта: Box(коробка) , Sphere(сфера) , Capsule(капсула) , TriangleMesh(сетка) *******************************
		//так же определяет вид геометрии персонажа если загружен токовой может исспользовать  Box(коробка) , Capsule(капсула)
		wchar_t GeomData[30];
		// ******************строка определяет вид геометрии обьекта: Box(коробка) , Sphere(сфера) , Capsule(капсула) , TriangleMesh(сетка) *******************************

	};

	struct FIZFileData {

		FIZFileData();
		//******************************Material****************

		PxReal StaticFriction, DynamicFriction, Restitution;

		//******************************Material****************

		// **************************моссив форм для обьекта
		ShapeData Shapes[100];
		// **************************моссив форм для обьекта
		int fullShapesCount;//счетчик заполненых обьектов предыдущего массива

        //*******************масса ********************************
		PxReal mass;
		PxReal MassSpaceInertiaTensorX , MassSpaceInertiaTensorY , MassSpaceInertiaTensorZ;
		//*******************масса ********************************
		//*******************Velocity******************************
		PxReal VelocityParam1, VelocityParam2 , VelocityParam3;
		//*******************Velocity******************************
		//*******************Damping******************************
		PxReal AngularDampingParam; // коофициент гошения угловых сил 
		PxReal LinearDampingParam; // коофициент гашения линейных сил
		//*******************Damping******************************
		// ******************Character******************************
		wchar_t ClimbingMode[40]; // эта переменна используется только для персонажа типа формы - капсула.
		// для типа формы коробка этот параметр игнорируется для более подробного описания PhysXGuide->Character Controllers->Climbing Mode 
		// может принимать два значения 1 EASY (EASY_CAPSULE_CLIMBING_MODE) если папраметр автостэпинга может не соблюдаться 
		// и CONSTRAINED (CONSTRAINED_CAPSULE_CLIMBING_MODE) если праметр автостепинга берется в полной мере и не может быть игнорирован
		//*******************UpVector******************************
		PxReal UpVectorX, UpVectorY, UpVectorZ; // вектор который показывает куда направлена верхушка персонажа
		//*******************UpVector******************************
		//*******************stepOffset****************************
		PxReal stepOffset; // величина которая означает насколько высоко может шагнуть персонаж
		//*******************stepOffset*****************************
		// ******************Character******************************
	};

	PhysXControler ();
	~PhysXControler ();

	void Init(HWND * g_hwnd , D3DGlobalContext *D3DGC );

	PxFoundation * gFoundation = nullptr;
	PxCooking *	theCooking = nullptr;
	PxPhysics *	gPhysics = nullptr;
//	PxProfileZoneManager * gProfileZoneManager = nullptr;
	PxScene * gScene = nullptr;
	PxDefaultCpuDispatcher * gDispatcher;
	PxGpuDispatcher * GPUDispatcher;
	PxOutputStream * outStream = nullptr;
	PxCudaContextManager* gCudaContextManager;
	PxPvdSceneClient* pvdClient;

	struct PxgDynamicsMemoryConfig
	{
		PxU32 constraintBufferCapacity; //!< Capacity of constraint buffer allocated in GPU global memory
		PxU32 contactBufferCapacity;    //!< Capacity of contact buffer allocated in GPU global memory
		PxU32 tempBufferCapacity;       //!< Capacity of temp buffer allocated in pinned host memory.
		PxU32 contactStreamCapacity;    //!< Capacity of contact stream buffer allocated in pinned host memory. This is double-buffered so total allocation size = 2* contactStreamCapacity.
		PxU32 patchStreamCapacity;      //!< Capacity of the contact patch stream buffer allocated in pinned host memory. This is double-buffered so total allocation size = 2 * patchStreamCapacity.
		PxU32 forceStreamCapacity;      //!< Capacity of force buffer allocated in pinned host memory.
		PxU32 heapCapacity;             //!< Initial capacity of the GPU and pinned host memory heaps. Additional memory will be allocated if more memory is required.
		PxU32 foundLostPairsCapacity;   //!< Capacity of found and lost buffers allocated in GPU global memory. This is used for the found/lost pair reports in the BP.


		PxgDynamicsMemoryConfig() :
			constraintBufferCapacity(32 * 1024 * 1024),
			contactBufferCapacity(24 * 1024 * 1024),
			tempBufferCapacity(16 * 1024 * 1024),
			contactStreamCapacity(6 * 1024 * 1024),
			patchStreamCapacity(5 * 1024 * 1024),
			forceStreamCapacity(1 * 1024 * 1024),
			heapCapacity(64 * 1024 * 1024),
			foundLostPairsCapacity(256 * 1024)
		{
		}
	} RCubePxRGB;
//	PxErrorCallback& PhysXControler::getSampleErrorCallback();

	PxShape* shape;

//#ifdef _DEBUG
//	PxVisualDebuggerConnection*	gConnection = nullptr;
	PxPvd *pvd = nullptr;
	PxPvdTransport *transport = nullptr;
//#endif

	void CreateHeildHield( Terrain* TerrainObject, std::wstring filename);
	int CreateObject(std::wstring filename, bool IsDynamic ,ObjectData * Data, _3DModel* Model ); // функция для добавления того или иного обьекта возвращает индекс элемента в соответст моссиве
	void CreateInstancingObjects(std::wstring filename, bool IsDynamic, int ObjInex, _3DModel* Model );
	int CreateCharacter(std::wstring filename , _3DModel* model); // функция для добавления персонажа (PxController) в физику возвращает идекс в (g_ControllerManager)
	void Frame();

	int JoinAllCubes(); // Соединяем кубы в связку
	bool CheckActorsStopMovement ();

	std::vector<PxRigidStatic*> StaticRigitArray;
	std::vector<PxRigidDynamic*> DymanicRigitArray;
	std::vector<PxTriangleMesh*> MeshesArray;

	std::vector<PxController*> ControllersArray; // массив контролерров персонажей - является скорее вспомогательным средством чем необходимостью ведь до любого контролера можно достучаться через g_ControllerManager
	PxUserControllerHitReport * g_ControlerReport;

	ObjectData * DynamicDataArr;// моссив хранит данные динамических обьектов для обновления их позиции в дальнейшем
//	std::vector<ObjectData> CharactersDataArr;// моссив хранит данные персонажей для обновления их позиции в дальнейшем
	int DynamicDataArrCounter; //хранит число заполненных элементов массива на строку выше(по сути это просто количество динамич обьектов)
	int  StaticNumObjects; // количество созданных статических обьектов

private:

	EventHelper Evhp;
	PxControllerManager* g_ControllerManager; // мэнеджер для управления персонажами
	int NumOfIndefinedCharacters;// переменная означает количество иденткфицированных персонажей

	PxHeightField*  aHeightField = nullptr;
	void ReadFIZFile(FIZFileData * Data, std::wstring filenam);

	HWND g_Hwnd;
	//модел лист активный в движке
//	ResourceManager* MyManager;

	//***************просто рабочие переменные которыет позвоняют не тратить время на создание локальных***************
	XMVECTOR WorkQuat;
	PxQuat WorkPxQ;
	PxMaterial* WorkgMaterial;
	//***************просто рабочие переменные которыет позвоняют не тратить время на создание локальных***************

	PxShape* CreateShape( _3DModel* model, ShapeData Shape);



};

#endif
