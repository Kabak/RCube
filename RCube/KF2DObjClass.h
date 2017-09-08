#pragma once
#include "stdafx.h"

#ifndef _KF2DOBJCLASS_H_
#define _KF2DOBJCLASS_H_

#include <d3d11.h>
#include <vector>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"

using namespace std;
using namespace DirectX;

struct InterfaceVertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
	};


//+++++++++++++++++++++++++++++++++++++++++++++++++++++ базовый класс дл€ рисовани€ всех обьектов в 2D
class KF2DObjClass
{
public:
	 KF2DObjClass();
	~KF2DObjClass();

	// ѕереопредел€ем операторы New и delete дл€ правильного размещени€ XMMATRIX в пам€ти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------

	HRESULT Init(
		D3DGlobalContext* _D3DGC,
		InterfaceVertexType* Vertexes,
		unsigned long* Indeces,
		ID3D11ShaderResourceView * texture, 
		int VertexCount, 
		int IndexCount,
		ID3D10Blob * WorkBlob
		);

	void SetVertexBuffer(InterfaceVertexType* Vertexes ) ;

	void Render();

	void SetObjectTexture(ID3D11ShaderResourceView* texture) ;

	ID3D11ShaderResourceView* GetTexture() ;

private:

	D3DGlobalContext*			D3DGC;
	ID3D11Buffer*				g_IndexBuffer; 
	ID3D11Buffer*				g_VertexBuffer;
	ID3D11InputLayout*			m_layout;
	ID3D11SamplerState*			m_sampleState;
	ID3D11ShaderResourceView*	Texture;

	int VertexNum; 
	int IndexNum;

	bool IsTextureRealise;

};
//----------------------------------------------------------------------------- базовый класс дл€ рисовани€ всех обьектов в 2D

#endif