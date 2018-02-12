#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "frustumclass.h"


FrustumClass::FrustumClass()
{
	RCubeMatrix Temp;
	Input_projectionMatrix = XMMatrixIdentity();
	matrix = XMMatrixIdentity();
	TempVector = XMVectorZero();
//	XMStoreFloat4x4A(&Temp,matrix);
	Temp.XMM = matrix;
}


FrustumClass::~FrustumClass()
{
}


void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX &projectionMatrix, XMMATRIX &viewMatrix)
{
	float zMinimum, r;
	RCubeMatrix Temp;
//	XMStoreFloat4x4A(&Temp,projectionMatrix);
	Temp.XMM = projectionMatrix;

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -Temp.XMF._43 / Temp.XMF._33;
	r = screenDepth / (screenDepth - zMinimum);
	Temp.XMF._33 = r;
	Temp.XMF._43 = -r * zMinimum;

	Input_projectionMatrix = Temp.XMM;// XMLoadFloat4x4A ( &Temp );
	matrix = viewMatrix * Input_projectionMatrix;

//	XMStoreFloat4x4A(&Temp,matrix);
	Temp.XMM = matrix;
	// Calculate left plane of frustum.
	u_plane.m_planes[0].x = Temp.XMF._14 + Temp.XMF._11;
	u_plane.m_planes[0].y = Temp.XMF._24 + Temp.XMF._21;
	u_plane.m_planes[0].z = Temp.XMF._34 + Temp.XMF._31;
	u_plane.m_planes[0].w = Temp.XMF._44 + Temp.XMF._41;

	// Calculate right plane of frustum.
	u_plane.m_planes[1].x = Temp.XMF._14 - Temp.XMF._11;
	u_plane.m_planes[1].y = Temp.XMF._24 - Temp.XMF._21;
	u_plane.m_planes[1].z = Temp.XMF._34 - Temp.XMF._31;
	u_plane.m_planes[1].w = Temp.XMF._44 - Temp.XMF._41;

	// Calculate top plane of frustum.
	u_plane.m_planes[2].x = Temp.XMF._14 - Temp.XMF._12;
	u_plane.m_planes[2].y = Temp.XMF._24 - Temp.XMF._22;
	u_plane.m_planes[2].z = Temp.XMF._34 - Temp.XMF._32;
	u_plane.m_planes[2].w = Temp.XMF._44 - Temp.XMF._42;

	// Calculate bottom plane of frustum.
	u_plane.m_planes[3].x = Temp.XMF._14 + Temp.XMF._12;
	u_plane.m_planes[3].y = Temp.XMF._24 + Temp.XMF._22;
	u_plane.m_planes[3].z = Temp.XMF._34 + Temp.XMF._32;
	u_plane.m_planes[3].w = Temp.XMF._44 + Temp.XMF._42;

	// Calculate near plane of frustum.
	u_plane.m_planes[4].x = Temp.XMF._14 + Temp.XMF._13;
	u_plane.m_planes[4].y = Temp.XMF._24 + Temp.XMF._23;
	u_plane.m_planes[4].z = Temp.XMF._34 + Temp.XMF._33;
	u_plane.m_planes[4].w = Temp.XMF._44 + Temp.XMF._43;

	// Calculate far plane of frustum.
	u_plane.m_planes[5].x = Temp.XMF._14 - Temp.XMF._13;
	u_plane.m_planes[5].y = Temp.XMF._24 - Temp.XMF._23;
	u_plane.m_planes[5].z = Temp.XMF._34 - Temp.XMF._33;
	u_plane.m_planes[5].w = Temp.XMF._44 - Temp.XMF._43;
	return;
}


void FrustumClass::ConstructFrustumNew( XMMATRIX &ViewProj )
{
	RCubeMatrix Temp;
//	XMStoreFloat4x4A(&Temp, ViewProj);
	Temp.XMM = ViewProj;
	// Calculate left plane of frustum.
//	_Res *Left;// = &u_plane.m_planes[0];
	Res.fres.x = Temp.XMF._14 + Temp.XMF._11;
	Res.fres.y = Temp.XMF._24 + Temp.XMF._21;
	Res.fres.z = Temp.XMF._34 + Temp.XMF._31;
	Res.fres.w = Temp.XMF._44 + Temp.XMF._41;
	u_plane.planes[0] = XMPlaneNormalize( Res.res );
	// Calculate right plane of frustum.
//	_Res *Right;// = &u_plane.m_planes[1];
	Res.fres.x = Temp.XMF._14 - Temp.XMF._11;
	Res.fres.y = Temp.XMF._24 - Temp.XMF._21;
	Res.fres.z = Temp.XMF._34 - Temp.XMF._31;
	Res.fres.w = Temp.XMF._44 - Temp.XMF._41;
	u_plane.planes[1] = XMPlaneNormalize( Res.res );
	// Calculate top plane of frustum.
//	_Res *Top;// = &u_plane.m_planes[2];
	Res.fres.x = Temp.XMF._14 - Temp.XMF._12;
	Res.fres.y = Temp.XMF._24 - Temp.XMF._22;
	Res.fres.z = Temp.XMF._34 - Temp.XMF._32;
	Res.fres.w = Temp.XMF._44 - Temp.XMF._42;
	u_plane.planes[2] = XMPlaneNormalize( Res.res );
	// Calculate bottom plane of frustum.
//	_Res *Bottom;// = &u_plane.m_planes[3];
	Res.fres.x = Temp.XMF._14 + Temp.XMF._12;
	Res.fres.y = Temp.XMF._24 + Temp.XMF._22;
	Res.fres.z = Temp.XMF._34 + Temp.XMF._32;
	Res.fres.w = Temp.XMF._44 + Temp.XMF._42;
	u_plane.planes[3] = XMPlaneNormalize( Res.res );
	// Calculate near plane of frustum.
//	_Res *Near;// = &u_plane.m_planes[4];
	Res.fres.x = Temp.XMF._13;
	Res.fres.y = Temp.XMF._23;
	Res.fres.z = Temp.XMF._33;
	Res.fres.w = Temp.XMF._43;
	u_plane.planes[4] = XMPlaneNormalize( Res.res );
	// Calculate far plane of frustum.
//	_Res *Far;// = &u_plane.m_planes[5];
	Res.fres.x = Temp.XMF._14 - Temp.XMF._13;
	Res.fres.y = Temp.XMF._24 - Temp.XMF._23;
	Res.fres.z = Temp.XMF._34 - Temp.XMF._33;
	Res.fres.w = Temp.XMF._44 - Temp.XMF._43;
	u_plane.planes[5] = XMPlaneNormalize( Res.res );
	return;
}


bool FrustumClass::CheckPoint(XMFLOAT3& Position)
{
	XMVECTOR Vec;

	Vec = XMLoadFloat3(&Position);
	// Check if the point is inside all six planes of the view frustum.
	for(int i=0; i<6; ++i) 
	{
		Res.res = XMPlaneDotCoord(u_plane.planes[i], Vec);
		if(Res.fres.x < 0.0f)
		{
			return false;
		}
	}

	return true;
}


bool FrustumClass::CheckCube(XMFLOAT3& Position, float& radius)
{
	XMVECTOR VecMMM;
	XMVECTOR VecPMM;
	XMVECTOR VecMPM;
	XMVECTOR VecPPM;
	XMVECTOR VecMMP;
	XMVECTOR VecPMP;
	XMVECTOR VecMPP;
	XMVECTOR VecPPP;

// Check if any one point of the cube is in the view frustum.
	for(int i=0; i<6; ++i) 
	{
		VecMMM = XMVectorSet ( Position.x - radius, Position.y - radius, Position.z - radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPMM = XMVectorSet ( Position.x + radius, Position.y - radius, Position.z - radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecMPM = XMVectorSet ( Position.x - radius, Position.y + radius, Position.z - radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPPM = XMVectorSet ( Position.x + radius, Position.y + radius, Position.z - radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPPM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecMMP = XMVectorSet ( Position.x - radius, Position.y - radius, Position.z + radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPMP = XMVectorSet ( Position.x + radius, Position.y - radius, Position.z + radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecMPP = XMVectorSet ( Position.x - radius, Position.y + radius, Position.z + radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPPP = XMVectorSet ( Position.x + radius, Position.y + radius, Position.z + radius, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPPP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}


bool FrustumClass::CheckSphere(XMFLOAT3& Position, float& radius)
{
	XMVECTOR Vec;

	Vec = XMLoadFloat3(&Position);
	// Check if the radius of the sphere is inside the view frustum.
	for(int i=0; i<6; ++i) 
	{
		Res.res = XMPlaneDotCoord(u_plane.planes[i], Vec);

		if ( Res.fres.x < -radius)
		{
			return false;
		}
	}

	return true;
}


bool FrustumClass::CheckRectangle(XMFLOAT3& Position, float& xSize, float& ySize, float& zSize)
{
	XMVECTOR VecMMM;
	XMVECTOR VecPMM;
	XMVECTOR VecMPM;
	XMVECTOR VecPPM;
	XMVECTOR VecMMP;
	XMVECTOR VecPMP;
	XMVECTOR VecMPP;
	XMVECTOR VecPPP;

	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for(int i=0; i<6; ++i)
	{

		VecMMM = XMVectorSet ( Position.x - xSize, Position.y - ySize, Position.z - zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPMM = XMVectorSet ( Position.x + xSize, Position.y - ySize, Position.z - zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecMPM = XMVectorSet ( Position.x - xSize, Position.y + ySize, Position.z - zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPPM = XMVectorSet ( Position.x + xSize, Position.y + ySize, Position.z - zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPPM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecMMP = XMVectorSet ( Position.x - xSize, Position.y - ySize, Position.z + zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPMP = XMVectorSet ( Position.x + xSize, Position.y - ySize, Position.z + zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecMPP = XMVectorSet ( Position.x - xSize, Position.y + ySize, Position.z + zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		VecPPP = XMVectorSet ( Position.x + xSize, Position.y + ySize, Position.z + zSize, 1.0f );
		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPPP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}