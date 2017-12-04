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
	// ���������� ������� ��������
	void SetPositon(int IndexOfObject, int InstanceIndex, float PosX, float PosY, float PosZ);
	void SetPositon(int IndexOfObject, int InstanceIndex, XMFLOAT3 &Pos);

	void SetRotation(int IndexOfObject, int InstanceIndex, XMFLOAT4 &Rotation);

	void Frame();
	
	vector <RCudeObjDesc*>ObjDescs;	// ������ 3D �������� �� �����

	KFModel ** RCubeModelList; // � ���� ������� ��� �������

	Material* GetMaterialData(int ObjIndex);
//	PositionType ** ObjPoses;// ��� ������ ������� ������ ������ �� ���������� �� ������� � �������� ��� �������� ����������� ������� ��� �����������

private:
 
	D3DGlobalContext * Local_D3DGC;
	FrustumClass * gActiveFrustum;
	vector<bool> IsObjUseClustering; //������ ����������� ���������� �� ��� ������ ���������� true ���������� false �� ���������� ������ ���������� [������ ���� ������]

	int ModelCout; // ���������� �������
	ResourceManager * myResourceManeger;

	int CoutObjetcs; // ���������� �������� �� �����
	vector<int> ActiveShaderIndexes;// ������ ������� ������ ��� ������� �������� ��������

	D3DClass * g_LightClass;

	int g_c, g_p;

};


#endif 