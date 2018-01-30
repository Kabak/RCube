#pragma once
#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FRUSTUMCLASS_H_
#define _FRUSTUMCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "DirectXMath.h"

using namespace DirectX;
////////////////////////////////////////////////////////////////////////////////
// Class name: FrustumClass
////////////////////////////////////////////////////////////////////////////////
class FrustumClass
{
public:
	FrustumClass();
   ~FrustumClass();

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

	void ConstructFrustum(float, XMMATRIX&, XMMATRIX&);
	void ConstructFrustumNew( XMMATRIX& );

	bool CheckPoint(XMFLOAT3&);
	bool CheckCube(XMFLOAT3&, float&);
	bool CheckSphere(XMFLOAT3&, float&);
	bool CheckRectangle(XMFLOAT3&, float&, float&, float&);

private:
	
	union plane 
	{
		plane() {}
		XMFLOAT4 m_planes[6];
		XMVECTOR planes[6];
	} u_plane;

	union _Res
	{
		_Res() {}; // res = { 0.0f, 0.0f, 0.0f, 0.0f }; 
		XMVECTOR res;
		XMFLOAT4 fres;
	} Res;

	XMMATRIX Input_projectionMatrix;
	XMMATRIX matrix;
	XMVECTOR TempVector;
//	XMFLOAT4X4A Temp;
//	RCubeMatrix Temp;
};

#endif