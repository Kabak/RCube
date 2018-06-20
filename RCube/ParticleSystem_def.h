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
	RANDOM = 0,	// Произвольно
	NORANDOM = 1// Не произвольно
};


// Particles Emitters types
enum ParticleSystemTypes
{
	BASIC		= 0,	// текстурная ситема частицы = BillBords квадратный
	SNOW		= 1,	// текстурная ситема частицы = BillBords квадратный
	PARTICLES	= 2,	// текстурная ситема частицы = BillBords квадратный ) для любых мелких частиц пыли, например, повисшей в воздухе.
	EXPLOSION	= 3,	// несколько систем частиц : SMOKE + FIRE + DIRT
	FIRE		= 4,	// текстурная ситема частицы не только BillBords для реалистичности
	SMOKE		= 5,	// текстурная ситема частицы = BillBords квадратный, возможно анимированный
	GALAXY		= 6,	// текстурная ситема частицы = BillBords квадратный
	STARS		= 7,	// ( MILKYWAY сюда же ) текстурная ситема частицы = BillBords квадратный
	COMETS		= 8,	// текстурная ситема частицы = BillBords квадратный
	WATER_DROPS = 9,	// объектная ситема частицы + BillBords квадратный с эфектами
	LIVES		= 10,	// текстурная ситема частицы = BillBords квадратный
	DIRT		= 11,	// объектная ситема частицы + Объёмные, вертексные объекты на расстоянии BillBords
	ROCKS		= 12,	// объектная ситема частицы + Объёмные, вертексные объекты на расстоянии BillBords
	GROUNDSTEP	= 13,	// несколько систем частиц : DIRT + PARTICLES
	ORBITAL		= 14,	// Bill / Objects Вращающиеся по орбите любой фигуры объекты
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
	XMFLOAT3 TorchPosition;	// Позиция факела
	int UX_Amount;			// Количество строк в текстуре анимации
	int VY_Amount;			// Количестко столбцов в текстуре анимации

	int FireFrameStart;		// Стартовый кадр Fire
	int SmokeFrameStart;	// Стартовый уадр Smoke

	int FireFlyFrameNumber;	// Кадр из которого нарезаются FireFly
	int FireFlyCutWidth;	// Размер нарезки куска для FireFly по ширине
	int FireFlyCutHeight;	// Размер нарезки куска для FireFly по высоте

	int FireFramesAmount;	// Реальное количество анимаций в текстуре Fire
	int SmokeFramesAmount;	// Реальное количество анимаций в текстуре Smoke
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
	float LifeTime;		// Сколько осталось жить
	float Velocity;		// 
	 UINT LightIndex;
	  int CurentFrame;
//	float Rotation;		// Поворот
	float Angel;		// Угол для расчёта синусоидальнго полёта частиц FireFly


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
	float LifeTime;		// Сколько осталось жить
	  int CurentFrame;
	float Rotation;		// Поворот в воздухе
	float Velocity;		// Скорость полёта
	float Angel;		// Угол для расчёта синусоидальнго полёта частиц FireFly
	 bool Active;		// Рисуется ли частица
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