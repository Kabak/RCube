#pragma once
#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONTSHADERCLASS_H_
#define _FONTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
//#include "D3Dcompiler.h"
//#include <d3dx11async.h>
#include <fstream>
using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: FontShaderClass
////////////////////////////////////////////////////////////////////////////////
class FontShaderClass
{
private:
	struct ConstantBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

// We have a new structure type to match the PixleBuffer in the pixel shader. It contains just the pixel color of the text that will be rendered.

	struct PixelBufferType
	{
		XMVECTOR pixelColor;
	};

	XMVECTOR OldValue;

public:
	FontShaderClass();
	FontShaderClass(const FontShaderClass&);
	~FontShaderClass();
// Чтобы не выполнять это перед каждым предложением, 
// делаем отдельную функцию
	void SetFontLayoutSempler(ID3D11DeviceContext* deviceContext);

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

	bool Initialize( D3DGlobalContext* D3DGC );
	void Shutdown();
	void Render(ID3D11DeviceContext*, int,  ID3D11ShaderResourceView*, XMVECTOR&);

private:
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

private:
// The FontShaderClass has a constant buffer for the pixel color that will be used to render the text fonts with color.
	ID3D11Buffer* m_pixelBuffer;
};

#endif
