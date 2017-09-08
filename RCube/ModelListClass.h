#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////
// Filename: modellistclass.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _MODELLISTCLASS_H_
#define _MODELLISTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "DirectXMath.h"
#include <stdlib.h>
#include <time.h>

using namespace DirectX;
///////////////////////////////////////////////////////////////////////////////
// Class name: ModelListClass
///////////////////////////////////////////////////////////////////////////////
class ModelListClass
{
private:
	struct ModelInfoType
	{
		XMVECTOR color;
//		float positionX, positionY, positionZ;
		XMFLOAT3 position;
	};

public:
	ModelListClass();
	ModelListClass(const ModelListClass&);
	~ModelListClass();

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

	bool Initialize(HWND hwnd, int);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, XMVECTOR&);

private:
	//__declspec(align(16))
	int m_modelCount;
	ModelInfoType m_ModelInfoList[25];
};

#endif