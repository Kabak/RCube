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
  float _QuadVertexStep;	// ���������� ����� ���������� � ���������
	int _QuadsPerCluster;	// ���������� ���������� � ����� ������� 7 * 7 , 15 * 15 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
	int _ClustersX_ROW;		// ���������� �������� �� X
	int	_ClustersZ_COL;		// ���������� �������� �� Z
   bool _CastShadow;		// �������� �� ���� �� ������� ?
   bool _ClusterRender;		// �������� ������� ?
   char* HightFileName;		// ��� ����� ����� ��� �������� Terrain
};


// BVH for Terrain cluster
struct Cluster_AABB
{
	RCube_VecFloat34 RUF;	// Right Up Forward coner
	RCube_VecFloat34 LDB;	// Left Down Backward coner
	RCube_VecFloat34 Pos;	// 3D position of this Cluster
	float Radius;// Radius of Cluster
};