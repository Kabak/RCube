#pragma once
#include "stdafx.h"
#include "Buffers_def.h"

using namespace DirectX;

enum TerrainLOD
{
	HIGH = 0,
	MEDIUM,
	LOW,
	MIN
};

enum MoveDirection
{
	DOWN = 0,
	  UP = 1
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


// BVH for Terrain cluster
struct Cluster_AABB
{
	RCube_VecFloat34 RUF;	// Right Up Forward coner
	RCube_VecFloat34 LDB;	// Left Down Backward coner
	RCube_VecFloat34 Pos;	// 3D position of this Cluster
	float XSize;
	float YSize;
	float ZSize;
	float Radius;// Radius of Cluster
};


// Terrain Landscape generator init structure
struct Terrain_GenInit
{
	int	  HeightNum;
	int   LowlandNum;
	float Min_Height;
	float Max_Height;
	float Min_Low;
	float Max_Low;
	float MinRadius;
	float MaxRadius;
};

struct PointsList
{
	RCube_VecFloat34 Point;
};


// Structure holds how much vertex in all directions from center vertex 
// we should take in account when drawing landscape shape
struct DirectionsAmount
{
	int Up;			 // Amount of points in Up direction
	int Down;		 // Amount of points in Down direction
	int Left;
	int Right;
	int PointsAmount;// How many points within a radius
	int Index;		 // Center point index in massive of vertex
  float Radius;		 //
    int LowestIndex; // affected points lowest Index in massive
	int LargestIndex;// affected points largest Index in massive
	PointCoord Start;
	PointCoord End;

};
