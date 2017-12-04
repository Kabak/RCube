#include "stdafx.h"
#include "KFModelList.h"

KFModelList::KFModelList(){

//	IsClusteringUse = true;
	ModelCout = 0;

}

KFModelList::~KFModelList(){


	int c = 0;
	while (c < ModelCout)
	{
		if(RCubeModelList[c] != nullptr)
		delete RCubeModelList[c];

		++c;

	}

	delete [] RCubeModelList;

	ObjDescs.clear ();
		
//	delete [] ObjPoses;


}


HRESULT KFModelList::Init(D3DGlobalContext * D3DGC
	, D3DClass * ActiveLightClass, FrustumClass * ActiveFrustum) {

	gActiveFrustum = ActiveFrustum;
	g_LightClass = ActiveLightClass;
	Local_D3DGC = D3DGC;

	RCubeModelList = new KFModel*[KF_MAX_MODEL_ARR_SIZE];
//	ObjPoses = new PositionType*[KF_MAX_MODEL_ARR_SIZE];

	return S_OK;
}

void KFModelList::AddObject(std::wstring FileNameKFO, UINT Shaders, int InstCout){

	KFModel * Model3DPointer;
	int c = 0;
	Model3DPointer = new KFModel;

	Model3DPointer->Init(FileNameKFO, Local_D3DGC, Shaders , InstCout , g_LightClass);

	RCubeModelList[ModelCout] = Model3DPointer;
	// сохраняю указатели на элементы модэлов (очень подходит для быстрого доступа)
	ObjDescs.push_back( &Model3DPointer->ThisObjDesc );
//	ObjPoses[ModelCout] = Model3DPointer->ThisObjDesc.Positions;

	++CoutObjetcs; // К количеству объектов на сцене + 1

	++ModelCout;
}


void KFModelList::LightRender(){
	 g_c = 0, g_p = 0;

	 KFModel ** Source3 = &RCubeModelList[0];
	 KFModel * Temp2 = nullptr;
	 
			//бегу по всем моделям
			while (g_p < CoutObjetcs)
			{// если обьекты для проверки просто напросто не закончились
				Temp2 = *Source3;

					if (Temp2->ThisObjDesc.IsObjDraw ) {

							Temp2->LightRender();
					}

				++Source3;
				++g_p;

			}

			g_p = 0;

		++g_c;


}

void KFModelList::Frame() {

	int counter = 0 , instanceCounter = 0 , CuledInstancesCout = 0;
	RCudeObjDesc ** DescPoinder = &ObjDescs[0];
	RCudeObjDesc * Temp;
	RCube_VecFloat34 TempPositions;
	
	size_t Size = sizeof ( XMVECTOR ) * 2;

	while (counter < ModelCout) 
	{

		Temp = *DescPoinder;
		CuledInstancesCout = 0;
		instanceCounter = 0;

		while (instanceCounter < Temp->InstanceCount) {


		 	 TempPositions.Vec = Temp->Positions[instanceCounter].position.Vec + Temp->ObjectBox.CentralPoint.Vec;
			 if (gActiveFrustum->CheckSphere(TempPositions.Fl3, Temp->ObjectBox.SphereRadius)) {

				 memcpy(&Temp->PositionsDraw[CuledInstancesCout], &Temp->Positions[instanceCounter], Size );

				 ++CuledInstancesCout;

			 }

			++instanceCounter;
		}

		Temp->InstanceFrustumPassedAmount = CuledInstancesCout;
		++DescPoinder;
		++counter;
	}

}

void KFModelList::InitRandInstansingPoses(int IndexOfInstElement, float MaxX, float MinX, float MaxY, float MinY
	, float MaxZ, float MinZ) {

	int c = 0;
	float ValueX = 0, ValueY = 0, ValueZ = 0;

	while (c < RCubeModelList[IndexOfInstElement]->ThisObjDesc.InstanceCount)
	{

		ValueX = ((float)(rand() % (int)MaxX * 2 * 10) / 10) + MinX;
		ValueY = ((float)(rand() % (int)MaxY * 2 * 10) / 10) + MinY;
		ValueZ = ((float)(rand() % (int)MaxZ * 2 * 10) / 10) + MinZ;

		RCubeModelList[IndexOfInstElement]->ThisObjDesc.Positions[c].position.Fl3 = XMFLOAT3(ValueX, ValueY, ValueZ);

		++c;
	}

	RCubeModelList[IndexOfInstElement]->UpdateInstancingPos();

}

void KFModelList::InitRandInstansingRots(int IndexOfInstElement) {

		int c = 0;
		float ValueX = 0, ValueY = 0, ValueZ = 0;
		XMVECTOR TempV;

		while (c < RCubeModelList[IndexOfInstElement]->ThisObjDesc.InstanceCount)
		{

			// float rnd(int min, int max)
			// {return min + rand() % (1000 * (max - min)) / 1000.0f; }

			ValueX = (float)(rand() % 1000 * (XM_2PI)) / 1000.0f;
			ValueY = (float)(rand() % 1000 * (XM_2PI)) / 1000.0f;
			ValueZ = (float)(rand() % 1000 * (XM_2PI)) / 1000.0f;

			TempV = XMQuaternionRotationRollPitchYaw(ValueX, ValueY, ValueZ);

			XMStoreFloat4(&RCubeModelList[IndexOfInstElement]->ThisObjDesc.Positions[c].rotation.Fl4, TempV);

			++c;
		}

		RCubeModelList[IndexOfInstElement]->UpdateInstancingPos();

	}

void KFModelList::SetPositon(int IndexOfObject , int InstanceIndex, float PosX, float PosY, float PosZ) {

	RCubeModelList[IndexOfObject]->ThisObjDesc.Positions[InstanceIndex].position.Fl3 = XMFLOAT3(PosX , PosY , PosZ);

	RCubeModelList[IndexOfObject]->UpdateInstancingPos();
}


void KFModelList::SetPositon(int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos)
{

	RCubeModelList [ IndexOfObject ]->ThisObjDesc.Positions[ InstanceIndex ].position.Fl3 = Pos;
//	ObjPoses[IndexOfObject][InstanceIndex].position.Fl3 = Pos;

	RCubeModelList[IndexOfObject]->UpdateInstancingPos();

}


void KFModelList::SetRotation(int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation)
{

	RCubeModelList [ IndexOfObject ]->ThisObjDesc.Positions [ InstanceIndex ].rotation.Fl4 = Rotation;
//	ObjPoses[IndexOfObject][InstanceIndex].rotation.Fl4 = Rotation;

	RCubeModelList[IndexOfObject]->UpdateInstancingPos();

}


Material* KFModelList::GetMaterialData(int ObjIndex) {

	//return &RCubeModelList[ObjIndex]->AllMaterials[ObjIndex];
	return nullptr;
}

void KFModelList::Render() {

	g_c = 0, g_p = 0;

	KFModel ** Source3 = &RCubeModelList[0];
	KFModel * Temp2 = nullptr;

			//бегу по всем моделям
			while (g_p < CoutObjetcs)
			{// если обьекты для проверки просто напросто не закончились
				Temp2 = *Source3;

					if (Temp2->ThisObjDesc.IsObjDraw) {

						if (Temp2->ThisObjDesc.InstanceFrustumPassedAmount != -1 || !IsObjUseClustering[g_c])
							Temp2->Render();

					}

				++Source3;
				++g_p;

			}

			g_p = 0;

		++g_c;

}