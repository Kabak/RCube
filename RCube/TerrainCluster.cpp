#include "stdafx.h"
#include "TerrainCluster.h"

TerrainCluster::TerrainCluster ()
{
	InFrustum = true;
		  LOD = 0;
		  ClusterBufferIndex  = -1;
		  ClusterBufferIndexH = -1;
		  ClusterBufferIndexM = -1;
		  ClusterBufferIndexL = -1;
		  ClusterBufferIndexN = -1;
}

TerrainCluster::~TerrainCluster ()
{

}