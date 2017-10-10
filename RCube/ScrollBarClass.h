#pragma once
#include "stdafx.h"

#ifndef _SCROLLBARCLASS_H_
#define _SCROLLBARCLASS_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif


#include "SquareObjectClass.h"
#include "FPSTimers.h"
#include <dinput.h>
//#include <WICTextureLoader.h>

class ScrollBarClass : public SquareObjectClass {

private:

	const float MIN_TRAVELER_SIZE = 20.0f; // � �������� 20

	// ������
	SquareObjectClass *MinButton;
	SquareObjectClass *MaxButton;
	SquareObjectClass *Traveller;

	Vertex_FlatObject *BodyVertexes;

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

	// ��������� �������� ���������� ���������
	float MinValue;		// ����������� ��������
	float MaxValue;		// ������������ ��������
	float CurrentValue; // ������� ��������
	float StepValue;	// ��� � ������� ������������� ��������
	
//	int ClickScroll;	// ������� �������� �������������� ��� ������� �� ���� ScrollBar

	float MiddleBodyPos;	// �������� ����, ��� ������� ���������� ������ ������������ ������ ScrollBox

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
	

//	bool StaticTravelerWidth; // ���������� ������ ��������

	// ����������� ������� ������ � ����� ����� ������ �� ���� ���������� ����� ���������� � ���������
	// � ������ ��������� �������� Min - Max � ���� ScrollBox
	float Ratio;		

	// �������� ����������� �������� ������������ ������ ���������
	float UpSideDownValue();

	// ���������� ��������� ��� ���������
	void SetScrollBarBodyParam();
	
	// �������� ������� �������� � ������������ � �������� ����� �� ���� ScrollBox
	void ChangeTravelerPositionByValue( float &value );

	// �������� ������� �������� �� ��������� �������� ������
	void ChangeTravelerPositionByScroll( long XX );

	// ������������� �������� �� ����������� ������������ �� �������� CurrentValue
	void BodyClickToValue( POINT &Position );

	// �������� ������ � ������ ��������
	// ���� �������� ��� ������ : ������, ������ ������, � TempData z,w = 0.0f , �� ���� ������� ��������
	void GetTextureParam( ID3D11ShaderResourceView * Texture, XMFLOAT4& Dimention );

	// ��������� ������� �� ��������
	bool TravellerPreseed( DXINPUTSTRUCT& InputClass );

	XMFLOAT4 ScreenCoords;
	XMFLOAT4 FormCoord;

	// ���� ������ �������� ��� �����������
	bool Enabled;

public:
	// �������� ������� �������� � ����� ��������
	void SetValue( float value );
	
	// �������� �� ������ �� �������� ���������
	bool MinMixButtonsShow;
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
		XMFLOAT4& _ScreenCoords,
		XMFLOAT4& _FormCoord,
		KFScrollBar_Elements& TempScrollBarData
		);

	void Draw();
	
	// ���������� ����� �������� � ������� ���� ���������
	int Frame( DXINPUTSTRUCT& InputClass, FPSTimers& fpstimers, bool &ObjectBUSY );

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

	float GetCurrentValue();

	bool CheckSign( float Val );

	float GetValuesDiaposone();

	// ��������� ��� ��������� ����������� � ��������
	void SetEnable ( bool Value );
};

#endif