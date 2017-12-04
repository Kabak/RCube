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

	float FontSize;			// ������ ������
	int FontHeightInPixel;// ������ ������ � ��������  ( ����� ��� StringList � �.�. )

	int GetFontHeightInPixels ();

	int FontTextureIndex;	// ����� �������� �������� ������ � ������� �������

//private:
	// ���������� ASCII �������� ������� �� ���������� � ������
		UINT SymbolAmount;		// ���������� �������� ��� ��������

  const float color_step;		// ��� � �������� ����� ( 0.1 �� ��������� )


};

#endif

