#include "stdafx.h"
#include "KFModelList.h"

KFModelList::KFModelList(){

	IsClusteringUse = true;
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

	c = 0;
	while (c < ShaderDrawObjArrSize) {

		delete[] ShaderObjArr[c];


		++c;
	}


	delete [] CoutObjetcs;
	delete [] ShaderObjArr;
	delete [] RCubeModelList;

/*	
	//	delete [] ObjDescs;
	size_t j = ObjDescs.size ();
	for (size_t i = 0; i < j; i++)
	{
//		RCUBE_DELETE ( ObjDescs[i]->Meshes );
		RCUBE_DELETE ( ObjDescs[i]);
	}
*/
	ObjDescs.clear ();
	
	
	delete [] ObjPoses;
	FrameIndexes.clear();
	PreviosFrameIndexes.clear();

}


HRESULT KFModelList::Init(HWND g_Hwnd, D3DGlobalContext * D3DGC, ResourceManager * ResourceManeger
	, D3DClass * ActiveLightClass, FrustumClass * ActiveFrustum) {

	gActiveFrustum = ActiveFrustum;
	g_LightClass = ActiveLightClass;
	myResourceManeger = ResourceManeger;
	g_Device = D3DGC->D11_device;
	g_SwapChain = D3DGC->D11_swapChain;
	g_DevCon = D3DGC->D11_deviceContext;
	g_HWND = g_Hwnd;

	ShaderObjArr = new int* [30];
	CoutObjetcs = new int[30];

	ZeroMemory ( CoutObjetcs, sizeof ( CoutObjetcs ) );

	ShaderDrawObjArrSize = 0;

	RCubeModelList = new KFModel*[KF_MAX_MODEL_ARR_SIZE];
//	ObjDescs = new RCudeObjDesc* [KF_MAX_MODEL_ARR_SIZE];
	ObjPoses = new PositionType*[KF_MAX_MODEL_ARR_SIZE];

	return S_OK;
}

void KFModelList::AddObject(std::wstring FileNameKFO, UINT BolobIndex, UINT Shaders, int InstCout){

	KFModel * Model3DPointer;
	int c = 0;
	Model3DPointer = new KFModel;

	Model3DPointer->Init(g_HWND, FileNameKFO, g_Device, g_DevCon,
		Shaders , InstCout , g_LightClass);

	RCubeModelList[ModelCout] = Model3DPointer;
	// сохран€ю указатели на элементы модэлов (очень подходит дл€ быстрого доступа)
	ObjDescs.push_back( &Model3DPointer->ThisObjDesc );
	ObjPoses[ModelCout] = Model3DPointer->ThisObjDesc.Positions;

	// отсейка обьектов по шейдеоам ******************************************************

	c = 0;
	int	IndexOfShader = -1;
	while (c < ShaderDrawObjArrSize) {

		if (ActiveShaderIndexes[c] == Shaders) {
			IndexOfShader = c;
			break;
		}

		++c;
	}

	if (IndexOfShader == -1) {

		ActiveShaderIndexes.push_back(Shaders);
		ShaderDrawObjArrSize++;
		IndexOfShader = ShaderDrawObjArrSize - 1;
		ShaderObjArr[IndexOfShader] = new int[1000];
		

	}
	++CoutObjetcs[IndexOfShader];
	ShaderObjArr[IndexOfShader][CoutObjetcs[IndexOfShader] - 1] = (int)(ModelCout);

	// отсейка обьектов по шейдеоам ******************************************************
	++ModelCout;
}


void KFModelList::LightRender(){
	 g_c = 0, g_p = 0 , g_ActiveObjCout = 0;

	 int * Source = &CoutObjetcs[g_c];

	 KFModel ** Source3 = &RCubeModelList[0];
	 KFModel * Temp2 = nullptr;
	// цикл шейдеров
	while (g_c < ShaderDrawObjArrSize)
	{
		// провер€ю есть ли обьeкты дл€ отрисовки вообще
		if (*Source > 0) {
			// устанавливаю активный в проге шейдерсписок
		//	myResourceManeger->SetActiveShadersInProgramm(ActiveShaderIndexes[g_c]);
			//бегу по всем модел€м
			while (g_p < *Source)
			{// если обьекты дл€ проверки просто напросто не закончились
				Temp2 = *Source3;
				if (*Source - 1 >= g_ActiveObjCout) 
					// провер€ю рисуетс€ ли обьект вообще ( до 25.03.2016&&  и соответствует ли этот обьект этому шейдеру)

					if (Temp2->ThisObjDesc.IsObjDraw ) {

					//	if ( Temp2->ThisObjDesc.InstanceFrustumPassedAmount != -1 )
							Temp2->LightRender();

						
						++g_ActiveObjCout;
					}

				++Source3;
				++g_p;

			}

			g_ActiveObjCout = 0;
			g_p = 0;

		}
		++Source;
		++g_c;

	}

}

void KFModelList::Frame() {

	int counter = 0 , instanceCounter = 0 , CuledInstancesCout = 0;
	RCudeObjDesc ** DescPoinder = &ObjDescs[0];
	RCudeObjDesc * Temp;
	RCube_VecFloat34 TempPositions;
	

	while (counter < ModelCout) {

		Temp = *DescPoinder;
		CuledInstancesCout = 0;
		instanceCounter = 0;

		while (instanceCounter < Temp->InstanceCount) {


		 	 TempPositions.Vec = Temp->Positions[instanceCounter].position.Vec + Temp->ObjectBox.CentralPoint.Vec;
			 if (gActiveFrustum->CheckSphere(TempPositions.Fl3, Temp->ObjectBox.SphereRadius)) {

				 memcpy(&Temp->PositionsDraw[CuledInstancesCout], &Temp->Positions[instanceCounter], sizeof(XMVECTOR) * 2);

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

	ObjPoses[IndexOfObject][InstanceIndex].position.Fl3 = Pos;

	RCubeModelList[IndexOfObject]->UpdateInstancingPos();

}


void KFModelList::SetRotation(int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation)
{

	ObjPoses[IndexOfObject][InstanceIndex].rotation.Fl4 = Rotation;

	RCubeModelList[IndexOfObject]->UpdateInstancingPos();

}


Material::SurfaceMaterial* KFModelList::GetMaterialData(int ObjIndex) {

	//return &RCubeModelList[ObjIndex]->AllMaterials[ObjIndex];
	return nullptr;
}

void KFModelList::Render() {

	g_c = 0, g_p = 0, g_ActiveObjCout = 0;

	int * Source = &CoutObjetcs[g_c];

	KFModel ** Source3 = &RCubeModelList[0];
	KFModel * Temp2 = nullptr;
	// цикл шейдеров
	while (g_c < ShaderDrawObjArrSize)
	{
		// провер€ю есть ли обькты дл€ отрисовки вообще
		if (*Source > 0) {
			// устанавливаю активный в проге шейдерсписок
			//	myResourceManeger->SetActiveShadersInProgramm(ActiveShaderIndexes[g_c]);
			//бегу по всем модел€м
			while (g_p < *Source)
			{// если обьекты дл€ проверки просто напросто не закончились
				Temp2 = *Source3;
				if (*Source - 1 >= g_ActiveObjCout)
					// провер€ю рисуетс€ ли обьект вообще ( до 25.03.2016&&  и соответствует ли этот обьект этому шейдеру)

					if (Temp2->ThisObjDesc.IsObjDraw) {

						if (Temp2->ThisObjDesc.InstanceFrustumPassedAmount != -1 || !IsObjUseClustering[g_c])
							Temp2->Render();


						++g_ActiveObjCout;
					}

				++Source3;
				++g_p;

			}

			g_ActiveObjCout = 0;
			g_p = 0;

		}
		++Source;
		++g_c;

	}

}