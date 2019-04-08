#pragma once
#include "stdafx.h"

#ifndef _SCROLLBARCLASS_H_
#define _SCROLLBARCLASS_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif


#include "FlatObjectClass.h"
#include "FPSTimers.h"
#include <dinput.h>
#include "DX11Buffers.h"
#include "D3DGlobalContext.h"

class ResourceManager;

class ScrollBarClass {

private:

	ResourceManager* ResManager;

	ID3D11ShaderResourceView *ButtonsTexture;
	ID3D11ShaderResourceView *MouseOnButtonTexture;
	ID3D11ShaderResourceView *ButtonPressTexture;
	ID3D11ShaderResourceView *NotEnalbledButtonTexture;
	ID3D11ShaderResourceView *MouseOnTravellerTexture;
	ID3D11ShaderResourceView *TravellerTexture;
	ID3D11ShaderResourceView *TravellerPressTexture;
	ID3D11ShaderResourceView *NotEnalbledTravellerTexture;
	ID3D11ShaderResourceView *BodyTexture;
	ID3D11ShaderResourceView *NotEnalbledBodyTexture;

	XMFLOAT4 ObjOriginalParam;
	XMFLOAT4 ObjParam;

	XMFLOAT4 Dummy;

	// ��������� �������� ���������� ���������
	float MinValue;		// ����������� ��������
	float MaxValue;		// ������������ ��������
	float CurrentValue; // ������� ��������
	float StepValue;	// ��� � ������� ������������� ��������
	
	float MiddleBodyPos;// �������� ����, ��� ������� ���������� ������ ������������ ������ ScrollBox

	// ����� ��� ������������ ������ ��������
	POINT MousePosXX;
	// ������ ���� �� ������ ScrollBar
	bool ButtonPressed;
	// �������� �����
	bool TravellerPressed;
	// ������� �� ����
	bool BodyPressed;

	// ��� �������� = true ����� ������ ����� ����� ����� ��������� ����� � ������� 
	// ��������� �� ������� Min � Max
	bool TimerStop;
	float Time;
	
	// �������� ����������� �������� ������������ ������ ���������
	float UpSideDownValue();

	// ���������� ��������� ��� ���������
	void UpdateBodyPos ();	// Calkulate body - vertex position on the screen
	
	// �������� ������� �������� � ������������ � �������� ����� �� ���� ScrollBox
	void ChangeTravelerPositionByValue( float &value );

	// �������� ������� �������� �� ��������� �������� ������
	void ChangeTravelerPositionByScroll( long XX );

	// ������������� �������� �� ����������� ������������ �� �������� CurrentValue
	void BodyClickToValue( POINT &Position );

	// ��������� ������� �� ��������
	bool TravellerPreseed( DXINPUTSTRUCT& InputClass );

//	XMFLOAT4 ScreenCoords;
	XMFLOAT4 FormCoord;

	// ���� ������ �������� ��� �����������
	bool Enabled;

public:
	int IndexMinButton;
	int IndexMaxButton;
	int IndexMinTraveler;
	int IndexBody;

	bool Changed;	// If any element was chenged last engine frame

	// ������
	FlatObjectClass *MinButton;
	FlatObjectClass *MaxButton;
	FlatObjectClass *Traveller;
	FlatObjectClass *Body;

	// �������� ������� �������� � ����� ��������
	void SetValue( float value );
	
	// �������� �� ������ �� �������� ���������
	bool MinMaxButtonsShow;
	// �������� ������������� �� ������������� ���������
//	bool Jumping;
	// �������������� ��� ������������ ��������
	bool Horizontal;
	// ���� ScrollBar ������������, �� ������ ����� ������� �������� - ������
	bool UpSideDown;


	// ���������� ���������� ��� �������� ���������� ���� �� �������
	long ABSoluteX;		// ���������� �������� ������� �� X
	long ABSoluteY;		// ���������� �������� ������� �� Y
	long ABSolute_Width; // ���������� ������
	long ABSolute_Height;// ���������� ������

	long MinABSoluteX;		// ���������� �������� ������� �� X
	long MinABSoluteY;		// ���������� �������� ������� �� Y
	long MinABSolute_Width; // ���������� ������
	long MinABSolute_Height;// ���������� ������

	long MaxABSoluteX;		// ���������� �������� ������� �� X
	long MaxABSoluteY;		// ���������� �������� ������� �� Y
	long MaxABSolute_Width; // ���������� ������
	long MaxABSolute_Height;// ���������� ������

	long TravelABSoluteX;		// ���������� �������� ������� �� X
	long TravelABSoluteY;		// ���������� �������� ������� �� Y
	long TravelABSolute_Width; // ���������� ������
	long TravelABSolute_Height;// ���������� ������

	ScrollBarClass();
	~ScrollBarClass();

	// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
	void* operator new(size_t i)
	{
		return _mm_malloc(i,16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}

	HRESULT Init( D3DGlobalContext* D3DGC,
		XMFLOAT4& _FormCoord,
		ScrollBar_Elements& TempScrollBarData,
		ResourceManager * _GlobalResourceManager
		);


	// ���������� ����� �������� � ������� ���� ���������
	bool Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY );

	void SetMouseOnButtonTexture(ID3D11ShaderResourceView * Texture);
	void SetButtonsPressTexture(ID3D11ShaderResourceView * Texture);
	void SetBarNotEnalbledButtonsTexture(ID3D11ShaderResourceView * Texture);
	void SetMouseOnTravellerTexture(ID3D11ShaderResourceView * Texture);
	void SetTravellerPressTexture(ID3D11ShaderResourceView * Texture);
	void SetTravellerNotEnalbledTexture(ID3D11ShaderResourceView * Texture);
	void SetBarNotEnalbledTexture(ID3D11ShaderResourceView * Texture);

	// ���������� ������ ������� ��� ���������������� ������� �� �������
	void UpdateABSElementSize();

	// ���������� ������ ������� ��� ���������������� ������� �� �������
	void UpdateABSElementPos();

	// ���������� ������ � ������� � ������� ��� ���������������� ������� �� �������
	void UpdateABSElementAll();

	void UpdateTravelerPos();

	void UpdateTravelerAll();

	// ��������� ������ � �������� � ������ ������� �� ��������� � ���� ScrollBox
	void SetButtonsToBody(bool _Horizontal, XMFLOAT4 &MinButton, XMFLOAT4 &MaxButton, XMFLOAT4 &TravellerButton );
	// Calculating Body position & and dimentions
	void CalculateBodyPosDim ( bool _Horizontal, XMFLOAT4 &Body, XMFLOAT4 &AnyButton );

	float GetCurrentValue();

	bool CheckSign( float Val );

	float GetValuesDiaposone();

	// ��������� ��� ��������� ����������� � ��������
	void SetEnable ( bool Value );
};

#endif