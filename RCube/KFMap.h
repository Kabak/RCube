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

		int ObjIndex; // инекс типа обьекта у которому относиться инстэс
		int InstIndex; // индекс инстэнса к которому который относиться к типу модели ObjIndex

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
	void RememberPoses(std::vector<KFModel*> & ClustersUsingObj); // эта функция запоминает положения статических обьектов в кластерах

	std::vector<std::vector<int>> ObjectsClusterIndexes;
	bool IsClastersDarw;
	bool IsItNeedToUpdate;
	std::vector<bool> UpdateData;
	std::vector<std::vector<int>> NeeedFruatrumData;
	void SetAutoGenerateClustGrid(AutoGridParam GridParam);
private:
	//int ClasterActiveValue , PraviosClasterActiveValue;
	std::vector<int> ClusterISee;// массив кластеров которые я вижу
	HWND g_hwnd;
	std::vector<std::vector<std::vector<int>>> ExternObjectsClusterIndex;
	RCudeObjDesc** g_Objcts;
	FrustumClass * g_Frustum;
	std::vector<std::vector<std::vector<int>>> ClusterHelpPosesData;//система заполнения XYZ
	float ClusterWidth ,ClusterHeight ,ClusterLength;
	std::vector<KFInt3> ClusterPos;//система заполнения XYZ
	std::vector<std::vector<ObjectDescStruct>> ClusterHaveObj; // массив описывающии нахождение каких либо обьектов в кластерах
	std::vector<std::vector<XMFLOAT3>> ObjPositionInClusters; //  массив описывающии положение обьектов в системе кластеров те если обьект в середине первого кластера по иксу то позиция по иксу = 0.5 формат заполнения [индекс типа][индекс инстэнса]
	std::vector<std::vector<int>> IndexOfObjInCluser; //массив который хранит индекс обьекта в его кластере формат заполнения [индекс типа обьекта][индекс инстэнса]
	std::vector<bool> IsItNeedToRefreshBuffer; //  массив описываюшщии состояние обьектов в лп=ане набоности обновления отрисовочного буфера (пример за чем он обязательно нужен : если обьект в друг вышел за пределы нашей видимости т.е его отсекает фраструм я просто в некотором случае не смогу его во всемя обновить в отрисовачный буфер и он будет вечно в отрисовачном буфере в положении пока его не отсел фрастум пока я не посмотрю на его кординаты в общем буфере) формаат [индекс обьекта]

	int FindClearSlot(std::vector<ObjectDescStruct>);

};


#endif