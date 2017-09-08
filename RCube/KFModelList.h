#include "stdafx.h"
#include "KFModel.h"
#include "KFResourceManager.h"
#include "D3DGlobalContext.h"
#include "FrustumClass.h"
#include "KF_PXControler.h"

#ifndef _KFMODELLIST_H_
#define _KFMODELLIST_H_

class KFModelList
{
public:
	KFModelList();
	~KFModelList();

	HRESULT Init(HWND g_Hwnd, D3DGlobalContext * D3DGC, KFResourceManager * ResourceManeger
		, LightClass * ActiveLightClass ,  FrustumClass * ActiveFrustum);

	void AddObject(std::wstring FileNameKFO, UINT BolobIndex, UINT Shaders, int InstCout);

	void LightRender();
	void Render();
	void InitRandInstansingPoses(int IndexOfInstElement, float MaxX, float MinX, float MaxY, float MinY, float MaxZ, float MinZ);
	void InitRandInstansingRots(int IndexOfInstElement);
	// ���������� ������� ��������
	void SetPositon(int IndexOfObject, int InstanceIndex, float PosX, float PosY, float PosZ);
	void SetPositon(int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos);

	void SetRotation(int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation);

	void Frame();
	std::vector <KFModel*> RCubeModelListNoClustring; // �� ���������� ����������
	std::vector <KFModel*> RCubeModelListClustring; // ���������� ����������
	KFModel ** RCubeModelList; // � ���� ������� ��� �������
	std::vector <int> FrameIndexes;
	bool IsClusteringUse;
	KFModel::RCudeObjDesc ** ObjDescs;
	Material::SurfaceMaterial* GetMaterialData(int ObjIndex);
	KFModel::PositionType ** ObjPoses;// ��� ������ ������� ������ ������ �� ���������� �� ������� � �������� ��� �������� ����������� ������� ��� �����������

private:
 
	FrustumClass * gActiveFrustum;
	std::vector<bool> IsObjUseClustering; //������ ����������� ���������� �� ��� ������ ���������� true ���������� false �� ���������� ������ ���������� [������ ���� ������]
	std::vector <int> PreviosFrameIndexes;// ��� ����� ����� ��� ����������� ��������� ���������� ����
	int ModelCoutClustring , ModelCoutNoClustring , ModelCout;
	KFResourceManager * myResourceManeger;
	ID3D11Device * g_Device;
	IDXGISwapChain * g_SwapChain;
	ID3D11DeviceContext *  g_DevCon;
	HWND g_HWND;
	//MyStruct * ShaderDrawObjArr;
	int * CoutObjetcs;
	std::vector<int> ActiveShaderIndexes;// ������ ������� ��������� ��� ������� �������� ��������
	int ** ShaderObjArr;
	int ShaderDrawObjArrSize;
	LightClass * g_LightClass;

	int g_c , g_p , g_ActiveObjCout;

};


#endif 