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
	PS_BASIC	= 0,	// One texture milti images | one image texture ( BillBords )
	PS_TORCH	= 1,	// One texture multi images. 4 buffers drowing 1 Draw call  ( BillBords )
/*
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
*/
};


enum AnimationType
{
	EM_NO_ANIMATION			= 0,	// 0 = No animation  ( OneFrame Picture )
	EM_ANIMATED_FROM_0		= 1,	// 1 = Animated FlipBook. ( Multipicture texture ) , Animation start from 0 index frame
	EM_ANIMATED_RANDOM		= 2,	// 2 = Animated FlipBook. ( Multipicture texture ) , Animation start from random frame number
	EM_NO_ANIMATION_RANDOM	= 3,	// 3 = No animation		  ( Multipicture texture ) , Random Init frame
	EM_TORCH_ANIMATION		= 4,	// 4 = Fire animation  ( Multipicture texture ) , One draw call for all Emitters buffers in particle system 
};


enum EmitterTypes
{
	EM_BILLBOARD	= 0,	// BillBoard Particles type

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

enum KillCondition
{
	KILL_UNDER	= -1,
	NO_KILL		=  0,
	KILL_UPPER	=  1,
};


struct ParticleSysInitData
{
	int ParticleSystemType;		// System Type from enum ParticleSystemTypes
    int ReserveEmittersCount;	// How much emitters should be reserved in memory
	bool GlobalSystem;			// Particles spread over whole level ( Frustum check should be skeeped )
	RCube_VecFloat234 Position;	// Position in 3D world
	RCube_VecFloat234 Rotation;	// Rotation in 3D world ( quaternion )

};


struct TorchInctances
{
	XMFLOAT3 InctancePosition;
	XMFLOAT3 InctanceRotation;

};


struct TorchInitData
{
	TorchInctances *TorchInstances;// Torches data
	int InstancesAmount;	// Torches amount
	
	int StartFrame;
	int EndFrame;

	int FrameNumber;		// Frame that will be cut to draw fire flying particles
//	int FireFlyCutWidth;	// ������ ������� ����� ��� FireFly �� ������
//	int FireFlyCutHeight;	// ������ ������� ����� ��� FireFly �� ������

	int FlameInstanceAmount;// 8 / Amount of flame particle per one FireTorch
	float BetweenNextTimeFlame;// 0.5f / Amount of time between new flame particle going to emitted
//   int FlameInstanceActive;

	int SmokeInstancesAmount;// 80 / Amount of smoke particle per one FireTorch
	float BetweenNextTimeSmoke;// 0.2f / Amount of time between new smoke particle going to emitted
//    int SmokeInstanceActive;

	int FireFlyInstancesAmount;// 20 / Amount of Firefly particle per one FireTorch
	float BetweenNextTimeFireFly;// 0.3f / Amount of time between new firefly particle going to emitted
//    int FireFlyInstancesActive;

	int FireFrameStart;		// ��������� ���� Fire
	int FireFramesEnd;		// �������� ���� Fire

	int SmokeFrameStart;	// ��������� ���� Smoke
	int SmokeFrameEnd;		// �������� ���� Smoke

	int FireFlyFrameNumber;	// ���� �� �������� ���������� FireFly
	int FireFlyCutWidth;	// ������ ������� ����� ��� FireFly �� ������
	int FireFlyCutHeight;	// ������ ������� ����� ��� FireFly �� ������

	XMFLOAT3 FlameStartDeviation;// { 0.3f, 0.0f, 0.5f };	// ������� ��������� ������� ������ Flame
	XMFLOAT3 SmokeStartDeviation;// { 0.3f, 1.0f, 0.5f };	// ������� ��������� ������� ������ Smoke
	XMFLOAT3 FireFlyStartDeviation;// { 0.5f, 1.0f, 0.5f };	// ������� ��������� ������� ������ FireFly

	XMFLOAT4 InitAlpha;		// { 1.0f, 1.0f, -1.723598f, 0.05f };

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
	int KillXPosition; // If true, Particles kill by X position will be occured
					   // -1 - KILL if particle position X lower KillPosition X
					   //  1 - KILL if particle velocity X bigger KillVelocity X
					   //  0 or any other value NO KILL will be occured
	int KillYPosition; // If true, Particles kill by Y position will be occured 
					   // -1 - KILL if particle position Y lower KillPosition Y
					   //  1 - KILL if particle velocity Y bigger KillVelocity Y
					   //  0 or any other value NO KILL will be occured
	int KillZPosition; // If true, Particles kill by Z position will be occured
					   // -1 - KILL if particle position Z lower KillPosition Z
					   //  1 - KILL if particle velocity Z bigger KillVelocity Z
					   //  0 or any other value NO KILL will be occured

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


struct ParticleUpdateCondition
{
	bool UpdateVelocity;
	bool UpdatePosition;
	bool UpdateColour;
	bool UpdateSize;

	 int Animated;		// enum AnimationType
						// 0 = No animation  ( OneFrame Picture )
						// 1 = Animated FlipBook. ( Multipicture texture ) , Animation start from 0 index frame
						// 2 = Animated FlipBook. ( Multipicture texture ) , Animation start from random frame number
						// 3 = No animation		  ( Multipicture texture ) , Random Init frame
};


struct ParticleEmittCondition
{
	RCube_VecFloat234 InitPosition;			// Initial particles position
	RCube_VecFloat234 InitPositionDeviation;// Initial particles position deviation in the world ( random generator init )
	RCube_VecFloat234 Init3DParticleVelocity;// 3D velocity
	RCube_VecFloat234 InitColour;
	float InitVelocity;				 // Init Particles velocity
	float InitVelocityVariation;	 // Init Particles velocity variation
	float InitSize;					 // Init Particles size

	float ParticlesPerSecond;		// How much particles required to emitted per second
};


// Data to init Particles Emitters
struct Emitter_Init_Data : ParticleEmittCondition , ParticleKillCondition, ParticleUpdateCondition, TorchInitData
{
	  int TextureIndex;					// Texture Index for Bill Board Emitter
	float ParticlesPerSecond;			// How much particles emmit per second
	  int MaxParticles;					// Maximum particles number in system ( buffer size )
	  int MaxActiveParticles;			// Maximum really visible in scene particles ( Light )
	  int ShaderForDraw;				// Shader number to draw that emitter ( will be chosen auto by ParticleSystemController Class )
	float PlayFPS;						// Animation desired FPS
	  int UX_Amount;					// Texture amount of frames UX
	  int VY_Amount;					// Texture amount of frames VY
	  int UpdateFunction;				// Choose formula for update particles ( enum UpdateParticleFunctions )
	  int EmitterType;					// EmitterTypes
	  int EmittFunction;				// Choose formula for emitt particles ( enum EmitterParticleFunctions )
	 bool ApplyLightSourceToParticlses;	// Use Lights with particles
	 bool ApplyPhysX;					// Use PhysX with particles

	 float LightAttenuationBegin;
	 float LightAttenuationEnd;
	 RCube_VecFloat234 LightColour;

private:

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