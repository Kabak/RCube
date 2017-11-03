#pragma once
#include "stdafx.h"

#ifndef _CUBEMAPPINGCLASS_H_
#define _CUBEMAPPINGCLASS_H_

#include <d3d11.h>
#include <vector>
#include "DirectXMath.h"
#include "D3Dcompiler.h"
#include <DDSTextureLoader.h>
#include "D3DGlobalContext.h"
#include "Buffers_def.h"	// Описание типов вертексных буферов

using namespace DirectX ;

class CubeMapping{
public :

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------

	HRESULT Init( D3DGlobalContext *D3DGC, ID3D11ShaderResourceView * Texure );
	void Render() ;
	CubeMapping();
	~CubeMapping() ;

	int ShaderForDraw = -1;

private:
	D3DGlobalContext* Local_D3DGC;
	ID3D11DeviceContext * d3d11DevCon ;
	ID3D11Device * d3d11Device ;
	ID3D11Buffer* sphereIndexBuffer;
    ID3D11Buffer* sphereVertBuffer;
	int NumSphereVertices;
    int NumSphereFaces;
	XMMATRIX Rotationx , Rotationy , Rotationz;
	XMMATRIX WVP;
	ID3D11ShaderResourceView* smrv;
	ID3D11DepthStencilState* DSLessEqual;

	struct cbPerObject
    {
	   XMMATRIX  WVP;
    };

cbPerObject cbPerObj;

};

#endif