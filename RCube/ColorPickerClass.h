#pragma once
#include "stdafx.h"
#include "wingdi.h"

#ifndef _COLORPICKERCLASS_H_
#define _COLORPICKERCLASS_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif


#include "FlatObjectClass.h"
#include "FPSTimers.h"
#include "D3DGlobalContext.h"

class ResourceManager;

class ColorPickerClass {

private:

	D3DGlobalContext* Local_D3DGC;
	ResourceManager* ResManager;
	ID3D11ShaderResourceView * PanthonTexture;

	int PanthonBufferIndex;
	int ColorPickerBufferIndex;
	int AlphaPickerBufferIndex;

	RCube_VecFloat234 PanthonColor;
	RCube_VecFloat234 ColorPickerColor;
	RCube_VecFloat234 CurrentColor;


	COLORREF CurrentColorCOLORREF;
	COLORREF PanthonSelectedColor;
	COLORREF ColorPickerColorCOLORREF;

	XMFLOAT4 ObjOriginalParam;
	XMFLOAT4 ObjParam;

	XMFLOAT4 Dummy;

	XMFLOAT4 ScreenCoords;		// Screen dementions in pixels X , Y
	XMFLOAT4 FormCoord;			// Menu body size & location on the screen

	 void RGBAtoCOLORREF ( XMFLOAT4 &Color, COLORREF &Output );
	 void COLORREFtoRGBA ( COLORREF Color, XMFLOAT4&RGBA );


	float PercentageValue;	// Value = Selector Position on the screen
							// used for Alpha & Panthon styles

	float Panthon_Alpha_Thickness; 

	float MiddleBodyPos;// �������� ����, ��� ������� ���������� ������ ������������ ������ ScrollBox

	void ConstructBody ( bool _Horizontal, XMFLOAT4 &ColorPickerBody, XMFLOAT4 &PanthonBody, XMFLOAT4 &AlphaBody, float WidthofElements );
	void CheckingColorPickerParam ();

	void GeneratePanthonVertexes ();
	void GenerateColorPickerVertexes ();
	void GenerateAlphaPickerVertexes ();

	// Move selector to clicked location + update Selected Color Value + Store ClickPosition
	void PanthonClickToValue ( POINT &Position );
	void ColorPickerClickToValue ( POINT &Position );
	void AlphaPickerClickToValue ( POINT &Position );

public:

	FlatObjectClass *Panthon;
	FlatObjectClass *ColorPicker;
	FlatObjectClass *AlphaPicker;

	HRESULT Init ( D3DGlobalContext* D3DGC,
				   XMFLOAT4& _ScreenCoords,
				   XMFLOAT4& _FormCoord,
				   ColorPicker_Elements& TempColorPickerData,
				   ResourceManager * _GlobalResourceManager
				);

	bool ColorPickerPressed;	// ColorPicker pressed
	bool PanthonPressed;		// ColorPicker pressed
	bool AlphaPickerPressed;	// ColorPicker pressed

	bool Enabled;		// ���� ������ �������� ��� �����������

	POINT PanthonLastClickPosition;	// Last click mouse position on the PanthonPicker surface
	POINT ColorPickerLastClickPosition;
	POINT AlphaPickerLastClickPosition;

	// �������������� ��� ������������ ��������
	bool Horizontal;

//	 int Style;	// enum COLOR_PICKER_STYLE

	// ���������� ���������� ��� �������� ���������� ���� �� �������
	long PanthonABSoluteX;		// ���������� �������� ������� �� X
	long PanthonABSoluteY;		// ���������� �������� ������� �� Y
	long PanthonABSolute_Width; // ���������� ������
	long PanthonABSolute_Height;// ���������� ������

	long ColorSelectorABSoluteX;		// ���������� �������� ������� �� X
	long ColorSelectorABSoluteY;		// ���������� �������� ������� �� Y
	long ColorSelectorABSolute_Width;	// ���������� ������
	long ColorSelectorABSolute_Height;	// ���������� ������

	long ColorPickerABSoluteX;		// ���������� �������� ������� �� X
	long ColorPickerABSoluteY;		// ���������� �������� ������� �� Y
	long ColorPickerABSolute_Width;	// ���������� ������
	long ColorPickerABSolute_Height;	// ���������� ������

	long AlphaPickerABSoluteX;		// ���������� �������� ������� �� X
	long AlphaPickerABSoluteY;		// ���������� �������� ������� �� Y
	long AlphaPickerABSolute_Width;	// ���������� ������
	long AlphaPickerABSolute_Height;	// ���������� ������



	ColorPickerClass ();
   ~ColorPickerClass ();

   // �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
   void* operator new( size_t i )
   {
	   return _mm_malloc ( i, 16 );
   }

   void operator delete( void* p )
   {
	   _mm_free ( p );
   }

   // ���������� ����� �������� � ������� ���� ���������
   int Frame ( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY );

   // ���������� ������ � ������� � ������� ��� ���������������� ������� �� �������
   void UpdateABSElementAll ();

   void SetColorPicker ( XMFLOAT4 Color );
   void SetColorPicker ( COLORREF Color );

   void SetAlphaPicker ( XMFLOAT4 Color );
   void SetAlphaPicker ( COLORREF Color );

   void ApplyAlpha ( XMFLOAT4 Color );
   void ApplyAlpha ( COLORREF Color );

   XMFLOAT4 GetSelectedColorFL4 ();
   COLORREF GetSelectedColorCOLREF ();


};

#endif