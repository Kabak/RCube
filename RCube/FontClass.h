#pragma once
#include "stdafx.h"

#include "Buffers_def.h"
#include "Text_def.h"

#ifndef _FontClass_H_
#define _FontClass_H_

using namespace std;
using namespace DirectX;

class FontClass
{
public:
	FontClass ( UINT _SymbolAmount, float _FontSize, WCHAR* Symbols );
   ~FontClass ();

	_Symbol_Dim* Symbols;

	WCHAR* Str;

	float FontSize;			// размер шрифта
	int FontHeightInPixel;// Высота шрифта в пикселях  ( нужно для StringList и т.д. )

	int GetFontHeightInPixels ();

	int FontTextureIndex;	// Номер текстуры символов шрифта в массиве текстур

//private:
	// количество ASCII символов которое мы используем в шрифте
		UINT SymbolAmount;		// количество символов без пробелов

  const float color_step;		// шаг в цветовой гамме ( 0.1 по умолчанию )


};

#endif

