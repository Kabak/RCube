#pragma once
#include "stdafx.h"

#include <d3d11.h>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
#include "ParticleSystemDefinitions.h"
#include "FPSTimers.h"
#include "D3DClass.h"
#include <vector>

using namespace std;
using namespace DirectX;

static const int PARTICLES_SYS_MAX = 100;

class ParticleSystemsManager
{
public:
	 ParticleSystemsManager();
	~ParticleSystemsManager();

	bool Initialize( HWND hwnd, D3DGlobalContext *D3DGC_Local, D3DClass *_EngineLight );
	void Shutdown();
	void Frame( FPSTimers &Timers );
	void Render();

// ++++++++++++++++++++++++++    НОВАЯ система частиц    +++++++++++++++++++++++++++++++++++++++++++
	// Создать систему частиц
	int CreateParticleSystem( int ParticeSysType,
							  int ParticlesCount,
							  int _InitType,
							  float RandomInitRadius = 5.0f
							  ); // 
								 // Удаляет систему частиц с конкретным номером
	void DeleteParticleSystem( int Number );
	// Удаляет все системы частиц
	void DeleteParticleSystems();
// --------------------------    НОВАЯ система частиц    ------------------------------------------



private:

	struct ActiveParticleSys
	{
					int	Type;	// Уникальный индекс системы частиц
	  int MAXParticlesAmaunt;	// Максимальное количество частиц
		     bool NeedUpdate;	// Нужно ли обновлять
				 bool Render;	// Нужно ли рисовать
				int	InitType;	// нужно ли инициализировать случайными числами
				float Radius;	// Радиус разброса случайных частиц
			   void *Pointer;	// Указатель на объект ParticleSystem
	};

	vector < ActiveParticleSys > CreatedParticleSystems;

};