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

// ++++++++++++++++++++++++++    ����� ������� ������    +++++++++++++++++++++++++++++++++++++++++++
	// ������� ������� ������
	int CreateParticleSystem( int ParticeSysType,
							  int ParticlesCount,
							  int _InitType,
							  float RandomInitRadius = 5.0f
							  ); // 
								 // ������� ������� ������ � ���������� �������
	void DeleteParticleSystem( int Number );
	// ������� ��� ������� ������
	void DeleteParticleSystems();
// --------------------------    ����� ������� ������    ------------------------------------------



private:

	struct ActiveParticleSys
	{
					int	Type;	// ���������� ������ ������� ������
	  int MAXParticlesAmaunt;	// ������������ ���������� ������
		     bool NeedUpdate;	// ����� �� ���������
				 bool Render;	// ����� �� ��������
				int	InitType;	// ����� �� ���������������� ���������� �������
				float Radius;	// ������ �������� ��������� ������
			   void *Pointer;	// ��������� �� ������ ParticleSystem
	};

	vector < ActiveParticleSys > CreatedParticleSystems;

};