#include "stdafx.h"
#include "FontClass.h"


FontClass::FontClass ( UINT _SymbolAmount, float _FontSize, WCHAR* SymbolsStr )
	: color_step ( 0.1f )
{
	SymbolAmount = _SymbolAmount; // ����� ������ ������� ������
//	RealAmontInString = SymbolAmount * 2; // ������� ������� ����� ������ �����
	FontSize = _FontSize;
	FontHeightInPixel = 0;

	// ��������� ������ ������ ������� ������� ����������� ��� ������������ ��������
	Symbols = new _Symbol_Dim[SymbolAmount];
	ZeroMemory ( Symbols, sizeof ( _Symbol_Dim ) * SymbolAmount );

	Str = SymbolsStr;

}


FontClass::~FontClass ()
{
	RCUBE_ARR_DELETE ( Symbols )
}


int FontClass::GetFontHeightInPixels ()
{
	return FontHeightInPixel;
}
