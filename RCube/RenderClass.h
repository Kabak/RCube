#pragma once
#include "stdafx.h"

#ifndef _RENDER_CLASS_H_
#define _RENDER_CLASS_H_

#ifdef RCube_DX11
#include <d3d11.h>
#include "Buffers_def.h"
#include "DX11Buffers.h"
#include "D3DGlobalContext.h"
#include "ResourceManager.h"
#include "D3DClass.h"
#include "TextClass.h"

class RenderClass
{
public:
	RenderClass ( D3DGlobalContext* GlobalContext, ResourceManager* _ResManager, D3DClass* m_D3D, TextClass* _text);
   ~RenderClass ();

    void RenderScene ();

	void RenderMenu ( int Index );

	void RenderFlatObject ( Flat_Obj_Buffers* FlatObject );

	void Render3DObject ();

// + Font Works

	// ������ ����� ������� TTF �� ��������
	bool RenderFontOnTexture ( ID3D11ShaderResourceView* textureForDraw,
		WCHAR* FontNameFullPath,
		WCHAR* text,
		FontOnTextureData *FOTData
		);

	// �������� �������� � ������� �� ������ ��������
	bool CopyTextTextureOnTexture ( ID3D11ShaderResourceView* textureForDraw,
		Flat_Obj_Buffers* Source,
		Flat_Obj_Buffers* Dest );

	UINT PackHelper ( float vectorX, float vectorY, float vectorZ, float vectorW );
	UINT PackUNorm ( float bitmask, float value );

	float ClampAndRound ( float value, float min, float max );

	// ���������� float ��� �������� < 228
	float Round ( float );
	// ���������� float ������ ��������
	float RoundToByte ( float );

	// ������������ ������ �������� �� ������� �������� �����
	void CalcFTextureDimention ( UINT FonColour );


// - Font Works

private:
	HRESULT hr;
	const WCHAR *Error = L"Render Error";

	ResourceManager* ResManager;
	D3DGlobalContext* Local_D3DGC;
	D3DClass* Local_D3D;
	TextClass* Global_text;

// + Font Works
	IDWriteFactory* DWriteFactory;
	ID2D1Factory* D2DFactory;
	ID2D1RenderTarget* D2DRenderTarget;
	IDXGIKeyedMutex* keyedMutex11;
	IDXGIKeyedMutex* keyedMutex10;
	ID3D11Texture2D* sharedTex11;
	ID3D11Texture2D* Created_textureBuf;
	ID3D11Texture2D* FontTexture;
	ID3D11ShaderResourceView* d2dTexture;
	ID3D11ShaderResourceView* FontShaderRes;

	// ����� �������� ������ �������� (������� �� ������� ������) ��� ���������� ��������� 
	int Width;
	int Height;
	// ������ �������� �� ������� �������� ����� ( ����������� ����� �������� ������ ����� ) ��� ��������� ������� �� ��������
	int Top;
	int Left;
	int Bottom;
	int Right;
	int FTextureWidth;
	int FTextureHeight;
// - Font Works
};


#endif //RCube_DX11




#endif
