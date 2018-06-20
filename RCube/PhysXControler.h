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

	// +++++++++++++++++++++    ��� ����!!!    +++++++++++++++++++++++++++
	// ++++++++++++++++   ��� ����������� �����   ++++++++++++++++++++++++
	PxDistanceJoint* Joints[MAX_OBJ-1];
	// +++++++++++++++++++++    ��� ����!!!    +++++++++++++++++++++++++++

	// +++++++++++++++++++++++   ��� �������� ����������� ������ �� PhysX   +++++++++++++++++++++++++
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
	// -----------------------   ��� �������� ����������� ������ �� PhysX   --------------------------

	// ��������� ��������� ��� ������� ��������� � ������������� ���� ����������
	struct ObjectData
	{

		int ModelIndex;// ���������� ����� ������
		int InstanceIndex;// ���������� ����� ��������

	};

	//��� �� ���������� ��� ��������� ��������� ���� �������� ������� ����� �������������  Box(�������) , Capsule(�������)
	struct ShapeData {

		//******************************��������� ����� �������*********************
		PxReal GeomParam1,//��� Box Geom �������� ���������� X ������ �������
			GeomParam2,//��� Box Geom �������� ���������� Y ������ �������
			GeomParam3;//��� Box Geom �������� ���������� Z ������ �������
	     //******************************��������� ����� �������*********************

		 //******************************��������� ������� �������*********************
		PxReal px,py,pz;
		//******************************��������� ������� �������*********************

		//******************************��������� �������� �������*********************
		PxReal qx,qy,qz,qw;
	    //******************************��������� �������� �������*********************

		// ******************������ ���������� ��� ��������� �������: Box(�������) , Sphere(�����) , Capsule(�������) , TriangleMesh(�����) *******************************
		//��� �� ���������� ��� ��������� ��������� ���� �������� ������� ����� �������������  Box(�������) , Capsule(�������)
		wchar_t GeomData[30];
		// ******************������ ���������� ��� ��������� �������: Box(�������) , Sphere(�����) , Capsule(�������) , TriangleMesh(�����) *******************************

	};

	struct FIZFileData {

		FIZFileData();
		//******************************Material****************

		PxReal StaticFriction, DynamicFriction, Restitution;

		//******************************Material****************

		// **************************������ ���� ��� �������
		ShapeData Shapes[100];
		// **************************������ ���� ��� �������
		int fullShapesCount;//������� ���������� �������� ����������� �������

        //*******************����� ********************************
		PxReal mass;
		PxReal MassSpaceInertiaTensorX , MassSpaceInertiaTensorY , MassSpaceInertiaTensorZ;
		//*******************����� ********************************
		//*******************Velocity******************************
		PxReal VelocityParam1, VelocityParam2 , VelocityParam3;
		//*******************Velocity******************************
		//*******************Damping******************************
		PxReal AngularDampingParam; // ���������� ������� ������� ��� 
		PxReal LinearDampingParam; // ���������� ������� �������� ���
		//*******************Damping******************************
		// ******************Character******************************
		wchar_t ClimbingMode[40]; // ��� ��������� ������������ ������ ��� ��������� ���� ����� - �������.
		// ��� ���� ����� ������� ���� �������� ������������ ��� ����� ���������� �������� PhysXGuide->Character Controllers->Climbing Mode 
		// ����� ��������� ��� �������� 1 EASY (EASY_CAPSULE_CLIMBING_MODE) ���� ��������� ������������ ����� �� ����������� 
		// � CONSTRAINED (CONSTRAINED_CAPSULE_CLIMBING_MODE) ���� ������� ������������ ������� � ������ ���� � �� ����� ���� �����������
		//*******************UpVector******************************
		PxReal UpVectorX, UpVectorY, UpVectorZ; // ������ ������� ���������� ���� ���������� �������� ���������
		//*******************UpVector******************************
		//*******************stepOffset****************************
		PxReal stepOffset; // �������� ������� �������� ��������� ������ ����� ������� ��������
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
	int CreateObject(std::wstring filename, bool IsDynamic ,ObjectData * Data, _3DModel* Model ); // ������� ��� ���������� ���� ��� ����� ������� ���������� ������ �������� � ��������� �������
	void CreateInstancingObjects(std::wstring filename, bool IsDynamic, int ObjInex, _3DModel* Model );
	int CreateCharacter(std::wstring filename , _3DModel* model); // ������� ��� ���������� ��������� (PxController) � ������ ���������� ����� � (g_ControllerManager)
	void Frame();

	int JoinAllCubes(); // ��������� ���� � ������
	bool CheckActorsStopMovement ();

	std::vector<PxRigidStatic*> StaticRigitArray;
	std::vector<PxRigidDynamic*> DymanicRigitArray;
	std::vector<PxTriangleMesh*> MeshesArray;

	std::vector<PxController*> ControllersArray; // ������ ������������ ���������� - �������� ������ ��������������� ��������� ��� �������������� ���� �� ������ ���������� ����� ����������� ����� g_ControllerManager
	PxUserControllerHitReport * g_ControlerReport;

	ObjectData * DynamicDataArr;// ������ ������ ������ ������������ �������� ��� ���������� �� ������� � ����������
//	std::vector<ObjectData> CharactersDataArr;// ������ ������ ������ ���������� ��� ���������� �� ������� � ����������
	int DynamicDataArrCounter; //������ ����� ����������� ��������� ������� �� ������ ����(�� ���� ��� ������ ���������� ������� ��������)
	int  StaticNumObjects; // ���������� ��������� ����������� ��������

private:

	EventHelper Evhp;
	PxControllerManager* g_ControllerManager; // �������� ��� ���������� �����������
	int NumOfIndefinedCharacters;// ���������� �������� ���������� ������������������ ����������

	PxHeightField*  aHeightField = nullptr;
	void ReadFIZFile(FIZFileData * Data, std::wstring filenam);

	HWND g_Hwnd;
	//����� ���� �������� � ������
//	ResourceManager* MyManager;

	//***************������ ������� ���������� �������� ��������� �� ������� ����� �� �������� ���������***************
	XMVECTOR WorkQuat;
	PxQuat WorkPxQ;
	PxMaterial* WorkgMaterial;
	//***************������ ������� ���������� �������� ��������� �� ������� ����� �� �������� ���������***************

	PxShape* CreateShape( _3DModel* model, ShapeData Shape);



};

#endif
