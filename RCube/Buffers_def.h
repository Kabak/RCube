#pragma once
#include "DirectXMath.h"
#include "d3d11.h"

using namespace DirectX;

enum LayoutTypes
{
	MODEL3D = 0,
	FLATOBJECT,
	FIRE_PARTICLES,
	TORCHFIRE_PARTICLES,
	NOLAYOUT = -1
};

// —истемные константные буферы - их индексы жЄстко закреплены ( пока не используетс€ )
enum ConstantBuffers_Types
{
	GLOBAL_CONSTANTS = 0,
	FXAA_CONSTANTS,
	LIGHT_TEST_PERFRAME_CONSTANTS,


};


// —труктура прив€зки Layout к шейдерам
namespace
{
	int ShaderLayoutBind[] =
	{
		FLATOBJECT,		// CubeMap	// номер Layout в  LayOutTypes
		FLATOBJECT,		// Font 
		FLATOBJECT,		// KF2DObj 
		FIRE_PARTICLES, // Snow Particles
		MODEL3D,		// ClusteredSM 
		NOLAYOUT,		// FXAA_vs			Ќет Layout
	    MODEL3D,		// LightRenderSM 
		FLATOBJECT,		// FireAnimation 
		FIRE_PARTICLES,	// FireParticle3D 
		TORCHFIRE_PARTICLES	// TorchFire3D
	};
}

namespace
{
	UINT FlatObjectIndices[6] = { 0, 1, 2, 0, 3, 1 };
}

typedef UINT Index_Type;
/*
struct Index_Type
{
	UINT Index;
};
*/

// 3D model Vertex structure
struct Vertex_Model3D
{
/*	Vertex_Model3D () {}
	Vertex_Model3D ( float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		float tx, float ty, float tz )
		: Position ( x, y, z ), TexCoord ( u, v ), Normal ( nx, ny, nz ),
		Tangent ( tx, ty, tz ) {}
*/
	XMFLOAT3 Position;	// из-за RCube_VecFloat34 в KFModel нельз€ переделать на XMFLOAT3
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	//XMFLOAT3 biTangent;
};


// + Terrain

struct PointCoord
{
	int x;
	int z;

	PointCoord ()
	{
		x = 0;
		z = 0;
	}
};

struct TerrainPositionType
{
	XMFLOAT3 position;
	float padding;
	XMFLOAT4 rotation;
};

struct TerrainVectorType
{
	float x, y, z;
};

// - Terrain

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


// Colour_Particles_Instance structure
struct Colour_Particles_Instance
{
	XMFLOAT4 Position;
	XMFLOAT4 Colour;
	XMFLOAT4 InstanceTexCoord;
};


// Snow , Fire Particles
namespace
{
	D3D11_INPUT_ELEMENT_DESC FireParticles_Layout[] =
	{
		{ "POSITION"	, 0 , DXGI_FORMAT_R32G32B32_FLOAT	, 0 , 0							   , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "TEXCOORD"	, 0 , DXGI_FORMAT_R32G32_FLOAT		, 0 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "POSITION"	, 1 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "COLOR"		, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "InsTEXCOORD" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
	};
}

// TorchFire_Particles_Instance structure
struct TorchFire_Particles_Instance
{
	XMFLOAT3 Position;
	XMFLOAT4 Colour;
	XMFLOAT4 InstanceTexCoord;
};
// TorchFire Particles
namespace
{
	D3D11_INPUT_ELEMENT_DESC TorchFire_Layout[5] =
	{
		{ "POSITION"	, 0 , DXGI_FORMAT_R32G32B32_FLOAT	, 0 , 0							   , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "TEXCOORD"	, 0 , DXGI_FORMAT_R32G32_FLOAT		, 0 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "POSITION"	, 1 , DXGI_FORMAT_R32G32B32_FLOAT	, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "COLOR"		, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "InsTEXCOORD" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
	};

}


// Global Constant Buffer Structure
struct ConstantBufferData
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
	XMMATRIX ViewProjection;
	XMMATRIX Ortho;
	XMMATRIX ScaleMatrix;
	XMFLOAT3 cameraPosition;
	float padding;
	XMFLOAT4 TransposedCameraRotation2;

};


// Constant buffer
struct CB_FXAA
{
	XMVECTOR m_fxaa;
};


struct CB_ShadowMap
{
	XMMATRIX LightViewProjection;
};


/*
// »спользуетс€ дл€ передачи массива данных при рисовании текстом на текстуре
// смотри D3DGlobalContext.h
struct TextOnTextureData
{
	IDWriteTextFormat* textFormat;
	XMFLOAT4 fonColour;
	XMFLOAT4 colour;
	XMFLOAT4 rectSize;

};
*/