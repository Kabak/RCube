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

	HRESULT Init(HWND g_Hwnd, D3DGlobalContext * D3DGC, ResourceManager * ResourceManeger
		, D3DClass * ActiveLightClass ,  FrustumClass * ActiveFrustum);

	void AddObject(wstring FileNameKFO, UINT BolobIndex, UINT Shaders, int InstCout);

	void LightRender();
	void Render();
	void InitRandInstansingPoses(int IndexOfInstElement, float MaxX, float MinX, float MaxY, float MinY, float MaxZ, float MinZ);
	void InitRandInstansingRots(int IndexOfInstElement);
	// Обновление позиции объектам
	void SetPositon(int IndexOfObject, int InstanceIndex, float PosX, float PosY, float PosZ);
	void SetPositon(int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos);

	void SetRotation(int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation);

	void Frame();
	vector <KFModel*> RCubeModelListNoClustring; // не используют кластеринг
	vector <KFModel*> RCubeModelListClustring; // используют кластеринг
	
	vector <RCudeObjDesc*>ObjDescs;	// Список 3D объектов на сцене
//	RCudeObjDesc ** ObjDescs;

	KFModel ** RCubeModelList; // в этом массиве все обьекты
	vector <int> FrameIndexes;
	bool IsClusteringUse;

	Material::SurfaceMaterial* GetMaterialData(int ObjIndex);
	PositionType ** ObjPoses;// это массив который хранит данные об указателях на позиции и поворота для наиболее скоростного доступа без посредников

private:
 
	FrustumClass * gActiveFrustum;
	vector<bool> IsObjUseClustering; //массив описывающий использует ли тип модели кластеринг true использует false не использует формат заполнения [индекс типа модели]
	vector <int> PreviosFrameIndexes;// эта фигня нужна для оптимизации проверять предыдущий кадр
	int ModelCoutClustring, ModelCoutNoClustring;
	int ModelCout; // количество моделей
	ResourceManager * myResourceManeger;
	ID3D11Device * g_Device;
	IDXGISwapChain * g_SwapChain;
	ID3D11DeviceContext *  g_DevCon;
	HWND g_HWND;
	//MyStruct * ShaderDrawObjArr;
	int * CoutObjetcs;
	vector<int> ActiveShaderIndexes;// массив котопый хранит все индексы активных шейдеров
	int ** ShaderObjArr;
	int ShaderDrawObjArrSize;
	D3DClass * g_LightClass;

	int g_c , g_p , g_ActiveObjCout;

};


#endif 