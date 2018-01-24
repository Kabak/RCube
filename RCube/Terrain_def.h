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
  float _QuadVertexStep; // Расстояние между вертексами в примитиве
	int _QuadsPerCluster;// Количество примитивов в одном секторе 7 * 7 , 15 * 15 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
//	int ClustersAmount;	// Количество секторов в Terrain  X * X
	int _ClustersX_ROW;		// Количество секторов по X
	int	_ClustersZ_COL;		// Количество секторов по Z
//	int VertexInRow;	// Количество вертексов в ряду
//	int VertexInColumn;	// Количество вертексов в столбце
//	int TotalVertex;	// ОБщее количество вертексов в терейне
//	int TotalIndex;		// ОБщее количество индексов в терейне
	char* HightFileName;// Имя файла высот для создания Terrain
};