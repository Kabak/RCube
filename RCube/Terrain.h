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

struct TerrainParam
{

	//*********************** для рандомной генерации вершин*****************************
	int HeightesCout, LowlandsCout;
	int MaxHeight, MinHeight;
	int MaxRadius, MinRadius;
	//*********************** для рандомной генерации вершин*****************************

	//*********************** для генерации по зананной текстуре ************************
	char* filename;
	//*********************** для генерации по зананной текстуре ************************

};


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

  float QuadVertexStep; // Расстояние между вертексами в примитиве
	int QuadsPerCluster;// Количество примитивов в одном секторе 7 * 7 , 16 * 16 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
	int ClustersAmount;	// Количество секторов в Terrain  X * Z
	int ClustersX_ROW;	// Количество секторов по X - ROW
	int	ClustersZ_COL;	// Количество секторов по Z - COLUMN
	int VertexInX_Row;	// Количество вертексов в ряду
	int VertexInZ_Col;	// Количество вертексов в столбце
	int TotalVertex;	// ОБщее количество вертексов в терейне
	int TotalIndex;		// ОБщее количество индексов в терейне
	
	Vertex_Model3D* VB_Data;// Массив всех вертексов с текстурными координатами и нормалями
	Index_Type* IB_Data;	// Индексный буфер

	HeightMapType * m_heightMap; // двумерный массив высот [x кордината][y кордината]
	RCube_VecFloat34 First_Vertex_Data;	// Положение и высота 1-го вертекса, от которго всё пляшет

};

#endif