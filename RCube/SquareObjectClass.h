#pragma once
#include "stdafx.h"

#ifndef _2DRENDERINGCLASSES_H_
#define _2DRENDERINGCLASSES_H_

#include "KF2DObjClass.h"
#include "D3DGlobalContext.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ класс для рисования (Прямоугольных) обьектов 
class SquareObjectClass{
protected:

	KF2DObjClass* Obj;

	float top;
	float bottom;
	float left;
	float right;

	// Используется для курсора мыши
	int previousPosX;
	int previousPosY;

	// поворот текстуры для кнопок Min и Max в ScrollBar 
	int TextureFlip; // 0 = NO_FLIP, 1 = FLIP_HORIZONTAL, 2 = FLIP_VERTICAL

	D3DGlobalContext* D3DGC_Obj;

	// максимальное значение для вертекса по X
	float _2DPixelXmax;
	// максимальное значение для вертекса по Y
	float _2DPixelYmax;

	void UpdateVertexPos();

public :

	 SquareObjectClass();
	~SquareObjectClass();

	int	ShaderIndex = 0; 

	// Координаты объекта на экране
	XMFLOAT4 ObjParam;	// x - позиция X , y - позиция Y , z - Width ширина элемента , w - Height высота элемента
	XMFLOAT4 ObjOriginalParam; // Оригинальные параметры при иницоализации объекта = 100% реального размера объекта

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
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

	// Изменить позицию и размер объекта
	void ResetObjectTexcord( XMFLOAT4& Position );

	ID3D11ShaderResourceView * GetTexture();

	void SetObjParam();

	// Изменить размер объекту в % к оригинальному размеру = 100%
	void ResizeObject( float Percent );

	void ChangeObject( XMFLOAT4& _ObjParam );

	// Используется исключительно для обновление положения курсора мышки на экране
	void ChangeMousePosition( int, int );

	void SetOriginalSize( void );

	// Генерация вертексов и текстуры объекта
	void GenerateVertexes( Vertex_FlatObject*, int TextureFlip );
    
};
//-------------------------------------------------------------------------------- класс для рисования (Прямоугольных) обьектов 

#endif