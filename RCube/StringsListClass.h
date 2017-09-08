#pragma once
#include "stdafx.h"

#ifndef _STRINGSLISTCLASS_H_
#define _STRINGSLISTCLASS_H_

#include "SquareObjectClass.h"
#include "FPSTimers.h"
#include "FontClass.h"
#include "TextClass.h"
#include <vector>


class StringsListClass : public SquareObjectClass
{

public:
	
	StringsListClass();
   ~StringsListClass();

	void ShutDown ();

	bool ShowStringsList;

	bool Enabled;

	// ��������� ������
	XMFLOAT4 ScreenCoords;
	// ��������� ������������ �����
	XMFLOAT4 FormCoord;

	// ���������� ���������� ��� �������� ���������� ���� �� �������
	long ABSoluteX;		// ���������� �������� ������� �� X
	long ABSoluteY;		// ���������� �������� ������� �� Y
	long ABSolute_Width; // ���������� ������
	long ABSolute_Height;// ���������� ������

//	vector <ID3D11ShaderResourceView*> StringsList_SRV; // ������ ������� � ��������� ������� ��� ��������� ����� �� StringsList

	int StringsNumber;		// ���������� ����� � StringList

	int MaxVisibleIndex;	// ������� ����� �������� �� ������ ������

	// ����������� ����� ����������� �� ���������� ��������� - ����� � ������. 
	// �� �������� �������. �� ���� ����, ��� ������ �������� �� �������� ���� ��� ��� ����������� ���������� 
	HRESULT Init( D3DGlobalContext* D3DGC,
			   XMFLOAT4& _ScreenCoords,
			   XMFLOAT4& _FormCoord,
			   StringsList_Elements& StringsListInit,
			   TextClass* g_text
			 );
																	
	void Draw();

	bool SaveTextureToPNG( ID3D11ShaderResourceView* );

	void UpdateABSElementAll();

	void SetStringsListParam();

	// ���������� ����� �������� � ������� ���� ���������
	int Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY );

	// �������� ��� ������ �����
	void ScrollUp();
	// �������� ��� ������ ����
	void ScrollDown();

//	void InitDynamicStringsList( vector <char*> StringsList ); // ������������ StringsList

private:
	
	D3DGlobalContext* Local_D3DGC;
	TextClass* Global_text;

	vector <char*> Strings;	// ������ ����� ���������� ���������

vector <int> SentencesResIndex;// ������ �������� ����������� ����������������� ��� ������ StringsList

	int SentencesIndex;			// ������ ������ ��������� ����� � ������ ���� ����� ������������� ��� ���������� StringsList, ����� ��� ������ ��� ����������� StringsList ���������� ������������
	int FontIndex;				// ����� ������ ������� �������� �����
	int FontHeightInPixel;		// ������ �������� ������ � �������� ( ��� ������������� ��������� ������ ������� )

	int Strings_Amount_ToRender;// ������� ����� ����� ����� ��������

	int First_Visible_Index;	// ����� 1-� ������������ ������ � ����� ������ �����
	int Last_Visible_Index;		// ����� ��������� ������������ ������ � ����� ������ �����

	int StringHeigt;			// ������ ������ ������ � ��������
	int StringsSpacing;			// ���������� ����� ��������
  float Scaling;				// ��� �������� ������ � ���� StringsList

	int Scroll_Y_Pos;			// �������� ����� ������� ������� ����� ��� ���������. ������������ ������� �������� ��� ������
	int ScrollSpeed;			// �������� ���������
	int StringsHeigh;			// ������ ����� ������ � StringsKist

	int ObjParam_X;				// int ������ ������� ObjParam ��� ��������� ������ � �������� �� ������ � int
	int ObjParam_Y;
	int ObjParam_Z;
	int ObjParam_W;

   bool StringsListPressed;		// ����� ������ ���� ������ �� StringsList
    int Old_MouseZ;				// ������ �������� 

	int StringsMAXLength;		// ������������ ������ ����� ������

	XMFLOAT2 FirstStringOffset; // ����� ����� ������ ������������ ����

	// ���������� � �������� ��� ����������� �����
	XMFLOAT2 TextureLeftTop;
	XMFLOAT2 TextureRightBottom;
	XMFLOAT2 TextureLeftBottom;
	XMFLOAT2 TextureRightTop;

	XMFLOAT4 StrList_Frame_pos;			// ������� ���� � �������� ��� �� ������ ������ ������ ( ��� ��������� ����� ����� � �������� ���� )

	// ScrollDir = 1 - Up, = 0 - Down
	void ScrollStringsList( bool ScrollDir ); // �������� �� ��������� ������ � ���� ���������

	void UpdateVisibleSentence( bool ScrollDir );	// �������� ������� ������ � StringsList

	bool SetInitFrameData();	// ���������� ��������� �������� ��� ���� � ������� ����� ������������ ������ ( ������� �� ������ ����� � ����������� ����� �������� )

	void UpdateVisibleSentance();	// ��������� ������ ��� ���� ������� �� ������ �����������

	ID3D11RenderTargetView* Default_RTV;// �������� �����
	ID3D11RenderTargetView* StrList_RTV;// �������� ���� �������� StringsList ������� ����� ������������ � ������
  ID3D11ShaderResourceView* StrList_SRV;// ��� ���������� � ���� � ����������� ����� � ����
};

#endif