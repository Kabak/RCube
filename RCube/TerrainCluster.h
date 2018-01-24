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

   int ClusterBufferIndex;	// Номер индексного буферы кластера

   int ClusterBufferIndexH;	// Номер индексного буферы кластера HIGH
   int ClusterBufferIndexM;	// Номер индексного буферы кластера MEDIUM
   int ClusterBufferIndexL;	// Номер индексного буферы кластера LOW
   int ClusterBufferIndexN;	// Номер индексного буферы кластера miNimum

   int LOD;					// Каким уровнем деталей рисуется кластер

   bool InFrustum;			// Рисуется ли кластер

};
#endif