#pragma once
#include "stdafx.h"

#ifndef _FLATOBJECTCLASS_H_
#define _FLATOBJECTCLASS_H_

#include "D3DGlobalContext.h"
#include "DX11Buffers.h"
#include "Buffers_def.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ класс для рисования (Прямоугольных) обьектов 
class FlatObjectClass {
public:
	   
	// Используется для курсора мыши
	int previousPosX;
	int previousPosY;

	// поворот текстуры для кнопок Min и Max в ScrollBar 
	int TextureFlip; // 0 = NO_FLIP, 1 = FLIP_HORIZONTAL, 2 = FLIP_VERTICAL

	Flat_Obj_Buffers* Buffers;

	 FlatObjectClass();
	~FlatObjectClass();

	float top;
	float bottom;
	float left;
	float right;

	XMFLOAT4 Dummy;	// Used as dummy for vertex generation

	// максимальное значение для вертекса по X
	float _2DPixelXmax;
	// максимальное значение для вертекса по Y
	float _2DPixelYmax;

	int	ShaderIndex = 0; 

	int BuffersIndex = -1; // Индекс буферов объекта в ресурс менеджере

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
	HRESULT Init ( int ScreenWidth, int ScreenHeight,
		XMFLOAT4& _ObjParam,
		ID3D11ShaderResourceView * texture,
		int _TextureFlip,
		Flat_Obj_Buffers* _Buffers
		);

	void SetObjectTexture(ID3D11ShaderResourceView* texture);

	// Изменить позицию и размер объекта
	void ResetObjectTexcord( XMFLOAT4& Position );

	ID3D11ShaderResourceView * GetTexture();

	void SetObjParam();

	void UpdateVertexPos ();	// Пересчёт позиций вертексов в реальные 2D координаты экрана

	// Изменить размер объекту в % к оригинальному размеру = 100%
	void ResizeObject( float Percent );

	// Используется исключительно для обновление положения курсора мышки на экране
	void ChangeMousePosition( int, int );

	void SetOriginalSize( void );

	// Генерация вертексов и текстуры объекта
	void GenerateVertexes( Vertex_FlatObject*, int TextureFlip, XMFLOAT4& Color );
    
};
//-------------------------------------------------------------------------------- класс для рисования (Прямоугольных) обьектов 

#endif