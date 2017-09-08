#pragma once
#include "stdafx.h"

#ifndef _KFCUBEMAPPINGCLASS_H_
#define _KFCUBEMAPPINGCLASS_H_

#include <d3d11.h>
#include <vector>
#include "DirectXMath.h"
#include "D3Dcompiler.h"
#include <DDSTextureLoader.h>
//#include "D3DX11.h"

using namespace DirectX ;

class KFCubeMapping{
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

	HRESULT Init(ID3D11DeviceContext * ActiveDeviceContext, ID3D11Device * ActiveDevice, ID3D11ShaderResourceView * Texure,
		ID3D10Blob * Blob) ;
	void Render(const XMMATRIX& WorldViewProj) ;
	KFCubeMapping();
	~KFCubeMapping() ;

private:
	ID3D11DeviceContext * d3d11DevCon ;
	ID3D11Device * d3d11Device ;
	ID3D11Buffer* sphereIndexBuffer;
    ID3D11Buffer* sphereVertBuffer;
	int NumSphereVertices;
    int NumSphereFaces;
	XMMATRIX Rotationx , Rotationy , Rotationz;
	XMMATRIX WVP;
	ID3D11Buffer* cbPerObjectBuffer;
	ID3D11ShaderResourceView* smrv;
	ID3D11SamplerState* CubesTexSamplerState;
	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;
	ID3D11InputLayout * vertLayout;

	struct Vertex	//Overloaded Vertex Structure
	{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
    };

	struct cbPerObject
    {
	   XMMATRIX  WVP;
    };

cbPerObject cbPerObj;

};

#endif