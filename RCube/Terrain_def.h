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
  float _QuadVertexStep;	// Расстояние между вертексами в примитиве
	int _QuadsPerCluster;	// Количество примитивов в одном секторе 7 * 7 , 15 * 15 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
	int _ClustersX_ROW;		// Количество секторов по X
	int	_ClustersZ_COL;		// Количество секторов по Z
   bool _CastShadow;		// Рисовать ли тень от объекта ?
   bool _ClusterRender;		// Рисовать кусками ?
   char* HightFileName;		// Имя файла высот для создания Terrain
};