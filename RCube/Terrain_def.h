#pragma once
#include "stdafx.h"

enum TerrainLOD
{
	HIGH = 0,
	MEDIUM,
	LOW,
	MIN
};


struct TerrainInitData
{
  float _QuadVertexStep; // ���������� ����� ���������� � ���������
	int _QuadsPerCluster;// ���������� ���������� � ����� ������� 7 * 7 , 15 * 15 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
//	int ClustersAmount;	// ���������� �������� � Terrain  X * X
	int _ClustersX_ROW;		// ���������� �������� �� X
	int	_ClustersZ_COL;		// ���������� �������� �� Z
//	int VertexInRow;	// ���������� ��������� � ����
//	int VertexInColumn;	// ���������� ��������� � �������
//	int TotalVertex;	// ����� ���������� ��������� � �������
//	int TotalIndex;		// ����� ���������� �������� � �������
	char* HightFileName;// ��� ����� ����� ��� �������� Terrain
};