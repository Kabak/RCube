#pragma once
#include "stdafx.h"

#ifndef _KFMAP_H_
#define _KFMAP_H_

#include "KFModel.h"
#include "FrustumClass.h"

class KFMap : public KFModel {

public:

	struct KFInt3 {

		int x , y, z;

	};

	struct ObjectDescStruct {

		int ObjIndex; // ����� ���� ������� � �������� ���������� ������
		int InstIndex; // ������ �������� � �������� ������� ���������� � ���� ������ ObjIndex

	};

	struct AutoGridParam {

		float MaxX , MinX , MaxY, MinY , MaxZ, MinZ;
		int NumXClusters, NumYClusters, NumZClusters;

	};

	KFMap();
	~KFMap();
	HRESULT Init(HWND hwnd, std::wstring filename, ID3D11Device *d3d11Device, IDXGISwapChain* SwapChain
		, ID3D11DeviceContext * DevCon, ID3D10Blob * Blob, UINT Shaders, XMFLOAT3 & CameraPos , FrustumClass * ActiveFrustumSystem);
	void Render();
	void Frame(bool IsItAnyCameraReplases);
	void addExternObject(RCudeObjDesc * ObjCube);
	void RememberPoses(std::vector<KFModel*> & ClustersUsingObj); // ��� ������� ���������� ��������� ����������� �������� � ���������

	std::vector<std::vector<int>> ObjectsClusterIndexes;
	bool IsClastersDarw;
	bool IsItNeedToUpdate;
	std::vector<bool> UpdateData;
	std::vector<std::vector<int>> NeeedFruatrumData;
	void SetAutoGenerateClustGrid(AutoGridParam GridParam);
private:
	//int ClasterActiveValue , PraviosClasterActiveValue;
	std::vector<int> ClusterISee;// ������ ��������� ������� � ����
	HWND g_hwnd;
	std::vector<std::vector<std::vector<int>>> ExternObjectsClusterIndex;
	RCudeObjDesc** g_Objcts;
	FrustumClass * g_Frustum;
	std::vector<std::vector<std::vector<int>>> ClusterHelpPosesData;//������� ���������� XYZ
	float ClusterWidth ,ClusterHeight ,ClusterLength;
	std::vector<KFInt3> ClusterPos;//������� ���������� XYZ
	std::vector<std::vector<ObjectDescStruct>> ClusterHaveObj; // ������ ����������� ���������� ����� ���� �������� � ���������
	std::vector<std::vector<XMFLOAT3>> ObjPositionInClusters; //  ������ ����������� ��������� �������� � ������� ��������� �� ���� ������ � �������� ������� �������� �� ���� �� ������� �� ���� = 0.5 ������ ���������� [������ ����][������ ��������]
	std::vector<std::vector<int>> IndexOfObjInCluser; //������ ������� ������ ������ ������� � ��� �������� ������ ���������� [������ ���� �������][������ ��������]
	std::vector<bool> IsItNeedToRefreshBuffer; //  ������ ������������ ��������� �������� � ��=��� ��������� ���������� ������������� ������ (������ �� ��� �� ����������� ����� : ���� ������ � ���� ����� �� ������� ����� ��������� �.� ��� �������� �������� � ������ � ��������� ������ �� ����� ��� �� ����� �������� � ������������ ����� � �� ����� ����� � ������������ ������ � ��������� ���� ��� �� ����� ������� ���� � �� �������� �� ��� ��������� � ����� ������) ������� [������ �������]

	int FindClearSlot(std::vector<ObjectDescStruct>);

};


#endif