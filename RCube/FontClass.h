#pragma once
#include "stdafx.h"

#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#include "D3DGlobalContext.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <DWrite.h>
#include <D2D1.h>
#include <vector>
#include <DirectXTex.h>
// ��� ��������������� ��������� ������� �� �������� ��� MAP
#include "SquareObjectClass.h"
#include "KFResourceManager.h"
//#include <afx.h>


using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: FontClass
////////////////////////////////////////////////////////////////////////////////

class FontClass
{
public:
	FontClass();
	FontClass(const FontClass&);
   ~FontClass();

// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------

	bool Initialize(D3DGlobalContext* D3DGC, FontOnTextureData *FOTData1, KFResourceManager *ResManager );
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();
//	void SetTexture(ID3D11ShaderResourceView* Texture);

	void BuildVertexArray(void*, char*, float&, float&, float&);

	// ������ ����� ������� TTF �� ��������
	bool RenderFontOnTexture ( ID3D11ShaderResourceView* textureForDraw, WCHAR* FontNameFullPath, WCHAR* text, FontOnTextureData *FOTData );

	bool CreateTextureFontFromFontFile( WCHAR* FontNameFullPath, FontOnTextureData *FOTData, bool fontTextureOut, bool fontTextDescOut );

	UINT GetFontHeight ();

	int GetFontHeightInPixels();

private:

	const WCHAR *Error = L"FontClass Error";

	// ���������� float ��� �������� < 228
   float Round ( float );
   // ���������� float ������ ��������
   float RoundToByte ( float );

	UINT PackHelper(float vectorX, float vectorY, float vectorZ, float vectorW);
	UINT PackUNorm(float bitmask, float value);

	UINT FontClass::R8G8B8A8toB8G8R8A8(UINT Value);
   
    UINT ColourMixing ( UINT Background, UINT Foreground );

	void UINToXMFloat4 ( UINT &Value, XMFLOAT4 &Result );

	float ClampAndRound(float value, float min, float max);

	void GCharWidth(wchar_t c);
	
	// ������������ ������ �������� �� ������� �������� �����
	void CalcFTextureDimention( UINT FonColour );

	// ��������� ������� �������� ����� ��������� ��������
	void CorrectSymbolsDim( float OutlineSize, int NewTextureWidth );

	// �������� �������� � ������� �� ������ ��������
	bool CopyTextTextureOnTexture ( ID3D11ShaderResourceView* textureForDraw );
	bool CopyTextTextureOnTexture_OLD ( ID3D11ShaderResourceView* textureForDraw );

struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x,y,z), texCoord(u, v){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

// �������� ������� ������� � ��������
	struct _Symbol_Dim 
	{
		 unsigned int SymbolPosInSymbolMassive;
			  wchar_t Code;
				float Symbol_Width;
				float Symbol_Heigth;
// ���������� ������� � ��������
				float Start;
				float End;
				  int Start_Pixel_XPos;
				  int Start_Pixel_YPos;
				  int End_Pixel_XPos;
				  int End_Pixel_YPos;
				  int PointsAmount;
// ���� ��������������� ������ ������� � ������� ��� �������������
				 UINT Color;

// ������� ��� ������ � �������� ����� �� ����� ������ �����
			     bool FirstPoint;
// �������� ������� �������� � �������� ������� ���� ������� �� ��������( ������ ��������� ���� ����� )
	};

// ������� ������ ������� - ��� ������ �������
			   WCHAR* FontName;
			   WCHAR* Str;
	     _Symbol_Dim* Symbols;
				 bool Result;

				const float fontsize_def;		// ������ ������ �� ���������
				const float color_step;		// ��� � �������� ����� ( 0.1 �� ��������� )
public:
				float FontSize;			// ������ ������
				  int FontHeightInPixel;// ������ ������ � ��������  ( ����� ��� StringList � �.�. )
private:
// ���������� ASCII �������� ������� �� ���������� � ������
				 int SymbolAmount;		// ���������� �������� ��� ��������
		   const UINT SymbolAmount_def; // ���������� �������� �� ���������
				 UINT RealAmontInString;// �������� �� ����� ���� - ������� �������

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

			         HWND hwnd;
			      HRESULT hr;
//		   IDXGIAdapter1* Adapter;			// �� �������
		    ID3D11Device* d3d11Device;		// �� �������
		D3DGlobalContext* Local_D3DGC;
	   KFResourceManager* LocalResManager;
#if defined( DEBUG ) || defined( _DEBUG )
			 ID3D11Debug* DebugDevice;
#endif

     ID3D11DeviceContext* d3d11DevCon;		// �� �������
		 IDXGIKeyedMutex* keyedMutex11;
         IDXGIKeyedMutex* keyedMutex10;	
       ID2D1RenderTarget* D2DRenderTarget;
	     ID3D11Texture2D* sharedTex11;
		 ID3D11Texture2D* Created_textureBuf;
		 ID3D11Texture2D* FontTexture;
	   	  IDWriteFactory* DWriteFactory;
ID3D11ShaderResourceView* d2dTexture;
ID3D11ShaderResourceView* FontShaderRes;

			ID2D1Factory* D2DFactory;


};

#endif
