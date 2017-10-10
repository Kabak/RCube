#pragma once
#include "stdafx.h"

#ifndef _2DRENDERINGCLASSES_H_
#define _2DRENDERINGCLASSES_H_

#include "KF2DObjClass.h"
#include "D3DGlobalContext.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ����� ��� ��������� (�������������) �������� 
class SquareObjectClass{
protected:

	KF2DObjClass* Obj;

	float top;
	float bottom;
	float left;
	float right;

	// ������������ ��� ������� ����
	int previousPosX;
	int previousPosY;

	// ������� �������� ��� ������ Min � Max � ScrollBar 
	int TextureFlip; // 0 = NO_FLIP, 1 = FLIP_HORIZONTAL, 2 = FLIP_VERTICAL

	D3DGlobalContext* D3DGC_Obj;

	// ������������ �������� ��� �������� �� X
	float _2DPixelXmax;
	// ������������ �������� ��� �������� �� Y
	float _2DPixelYmax;

	void UpdateVertexPos();

public :

	 SquareObjectClass();
	~SquareObjectClass();

	int	ShaderIndex = 0; 

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
	HRESULT Init(D3DGlobalContext* D3DGC,
		XMFLOAT4& _ObjParam,
		ID3D11ShaderResourceView * texture,
		int _TextureFlip
		);

	void Draw();

	void SetObjectTexture(ID3D11ShaderResourceView* texture);

	// �������� ������� � ������ �������
	void ResetObjectTexcord( XMFLOAT4& Position );

	ID3D11ShaderResourceView * GetTexture();

	void SetObjParam();

	// �������� ������ ������� � % � ������������� ������� = 100%
	void ResizeObject( float Percent );

	void ChangeObject( XMFLOAT4& _ObjParam );

	// ������������ ������������� ��� ���������� ��������� ������� ����� �� ������
	void ChangeMousePosition( int, int );

	void SetOriginalSize( void );

	// ��������� ��������� � �������� �������
	void GenerateVertexes( Vertex_FlatObject*, int TextureFlip );
    
};
//-------------------------------------------------------------------------------- ����� ��� ��������� (�������������) �������� 

#endif