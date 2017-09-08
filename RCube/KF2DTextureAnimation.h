#pragma once
#include "stdafx.h"

#include "SquareObjectClass.h"
#include "D3DGlobalContext.h"
#include "KFResourceManager.h"
#include "FPSTimers.h"

#ifndef _KF2DTextureANIMATION_H_
#define _KF2DTextureANIMATION_H_

class KF2DTextureAnimation
{
public:
	 KF2DTextureAnimation();
	~KF2DTextureAnimation();

	HRESULT Init(HWND hwnd, D3DGlobalContext* D3DGC, 
		int UX_Amount, 
		int VY_Amount, 
		ID3D11ShaderResourceView * animTexture,
		int DrawShaderIndex,				// Номер шейдера в движке которым нужно рисовать анимацию
		int ReturnShaderIndex,				// Номер шейдера в движке который нужно включить после анимации
		KFResourceManager *_ShaderManager,
		XMFLOAT4& _ObjParam,				// x,y - позиция на экране  z - Width , w - Height
		ID3D10Blob * WorkBlob);

	void Frame( FPSTimers &fpstimers );
	void Draw();

	bool Enabled;
	// Параметры экрана
//	XMFLOAT4 ScreenCoords;
	// Параметры родительской формы
//	XMFLOAT4 FormCoord;
	
	XMFLOAT4 ObjParam;	// Дублирует параметры объекта.
//	XMFLOAT4 ObjPreviosParam; // Предыдущие параметры объекта ( из предыдущего кадра )

// Переместить меню на позицию  ( Координаты левого верхнего угла )
	void SetAnimePos( XMFLOAT2& Pos );

	void SetAnimeSize( XMFLOAT2& Size );

	void SetAnimeParam(XMFLOAT4& Param);

	// Вернуть анимацию на место где она была создана
	void SetAnimeOrigin();

	void SetFPSpeed(float FPS );

private:

	int UX_Amount;			// Ширина текстуры Width X U
	int VY_Amount;			// Высота текстуры Height Y V

	int ShaderForDraw;
	int ShaderReturnTo;
	KFResourceManager *ShaderManager;
	// Сколько всего кадров в анимации
	int NumOfAnimeFrams;
	// Текущий кадр
	int CurentFrame;
	// С какой скоростью в секунду прокручиваются кадры 
	float PlayFPS;
	// Сколько времени прошло
	float TimePassed;

	// Массив указателей на координаты кадра в текстуре 
	float *TextcordTop; 
	float *TextcordLeft;

	// Текстура с анимацией
	SquareObjectClass * AnimeTextures;
	
	// ШИрина одного кадра в текстурных координатах 0 - 1
	float OneFrameWidth;
	// Высота одного кадра в текстурных координатах 0 - 1
	float OneFrameHeight;

};

#endif