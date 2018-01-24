#pragma once
#include "stdafx.h"
#include "Terrain_def.h"

#ifndef _TERRAIN_CLUSTER_H_
#define _TERRAIN_CLUSTERH_

class TerrainCluster
{
public:
	TerrainCluster ();
   ~TerrainCluster ();

   int ClusterBufferIndex;	// ����� ���������� ������ ��������

   int ClusterBufferIndexH;	// ����� ���������� ������ �������� HIGH
   int ClusterBufferIndexM;	// ����� ���������� ������ �������� MEDIUM
   int ClusterBufferIndexL;	// ����� ���������� ������ �������� LOW
   int ClusterBufferIndexN;	// ����� ���������� ������ �������� miNimum

   int LOD;					// ����� ������� ������� �������� �������

   bool InFrustum;			// �������� �� �������

};
#endif