#pragma once
#include "stdafx.h"

#include "FlatObjectClass.h"
#include "D3DGlobalContext.h"
#include "FPSTimers.h"
#include "DX11Buffers.h"

#ifndef _KF2DTextureANIMATION_H_
#define _KF2DTextureANIMATION_H_

class ResourceManager;

class KF2DTextureAnimation
{
public:
	 KF2DTextureAnimation();
	~KF2DTextureAnimation();

	ResourceManager* ResManager;

	HRESULT Init(HWND hwnd, D3DGlobalContext* D3DGC, 
		int UX_Amount, 
		int VY_Amount, 
		ID3D11ShaderResourceView * animTexture,
		int DrawShaderIndex,				// ����� ������� � ������ ������� ����� �������� ��������
//		int ReturnShaderIndex,				// ����� ������� � ������ ������� ����� �������� ����� ��������
//		Flat_Obj_Buffers* _Buffers,
		XMFLOAT4& _ObjParam,				// x,y - ������� �� ������  z - Width , w - Height
		ResourceManager * _GlobalResourceManager
		);

	void Frame( FPSTimers &fpstimers );

	bool Enabled;

	int ShaderForDraw = -1;
//	int ShaderReturnTo = -1;

	XMFLOAT4 ObjParam;	// ��������� ��������� �������.
//	XMFLOAT4 ObjPreviosParam; // ���������� ��������� ������� ( �� ����������� ����� )

// ����������� ���� �� �������  ( ���������� ������ �������� ���� )
	void SetAnimePos( XMFLOAT2& Pos );

	void SetAnimeSize( XMFLOAT2& Size );

	void SetAnimeParam(XMFLOAT4& Param);

	// ������� �������� �� ����� ��� ��� ���� �������
	void SetAnimeOrigin();

	void SetFPSpeed(float FPS );

	// �������� � ���������
	FlatObjectClass * AnimeTexture;

	int AnimeTextureBufferIndex;

private:

	int UX_Amount;			// ������ �������� Width X U
	int VY_Amount;			// ������ �������� Height Y V

//	ResourceManager * GlobalResourceManager;
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

	// ������ ������ ����� � ���������� ����������� 0 - 1
	float OneFrameWidth;
	// ������ ������ ����� � ���������� ����������� 0 - 1
	float OneFrameHeight;

};

#endif