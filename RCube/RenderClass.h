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

static WCHAR *CharStr = L"! \" # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \\ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ А Б В Г Д Е Ж З И Й К Л М Н О П Р С Т У Ф Х Ц Ч Ш Щ Ъ Ы Ь Э Ю Я а б в г д е ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я Ё ё ";

class RenderClass
{
public:
	RenderClass ( D3DGlobalContext* GlobalContext, ResourceManager* _ResManager, D3DClass* m_D3D);
   ~RenderClass ();

    void RenderScene ();

	void RenderMenu ( int Index );

	void RenderFlatObject ( Flat_Obj_Buffers* FlatObject );

	// Рисует все предложения которые есть в списке и у них включен Render
	void RenderText ( int Level );

	void Render3D_Object ( int ObjectIndex );
	void Render3D_Object_With_Light ( int ObjectIndex );


// + Font Works

	FontClass* CreateRFont ( FontOnTextureData* FOTData1, WCHAR* SymbolsString);

	bool CreateTextureFontFromFontFile ( WCHAR* FontNameFullPath,
		FontOnTextureData *FOTData,
		FontClass* NewFont,
		bool fontTextureOut,
		bool fontTextDescOut );

	// Рисуем любым шрифтом TTF на текстуре
	bool RenderFontOnTexture ( ID3D11ShaderResourceView* textureForDraw,
		WCHAR* FontNameFullPath,
		WCHAR* text,
		FontOnTextureData *FOTData
		);

	// Копируем текстуру с текстом на нужную текстуру
	bool CopyTextTextureOnTexture ( ID3D11ShaderResourceView* textureForDraw,
		Flat_Obj_Buffers* Source,
		Flat_Obj_Buffers* Dest );

	UINT PackHelper ( float vectorX, float vectorY, float vectorZ, float vectorW );
	UINT PackUNorm ( float bitmask, float value );

	float ClampAndRound ( float value, float min, float max );

	// Округление float для значений < 228
	float Round ( float );
	// Округление float полный диапозон
	float RoundToByte ( float );

	// подсчитываем размер текстуры на которой написали текст
	void CalcFTextureDimention ( UINT FonColour );

	int CalkNoSpaceAmount ( const WCHAR* String );

	// Коррекция позиций символов после обрезания текстуры
	void RenderClass::CorrectSymbolsDim ( float OutlineSize, int NewTextureWidth, UINT _SymbolAmount, _Symbol_Dim* _Symbols );

// - Font Works

	FPSTimers fpstimers;

private:
	HRESULT hr;
	const WCHAR *Error = L"Render Error";

	ResourceManager* ResManager;
	D3DGlobalContext* Local_D3DGC;
	D3DClass* Local_D3D;
//	TextClass* Global_text;

// + Font Works
	void RenderSentence ( SentenceType* );
	void ShowGlowing ( int Number );
	void ShowScrolling ( int Number );
// - Font Works

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

	// Задаём условный размер текстуры (зависит от размера шрифта) для начального рисования 
	int Width;
	int Height;
	// размер текстуры на которой написали текст ( вычисляется чтобы обрезать лишние куски ) при рисовании текстом на текстуре
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
