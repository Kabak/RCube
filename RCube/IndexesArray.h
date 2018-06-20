#pragma once
#include "stdafx.h"
#include "DirectXMath.h"

#ifndef _INDEXES_ARRAY_H_
#define _INDEXES_ARRAY_H_

using namespace std;
using namespace DirectX;

class IndexseArray
{
private:

public:
	 IndexseArray();
	~IndexseArray();

	RCube_VecFloat234 Camera;
	RCube_VecFloat234 Object;

	// Массив индексов
	int *Array;

	// Количество элементов в массиве
	int EleventsAmount;

	// Индекспоследнего элемента
	int LastElement;

	// СОздаём массив с нужным колличеством элементом
	// Если FartoCamFirst - true, то сортировка будет по удалению от камеры
	bool InitArray(int Amount, bool FartoCamFirst );

	void Shutdown();
	// Сортировка элементов массива по глубине ПО ВОЗРАСТАНИЮ
	template <class T> void QuickDepthSortAscending( T* indices, float* depths, int lo, int hi );
	// Сортировка элементов массива по глубине ПО УБЫВАНИЮ
	template <class T> void QuickDepthSortDescending( T* indices, float* depths, int lo, int hi );
	// Рассчёт расстония до камеры
	void DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );
};


#endif
