#include "stdafx.h"
// Now we will look at the new light class which is very simple. Its purpose is only to maintain the direction and color of lights.

////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _KFSCROLLBAR_H_
#define _KFSCROLLBAR_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif


#include "SquareObjectClass.h"
#include <dinput.h>
#include <WICTextureLoader.h>

class ScrollBarClass : public SquareObjectClass {
protected:
	int startScreenWidth, startScreenHeigth;
	KF2DObjClass * Button1, *Button2, *Traveller;
	InterfaceVertexType *TravellerVertexes;
	int TravellerLenght, g_ScreenWidth, g_ScreenHeight, MidleTraveller,
		TravellerTop, TravellerLeft, DrawingTravellerLenght, g_TravellerLeftInGlobalCord, g_TravellerTopInGlobalCord;
	int ChainlesBarTop, ChainlessBarBottom, ChainlessBarLeft, ChainlessBarRight; // эти нужны для глобальных кординат
	int ChainlesBarTop2, ChainlessBarBottom2, ChainlessBarLeft2, ChainlessBarRight2; // а эти для кординат в системе DirectX
	int g_HorOrVer;
	int StartWindowWidth, StartWindowHeigth;

	ID3D11ShaderResourceView * OsnButtonsTexture, *IsMouseOnButtonTexture, *IsButtonPressTexture, *IsBarNotEnalbledButtonsTexture
		, *IsMouseOnTravellerTexture, *OsnTravellerTexture, *IsTravellerPressTexture, *IsNotEnalbledTravellerTexture,
		*OsnBarTexture, *NotEnalbledBarTexture;
	ID3D11Device* g_Device;
	ID3D11DeviceContext * g_DeviceContext;
	int MouseXInWindow, MouseYInWindow;
	int g_TravellerLeft, g_TravellerTop;
	bool IsButtonShow;
	int CordInPixX;
	int CordInPixY;
	int Previostop, Previosleft, Previosbottom, Previosright;

	float previosTop, previosLeft;

	int RealPixHeigth, RealPixWidth;

	HRESULT SetPos();
	bool * ReSetsControl;
	bool IsMouseDownOnObjec;
	bool IsSomethingReSet;
	void openDraw(bool finalcont);
public:

	ScrollBarClass();
	~ScrollBarClass();
	int g_Indention;

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
	void* operator new(size_t i)
	{
		return _mm_malloc(i,16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}

	bool Enalbled;

	HRESULT Init(HWND hwnd, int m_screenWidth, int m_screenHeight, int WindowPosX, int WindowPosY, float positionX
		, float positionY, ID3D11ShaderResourceView * OsnTexture, ID3D11ShaderResourceView * TravellerTexture
		, ID3D11ShaderResourceView * ButtonsTexture, bool * ReSetsCont, ID3D11Resource * res, XMMATRIX & OrthoMatrix
		, D3DGlobalContext* D3DGC, XMMATRIX & BaseViewMatrix, ID3D10Blob * WorkBlob, int HorOrVer, int Indention, bool ButtonsShow 
		, int TravellerLength);
	HRESULT Draw();
	HRESULT Frame(DIMOUSESTATE2& MouseState);

	void SetIsMouseOnButtonTexture(ID3D11ShaderResourceView * Texture);
	void SetIsButtonsPressTexture(ID3D11ShaderResourceView * Texture);
	void SetIsBarNotEnalbledButtonsTexture(ID3D11ShaderResourceView * Texture);
	void SetTravellerMouseOnTexture(ID3D11ShaderResourceView * Texture);
	void SetTravellerPressTexture(ID3D11ShaderResourceView * Texture);
	void SetTravellerNotEnalbledTexture(ID3D11ShaderResourceView * Texture);
	void SetBarNotEnalbledTexture(ID3D11ShaderResourceView * Texture);

	void ResetWindowParam(int WindowPosX, int WindowPosY, int WindowWidth, int WindowHeigth);
	void ResetObjReadingState(int left, int rigth, int top, int bottom);
	int GetBarState();

};

#endif