#pragma once
#include "stdafx.h"

#include "SquareObjectClass.h"
#include "D3DGlobalContext.h"
#include "ResourceManager.h"
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
		int DrawShaderIndex,				// ����� ������� � ������ ������� ����� �������� ��������
		int ReturnShaderIndex,				// ����� ������� � ������ ������� ����� �������� ����� ��������
		ResourceManager *_ShaderManager,
		XMFLOAT4& _ObjParam				// x,y - ������� �� ������  z - Width , w - Height
		);

	void Frame( FPSTimers &fpstimers );
	void Draw();

	bool Enabled;

	int ShaderForDraw = -1;
	int ShaderReturnTo = -1;

	XMFLOAT4 ObjParam;	// ��������� ��������� �������.
//	XMFLOAT4 ObjPreviosParam; // ���������� ��������� ������� ( �� ����������� ����� )

// ����������� ���� �� �������  ( ���������� ������ �������� ���� )
	void SetAnimePos( XMFLOAT2& Pos );

	void SetAnimeSize( XMFLOAT2& Size );

	void SetAnimeParam(XMFLOAT4& Param);

	// ������� �������� �� ����� ��� ��� ���� �������
	void SetAnimeOrigin();

	void SetFPSpeed(float FPS );

private:

	int UX_Amount;			// ������ �������� Width X U
	int VY_Amount;			// ������ �������� Height Y V

	ResourceManager *ShaderManager;
	// ������� ����� ������ � ��������
	int NumOfAnimeFrams;
	// ������� ����
	int CurentFrame;
	// � ����� ��������� � ������� �������������� ����� 
	float PlayFPS;
	// ������� ������� ������
	float TimePassed;

	// ������ ���������� �� ���������� ����� � �������� 
	float *TextcordTop; 
	float *TextcordLeft;

	// �������� � ���������
	SquareObjectClass * AnimeTextures;
	
	// ������ ������ ����� � ���������� ����������� 0 - 1
	float OneFrameWidth;
	// ������ ������ ����� � ���������� ����������� 0 - 1
	float OneFrameHeight;

};

#endif