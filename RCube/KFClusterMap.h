#pragma once
#include "stdafx.h"

#ifndef _KFCLUSTERMAP_H_
#define _KFCLUSTERMAP_H_

#include "DirectXMath.h"
#include <vector>

using namespace DirectX;

class KFClusterMap
{
public:
	KFClusterMap(float Width, float Height, float Depth, float ClusterWidth, float ClusterHeight, float ClusterDepth);
	~KFClusterMap();

private:

	std::vector<XMFLOAT3> ClustersPoses;

};

#endif

