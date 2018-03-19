#include "stdafx.h"
#include "TerrainCluster.h"

TerrainCluster::TerrainCluster ()
{
//	InFrustum = true;
//		  LOD = 0;
		  ClusterBufferIndex  = -1;
//		  ClusterBufferIndexH = -1;
//		  ClusterBufferIndexM = -1;
//		  ClusterBufferIndexL = -1;
//		  ClusterBufferIndexN = -1;
		  AABB.LDB.Vec	= { 0.0f, 0.0f, 0.0f, 0.0f};
		  AABB.RUF.Vec	= { 0.0f, 0.0f, 0.0f, 0.0f };
		  AABB.Pos.Vec	= { 0.0f, 0.0f, 0.0f, 0.0f };
		  AABB.Radius	= 0.0f;
}

TerrainCluster::~TerrainCluster ()
{
//		InFrustum = false;
}