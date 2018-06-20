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

	// ������ ��������
	int *Array;

	// ���������� ��������� � �������
	int EleventsAmount;

	// ���������������� ��������
	int LastElement;

	// ������ ������ � ������ ������������ ���������
	// ���� FartoCamFirst - true, �� ���������� ����� �� �������� �� ������
	bool InitArray(int Amount, bool FartoCamFirst );

	void Shutdown();
	// ���������� ��������� ������� �� ������� �� �����������
	template <class T> void QuickDepthSortAscending( T* indices, float* depths, int lo, int hi );
	// ���������� ��������� ������� �� ������� �� ��������
	template <class T> void QuickDepthSortDescending( T* indices, float* depths, int lo, int hi );
	// ������� ��������� �� ������
	void DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );
};


#endif
