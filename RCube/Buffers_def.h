#pragma once
#include "DirectXMath.h"

#ifdef RCube_DX11
#include "d3d11.h"
#endif // RCUBE_DX11

#ifdef RCube_DX12
#include "d3d12.h"
#endif //  RCUBE_DX12

using namespace DirectX;

enum LayoutTypes
{
	MODEL3D = 0,
	FLATOBJECT,
	BB_PARTICLES,
	NOLAYOUT = -1
};


// Структура привязки Layout к шейдерам
namespace
{
	int ShaderLayoutBind[] =
	{
		FLATOBJECT,		// CubeMap	// номер Layout в  LayOutTypes
		FLATOBJECT,		// Font 
		FLATOBJECT,		// KF2DObj 
		BB_PARTICLES,	// Snow Particles
		MODEL3D,		// ClusteredSM 
		NOLAYOUT,		// FXAA_vs			Нет Layout
	    MODEL3D,		// LightRenderSM 
		FLATOBJECT,		// FireAnimation 
		BB_PARTICLES,	// ParticleAnimated 
		BB_PARTICLES	// TorchFire3D
	};
}

// Flat Object Indexes
namespace
{
	UINT FlatObjectIndices[6] = { 0, 1, 2, 0, 3, 1 };
}


// Index Buffer data
typedef UINT Index_Type;


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
	XMFLOAT4 Position;	// из-за RCube_VecFloat234 в KFModel нельзя переделать на XMFLOAT3
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

// - Terrain

// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
struct PositionType
{
	RCube_VecFloat234 position;
	RCube_VecFloat234 rotation;
};
// -----------------   Instancing   --------------------------------


// https://stackoverflow.com/questions/10046485/error-lnk2005-already-defined
namespace
{
#ifdef RCube_DX11
	D3D11_INPUT_ELEMENT_DESC Model3D_Layout[] =
	{
		{ "POSITION",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,							 D3D11_INPUT_PER_VERTEX_DATA,   0 },  
		{ "TEXCOORD",	 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL",		 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TANGENT",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },

		{ "ObjPOSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,							 D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "ObjROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
#endif // RCube_DX11

#ifdef RCube_DX12
	D3D12_INPUT_ELEMENT_DESC Model3D_Layout [] =
	{
		{ "POSITION",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,							 D3D12_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD",	 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL",		 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_PER_VERTEX_DATA,   0 },
		{ "TANGENT",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_PER_VERTEX_DATA,   0 },

		{ "ObjPOSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,							 D3D12_INPUT_PER_INSTANCE_DATA, 1 },
		{ "ObjROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_PER_INSTANCE_DATA, 1 }
	};
#endif // RCube_DX12

}


// Flat 2D object vertex structure
// Shader Flat Object per Vertex data 
struct Vertex_FlatObject
{
	XMFLOAT3 Position;
	   float padding;
	XMFLOAT4 TexCoord;
	XMFLOAT4 ColorPicker;
};

namespace
{
#ifdef RCube_DX11
	D3D11_INPUT_ELEMENT_DESC FlatObject_Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// НЕ создаётся LAYOUT если следующую семантику назвать COLOR
		{ "OMGCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};
#endif // RCube_DX11
#ifdef RCube_DX12
	D3D12_INPUT_ELEMENT_DESC FlatObject_Layout [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,							  D3D12_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G3B32A322_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_PER_VERTEX_DATA, 0 },
		// НЕ создаётся LAYOUT если следующую семантику назвать COLOR
		{ "OMGCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_PER_VERTEX_DATA, 0 }

	};
#endif // RCube_DX12
}


// +++++++++++++++++   Define particles Instancing buffer types for Shaders    ++++++++

// BillBoard Colored, Animated Particle Instance
// Shader Particle data per Instance
struct BB_Particle_Instance
{
	BB_Particle_Instance () {};
	XMFLOAT3 position;
	   float Dummy;		// Draw particle ? 0.0f - NO   1.0 - YES
	XMFLOAT4 color;		// Color 
						// or ( for not colored particles )
						// x - размер BillBoard , z - угол поворота по Z , w - Alpha
	XMFLOAT4 NewTexCord;// Billboard animated texture coordinates
};
// -----------------   Define particles Instancing buffer types for Shaders   --------------------------------


// Snow , Fire Particles , TorchFire
namespace
{
#ifdef RCube_DX11
	D3D11_INPUT_ELEMENT_DESC BB_Particle_Instance_Layout[] =
	{
		{ "POSITION"	, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , 0							   , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "TEXCOORD"	, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA   , 0 },

		{ "POSITION"	, 1 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "COLOR"		, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "InsTEXCOORD" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
	};
#endif //RCube_DX11
#ifdef RCube_DX12
	D3D12_INPUT_ELEMENT_DESC BB_Particle_Instance_Layout [] =
	{
		{ "POSITION"	, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , 0							   , D3D12_INPUT_PER_VERTEX_DATA   , 0 },
		{ "TEXCOORD"	, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_PER_VERTEX_DATA   , 0 },

		{ "POSITION"	, 1 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_PER_INSTANCE_DATA , 1 },
		{ "COLOR"		, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_PER_INSTANCE_DATA , 1 },
		{ "InsTEXCOORD" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D12_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_PER_INSTANCE_DATA , 1 }
	};
#endif //RCube_DX12
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
