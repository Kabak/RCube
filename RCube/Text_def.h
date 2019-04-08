#pragma once
#include "stdafx.h"

#include <DWrite.h>
#include <d2d1.h>
#include <DirectXMath.h>

#ifndef _TEXTDEF_H_
#define _TEXTDEF_H_

using namespace std;
using namespace DirectX;

// ������������ ������ �������� ����������� - ������ ������
const UINT STRMAXLENGTH = 256;

// ������������ ������ ����� ��������� �����������
typedef enum SHOW_TEXT
{
	SHOW = 0, // ������ ����������� ��� �������
	SHOW_FPS = 1, // NOT IMPLEMENTED YET
	SHOW_GLOWING = 2, // ��������� ������
	SHOW_SCROLLING = 4, // ���������� ������ 
	SHOW_FROM_DIM = 8,	// Sentence appear
	HIDE_TO_DIM = 16,	// Sentence disappear to dim
} ShowMethod;

// ������������ ������ ����� �������� ����������� � ������
typedef enum HIDE_TEXT {
	HIDE = 0,			// ������ ��������� ��� �������
//	HIDE_TO_DIM = 1,	// disappear to dim

} HideMethod;


struct SENTENCE_INIT_DATA
{
	XMFLOAT4	Colour;	// ���� ����������� RGBA
	float	ScrollSpeed;// �������� ������ �����������

	int	MaxLength;	// ������������ ������ �����������
	int	FontType;	// ����� ������ �� ��������� ������������� ( ������ ������ � ������� �������� )
	int	ShowType;	// ��� ������ ��������������, ����������� �� ������ ( ������ ������� � ������� )
	int	HideType;	// ��� ������ ��������� � ������ ��� ��������� render = false ( ������ ������� � ������� )
	int PosX;		// ������� X �� ������
	int	PosY;		// ������� Y �� ������
	int	Level;		// 0 - ����� � ����; 1 - HUD; 2 - Menu; 3 - StringsList1
	bool Render;	// �������� �� ����������� �� ������. False = ����������� �� ������ � ������� ��������������

};


struct SentenceType : SENTENCE_INIT_DATA
{
	int	VertexBufferIndex;	// ������ ����������� ������ �����������
	XMFLOAT4	ColourTemp; // ��������� �������� �����
	XMFLOAT4	ColourAction;// ���������� ��� ������ � ������
	float	TempValShow;	// ��������� ���������� ��� ������ Show
	float	TempValHide;	// ��������� ���������� ��� ������ Hide
	float	TempValScroll;	// ��������� ���������� ��� ���������
	char	Text[STRMAXLENGTH];
	int		vertexCount;
	bool	ShowGoOn;	// �������� ���������
	bool	FromDim;	// 
//	bool	HideGoOn;	// �������� ����������� ���������
	bool	ScrollGoOn;	// ������������ ��� ��������� �����������
	UINT	Height;		// ������ ����������� � ��������
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


// ������������ ��� ��������� ������� �� ��������
struct RCube_Font_Style
{
	RCube_Font_Style ()
	{
		FillBrush		= nullptr;
		OutlineBrush	= nullptr;
		FontName		= nullptr;
	};

	~RCube_Font_Style ()
	{

		RCUBE_ARR_DELETE ( FontName );
		RCUBE_RELEASE ( FillBrush );
		RCUBE_RELEASE ( OutlineBrush );
	};

// https://msdn.microsoft.com/query/dev14.query?appId=Dev14IDEF1&l=EN-US&k=k(DWrite%2FDWRITE_FONT_FACE_TYPE_TRUETYPE);k(DWRITE_FONT_FACE_TYPE_TRUETYPE);k(DevLang-C%2B%2B);k(TargetOS-Windows)&rd=true
	DWRITE_FONT_FACE_TYPE FontFaceType;
	D2D1_ANTIALIAS_MODE AliasingMode;
	WCHAR* FontName;
	float  FontSize;			// ������ ������ ��� ��������� ( 20.0f / 72.0f )*96.0f
	ID2D1Brush* FillBrush;		// ����� ��� �������  �������
	ID2D1Brush* OutlineBrush;	// ����� ��� ��������� �������
	bool  Outline;				// ��������� ������ ������ ����� ?
	int  OutlineWidth;		// ������ ��������� ������ ��������
};

#endif