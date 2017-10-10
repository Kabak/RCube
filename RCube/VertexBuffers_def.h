#pragma once
#include "DirectXMath.h"
#include "d3d11.h"

using namespace DirectX;

enum LayoutTypes
{
	MODEL3D = 0,
	FLATOBJECT,
	PARTICLES_BASIC
};

namespace
{
	int ShaderLayoutBind[] =
	{
		1,// CubeMap	// номер Layout в  LayOutTypes
		1,// Font 
		1,// KF2DObj 
		1,// particle
		0,// ClusteredSM 
	   -1,// FXAA_vs =	// Ќет Layout
		0,// LightRenderSM 
		1,// FireAnimation 
		1,// FireParticle3D 
		1 // TorchFire3D
	};
}

// 3D model Vertex structure
struct Vertex_Model3D
{
/*	Vertex_Model3D () {}
	Vertex_Model3D ( float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		float tx, float ty, float tz )
		: pos ( x, y, z, 1.0f ), texCoord ( u, v ), normal ( nx, ny, nz ),
		tangent ( tx, ty, tz ) {}
*/
	XMFLOAT3 Position;	// из-за RCube_VecFloat34 в KFModel нельз€ переделать на XMFLOAT3
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	//XMFLOAT3 biTangent;
};


// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
struct PositionType
{
	RCube_VecFloat34 position;
	RCube_VecFloat34 rotation;
};
// -----------------   Instancing   --------------------------------


// https://stackoverflow.com/questions/10046485/error-lnk2005-already-defined
namespace
{
	D3D11_INPUT_ELEMENT_DESC Model3D_Layout[] =
	{
		{ "POSITION",	 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,							 D3D11_INPUT_PER_VERTEX_DATA,   0 },  
		{ "TEXCOORD",	 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL",		 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TANGENT",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "ObjPOSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,							 D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "ObjROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
}


// Flat 2D object vertex structure
struct Vertex_FlatObject
{
	XMFLOAT3 Position;
	XMFLOAT2 TexCoord;
};

namespace
{
	D3D11_INPUT_ELEMENT_DESC FlatObject_Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							   D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
}


namespace
{
	D3D11_INPUT_ELEMENT_DESC ParticlesBASIC_Layout[] =
	{
		{ "POSITION"	, 0 , DXGI_FORMAT_R32G32B32_FLOAT	, 0 , 0							   , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "TEXCOORD"	, 0 , DXGI_FORMAT_R32G32_FLOAT		, 0 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "POSITION"	, 1 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "COLOR"		, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "InsTEXCOORD" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
	};
}

