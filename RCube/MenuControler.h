#pragma once
#include "stdafx.h"

#ifndef _MENUCONTROLER_H_
#define _MENUCONTROLER_H_

#include "D3DGlobalContext.h"

#include "FPSTimers.h"
#include "InputClass.h"
#include "Menu.h"
#include "ResourceManager.h"

using namespace std;

class MenuControllerClass : public Menu {

private :

	D3DGlobalContext* McD3DGC;
	ResourceManager * GlobalResourceManager;

	InputClass* GlobalInput;

	// ������� � ������� ���� ���� ����
	//	x - X � % , y - Y � % , Width � % � ������������� ������� ��������,
	// Height � % � ������������� ������� ��������
	XMFLOAT4 BackGroundCoord;	// ������� ���� 

	// ������� ���� �� ����� ��������
//	int g_Width;
//	int g_Heigth;

	// ������ ��� ����������� ������� � Edit
	bool CursorOn_Off;

	float CursorTime;

	// �������� �� ������� ������� ����� � ������ ��� ����� FLOAT
	bool PointSet ( char* );
		
	bool KeyPressed;	// ��� ���� ����� ������ ����� ���� ��������� � ��������� �� ������ �������
	int TempKeyCode;

	// �������� ������ ���������� ��������� Edit
	// ����� ������ ��� ����, ����� ��������� ������ � ���������� �������� Edit, ���� ��� ������� ���������� ������
	int LastActiveButtonIndex;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++     Buttons functions     ++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	// ������ �������� Edit �� �����
	void ChangeActiveEdit( int ActiveNumber );

	// ��������� ������ � Edit
	void SwithOFFCursor ( KFButton* Button );

// -----------------------------------------------------------------------------------------
// ------------------------------     Buttons functions     --------------------------------
// -----------------------------------------------------------------------------------------

	// ���� ��� ���������� ����������� � true , ������ �� ����� �� ����������� � ���� ���� ������ ������ � �� ��������
	// ������ �� ������� ��������� ������� ������ ���������� ���� ������� � true � ������ �������� ��� ����� ����� ���������
	// ����� ����� ������� �� �����-���� ������� ����� �� �������� ������� �� ������ ��������� ���� ��� ��������� �������� �������
	// �� ������ � ������� ������ ���� ( ����� ������ ������� �� ������������ �������, ��� �� �� ������ ����

	// ��������������� ��� ����, ����� ���� ������������ ��������������� ������ ��� ����
	bool KeyDefineExpected;

	void GetLabelBindKeyText();

	void UpdateObjectText( KFButton* TempButton, char* Str );	// ��������� ����� ������ ��������� ���� � ����� �������� �� Y


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vector <MenuItem*> MenuItemsList;
	vector <GroupItem*> ItemsGroups;
	int FindMinXYPosItem ( vector <MenuItem*> &MenuItemsList, vector <GroupItem*> &ItemsGroups );
	void PushIntoGroup ( int GroupIndex, int MenuItemIndex );
	void UpdateGroupDim ( int GroupIndex, int NewItemIndex );
	void CompareItems ( MenuItem* One, MenuItem* GroupedItem, bool &Direction, bool &Identical );
	bool FindItemBetweenItems ( GroupItem* Group, MenuItem* CheckItem, bool &Direction ); // return true if its some item between One & Two items
	void CalculateMenuObjectsPositions ( vector <MenuItem*> &MenuItemsList, vector <GroupItem*> &ItemsGroups );
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

public :

	// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

	 MenuControllerClass();
	~MenuControllerClass();

	// _Animation - ��������� �� �������� � ����
	// BackGroundCoord - ���������� ������� ���� � ������� �� ������
	HRESULT Init(D3DGlobalContext* D3DGC,
		KFButton_Elements ButtonsData[], int NumOfButtons, // �������� �������� ������ � �� ���������� � ������� ��������
		ScrollBar_Elements ScrollBarData[], int NumOfScrollbars,// �������� �������� ScrollBars � �� ���������� � ������� ��������
		StringsList_Elements StringsLists[], int NumOfStringsList,// StringsList � �� ����������
		ColorPicker_Elements ColorPicker[], int NumOfColorPickers,	// ColorPickers
		KF2DTextureAnimation *_Animation,			// ��������� �� �������� � ����, ���� NULL, �� ��� ��������
		XMFLOAT4 BackGroundCoord,					// ���������� ������� ���� � ������� �� ������
		ID3D11ShaderResourceView* BackgroundTexture,// ��������� �� �������� ����
		ResourceManager * GlobalResManager			// ��������� �� ����� ��������� ������� � ������� ��������
		);

	HRESULT Frame( InputClass* InputClass, int UTF16_KeyCODE, FPSTimers& fpstimers);

	// ��������� UTF16 ���� ������� ������� � ASCII Cyrillic char
	char UTF16toCHAR ( int UTF16_KeyPressed );

	// ��������� UTF16 ���� ������� ������� � ASCII Integer
	char UTF16toINT ( int UTF16_KeyPressed );

	// ��������� UTF16 ���� ������� ������� � ASCII Float
	char UTF16toFLOAT ( int UTF16_KeyPressed );

	// ����������� ���� �� �������  ( ���������� ������ �������� ���� )
	void SetMenuPos (XMFLOAT2& Pos );

	// �������� ������� ���� � % � ��� �����������
	// ������ ������ � ��������� � ������������� �������
	void SetMenuSize ( float Percent );

	// �������� ������ � ��������� ���� ��������� ����
	void UpdateMenuElementsAll ();

	// ������� ���� �� ����� ��� ��� ���� �������
	void SetMenuOrigin ();

	// �������� ������� ���� ��������� ����
	void UpdateMenuElementsPos();

	void AddAnyMenuEffect(int Index);

	void SetMenuActive(bool Value);

	int BUSY_ObjectIndex;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++     Buttons functions     ++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	// �������� ��� ��������� Button
	void SetButtonEnable( int ButtonIndex, bool value );
	
	// ���������� ������ �� ������. ���� ������ �������������� ������ Button ������� ���������� false
	bool GetButtonState( int ButtonIndex );	// For CHECKBOX will be returned Checked  For other types will be returned State.IsPress
	bool GetEditFinished ( int ButtonIndex ); // true if Edit was finished in that engine Frame
	bool GetButtonChanged ( int ButtonIndex ); // For CHECKBOXes ONLY. Will be returned true if was changed last engine Frame
	bool GetScrollBarChanged ( int ScrBarIndex ); // For ScrollBars was changed 
	// ���������� ������� �������� �� ����� 
	void SetButtonPos( int buttonIndex, float x, float y );

	// ���������� ������� �������� �� ����� � ��������� �� ��������� � ������������ ������� ����� �������� �� �����
	void SetButtonPos( int ButtonIndex, float Percent );

	void SetButtonSize( int ButtonIndex, float width, float heigth );

	// ���������� ������ �������� �� ����� � �������� �� ��������� � ������������� ������� ����� �������� �� �����
	void SetButtonSize( int ButtonIndex, float Percent );

	// ���������� ����� ����������� ������
	void SetButtonParam( int ButtonIndex, XMFLOAT4& Param );

	// ���������� ������ � ��������� ��������� ������� ���������� ��� � �������� �� �����.
	void SetButtonToOrigin( int ButtonIndex );

	// �����-�� ������� ��������� ��� ������ ���� ������ ( �� ����� ����� )
	void ClearButtonState( int ButtonIndex );
	
	// ��������� � ����� Label ��� Edit ����� �� ����������� � �������� �����������
	void PutLabelsInsideButtons();

	// ��������� � ����� Label ��� Edit ����� �� ������������ � ������� �����������
	void PutLabelInsideButton( int ButtonIndex );

	void SetButtonAnimation( int NumOfVerses, int NumofColom, ID3D11ShaderResourceView * KF2DTextureFileName,
							 int IndexOfButton, int FramesSpead );

	float GetButtonTextAsFloat( int ButtonIndex );
	  int GetButtonTextAsInt ( int ButtonIndex );

XMFLOAT4 GetButtonColour ( int ButtonIndex );



	void SetButtonText( char* Text );

// -----------------------------------------------------------------------------------------
// ------------------------------     Buttons functions     --------------------------------
// -----------------------------------------------------------------------------------------

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++     ScrollBar functions     +++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	// �������� ��� ��������� ScrollBar
	void SetScrollBarEnable( int ScrollBarIndex, bool value );

	// ���������� ������� �������� ScrollBar.  ���� ������ �������������� ������ ScrollBar, ������� ���������� -666.66666666f;
	float GetScrollBarValue( int ScrBarNumber );

	bool SetScrollBarValue( int ScrBarNumber, float &Value );

// -----------------------------------------------------------------------------------------
// -----------------------------     ScrollBar functions     -------------------------------
// -----------------------------------------------------------------------------------------
};

#endif 