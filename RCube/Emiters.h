#pragma once
#include "stdafx.h"
#include "DirectXMath.h"
#include "ParticleSystem_def.h"

#ifdef RCube_DX11
#include "DX11Buffers.h"
#endif 

class Emitter
{
public:
	Emitter ( Emitter_Init_Data* _Init_Data );
   ~Emitter ();

   bool GPU;				// GPU Particles
   bool Active;				// Set if Emitter need to be updated
   bool GlobalEmitter;		// Set if no AABB needed to check. Emitter in global 3D World ( Snow for example )
   bool KillOnDeactivate;	// ����������
   bool KillOnCompleted;	// 

 // For PS_BASIC particle system emitter
   int CreatedParticlesCount;
   int ActiveParticlesCount;

// For PS_TORCH particle system emitter
   int FlameInstanceActive;
   int SmokeInstanceActive;
   int FireFlyInstancesActive;

//	int UpdateType;			// Choose formula for update particles by enum UpdateParticleFunctions
//	int EmitterType;		// Choose formula for emit particles

	BB_Particle_Instance* BBInstances;

   Emitter_Init_Data    Init_Data;			// Initial emitter data
      Particles_Data*	CPU_Particles_Data;	// ��������� ������
    GPUParticle_Data*	GPU_Particles_Data; // ��������� GPU ������

	int Emitter_Buffers_Index;// ����� ������ ������ ����� �������� � ����� ������ �������

    int Type;	// Type of Emitter

//   RCube_VecFloat234 RelativePosition;	// Position Relative to ParticleSystem Position
//   RCube_VecFloat234 RelativeRotation;	// Rotation Relative to ParticleSystem Rotation
//   RCube_VecFloat234 AABB;// Dimension of Emitter

// Animations
   float OneFrameWidth;		// One animation frame width in texture from 0 - 1
   float OneFrameHeight;	// One animation frame Height in texture from 0 - 1
   float FireFlyWidth;		// ������ ������ �������� FireFly � �������� �� 0 - 1
   float FireFlyHeight;		// ������ ������ �������� FireFly � �������� �� 0 - 1


   int NumOfAnimeFrames;		// Total animation frames in texture
   int CurentFrame;				// Current animation frame
// Precalculated TextCoords for animations frames
   float *TextcordTop;
   float *TextcordLeft;

// Timers  For PS_BASIC 
   float AccumulatedTime;
   float TimePassed;			// ������� ������� ������

// Timers  For PS_TORCH
   float TimeToEmitFlames;
   float TimeToEmitSmoke;
   float TimeToEmitFireFly;
   float UpdateTimeFlames;	// ��� ����� ����� ��������� ��������� ��������
   float TimePassedFlames;
   float UpdateTimeSmoke;
   float UpdateTimeFireFly;

   // ������ �������� ��� ���������� �� ���������� �� ������
   int *FireInstIndNumber;
   // ������ ��������� Instance �� ������
   float *FireInstDistance;

// Distance from camera calk
   RCube_VecFloat234 Camera;
   RCube_VecFloat234 Object;

private :

	void Init_Data_Check ( Emitter_Init_Data* _Init_Data );
};
