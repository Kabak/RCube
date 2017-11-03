#pragma once
#include "stdafx.h"

#ifndef _FLATOBJECTCLASS_H_
#define _FLATOBJECTCLASS_H_

//#include "D3DGlobalContext.h"
#include "DX11Buffers.h"
#include "Buffers_def.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ����� ��� ��������� (�������������) �������� 
class FlatObjectClass {
protected:

	float top;
	float bottom;
	float left;
	float right;

	// ������������ ��� ������� ����
	int previousPosX;
	int previousPosY;

	// ������� �������� ��� ������ Min � Max � ScrollBar 
	int TextureFlip; // 0 = NO_FLIP, 1 = FLIP_HORIZONTAL, 2 = FLIP_VERTICAL

	// ������������ �������� ��� �������� �� X
	float _2DPixelXmax;
	// ������������ �������� ��� �������� �� Y
	float _2DPixelYmax;

	void UpdateVertexPos();	// �������� ������� ��������� � �������� 2D ���������� ������

public :
	Flat_Obj_Buffers* Buffers;

	 FlatObjectClass();
	~FlatObjectClass();

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

	// �������� ������ ������� � % � ������������� ������� = 100%
	void ResizeObject( float Percent );

	// ������������ ������������� ��� ���������� ��������� ������� ����� �� ������
	void ChangeMousePosition( int, int );

	void SetOriginalSize( void );

	// ��������� ��������� � �������� �������
	void GenerateVertexes( Vertex_FlatObject*, int TextureFlip );
    
};
//-------------------------------------------------------------------------------- ����� ��� ��������� (�������������) �������� 

#endif