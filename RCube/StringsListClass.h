#pragma once
#include "stdafx.h"

#ifndef _STRINGSLISTCLASS_H_
#define _STRINGSLISTCLASS_H_

#include "FlatObjectClass.h"
#include "FPSTimers.h"
#include "D3DGlobalContext.h"
#include <vector>

class ResourceManager;

class StringsListClass : public AboutMenuElement
{

public:
	
	StringsListClass();
   ~StringsListClass();

	void ShutDown ();

	bool ShowStringsList;

	bool Enabled;

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

	FlatObjectClass* StringListObj;

	// ����������� ����� ����������� �� ���������� ��������� - ����� � ������. 
	// �� �������� �������. �� ���� ����, ��� ������ �������� �� �������� ���� ��� ��� ����������� ���������� 
	HRESULT Init( D3DGlobalContext* D3DGC,
			   XMFLOAT4& _FormCoord,
			   StringsList_Elements& StringsListInit,
			   ResourceManager * _GlobalResourceManager
			 );
																	
	bool SaveTextureToPNG( ID3D11ShaderResourceView* );

	void UpdateABSElementAll();

	void UpdateBodyPos();	// Calkulate body - vertex position on the screen

	// ���������� ����� �������� � ������� ���� ���������
	int Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY );

	// �������� ��� ������ �����
	void ScrollUp();
	// �������� ��� ������ ����
	void ScrollDown();

	int SentencesIndex;			// ������ ������ ��������� ����� � ������ ���� ����� ������������� ��� ���������� StringsList, ����� ��� ������ ��� ����������� StringsList ���������� ������������

private:
	
	D3DGlobalContext* Local_D3DGC;
	ResourceManager* ResManager;

	XMFLOAT4 ObjOriginalParam;
	XMFLOAT4 ObjParam;

	int StringListObjBufferIndex;

	vector <char*> Strings;	// ������ ����� ���������� ���������

vector <int> SentencesResIndex;// ������ �������� ����������� ����������������� ��� ������ StringsList

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
	XMFLOAT4 TextureLeftTop;
	XMFLOAT4 TextureRightBottom;
	XMFLOAT4 TextureLeftBottom;
	XMFLOAT4 TextureRightTop;

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