#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "frustumclass.h"


FrustumClass::FrustumClass()
{
	Input_projectionMatrix = XMMatrixIdentity();
	matrix = XMMatrixIdentity();
	TempVector = XMVectorZero();
	XMStoreFloat4x4A(&Temp,matrix);
}


FrustumClass::~FrustumClass()
{
}


void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX &projectionMatrix, XMMATRIX &viewMatrix)
{
	float zMinimum, r;

	XMStoreFloat4x4A(&Temp,projectionMatrix);
	
	// Calculate the minimum Z distance in the frustum.
	zMinimum = -Temp._43 / Temp._33;
	r = screenDepth / (screenDepth - zMinimum);
	Temp._33 = r;
	Temp._43 = -r * zMinimum;

	Input_projectionMatrix = XMLoadFloat4x4A(&Temp);
	matrix = viewMatrix * Input_projectionMatrix;

	XMStoreFloat4x4A(&Temp,matrix);
	// Calculate left plane of frustum.
	u_plane.m_planes[0].x = Temp._14 + Temp._11;
	u_plane.m_planes[0].y = Temp._24 + Temp._21;
	u_plane.m_planes[0].z = Temp._34 + Temp._31;
	u_plane.m_planes[0].w = Temp._44 + Temp._41;

	// Calculate right plane of frustum.
	u_plane.m_planes[1].x = Temp._14 - Temp._11;
	u_plane.m_planes[1].y = Temp._24 - Temp._21;
	u_plane.m_planes[1].z = Temp._34 - Temp._31;
	u_plane.m_planes[1].w = Temp._44 - Temp._41;

	// Calculate top plane of frustum.
	u_plane.m_planes[2].x = Temp._14 - Temp._12;
	u_plane.m_planes[2].y = Temp._24 - Temp._22;
	u_plane.m_planes[2].z = Temp._34 - Temp._32;
	u_plane.m_planes[2].w = Temp._44 - Temp._42;

	// Calculate bottom plane of frustum.
	u_plane.m_planes[3].x = Temp._14 + Temp._12;
	u_plane.m_planes[3].y = Temp._24 + Temp._22;
	u_plane.m_planes[3].z = Temp._34 + Temp._32;
	u_plane.m_planes[3].w = Temp._44 + Temp._42;

	// Calculate near plane of frustum.
	u_plane.m_planes[4].x = Temp._14 + Temp._13;
	u_plane.m_planes[4].y = Temp._24 + Temp._23;
	u_plane.m_planes[4].z = Temp._34 + Temp._33;
	u_plane.m_planes[4].w = Temp._44 + Temp._43;

	// Calculate far plane of frustum.
	u_plane.m_planes[5].x = Temp._14 - Temp._13;
	u_plane.m_planes[5].y = Temp._24 - Temp._23;
	u_plane.m_planes[5].z = Temp._34 - Temp._33;
	u_plane.m_planes[5].w = Temp._44 - Temp._43;
	return;
}


void FrustumClass::ConstructFrustumNew( XMMATRIX &ViewProj )
{

	XMStoreFloat4x4A(&Temp, ViewProj);

	// Calculate left plane of frustum.
//	_Res *Left;// = &u_plane.m_planes[0];
	Res.fres.x = Temp._14 + Temp._11;
	Res.fres.y = Temp._24 + Temp._21;
	Res.fres.z = Temp._34 + Temp._31;
	Res.fres.w = Temp._44 + Temp._41;
	u_plane.planes[0] = XMPlaneNormalize( Res.res );
	// Calculate right plane of frustum.
//	_Res *Right;// = &u_plane.m_planes[1];
	Res.fres.x = Temp._14 - Temp._11;
	Res.fres.y = Temp._24 - Temp._21;
	Res.fres.z = Temp._34 - Temp._31;
	Res.fres.w = Temp._44 - Temp._41;
	u_plane.planes[1] = XMPlaneNormalize( Res.res );
	// Calculate top plane of frustum.
//	_Res *Top;// = &u_plane.m_planes[2];
	Res.fres.x = Temp._14 - Temp._12;
	Res.fres.y = Temp._24 - Temp._22;
	Res.fres.z = Temp._34 - Temp._32;
	Res.fres.w = Temp._44 - Temp._42;
	u_plane.planes[2] = XMPlaneNormalize( Res.res );
	// Calculate bottom plane of frustum.
//	_Res *Bottom;// = &u_plane.m_planes[3];
	Res.fres.x = Temp._14 + Temp._12;
	Res.fres.y = Temp._24 + Temp._22;
	Res.fres.z = Temp._34 + Temp._32;
	Res.fres.w = Temp._44 + Temp._42;
	u_plane.planes[3] = XMPlaneNormalize( Res.res );
	// Calculate near plane of frustum.
//	_Res *Near;// = &u_plane.m_planes[4];
	Res.fres.x = Temp._13;
	Res.fres.y = Temp._23;
	Res.fres.z = Temp._33;
	Res.fres.w = Temp._43;
	u_plane.planes[4] = XMPlaneNormalize( Res.res );
	// Calculate far plane of frustum.
//	_Res *Far;// = &u_plane.m_planes[5];
	Res.fres.x = Temp._14 - Temp._13;
	Res.fres.y = Temp._24 - Temp._23;
	Res.fres.z = Temp._34 - Temp._33;
	Res.fres.w = Temp._44 - Temp._43;
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

		VecMMM = XMVectorSet(Position.x - radius, Position.y - radius, Position.z - radius,1.0f);
		VecPMM = XMVectorSet(Position.x + radius, Position.y - radius, Position.z - radius,1.0f);
		VecMPM = XMVectorSet(Position.x - radius, Position.y + radius, Position.z - radius,1.0f);
		VecPPM = XMVectorSet(Position.x + radius, Position.y + radius, Position.z - radius,1.0f);
		VecMMP = XMVectorSet(Position.x - radius, Position.y - radius, Position.z + radius,1.0f);
		VecPMP = XMVectorSet(Position.x + radius, Position.y - radius, Position.z + radius,1.0f);
		VecMPP = XMVectorSet(Position.x - radius, Position.y + radius, Position.z + radius,1.0f);
		VecPPP = XMVectorSet(Position.x + radius, Position.y + radius, Position.z + radius,1.0f);
		  
// Check if any one point of the cube is in the view frustum.
	for(int i=0; i<6; ++i) 
	{

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPPM);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPP);
		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

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

		VecMMM = XMVectorSet(Position.x - xSize, Position.y - ySize, Position.z - zSize,1.0f);
		VecPMM = XMVectorSet(Position.x + xSize, Position.y - ySize, Position.z - zSize,1.0f);
		VecMPM = XMVectorSet(Position.x - xSize, Position.y + ySize, Position.z - zSize,1.0f);
		VecPPM = XMVectorSet(Position.x + xSize, Position.y + ySize, Position.z - zSize,1.0f);
		VecMMP = XMVectorSet(Position.x - xSize, Position.y - ySize, Position.z + zSize,1.0f);
		VecPMP = XMVectorSet(Position.x + xSize, Position.y - ySize, Position.z + zSize,1.0f);
		VecMPP = XMVectorSet(Position.x - xSize, Position.y + ySize, Position.z + zSize,1.0f);
		VecPPP = XMVectorSet(Position.x + xSize, Position.y + ySize, Position.z + zSize,1.0f);

	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for(int i=0; i<6; ++i)
	{

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMM);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMM);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPM);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPPM);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMMP);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPMP);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecMPP);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		Res.res = XMPlaneDotCoord(u_plane.planes[i], VecPPP);

		if(Res.fres.x >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}