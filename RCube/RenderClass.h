#pragma once
#include "stdafx.h"

#ifndef _RENDER_CLASS_H_
#define _RENDER_CLASS_H_

#ifdef RCube_DX11
#include <d3d11.h>
#include "Buffers_def.h"
#include "Text_def.h"
#include "DX11Buffers.h"
#include "D3DGlobalContext.h"
#include "ResourceManager.h"
#include "D3DClass.h"
#include "FontClass.h"
#include "FPSTimers.h"
#include "FrustumClass.h"

static WCHAR *CharStr = L"! \" # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � ";

class RenderClass
{
public:
	RenderClass ( D3DGlobalContext* GlobalContext, ResourceManager* _ResManager, D3DClass* m_D3D, FrustumClass* _Frustum);
   ~RenderClass ();

    void RenderScene ();

	void RenderMenu ( int Index );

	void RenderParticleSystem ( int Index );

	void RenderFlatObject ( Flat_Obj_Buffers* FlatObject );

	// ������ ��� ����������� ������� ���� � ������ � � ��� ������� Render
	void RenderText ( int Level );

	void Render3D_Object ( int ObjectIndex, bool Shadows );
//	void Render3D_Object_With_Light ( int ObjectIndex );

	void RenderCubeMap ( int ObjectIndex );

	void RenderTerrain ( int ObjectIndex, bool Shadows );	// ���� Shadows = 1 , �� ObjectIndex ���������� � ShadowMap

	void Init_ShadowMap_RasterizerState ( int DepthBias, float SlopeScaledDepthBias );	// ��� ����� ��� ������� ��������� ����

	void RenderSpotLightsSadowMaps ( std::vector <int>* SpotLightsWithShadowsIndexes );
	
	void DrawObjectUsingShadows ( XMVECTOR DrawPosition );

// + Font Works

	FontClass* Create_RCube_Font ( RCube_Font_Style* FOTData1, WCHAR* SymbolsString);
	RCube_Font_Style* Create_RCube_FontStyle ( Font_Param* Fparam );


	bool CreateTextureFontFromFontFile ( WCHAR* FontNameFullPath,
		RCube_Font_Style *FOTData,
		FontClass* NewFont,
		bool fontTextureOut,
		bool fontTextDescOut );

	// ������ ����� ������� TTF �� ��������
	bool Render_Font_OnTexture ( ID3D11ShaderResourceView* textureForDraw,
		WCHAR* FontNameFullPath,
		WCHAR* text,
		RCube_Font_Style *FOTData
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

	int CalkNoSpaceAmount ( const WCHAR* String );

	// ��������� ������� �������� ����� ��������� ��������
	void RenderClass::CorrectSymbolsDim ( int OutlineSize, int NewTextureWidth, UINT _SymbolAmount, _Symbol_Dim* _Symbols );

// - Font Works

	FPSTimers fpstimers;

private:
	HRESULT hr;
	const WCHAR *Error = L"RenderClass Error";

	ResourceManager* ResManager;
	D3DGlobalContext* Local_D3DGC;
	D3DClass* Local_D3D;
	FrustumClass* Local_Frustum;
//	TextClass* Global_text;

// + Font Works
	void RenderSentence ( SentenceType* );
	void ShowGlowing	( int Number );
	void ShowScrolling  ( int Number );
	void ShowFromDim	( int i );
	void HideToDim		( int i );
// - Font Works

	IDWriteFactory* DWriteFactory;
	ID2D1Factory* D2DFactory;
	ID2D1RenderTarget* D2DRenderTarget;
	IDXGIKeyedMutex* keyedMutex11;
	IDXGIKeyedMutex* keyedMutex10;
	ID3D11Texture2D* sharedTex11;
	ID3D11Texture2D* Created_textureBuf;
//	ID3D11Texture2D* FontTexture;
	ID3D11ShaderResourceView* d2dTexture;
	ID3D11ShaderResourceView* FontShaderRes;

	D3D11_DEPTH_STENCIL_VIEW_DESC DSD2;
	RCube_VecFloat234 LightPosition, LightTarget;
	XMVECTOR Up;

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
