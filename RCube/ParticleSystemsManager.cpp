#include "stdafx.h"
#include "ParticleSystemDefinitions.h"
#include "ParticleSystemsManager.h"


ParticleSystemsManager::ParticleSystemsManager()
{
	CreatedParticleSystems.reserve( PARTICLES_SYS_MAX );
}


ParticleSystemsManager::~ParticleSystemsManager()
{
	DeleteParticleSystems();
	CreatedParticleSystems.clear();
}


int ParticleSystemsManager::CreateParticleSystem( int ParticeSysType,
											   int ParticlesCount,
											   int _InitType,
											   float RandomInitRadius )
{
	// Проверяем не пытаемся ли мы создать ещё одну систему частиц больше максимального количества 
	// возможных систем частиц
	//	assert( CreatedParticleSystems.size() < PARTICLES_SYS_MAX );
	if ( CreatedParticleSystems.size() >= PARTICLES_SYS_MAX )
		return -1;

	ActiveParticleSys NewSys;

	switch ( ParticeSysType )
	{
		case BASIC:
			NewSys.Pointer = new BASIC_ParticlesSystem;
			NewSys.Type = BASIC;
			break;

		case PARTICLES:
			NewSys.Pointer = new ParticleAdvancedType;
			NewSys.Type = PARTICLES;

		default:;
	}

	NewSys.NeedUpdate = false;
	NewSys.Render = false;
	NewSys.Radius = RandomInitRadius;
	NewSys.InitType = _InitType;
	NewSys.MAXParticlesAmaunt = ParticlesCount;

	CreatedParticleSystems.push_back( NewSys );

	return ( int ) CreatedParticleSystems.size() - 1;
}


void ParticleSystemsManager::DeleteParticleSystem( int Number )
{
	if ( Number <= ( int ) CreatedParticleSystems.size() )
		{
			delete  CreatedParticleSystems[Number].Pointer;
			CreatedParticleSystems.erase( CreatedParticleSystems.begin() + Number );
		}
}


void ParticleSystemsManager::DeleteParticleSystems()
{
	int Counter = ( int ) CreatedParticleSystems.size() - 1;

	while ( Counter >= 0 )
	{
		delete CreatedParticleSystems[Counter].Pointer;
		--Counter;
	}
	CreatedParticleSystems.clear();
}