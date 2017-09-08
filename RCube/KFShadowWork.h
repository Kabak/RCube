#pragma once
#include "stdafx.h"

#include "D3DGlobalContext.h"
#include <vector>
#include "KFResourceManager.h"
#include "LightClass.h"
#include "FrustumClass.h"
#include "KFTerrain.h"
#include "KFModelList.h"

#ifndef _KFSHADOWWORK_H_
#define _KFSHADOWWORK_H_

class KFShadowWork
{

private:

	ID3D11DepthStencilView* DSV_ShadowMap3D; // 
	ID3D11Buffer * cbShadowBuffer;
	D3DGlobalContext * g_D3DGC;
	KFResourceManager * myManeger;
	ID3D11RasterizerState* LightRender_RS;
	ID3D11DepthStencilState* LightRender_DS;
	LightClass * g_Light;
	D3D11_DEPTH_STENCIL_VIEW_DESC DSD2;
	XMVECTOR Up;
	FrustumClass* g_Frustum;

	void DrawObjectUsingShadows(XMVECTOR DrawPosition, bool ReplaseData);

	struct cbShadowObject
	{

		XMMATRIX ViewProjection;

	};

	RCube_VecFloat34 LightPosition, LightTarget;


public:

	KFShadowWork();
	~KFShadowWork();

	struct ObjectUsingShadows {

		KFModelList * ModelList;
		KFTerrain * Terrain;

	} DrawShadowsObjects;

	int UsingShaderIndex;
	void Init(HWND hwnd, D3DGlobalContext * D3DGC , KFResourceManager * Maneger , LightClass * Light , FrustumClass* Frustum 
		, ObjectUsingShadows Objects);
	void RenderSpotLightsSadowMaps(std::vector <int> SpotLightsWithShadowsIndexes);

	void InitRasterizerState( int DepthBias, float SlopeScaledDepthBias );

};


#endif
