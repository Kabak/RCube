#include "stdafx.h"
#include "Emiters.h"


Emitter::Emitter ( Emitter_Init_Data* _Init_Data )
{
	TextcordTop = nullptr;
	TextcordLeft = nullptr;

	BBInstances = nullptr;

	Init_Data_Check ( _Init_Data );

	memcpy ( &Init_Data, _Init_Data, sizeof ( Emitter_Init_Data ) );

	CPU_Particles_Data = new Particles_Data[Init_Data.MaxParticles];

	memset ( CPU_Particles_Data, NULL, (sizeof ( Particles_Data ) * Init_Data.MaxParticles) );	// Require to set Active particles state to false

	switch ( Init_Data.EmitterType )
	{
		case EM_BILLBOARD :
		{
			size_t i = sizeof ( BB_Particle_Instance ) * Init_Data.MaxParticles;

			BBInstances = new BB_Particle_Instance[i];

			memset ( BBInstances, NULL, i );
			
			break;
		}

		default:;
	}

	OneFrameWidth = 1.0f / Init_Data.VY_Amount;
	OneFrameHeight = 1.0f / Init_Data.UX_Amount;

	TextcordTop = new float[Init_Data.VY_Amount];
	TextcordLeft = new float[Init_Data.UX_Amount];


	CurentFrame		= 0;
	TimePassed		= 0.0f;
	NumOfAnimeFrames = Init_Data.UX_Amount * Init_Data.VY_Amount;

	AccumulatedTime = 0.0f;

	ActiveParticlesCount = 0;

	// ”меньшаем количество кадров, потому что их индексы начинаютс€ с Ќ”Ћя 0 - 63
	--NumOfAnimeFrames;

	int UX = 0;
	int VY = 0;
	while ( VY < _Init_Data->VY_Amount )
	{
		TextcordTop[VY] = static_cast<float>( VY * OneFrameHeight );
		++VY;
	}

	while ( UX <  _Init_Data->UX_Amount )
	{
		TextcordLeft[UX] = static_cast<float>( UX * OneFrameWidth );
		++UX;
	}
}

Emitter::~Emitter ()
{
	switch ( Init_Data.EmitterType )
	{
		case EM_BILLBOARD:
		{
			RCUBE_ARR_DELETE ( BBInstances );
			break;
		}

		default:;
	}

	RCUBE_ARR_DELETE ( TextcordTop );
	RCUBE_ARR_DELETE ( TextcordLeft );
	RCUBE_ARR_DELETE ( CPU_Particles_Data );

}


void Emitter::Init_Data_Check ( Emitter_Init_Data* _Init_Data )
{
	_Init_Data->UX_Amount < 1 ? _Init_Data->UX_Amount = 1 : _Init_Data->UX_Amount;

	_Init_Data->VY_Amount < 1 ? _Init_Data->VY_Amount = 1 : _Init_Data->VY_Amount;

	_Init_Data->PlayFPS < 1 ? _Init_Data->PlayFPS = 30 : _Init_Data->PlayFPS;

	if ( _Init_Data->ApplyLightSourceToParticlses && _Init_Data->MaxActiveParticles > 3000 )
	{
		MessageBox ( nullptr, L"MaxActiveParticles > 3000. ApplyLightSourceToParticlses switched OFF", L"Warning", MB_OK );
		_Init_Data->ApplyLightSourceToParticlses = false;
	}

	// PS_TORCH
	_Init_Data->FireFrameStart < _Init_Data->FireFramesEnd ? _Init_Data->FireFramesEnd = 0 : _Init_Data->FireFramesEnd;
	_Init_Data->SmokeFrameStart < _Init_Data->SmokeFrameEnd ? _Init_Data->SmokeFrameEnd = 0 : _Init_Data->SmokeFrameEnd;
	// PS_TORCH
}