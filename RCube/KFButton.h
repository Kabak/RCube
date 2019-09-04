#pragma once
#include "stdafx.h"

#ifndef _KFBUTTON_H_
#define _KFBUTTON_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif

#include <dinput.h>
#include "FlatObjectClass.h"
#include "D3DGlobalContext.h"
#include "DX11Buffers.h"
				
class ResourceManager;

struct ButtonState{
	ButtonState () { IsPress = false; IsMouseOnButton = false; }

	bool	IsPress;		// Button was pressed last engine Frame
	bool    IsMouseOnButton;// Mouse was on button body last engine Frame
};

class KFButton : public AboutMenuElement{

private :
		      ResourceManager* ResManager;

				  ButtonState   g_State ;

						 HWND   g_hwnd;
	ID3D11ShaderResourceView*	OsnButtontexture;
	ID3D11ShaderResourceView*	IsButonPressTexture;
	ID3D11ShaderResourceView*	IsMouseOnButtonTexture;
	ID3D11ShaderResourceView*	NotEnalbledTexture;
	
	int BodyBufferIndex;

	void ConstructBody ( XMFLOAT4 &BodyPos ); // Set the button on correct menu place

	XMFLOAT4 Colour;	// Color if the button should draw like color square

	// ��� ��������, ��� ���� � ���������� ����� ���� �� ���������� �������� ����
	// �������� : ���� ������ � ��������� - CheckBox, �� ��������� ���� ����� �� �������. ����� Checked �� ���������
	// ��� ������ ���� ������� Button : ��������� ����� ����� �� ��� �� ������� �� ������� ������ ������ ����, ����� ������� ������� �� ���������.
	int NeedReleaseOnObject; 

	// ���������� ��������� ��� ���������
//	void UpdateBodyPos ();	// Calkulate body - vertex position on the screen
//	void ConstuctBoby ();

	// � �������� �������� ��� �����������
	bool Enabled;

public :

	// if CHECKBOX
	bool Checked;		// �������������� � ������� ���������
	bool Changed;		// true for CHECKBOX if changed this frame 
	// if EDIT
	bool ActiveAsEdit;	// ���� ���������� Edit ������� ��� ����� ������. ��� ����������� ������� ( ���������� / �� ���������� )
	bool CursorOn;		// �����������, ���� ������ ��� ������������ ( ����� �� ��������� ������ ��� ����������� )
	bool EditFinished;  // Set true if user press Enter after Editing EDIT type  or pressed key in LABEL
	// if COLOR
	bool SetColour;		// = true If Button is COLOR type and was pressed for colorpicking.  If pressed once again Setcolour = false

	int ObjectType;		// ��� ������� ������� ����� �������� ��������� ������ BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3, COLOR = 4

	int EditType;		// ANYSYMBOL = 1, AS_INTEGER = 2, AS_FLOAT = 3

	// ��� Label
	bool WaitingForKeyPress;// ���������� ��� Label ��� ������� �� ������� ( ��� �������� ������ � ��������� � ������ )
	 int VirtualKeyIndex;	// �������� � ����������� �������� � ����
    bool SecondSlot;		// ���� ��� ����������� ������ ���� true, �� �������� �� 2-�� ����� ( ��� ������� ������ ��� ������� )

	// ��������� ������
	XMFLOAT4 ScreenCoords;
	// ��������� ������������ �����
	XMFLOAT4 FormCoord;

	XMFLOAT4 ObjOriginalParam;
	XMFLOAT4 ObjParam;

	FlatObjectClass *Body;

	// ���������� ���������� ��� �������� ���������� ���� �� �������
	long ABSoluteX;		// ���������� �������� ������� �� X
	long ABSoluteY;		// ���������� �������� ������� �� Y
	long ABSolute_Width; // ���������� ������
	long ABSolute_Height;// ���������� ������
	
	int SentenceIndex;// ������ ������ ������� ������� � TextClass � ������������� � ����� ����������.  
					  // ���� -1, �� ������ ����������� ( -1 �������� ������ ��� Button � CheckBox. ��� Label � Edit ����� ������ ���������� )

	 KFButton() ;
	~KFButton() ;

		// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------
	HRESULT Init(
		D3DGlobalContext* D3DGC,
				XMFLOAT4& ScreenCoords,
				XMFLOAT4& FormCoord,
	   KFButton_Elements& ButtonInitData,
//		Flat_Obj_Buffers* _Buffers,
		ResourceManager * _GlobalResourceManager
		);


	// ���������� true ���� ��������� ����� ��� ��������� ������ �� Edit �� �����
	// ����� ��� ����, ����� ������ ����� ������ � �������� Edit ��� �����,
	// � ��� ������� ��� ����� � ���������� ������ ���� Edit �� �����.
	// ���� �������� Edit �� ������� �� ������������ false
	// ObjectBUSY - ��� ������������ ������ �� ����� �� �����-���� ������� � ���������� ����
	bool Frame( DXINPUTSTRUCT& InputClass, bool &ObjectBUSY);

	void SetIfButtonPressTexture(ID3D11ShaderResourceView* Texture);

	void SetIfButtonNotEnalbledTexture(ID3D11ShaderResourceView* Texture);

	void SetIfMouseOnButtonTexture(ID3D11ShaderResourceView* Texture);

	void SetOsnTexture(ID3D11ShaderResourceView* Texture);

	void GetButtonState(ButtonState& State);

	void ClearButtonState();

	// ��������� ������ � ��������� Button �� �����
	void ResetButtonParam();

	// ���������� ������ ������� ��� ���������������� ������� �� �������
	void UpdateABSElementSize();

	// ���������� ������ ������� ��� ���������������� ������� �� �������
	void UpdateABSElementPos();

	// ���������� ������ � ������� � ������� ��� ���������������� ������� �� �������
	void UpdateABSElementAll();

	// ��������� ��� ��������� ����������� � ��������
	void SetEnable ( bool Value );

	void SetButtonColor ( XMFLOAT4 _Color );
	void SetButtonColorREF ( COLORREF Color );

	COLORREF GetButtonColorREF ();
	XMFLOAT4 GetButtonColor ();
};

#endif

