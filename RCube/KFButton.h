#pragma once
#include "stdafx.h"

#ifndef _KFBUTTON_H_
#define _KFBUTTON_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif

#include <dinput.h>
#include "FlatObjectClass.h"
#include "TextClass.h"
#include "D3DGlobalContext.h"
#include "DX11Buffers.h"
						
struct ButtonState{
	bool	IsPress;
	bool    IsMouseOnButton;
};

class KFButton : public FlatObjectClass{

private :
	
				  ButtonState   g_State ;

						 HWND   g_hwnd;
	ID3D11ShaderResourceView*	OsnButtontexture;
	ID3D11ShaderResourceView*	IsButonPressTexture;
	ID3D11ShaderResourceView*	IsMouseOnButtonTexture;
	ID3D11ShaderResourceView*	NotEnalbledTexture;
//		   ID3D11BlendState *   g_BlendState;
	
	// ��� CheckBox
	bool Checked;	// �������������� � ������� ���������

	// ��� ��������, ��� ���� � ���������� ����� ���� �� ���������� �������� ����
	// ���� ������ � ��������� - CheckBox, �� ��������� ���� ����� �� ������� ����� Checked �� ���������
	// ��� ������ ������� : ��������� ����� ����� �� ��� �� ������� �� ������� ������
	int NeedReleaseOnObject; 

	// ���������� ��������� ��� ���������
	void SetButtonParam();

	// � �������� �������� ��� �����������
	bool Enabled;

public :

	// ��� Edit
	bool ActiveAsEdit;	// ���� ���������� Edit ������� ��� ����� ������. ��� ����������� ������� ( ���������� / �� ���������� )
	bool CursorOn;		// �����������, ���� ������ ��� ������������ ( ����� �� ��������� ������ ��� ����������� )

	int ObjectType;		// ��� ������� ������� ����� �������� ��������� ������ BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3

	int EditType;		// ANYSYMBOL = 1, AS_INTEGER = 2, AS_FLOAT = 3

	// ��� Label
	bool WaitingForKeyPress;// ���������� ��� Label ��� ������� �� �������
	 int VirtualKeyIndex;	// �������� � ����������� �������� � ����
    bool SecondSlot;		// ���� ��� ����������� ������ tckb true, �� �������� �� 2-�� �����

	// ��������� ������
	XMFLOAT4 ScreenCoords;
	// ��������� ������������ �����
	XMFLOAT4 FormCoord;

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
		Flat_Obj_Buffers* _Buffers
		);

	HRESULT Draw();

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

};

#endif

