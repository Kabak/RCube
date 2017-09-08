#pragma once
#include "stdafx.h"


#ifndef _FONTCUSTOMFROMFILECLASS_H_
#define _FONTCUSTOMFROMFILECLASS_H_

#include <vector>
#include "DirectXMath.h"
#include <DWrite.h>
#include <D2D1.h>
#include <DirectXTex.h>


using namespace std;
using namespace DirectX;

class FontCustomFromFileClass
{

public:
	FontCustomFromFileClass();
   ~FontCustomFromFileClass();

	bool Initialize(IDWriteFactory*	DWriteFactory, const WCHAR* FontsPath, WCHAR**FontNames, int amount );
	void ShutDown();

private:
	HRESULT hr;
//	IDWriteFontFile*					pFontFile;
//	IDWriteFontFace*					pFontFace;
//	ID2D1PathGeometry*					pPathGeometry;
//	ID2D1GeometrySink*					pSink;
//	ID2D1Factory*						pD2DFactory;
	IDWriteFactory*						DWriteFactory;
//	IDWriteTextFormat*					TextFormat;
				WCHAR*					FontsPath;
				WCHAR*					FontName;
	vector < WCHAR* >					FontNames;
	vector < IDWriteFontFile*>			FontsFiles;
	vector < IDWriteFontFace*>			FontsFaces;
	vector < ID2D1PathGeometry*>		FontsPathGeometrys;
	vector < ID2D1GeometrySink*>		FontsSinks;

};
#endif