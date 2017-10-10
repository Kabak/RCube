#pragma once
#include "stdafx.h"

#ifndef _PARTICLE_SYSTEM_DEFINITION_H_
#define _PARTICLE_SYSTEM_DEFINITION_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "VertexBuffers_def.h"

using namespace DirectX;

//  +++++++++++++++++++++++++++++++++        Particle System     +++++++++++++++++++++++++++++++++++++++
enum
{
	RANDOM = 0,	// �����������
	NORANDOM = 1		// �� �����������
};


// ���� ������ ������
enum
{
	BASIC		= 0,	// ���������� ������ ������� = BillBords ����������
	SNOW		= 1,	// ���������� ������ ������� = BillBords ����������
	PARTICLES	= 2,	// ���������� ������ ������� = BillBords ���������� ) ��� ����� ������ ������ ����, ��������, �������� � �������.
	EXPLOSION	= 3,	// ��������� ������ ������ : SMOKE + FIRE + DIRT
	FIRE		= 4,	// ���������� ������ ������� �� ������ BillBords ��� ��������������
	SMOKE		= 5,	// ���������� ������ ������� = BillBords ����������, �������� �������������
	GALAXY		= 6,	// ���������� ������ ������� = BillBords ����������
	STARS		= 7,	// ( MILKYWAY ���� �� ) ���������� ������ ������� = BillBords ����������
	COMETS		= 8,	// ���������� ������ ������� = BillBords ����������
	WATER_DROPS = 9,	// ��������� ������ ������� + BillBords ���������� � ��������
	LIVES		= 10,	// ���������� ������ ������� = BillBords ����������
	DIRT		= 11,	// ��������� ������ ������� + ��������, ���������� ������� �� ���������� BillBords
	ROCKS		= 12,	// ��������� ������ ������� + ��������, ���������� ������� �� ���������� BillBords
	GROUNDSTEP	= 13	// ��������� ������ ������ : DIRT + PARTICLES
};

// +++++++++++++++++   Define particles Instancing buffer types for Shaders    ++++++++

// BASIC
// �������� ���������� ������� ������������ � Vertex ������
struct ParticleShaderInstance_BASIC
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};
// BASIC ------------------------------------------------------

// �������� ���������� ������� ������������ � Vertex ������
struct ParticleShaderInstance_FIRE
{
	XMFLOAT3 position;
	float Dummy; // �������� �� ������� 0.0f - ���   1.0 - ��
	XMFLOAT4 color;
	XMFLOAT4 NewTexCord;
};

// TORCH-FIRE 
// �������� ���������� ������� ������������ � Vertex ������
struct ParticleShaderInstance_TORCH_FIRE
{
	XMFLOAT3 position;
	XMFLOAT4 Alpha;		// x - ������ BillBoard , z - ���� �������� �� Z , w - Alpha
	XMFLOAT4 NewTexCord;
};
// TORCH-FIRE -------------------------------------------------------


// SMOKE 
struct ParticleShaderInstance_SMOKE
{
	XMFLOAT3 position;
	XMFLOAT4 Alpha;
	XMFLOAT4 NewTexCord;
};
// SMOKE -------------------------------------------------------

// -----------------   Define particles Instancing buffer types for Shaders   --------------------------------



// ++++++++++++++++   Additional particle parameters   ++++++++++++++++++++++++++++++++

// �������������� ������ ��� ������� ������� ������
struct BasicType
{
	float Velocity;
	 bool active;
	 UINT LightIndex;
};


struct FireType1
{
	float Velocity;
	bool active;
	UINT LightIndex;
	int CurentFrame;
};


struct Torch_FireSmoke1
{
	float LifeTime;		// ������� �������� ����
//	float TimePased;	// ������� ������� ������ � ������� �������� �������
	int CurentFrame;
	float Rotation;		// ������� � �������
	float Velocity;		// �������� �����
	float Angel;		// ���� ��� ������� �������������� ����� ������ FireFly
//	bool Smoke;			// Smoke Instance if true
//	bool Fire;			// Fire Instance if true 
//	bool Flyer;			// Flyer Instance if true 
//	bool Heart;			// Heart Instance if true 
	bool Active;		// �������� �� �������
};

/*
struct Smoke1
{
	float LifeTime;		// ������� �������� ����
	  int CurentFrame;	
	float Rotation;		// ������� � �������
	float Velocity;		// �������� �����
};
*/

// �������������� ������ ��� ����
struct FireBillBord
{
	float size;					// ������ BillBord
	 UINT LightIndex;			// ����� ����� ����������� � ����������� Instance BillBord
	float LightAttenuationBegin;// ������ ����� ������
	float LightAttenuationEnd;	// ������ ����� �����
 XMFLOAT3 SmokeDirection;		// ����������� ����
};


struct AdvancedType
{
	XMFLOAT4 Velocity;
	XMVECTOR Rotation;
	XMFLOAT4 Color;
	XMFLOAT3 Position;
	float Mass;
	float Life;
	float Fade;
	float RotateRate;
	bool active;
};
// ++++++++++++++++   Additional particle parameters   ++++++++++++++++++++++++++++++++



// +++++++++++++++++++++++++++++  System Types Definition   +++++++++++++++++++++++++++++
// BASIC
struct BASIC_ParticlesSystem
{
			 XMFLOAT3   Deviation;			// ������� ������� ��� Emiter
			 XMFLOAT3	Translation;		// �������� ������������ ������ �������
				float   VelocityVariation;	// ������� �������� ��� Emiter
				float   ParticlesSize;		// ������ ������
				float   EmitsPerSecond;		// ������� ������ �������� � �������
				float   ParticlesMAXAmount;	// ������������ ���������� ������
			BasicType  *ParticlesData;		// ��������� �� �������������� ������ ��� ������
		   ParticleShaderInstance_BASIC  *ShaderInstances;	// ��������� �� ������ ������� ������ ������������ � ������� �������� �������   ParticleShaderInstanceType1
					   Vertex_FlatObject	 *VertexStructure;	// ��������� �� �������� �������� ������������� � ������						ParticleShaderVertexType1
					  ID3D11InputLayout  *Layout;
					  ID3D11VertexShader *VertexShader;
					  ID3D11PixelShader  *PixelShader;
					  ID3D11SamplerState *SamplerState;
				 void  *Emitter;			// ��������� �� ����������
				 void  *Updater;			// ��������� �� ������ ������
				 void  *Killer;				// E�������� �� ������������
   ID3D11ShaderResourceView *Texture;		// ��������� �� �������� ��� ������
				bool	Active;				// ������� �� ������� ��� ��������� � ��������


};


struct ParticleAdvancedType
{
	XMFLOAT3 Position;
	XMFLOAT4 Direction;
	XMFLOAT4 Rotation;
	XMFLOAT4 Color;
	   float Velocity;
	   float Mass;
	   float Life;
	   float Fade;
	   float RotateRate;
	    bool active;
};

// +++++++++++++++++++++++++++++  System Types Definition   +++++++++++++++++++++++++++++

//  +++++++++++++++++++++++++++++++++        Particle System     +++++++++++++++++++++++++++++++++++++++
// LAYOUTS
/* EXAPLE
polygonLayout[0].SemanticName = "POSITION";
polygonLayout[0].SemanticIndex = 0;
polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
polygonLayout[0].InputSlot = 0;
polygonLayout[0].AlignedByteOffset = 0;
polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
polygonLayout[0].InstanceDataStepRate = 0;
*/
/*
// BASIC Layout
D3D11_INPUT_ELEMENT_DESC BASIC_Layout[4] =
{
{ "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , 0 , D3D11_INPUT_PER_VERTEX_DATA , 0 },
{ "TEXCOORD" , 0 , DXGI_FORMAT_R32G32_FLOAT , 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA , 0 },
{ "POSITION" , 1 , DXGI_FORMAT_R32G32B32_FLOAT , 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
{	 "COLOR" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT , 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
};
*/

// LAYOUTS ------------------------------------------------------------------------------
#endif