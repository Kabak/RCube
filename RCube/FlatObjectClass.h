#pragma once
#include "stdafx.h"

#ifndef _FLATOBJECTCLASS_H_
#define _FLATOBJECTCLASS_H_

#include "D3DGlobalContext.h"
#include "DX11Buffers.h"
#include "Buffers_def.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ����� ��� ��������� (�������������) �������� 
class FlatObjectClass {
public:
	   
	// ������������ ��� ������� ����
	int previousPosX;
	int previousPosY;

	// ������� �������� ��� ������ Min � Max � ScrollBar 
	int TextureFlip; // 0 = NO_FLIP, 1 = FLIP_HORIZONTAL, 2 = FLIP_VERTICAL

	Flat_Obj_Buffers* Buffers;

	 FlatObjectClass();
	~FlatObjectClass();

	float top;
	float bottom;
	float left;
	float right;

	XMFLOAT4 Dummy;	// Used as dummy for vertex generation

	// ������������ �������� ��� �������� �� X
	float _2DPixelXmax;
	// ������������ �������� ��� �������� �� Y
	float _2DPixelYmax;

	int	ShaderIndex = 0; 

	int BuffersIndex = -1; // ������ ������� ������� � ������ ���������

	// ���������� ������� �� ������
	XMFLOAT4 ObjParam;	// x - ������� X , y - ������� Y , z - Width ������ �������� , w - Height ������ ��������
	XMFLOAT4 ObjOriginalParam; // ������������ ��������� ��� ������������� ������� = 100% ��������� ������� �������

	// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------
	HRESULT Init ( int ScreenWidth, int ScreenHeight,
		XMFLOAT4& _ObjParam,
		ID3D11ShaderResourceView * texture,
		int _TextureFlip,
		Flat_Obj_Buffers* _Buffers
		);

	void SetObjectTexture(ID3D11ShaderResourceView* texture);

	// �������� ������� � ������ �������
	void ResetObjectTexcord( XMFLOAT4& Position );

	ID3D11ShaderResourceView * GetTexture();

	void SetObjParam();

	void UpdateVertexPos ();	// �������� ������� ��������� � �������� 2D ���������� ������

	// �������� ������ ������� � % � ������������� ������� = 100%
	void ResizeObject( float Percent );

	// ������������ ������������� ��� ���������� ��������� ������� ����� �� ������
	void ChangeMousePosition( int, int );

	void SetOriginalSize( void );

	// ��������� ��������� � �������� �������
	void GenerateVertexes( Vertex_FlatObject*, int TextureFlip, XMFLOAT4& Color );
    
};
//-------------------------------------------------------------------------------- ����� ��� ��������� (�������������) �������� 

#endif