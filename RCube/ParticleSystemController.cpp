#include "stdafx.h"
#include "ParticleSystemController.h"

ParticleSystemController::ParticleSystemController ( FPSTimers* _Timers, D3DClass* _EngineLight, PhysXControler* _PhysXCTRL, ResourceManager* _ResManager, ParticleSysInitData* _InitData )
{
	Timers		= _Timers;
	EngineLight = _EngineLight;
	PhysXCTRL	= _PhysXCTRL;
	ResManager	= _ResManager;
	
	TempLight	= new  PointLight;

	TempLight->attenuationBegin = 0.1f;
	TempLight->attenuationEnd = 10.5f;
	//	TempLight->color = { 1.0f, 0.7f, 1.0f };
	//	TempLight->position = { 0.0f, 0.0f, 0.0f };//;{ 0.0f, 10.0f, 00.0f }
	//	TempLight->direction = { 0.0f, 0.0f, 0.0f };//;{ 0.0f, -0.3f, -1.0f }
	TempLight->angel = 0.0f;
	TempLight->ShadowMapSliceNumber = -1;
	TempLight->LightID = -2;
	TempLight->Dummy = 1;
	//	TempLight->qtwvp = MyMat.XMF;
	TempLight->HaveShadow = false;

	CheckParticleSystemInitData ( _InitData );

	memcpy ( &InitData, _InitData, sizeof ( ParticleSysInitData ) );

	Emitters.reserve ( _InitData->ReserveEmittersCount );
/*
	switch ( _InitData->ParticleSystemType )
	{
		case PS_BASIC:
			Emitters.reserve ( _InitData->ReserveEmittersCount );
			break;

		case PS_TORCH :
			Emitters.reserve ( 4 );
			break;
	}
*/
	// Add Particle system to Render List
	ResManager->AddParticleSystem ( dynamic_cast< ParticleSystem* >( this ) );
}


ParticleSystemController::~ParticleSystemController()
{
	RCUBE_DELETE ( TempLight );

	int size = ( int ) Emitters.size ();
	int c = 0;
	while ( c < size )
	{
		DeleteEmitter ( Emitters[c]->Emitter_Buffers_Index );
		++c;
	}

	Emitters.clear ();
	// Remove yourself from render ParticleSystems in Resource Manager
	ResManager->DeleteParticleSystem ( ParticleSystem_Object_Index );
}


void ParticleSystemController::CheckParticleSystemInitData ( ParticleSysInitData* _InitData )
{
	_InitData->ReserveEmittersCount < 0 ? _InitData->ReserveEmittersCount = 0 : _InitData->ReserveEmittersCount;

}


void ParticleSystemController::Frame ( )
{
	int Size = (int)Emitters.size ();

	for (int i = 0; i < Size; i++ )
	{
		Emitter* TempEmitter = Emitters[i];

		KillParticles	( TempEmitter );

		if ( TempEmitter->ActiveParticlesCount < TempEmitter->Init_Data.MaxActiveParticles )
		EmitParticles	( TempEmitter );
		
		UpdateParticles ( TempEmitter );

		// Update Buffers
		switch ( TempEmitter->Init_Data.EmitterType )
		{
			case EM_BILLBOARD : 
				BB_Particles_Buffers* Buffer = ResManager->GetEmitters_BB_Buffers_By_Index ( TempEmitter->Emitter_Buffers_Index );
				Buffer->InstanceVBs->Update ( TempEmitter->BBInstances );
				break;
		}
	}

}


void ParticleSystemController::EmitParticles ( Emitter* TempEmitter )
{
		//+ CHOOSE PARTICLES EMITTER FUNCTION
		switch ( TempEmitter->Init_Data.EmittFunction )
		{
			case E_SNOWFALL:
				SnowFallEmitt ( TempEmitter );
				break;
		}
		//- CHOOSE PARTICLES EMITTER FUNCTION
}


void ParticleSystemController::UpdateParticles ( Emitter* TempEmitter )
{
	// 
	float j = 0;
	bool UpdateFrame = false;
	
		if ( Timers->FpsRate > 0 )
			TempEmitter->TimePassed += 1.0f / float ( Timers->FpsRate );

		if ( TempEmitter->TimePassed > 1.0f / TempEmitter->Init_Data.PlayFPS ) //  Скорость смены кадров 24 кадра в секунду по умолчанию 
		{
			TempEmitter->TimePassed = 0.0f;
			UpdateFrame = true;
		}

		//+ CHOOSE PARTICLES UPDATE FUNCTION
		switch ( TempEmitter->Init_Data.UpdateFunction )
		{
			case U_SNOWFALL:
				SnowFallUpdate ( UpdateFrame, TempEmitter );
				break;

		}
		//- CHOOSE PARTICLES UPDATE FUNCTION

}

void ParticleSystemController::DiactivateParticle ( Emitter* _Emitter, Particles_Data* Part, BB_Particle_Instance* Part2 )
{
	Part->Active = false;	// CPU side array set particle OFF
	Part2->Dummy = 0.0f;	// Shader particle not draw
	--_Emitter->ActiveParticlesCount;

	// Remove Light source if used
	if ( _Emitter->Init_Data.ApplyLightSourceToParticlses )
	{
		EngineLight->FreeLightSource ( Part->LightIndex );
	}
}


void ParticleSystemController::KillParticles ( Emitter* _Emitter )
{
	int Temp = _Emitter->CreatedParticlesCount - 1;

	// Y Position
	// If Particles should be killed by Y Position
	if ( _Emitter->Init_Data.KillYPosition == KILL_UNDER )
	{

		// Kill all the particles that have gone below a certain height range.
		for ( int i = 0; i < _Emitter->CreatedParticlesCount; ++i )
		{
			Particles_Data* Part = &_Emitter->CPU_Particles_Data[i];
			BB_Particle_Instance* Part2 = &_Emitter->BBInstances[i];


			if ( Part->Active == true && Part2->position.y < _Emitter->Init_Data.KillPosition.Fl4.y )
			{

				// DiactivateParticle ( _Emitter, Part, Part2 );

				Part->Active = false;	// CPU side array set particle OFF
				Part2->Dummy = 0.0f;	// Shader particle not draw
				--_Emitter->ActiveParticlesCount;

				// Remove Light source if used
				if ( _Emitter->Init_Data.ApplyLightSourceToParticlses )
				{
					EngineLight->FreeLightSource ( Part->LightIndex );
				}

				// If last particle in list get not active - decreasing rendering particles amount
				i == Temp ? --_Emitter->CreatedParticlesCount : _Emitter->CreatedParticlesCount;

			}
		}
	}
}


void ParticleSystemController::SetInstanceStartFrame ( Emitter* _Emitter, int FrameNumber, XMFLOAT4& Data2 )
{
	int Column;
	int Line;

	Line = FrameNumber / _Emitter->Init_Data.UX_Amount;

	Line > 0 ? Column = FrameNumber - Line * _Emitter->Init_Data.UX_Amount : Column = FrameNumber;

	float Top = _Emitter->TextcordTop[Line];
	float Left = _Emitter->TextcordLeft[Column];

	Data2 =
	{
		Top,
		Left,
		Top + _Emitter->OneFrameHeight,
		Left + _Emitter->OneFrameWidth
	};
}


void ParticleSystemController::CheckEmitterInitData ( Emitter_Init_Data* _Init_Data )
{
	_Init_Data->MaxActiveParticles > _Init_Data->MaxParticles ? _Init_Data->MaxActiveParticles = _Init_Data->MaxParticles :	_Init_Data->MaxActiveParticles;

	switch ( InitData.ParticleSystemType )
	{
		case PS_BASIC :
			_Init_Data->EmitterType = EM_BILLBOARD;
			break;

		case PS_TORCH :
			_Init_Data->EmitterType = EM_BILLBOARD;
			_Init_Data->Animated	= EM_TORCH_ANIMATION;
			
			_Init_Data->FrameNumber			< 1 ? _Init_Data->FrameNumber		= 1 : _Init_Data->FrameNumber;		// Frame number to cut should be in range
			_Init_Data->FireFlyCutWidth		< 1 ? _Init_Data->FireFlyCutWidth	= 1 : _Init_Data->FireFlyCutWidth;	// Cut amount should be in range
			_Init_Data->FireFlyCutHeight	< 1 ? _Init_Data->FireFlyCutHeight	= 1 : _Init_Data->FireFlyCutHeight;	// Cut amount should be in range
			break;


	} 

	switch ( _Init_Data->Animated )
	{
		case EM_NO_ANIMATION :
			_Init_Data->ShaderForDraw = ResManager->GetShaderIndexByName ( L"particle" );
			// One frame = whole texture
			_Init_Data->UX_Amount = 1;
			_Init_Data->VY_Amount = 1;
			break;

		case EM_ANIMATED_FROM_0 :
			_Init_Data->ShaderForDraw = ResManager->GetShaderIndexByName ( L"ParticleAnimated" );
			break;

		case EM_ANIMATED_RANDOM :
			_Init_Data->ShaderForDraw = ResManager->GetShaderIndexByName ( L"ParticleAnimated" );
			break;

		case EM_NO_ANIMATION_RANDOM :
			_Init_Data->ShaderForDraw = ResManager->GetShaderIndexByName ( L"ParticleAnimated" );
			break;

		case EM_TORCH_ANIMATION:
			_Init_Data->ShaderForDraw = ResManager->GetShaderIndexByName ( L"TorchFire3D" );
			break;
	}
}


int   ParticleSystemController::AddEmitter ( Emitter_Init_Data* _Init_Data )
{
	int Index = -1;

	CheckEmitterInitData ( _Init_Data );

	Emitter* NewEmitter = new Emitter ( _Init_Data );

	TempLight->attenuationBegin = _Init_Data->LightAttenuationBegin;
	TempLight->attenuationEnd = _Init_Data->LightAttenuationEnd;
	TempLight->color = _Init_Data->LightColour.Fl3;

	// Create buffer according EmitterType
	switch ( _Init_Data->EmitterType )
	{
		case EM_BILLBOARD:
		{
			// Save emitter buffers index in the emitter
			NewEmitter->Emitter_Buffers_Index = ResManager->Create_Emitter_BB_Buffers ( CPU_ACCESS_BUFFER, _Init_Data->MaxParticles, _Init_Data->TextureIndex, NewEmitter );
			NewEmitter->Active = true;
			break;
		}

		default:;
	}

	Emitters.push_back ( NewEmitter );

	Index = ( int ) Emitters.size () - 1;

	return Index;
}


int  ParticleSystemController::DeleteEmitter ( int EmitterIndex )
{
	int Size = ( int ) Emitters.size ();

	Emitter* TempEmitter = Emitters[EmitterIndex];

	if ( EmitterIndex < Size )
	{
		switch ( TempEmitter->Init_Data.EmitterType )
		{
			case EM_BILLBOARD:
			{
				ResManager->Delete_Emitter_BB_Buffer ( TempEmitter->Emitter_Buffers_Index );
				break;
			}

			default:;
		}

		RCUBE_DELETE ( Emitters[EmitterIndex] );
	}


	return -1;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//									FUNCTIONS
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ParticleSystemController::SnowFallUpdate ( bool UpdateFrame, Emitter* _Emitter )
{
	float Variable1 = 0.0f;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for ( int i = 0; i < _Emitter->CreatedParticlesCount; ++i )
	{
		if ( Variable1 > XM_2PI )
			Variable1 = 0;
		Particles_Data *Part = &_Emitter->CPU_Particles_Data[i];
		BB_Particle_Instance *Part2 = &_Emitter->BBInstances[i];
		Part2->position.y = Part2->position.y - Part->Velocity * ( Timers->FrameTime );
		Part2->position.x = Part2->position.x + cos ( Variable1 ) * 0.0006f;
		Part2->position.z = Part2->position.z + sin ( Variable1 ) * 0.0006f;
//		Part2->position.z = Part2->position.z + sin( Variable1 ) * Part->Velocity * 0.05;
		Variable1 += 0.0174533f;

		if ( Part->Active )
		{

// update Light pos
			if ( _Emitter->Init_Data.ApplyLightSourceToParticlses )
			{
				EngineLight->ChangeLightPos ( Part->LightIndex, Part2->position );
			}


// +++   ANIMATION   +++
			if ( (_Emitter->Init_Data.Animated == EM_ANIMATED_RANDOM || _Emitter->Init_Data.Animated == EM_ANIMATED_FROM_0 ) && UpdateFrame )
			{
				Part->CurentFrame == _Emitter->NumOfAnimeFrames ? Part->CurentFrame = 0 : ++Part->CurentFrame;
				SetInstanceStartFrame ( _Emitter, Part->CurentFrame, Part2->NewTexCord );
			}
// ---   ANIMATION   ---

			// Заполняем массив расстояний частиц от камеры
			//		DistanceCalk( Part2->position, D3DGC_Local->CameraPosition, FireInstDistance[i] );
			// Сохраняем индексы Instance в массиве индексов
			//		FireInstIndNumber[i] = i;
		}
		//	if ( CreatedParticlesCount > 1 )
		{
			//		QuickDepthSortDescending( FireInstIndNumber, FireInstDistance, 0, CreatedParticlesCount - 1 );

		}

		}
}


void ParticleSystemController::SnowFallEmitt ( Emitter* _Emitter )
{
	bool emitParticle = false;

	bool found;
	//  Обновление данные системы частиц 
	Particles_Data	Data;
	// Позиция и цвет частицы обновляется в массиве который отправляется в шейдер
	BB_Particle_Instance Data1;

	int index;

	RCube_VecFloat234 Random, Temp;

	// Increment the frame time.
	if ( Timers->FrameTime < 1.0f )
		_Emitter->AccumulatedTime += Timers->FrameTime;

	// Check if it is time to emit a new particle or not.
	if ( _Emitter->AccumulatedTime > ( 1.0f / _Emitter->Init_Data.ParticlesPerSecond ) )
	{
		_Emitter->AccumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ( ( emitParticle == true ) && ( _Emitter->ActiveParticlesCount < _Emitter->Init_Data.MaxParticles ) )
	{
		++_Emitter->ActiveParticlesCount;

// +++   ANIMATION   +++
		Data.CurentFrame = 0;

		if ( _Emitter->Init_Data.Animated == EM_ANIMATED_RANDOM || _Emitter->Init_Data.Animated == EM_NO_ANIMATION_RANDOM )
		{
			int FrameNumber = int((( float ) rand () / RAND_MAX) * _Emitter->NumOfAnimeFrames);
				Data.CurentFrame = FrameNumber;
				SetInstanceStartFrame ( _Emitter, FrameNumber, Data1.NewTexCord );
		}
// ---   ANIMATION   ---

		if ( _Emitter->CreatedParticlesCount < _Emitter->Init_Data.MaxActiveParticles )
			++_Emitter->CreatedParticlesCount;

		Random.Fl4.w = 1.0f;
		// Now generate the randomized particle properties.
		Random.Fl3.x = ( float ) rand () / RAND_MAX - ( float ) rand () / RAND_MAX;
		Random.Fl3.y = ( float ) rand () / RAND_MAX - ( float ) rand () / RAND_MAX;
		Random.Fl3.z = ( float ) rand () / RAND_MAX - ( float ) rand () / RAND_MAX;
		Temp.Vec = Random.Vec * _Emitter->Init_Data.InitPositionDeviation.Vec + _Emitter->Init_Data.InitPosition.Vec;
		Data1.position = Temp.Fl3;

		Data.Velocity = _Emitter->Init_Data.InitVelocity + ( ( ( float ) rand () - ( float ) rand () ) / RAND_MAX ) * _Emitter->Init_Data.InitVelocityVariation;

		Random.Fl3.x = ( float ) rand () / RAND_MAX ;
		Random.Fl3.y = ( float ) rand () / RAND_MAX ;
		Random.Fl3.z = ( float ) rand () / RAND_MAX ;
		Random.Fl4.w = 1.0f;

		Data1.color = Random.Fl4;

		Data1.Dummy = 1.0f;	// Set particle to render in Shader

		Data.Active = true;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.

		found = false;

		for (index = 0; index < _Emitter->CreatedParticlesCount; ++index )
		{
			if ( _Emitter->CPU_Particles_Data[index].Active == false )
			{
				found = true;
				break;
			}
		}

		if ( !found )
			return;


		if ( _Emitter->Init_Data.ApplyLightSourceToParticlses )
		{
			TempLight->position = Data1.position;
			RCube_VecFloat234 TempColor;
			TempColor.Fl4 = Data1.color;
			TempLight->color = TempColor.Fl3;

			Data.LightIndex = EngineLight->AddLightSource ( *TempLight );
		}

		if ( _Emitter->Init_Data.EmitterType == EM_BILLBOARD )
		{
			// Now insert it into the particle array in the correct depth order.
			_Emitter->CPU_Particles_Data[index] = Data;

			_Emitter->BBInstances[index] = Data1;
		}

	}
}


void ParticleSystemController::ChangeActiveParticleAmount (int EmitterIndex, int Amount )
{
	int Size = ( int ) Emitters.size ();
	if ( EmitterIndex < Size )
	{
		Emitter* TempEmitter = Emitters[EmitterIndex];
		
		TempEmitter->Init_Data.MaxParticles >= Amount ? TempEmitter->Init_Data.MaxActiveParticles = Amount
													  : TempEmitter->Init_Data.MaxParticles;
	}
}


void ParticleSystemController::DistanceCalk ( Emitter* _Emitter, XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore )
{
	// http://stackoverflow.com/questions/10291862/what-is-the-best-way-to-get-distance-between-2-points-with-directxmath

	RCube_VecFloat234 Result;

	_Emitter->Camera.Fl3 = Camerapos;
	_Emitter->Object.Fl3 = ObjPos;

	Result.Vec = XMVector3Length ( XMVectorSubtract ( _Emitter->Camera.Vec, _Emitter->Object.Vec ) );

	if ( Result.Fl3.x < 0.0f )
		DistanceStore = -Result.Fl3.x;
	else
		DistanceStore = Result.Fl3.x;
}


template <class T> void ParticleSystemController::QuickDepthSortAscending ( T* indices, float* depths, int lo, int hi )
{
	//http://kvodo.ru/quicksort.html
	//  lo is the lower index, hi is the upper index
	//  of the region of array a that is to be sorted
	int i = lo, j = hi;
	float h;
	T index;
	float x = depths[( lo + hi ) / 2];

	//  partition
	do
	{
		while ( depths[i] < x ) i++;
		while ( depths[j] > x ) j--;
		if ( i <= j )
		{
			h = depths[i]; depths[i] = depths[j]; depths[j] = h;
			index = indices[i]; indices[i] = indices[j]; indices[j] = index;
			i++; j--;
		}
	}
	while ( i <= j );

	//  recursion
	if ( lo < j ) QuickDepthSortAscending ( indices, depths, lo, j );
	if ( i < hi ) QuickDepthSortAscending ( indices, depths, i, hi );


}


template <class T> void ParticleSystemController::QuickDepthSortDescending ( T* indices, float* depths, int lo, int hi )
{
	//http://kvodo.ru/quicksort.html
	//  lo is the lower index, hi is the upper index
	//  of the region of array a that is to be sorted
	int i = lo, j = hi;
	float h;
	T index;
	float x = depths[( lo + hi ) / 2];

	//  partition
	do
	{
		while ( depths[i] > x ) i++;
		while ( depths[j] < x ) j--;
		if ( i <= j )
		{
			h = depths[j]; depths[j] = depths[i]; depths[i] = h;
			index = indices[j]; indices[j] = indices[i]; indices[i] = index;
			i++; j--;
		}
	}
	while ( i <= j );

	//  recursion
	if ( lo < j ) QuickDepthSortDescending ( indices, depths, lo, j );
	if ( i < hi ) QuickDepthSortDescending ( indices, depths, i, hi );


}