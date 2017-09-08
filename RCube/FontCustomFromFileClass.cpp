#include "stdafx.h"
#include "FontCustomFromFileClass.h"

FontCustomFromFileClass::FontCustomFromFileClass()
{
//	pFontFile		= nullptr;
//	pFontFace		= nullptr;
//	pPathGeometry	= nullptr;
//	pSink			= nullptr;
	DWriteFactory	= nullptr;
	FontsPath		= new WCHAR[512];
	FontName		= new WCHAR[32];
	FontsPath[0]	= NULL;
	FontName[0]		= NULL;

}


FontCustomFromFileClass::~FontCustomFromFileClass()
{
	delete [] FontsPath;
	delete [] FontName;
}


void FontCustomFromFileClass::ShutDown()
{

}

bool FontCustomFromFileClass::Initialize(IDWriteFactory* Factory, const WCHAR* FontsPathinput, WCHAR* FontNames[], int amount)
{

	IDWriteFontFile*					pFontFile		= nullptr;
	IDWriteFontFace*					pFontFace		= nullptr;
	ID2D1PathGeometry*					pPathGeometry	= nullptr;
	ID2D1GeometrySink*					pSink			= nullptr;

	DWriteFactory = Factory;

	WCHAR* TempStr = new WCHAR [1024];
		TempStr[0] = NULL;

	// Если путь в инициализации не задан
	if ( FontsPathinput == NULL )
	{
		GetCurrentDirectory(512, FontsPath);
		// добавляем путь к шрифтам
		wcscat_s(FontsPath, 512, L"\\Fonts\\");
	}
	else
	{
		wcscat_s(FontsPath, 512, FontsPathinput);
	}

// Создаём всё что нужно для всех шрифтов
	for (int i = 0; i < amount; ++i)
	{
		wcscpy_s(TempStr, 1024, FontsPath);
		wcscat_s(TempStr, 1024, FontNames[i]);

		hr = DWriteFactory->CreateFontFileReference(TempStr, NULL, &pFontFile);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Create font file reference failed!", L"Error", 0);
			return false;
		}

		IDWriteFontFile* fontFileArray[] = { pFontFile };

		hr = DWriteFactory->CreateFontFace(DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, fontFileArray, 0, DWRITE_FONT_SIMULATIONS_NONE, &pFontFace);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Create font file face failed!", L"Error", 0);
			return false;
		}


		pFontFile->Release();
	}

	delete [] TempStr;

	return true;
}