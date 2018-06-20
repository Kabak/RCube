#pragma once
#include "stdafx.h"

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <DirectXMath.h>
#include "Buffers_def.h"
#include "TerrainCluster.h"
#include "DX11Buffers.h"
#include <vector>


using namespace DirectX;
using namespace std;


class Terrain
{
public:
	Terrain ();
   ~Terrain ();

	const int TEXTURE_REPEAT = 8;

	int TerrainBuffersIndex;	// Номер буфера в списке буферов
	int TerrainTextureIndex;	// Номер текстуры накладываемой на Terrain
	int Terrain_Object_Index;	// Номер самого себя в списке

	VertexBuffer <PositionType>* InstancePositionsBuffer;	// Буфер позиций всех Instance объекта уходящий в шейдер

	vector <TerrainCluster*> Clusters;	// Массив кластеров из которых состоит Terrain
	vector <TerrainCluster*> PassedFrustumClasters;// Массив индексов кластеров прошедших Frustum

  float QuadVertexStep; // Расстояние между вертексами в примитиве
	int QuadsPerCluster;// Количество примитивов в одном секторе X * X , 7 * 7 , 16 * 16 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
	int ClustersAmount;	// Количество кластеров в Terrain  X * Z
	int ClustersX_ROW;	// Количество кластеров по X - ROW
	int	ClustersZ_COL;	// Количество кластеров по Z - COLUMN
	int VertexInX_Row;	// Количество вертексов в ряду всего Terrain
	int VertexInZ_Col;	// Количество вертексов в столбце всего Terrain
	int Total_Vertexs_In_Terrain;	// Общее количество вертексов в Terrain
	int Total_Indexs_In_Terrain;	// Общее количество индексов в Terrain
	int Total_Indexs_In_Cluster;	// Общее количество индексов в кажом кластере терейна
	int Row_Col_VertexAmountInClasterAnyDir;// Общее количество вертексов в кластере в вверх или вниз Row или Col

	Vertex_Model3D* VB_Data;// Массив всех вертексов с текстурными координатами и нормалями
	Index_Type* IB_Data;	// Индексный буфер

	RCube_VecFloat234 First_Vertex_Data;	// Положение и высота 1-го вертекса, от которго всё пляшет

	bool CastShadow;	// Рисовать ли тень от этого объекта ?
	bool ClusterRender; // Рисовать кластерами ? ( кластеры проходят Frustum )
};

#endif