#pragma once
#include "stdafx.h"

#include <DWrite.h>
#include <d2d1.h>
#include <DirectXMath.h>

#ifndef _TEXTDEF_H_
#define _TEXTDEF_H_

using namespace std;
using namespace DirectX;

// Максимальный размер быстрого предложения - строки текста
const UINT STRMAXLENGTH = 256;

// Перечисление разных типов отрисовки предложений
typedef enum SHOW_TEXT {
	SHOW = 0, // просто отображение без эфектов
	SHOW_FPS = 1, // NOT IMPLEMENTED YET
	SHOW_GLOWING = 2, // моргающая строка
	SHOW_SCROLLING = 3, // движущаяся строка 
} ShowMethod;

// Перечисление разных типов убирания предложений с экрана
typedef enum HIDE_TEXT {
	HIDE = 0, // просто отключить без эфектов

} HideMethod;

struct SentenceType
{
	int	VertexBufferIndex;	// Индекс вертексного буферы предложения
	XMVECTOR	Colour;		// цвет и alpha
	XMFLOAT4	ColourTemp; // Временное хранение цвета
	XMFLOAT4	ColourAction;// переменная для работы с цветом
	float	TempValShow;// Временная переменная для работы Show
	float	TempValHide;// Временная переменная для работы Hide
	float	TempValScroll; // Временная переменная для скролинга
	float	ScrollSpeed; // Скорость сдвига предложения
	char	Text[STRMAXLENGTH];
	int	vertexCount;
	int	indexCount;
	int	MaxLength;	// максимальная длинна предложения
	int	FontType;	// номер шрифта из созданных пользователем ( индекс шрифта в массиве векторов )
	int	ShowType;	// как именно отрисовывается, анимируется на экране ( разные шейдеры и функции )
	int	HideType;	// как именно убирается с экрана при установке render = false ( разные шейдеры и функции )
	int	PosX;		// позиция X на экране
	int	PosY;		// позиция Y на экране
	int	Level;		// 0 - текст в игре; 1 - HUD; 2 - Menu
	bool	Render;		// рисовать ли предложение на экране. False = исключается из списка в массиве отрисовываемых
	bool	ShowGoOn;	// Анимация действует
	bool	HideGoOn;	// Убирание предложения действует
	bool	ScrollGoOn;	// Используется для скролинга предложения
	UINT	Height;		// Высота предложения в пикселях
};

struct SENTENCE_INIT_DATA
{
	XMFLOAT4	Colour;		// цвет предложения RGBA
	float	ScrollSpeed; // Скорость сдвига предложения

	int	MaxLength;	// максимальная длинна предложения
	int	FontType;	// номер шрифта из созданных пользователем ( индекс шрифта в массиве векторов )
	int	ShowType;	// как именно отрисовывается, анимируется на экране ( разные шейдеры и функции )
	int	HideType;	// как именно убирается с экрана при установке render = false ( разные шейдеры и функции )
	int PosX;		// позиция X на экране
	int	PosY;		// позиция Y на экране
	int	Level;		// 0 - текст в игре; 1 - HUD; 2 - Menu; 3 - StringsList1
   bool	Render;		// рисовать ли предложение на экране. False = исключается из списка в массиве отрисовываемых

};

// описание каждого символа в текстуре
struct _Symbol_Dim
{
	unsigned int SymbolPosInSymbolMassive;
	wchar_t Code;
	float Symbol_Width;
	float Symbol_Heigth;
	// Координаты символа в текстуре
	float Start;
	float End;
	int Start_Pixel_XPos;
	int Start_Pixel_YPos;
	int End_Pixel_XPos;
	int End_Pixel_YPos;
	int PointsAmount;
	// цвет соответствующий номеру символа в массиве при распознавании
	UINT Color;

	// Признак что символ в текстуре найдён по цвету первой точки
	bool FirstPoint;
	// Реальные размеры текстуры в пределах которых есть пиксели от символов( начало координат лево вверх )
};


// Используется для рисования шрифтом на текстуре
struct FontOnTextureData
{
	FontOnTextureData ()
	{
		//			D2D1::ColorF ( 0.0f, 0.0f, 0.0f, 0.0f );
	};

	// https://msdn.microsoft.com/query/dev14.query?appId=Dev14IDEF1&l=EN-US&k=k(DWrite%2FDWRITE_FONT_FACE_TYPE_TRUETYPE);k(DWRITE_FONT_FACE_TYPE_TRUETYPE);k(DevLang-C%2B%2B);k(TargetOS-Windows)&rd=true
	DWRITE_FONT_FACE_TYPE FontFaceType;
	D2D1_ANTIALIAS_MODE AliasingMode;
	WCHAR* FontName;
	float  FontSize;		// Размер шрифта для рисования ( 20.0f / 72.0f )*96.0f
	ID2D1Brush* FillBrush;	// Кисть для заливки  символа
	ID2D1Brush* OutlineBrush;	// Кисть для окантовки символа
	bool  Outline;		// Окантовка вокруг текста нужна ?
	float  OutlineWidth;	// Ширина окантовки вокруг символов
							//			XMFLOAT4 GradientPos;	// x,y - start point  z,w - end point 

};
#endif