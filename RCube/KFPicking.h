#pragma once
#include "stdafx.h"

#ifndef _KFPICKING_H_
#define _KFPICKING_H_

#include <vector>
#include "DirectXMath.h"

using namespace DirectX;
  
struct KF3DObjectData{

	XMMATRIX * ObjWorld ;
	std::vector<XMFLOAT3> ObjVertPosArray;
	std::vector<DWORD> ObjVertIndexArray ;

};

class KFPicking {
private:
	int ObjectNum ;
	int g_ScreenWidth ;
	int g_ScreenHeight ;
	int g_WindowPosX , g_WindowPosY ;

	XMMATRIX * g_Projection , * g_View;
	bool PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point );
	KF3DObjectData * ObjDataArray ;
	void pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir , 
							  XMMATRIX & camProjection , XMMATRIX & camView);
    float pick(XMVECTOR pickRayInWorldSpacePos,
	XMVECTOR pickRayInWorldSpaceDir,
	std::vector<XMFLOAT3>& vertPosArray,
	std::vector<DWORD>& indexPosArray ,
	XMMATRIX& worldSpace);
	int AproofTriangeleIndex ;
public :
	~KFPicking() ;

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------

	HRESULT Init(HWND hwnd , int WindowPosX , int WindowPosY , int ScreenWidth , int ScreenHeight 
						, KF3DObjectData ObjArray[] , int NumOfObjects) ;

    int ApproofObjs(int X , int Y , XMMATRIX& Projection , XMMATRIX& View , int & TriangleIndex) ; 

	bool * AproofObjectsDesc ;
};


#endif