#include "stdafx.h"
#include "KFModel.h"
#include "D3dClass.h"
#include "ResourceManager.h"
#include "D3DGlobalContext.h"
#include "FrustumClass.h"
#include "KF_PXControler.h"

#ifndef _KFMODELLIST_H_
#define _KFMODELLIST_H_

using namespace std;

class KFModelList
{
public:
	KFModelList();
	~KFModelList();

	HRESULT Init(D3DGlobalContext * D3DGC
		, D3DClass * ActiveLightClass ,  FrustumClass * ActiveFrustum);

	void AddObject(wstring FileNameKFO, UINT Shaders, int InstCout);

	void LightRender();
	void Render();
	void InitRandInstansingPoses(int IndexOfInstElement, float MaxX, float MinX, float MaxY, float MinY, float MaxZ, float MinZ);
	void InitRandInstansingRots(int IndexOfInstElement);
	// Обновление позиции объектам
	void SetPositon(int IndexOfObject, int InstanceIndex, float PosX, float PosY, float PosZ);
	void SetPositon(int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos);

	void SetRotation(int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation);

	void Frame();
	
	vector <RCudeObjDesc*>ObjDescs;	// Список 3D объектов на сцене

	KFModel ** RCubeModelList; // в этом массиве все обьекты

	Material* GetMaterialData(int ObjIndex);
//	PositionType ** ObjPoses;// это массив который хранит данные об указателях на позиции и поворота для наиболее скоростного доступа без посредников

private:
 
	D3DGlobalContext * Local_D3DGC;
	FrustumClass * gActiveFrustum;
	vector<bool> IsObjUseClustering; //массив описывающий использует ли тип модели кластеринг true использует false не использует формат заполнения [индекс типа модели]

	int ModelCout; // количество моделей
	ResourceManager * myResourceManeger;

	int CoutObjetcs; // Количество объектов на сцене
	vector<int> ActiveShaderIndexes;// массив котопый хранит все индексы активных шейдеров

	D3DClass * g_LightClass;

	int g_c, g_p;

};


#endif 