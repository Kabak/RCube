#pragma once
#include "stdafx.h"

#ifndef _PARTICLE_SYSTEM_DEFINITION_H_
#define _PARTICLE_SYSTEM_DEFINITION_H

#include <DirectXMath.h>
#include "Buffers_def.h"

using namespace DirectX;
using namespace std;


//  +++++++++++++++++++++++++++++++++        Particle System     +++++++++++++++++++++++++++++++++++++++
enum
{
	RANDOM = 0,	// �����������
	NORANDOM = 1// �� �����������
};


// Particles Emitters types
enum ParticleSystemTypes
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
	GROUNDSTEP	= 13,	// ��������� ������ ������ : DIRT + PARTICLES
	ORBITAL		= 14,	// Bill / Objects ����������� �� ������ ����� ������ �������
	ONEBUFFER	= 15,	// All in onetexture
};


enum EmitterTypes
{
	EM_BILLBOARD	= 0,// BillBoard Annimated / Not annimated

};

// Update particle functions types
enum UpdateParticleFunctions
{
	U_SNOWFALL	= 0	,
	U_SNOWFALL2		,

};

// Emitter particle functions types
enum EmitterParticleFunctions
{
	E_SNOWFALL = 0,
	E_SNOWFALL2,

};

// Orbital Shape
enum OrbitShape
{
	TRIAGLE		= 0,

};

struct ParticleSysInitData
{
	int ParticleSystemType;		// System Type from enum ParticleSystemTypes
    int ReserveEmittersCount;	// How much emitters should be reserved in memory
	bool GlobalSystem;			// Particles spread over whole level ( Frustum check should be skeeped )
	RCube_VecFloat234 Position;	// Position in 3D world
	RCube_VecFloat234 Rotation;	// Rotation in 3D world ( quaternion )

};


struct TorchFireSmoke
{
	XMFLOAT3 TorchPosition;	// ������� ������
	int UX_Amount;			// ���������� ����� � �������� ��������
	int VY_Amount;			// ���������� �������� � �������� ��������

	int FireFrameStart;		// ��������� ���� Fire
	int SmokeFrameStart;	// ��������� ���� Smoke

	int FireFlyFrameNumber;	// ���� �� �������� ���������� FireFly
	int FireFlyCutWidth;	// ������ ������� ����� ��� FireFly �� ������
	int FireFlyCutHeight;	// ������ ������� ����� ��� FireFly �� ������

	int FireFramesAmount;	// �������� ���������� �������� � �������� Fire
	int SmokeFramesAmount;	// �������� ���������� �������� � �������� Smoke
};


struct ParticleKillCondition
{
	bool XPosition; // If true, Particles kill by X position will be occured
	bool YPosition; // If true, Particles kill by Y position will be occured 
	bool ZPosition;	// If true, Particles kill by Z position will be occured
	RCube_VecFloat234 KillPosition; // Set real kill position

	float LifeTime; // If > 0, Particles going to be killed by LifeTime

//	bool Collision;	// If true, 

	int KillByVelocity;// Partticles going to be killed by Velocity under or grater KillVelocity
					   // -1 - KILL if particle velocity lower KillVelocity
					   //  1 - KILL if particle velocity bigger KillVelocity
					   //  0 or any other value NO KILL will be occured
	RCube_VecFloat234 KillVelocity; // particle going to be killed ( watch KillByVelocity )

	int KillBySize;	// If -1 or 1 , Particles going to be killed by Size.   0 - NO kill by size
					// -1 - KILL if particle lower KillSize
					//  1 - KILL if particle bigger Killsize
					//  0 or any other value NO KILL will be occured

	RCube_VecFloat234 KillSize;	// watch KillBySize 
};


struct ParticleEmittCondition
{
	RCube_VecFloat234  InitPosition;		 // Initial particles position
	RCube_VecFloat234  InitPositionDeviation;// Initial particles position deviation in the world ( random generator init )
	RCube_VecFloat234  Init3DParticleVelocity;// 3D velocity
	float InitParticleVelocity;				 // Init Particles velocity
	float InitParticleVelocityVariation;	 // Init Particles velocity variation
	float InitParticleSize;					 // Init Particles size

	float ParticlesPerSecond;		// How much particles going to emitted per second
};


// Data to init Particles Emitters
struct Emitter_Init_Data : ParticleEmittCondition , ParticleKillCondition
{
	  int TextureIndex;					// Texture Index for Bill Board Emitter
	float ParticlesPerSecond;			// How much particles emmit per second
	  int MaxParticles;					// Maximum particles number in system ( buffer size )
	  int MaxActiveParticles;			// Maximum really visible in scene particles ( Light )
	  int ShaderForDraw;				// Shader number to draw that emitter
	float PlayFPS;						// Animation desired FPS
	  int UX_Amount;					// Texture amount of frames UX
	  int VY_Amount;					// Texture amount of frames VY
	  int UpdateType;					// Choose formula for update particles by enum UpdateParticleFunctions
	  int EmitterType;					// EmitterTypes
	 bool ApplyLightSourceToParticlses;	// Use Lights with particles
};

// ++++++++++++++++   Particle parameters on CPU side   ++++++++++++++++++++++++++++++++
struct Particles_Data
{
	 bool Active;
	float LifeTime;		// ������� �������� ����
	float Velocity;		// 
	 UINT LightIndex;
	  int CurentFrame;
//	float Rotation;		// �������
	float Angel;		// ���� ��� ������� �������������� ����� ������ FireFly


};

// FireType1
/*
struct Particles_Data1 
{
	float Velocity;
	 bool Active;
	 UINT LightIndex;
	  int CurentFrame;
};

// Torch_FireSmoke1

struct Particles_Data2 
{
	float LifeTime;		// ������� �������� ����
	  int CurentFrame;
	float Rotation;		// ������� � �������
	float Velocity;		// �������� �����
	float Angel;		// ���� ��� ������� �������������� ����� ������ FireFly
	 bool Active;		// �������� �� �������
};
*/
// ++++++++++++++++   Particle parameters on CPU side   ++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++  System Types Definition   +++++++++++++++++++++++++++++


// Shader or GPU particle data
struct GPUParticle_Data
{
	XMFLOAT3 Position;
	   float RotateRate;
	XMFLOAT4 Rotation;	// Quaternion may be or just XYZ rotation.
    XMFLOAT3 Direction;
	   float Velocity;
	XMFLOAT4 Color;
	   float Mass;
	   float Life;
	   float Fade;
	   float active;	// 0.0 = not active
};

// +++++++++++++++++++++++++++++  System Types Definition   +++++++++++++++++++++++++++++
#endif