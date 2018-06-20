#pragma once
#include "stdafx.h"

#ifndef _PARTICLESYSTEMCONTROLER_H_
#define _PARTICLESYSTEMCONTROLER_H_

#include "D3DGlobalContext.h"

#include "ParticleSystem.h"
#include "ResourceManager.h"
#include "FPSTimers.h"
#include "Light_def.h"
#include "D3dClass.h"
#include "PhysXControler.h"

using namespace std;

class ParticleSystemController : public ParticleSystem
{
public:
	ParticleSystemController ( FPSTimers* _Timers, D3DClass* _EngineLight, PhysXControler* _PhysXCTRL, ResourceManager* _ResManager, ParticleSysInitData* _InitData );
   ~ParticleSystemController ();

   FPSTimers* Timers;
   D3DClass* EngineLight;
   PhysXControler* PhysXCTRL;
   ResourceManager* ResManager;
   
   PointLight *TempLight;

   ParticleSysInitData InitData;

    void ChangeActiveParticleAmount ( int EmitterIndex, int Amount );
	
	void CheckEmitterInitData ( Emitter_Init_Data* _Init_Data );

	void UpdateAABB ();	// Required after adding new Emitter ( for correct exclude from rendering )

	void Frame ();
	void EmitParticles ( Emitter* TempEmitter );
	void UpdateParticles ( Emitter* TempEmitter );
	void KillParticles ( Emitter* TempEmitter );

	// Находим расстояние от объекта до камеры
	void DistanceCalk ( Emitter* _Emitter, XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );

	// Apply to BB animated particle instance a texture coordinates
	// according to animated frame number
	void SetInstanceStartFrame ( Emitter* _Emitter, int FrameNumber, XMFLOAT4& Data2 );

	// Сортировка ПО ВОЗРАСТАНИЮ элементов массива по глубине
	template <class T> void QuickDepthSortAscending ( T* indices, float* depths, int lo, int hi );
	template <class T> void QuickDepthSortDescending ( T* indices, float* depths, int lo, int hi );

	int AddEmitter ( Emitter_Init_Data* _Init_Data );
	int DeleteEmitter ( int EmitterIndex );

	int GetActiveParticleAmmount ( Emitter* _Emitter );

// ++++++++++++++++++++     Fuctions     ++++++++++++++++++++++

// Update particles formulas
// Snow
	void SnowFallUpdate ( bool UpdateFrame, Emitter* _Emitter );
	void SnowFallEmitt ( Emitter* _Emitter );

};

#endif