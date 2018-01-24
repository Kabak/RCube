#pragma once
#include "stdafx.h"
#include <d3d11.h>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
#include "ParticleSystemDefinitions.h"
#include "FPSTimers.h"
#include "D3DClass.h"
#include "Light_def.h"
#include <vector>

#ifndef _TORCH_FIRE_PARTICLE_H_
#define _TORCH_FIRE_PARTICLE_H_

using namespace std;
using namespace DirectX;

class TorchFireParticles
{
public:
	 TorchFireParticles();
	~TorchFireParticles();

	// ���� ���������� ������ ������ ��� ��� ��������, �� FramesAmount ������ ���� �� ����
	// , � ����� ���������� �������� ������. ���� ����� �������� ��� ��������,�� FramesAmount = 0
	bool Initialize( HWND hwnd,
					 D3DGlobalContext *D3DGC_Local,

					 ID3D11ShaderResourceView* texture,
					 TorchFireSmoke TorchFIreSmokeInit,
					 D3DClass *_EngineLight
					 );


	bool InitFireSystem( HWND hwnd,
						 D3DGlobalContext *D3DGC,

						 ID3D11ShaderResourceView *_FireTexture,
						 //	ID3D11ShaderResourceView *_SmokeTexture,
						 //	ID3D11ShaderResourceView *_HeartTexture,
						 //	ID3D11ShaderResourceView *_ParticlesTexture,
						 //						 int  InstanceAmount,
						 D3DClass *_EngineLight
						 );

	void Shutdown();
	void Frame( FPSTimers &Timers );
	void Render();

	bool InitializeBuffers( ID3D11Device* );
	void ShutdownBuffers();

	void EmitFlameParticles( FPSTimers& Timers );
	void EmitSmokeParticles( FPSTimers& Timers );
	void EmitFireFlyParticles( FPSTimers& Timers );

	void UpdateFlameParticles( FPSTimers& Timers );
	void UpdateSmokeParticles( FPSTimers& Timers );
	void UpdateFireFlyParticles( FPSTimers& Timers );

	void UpdateLight( FPSTimers& Timers );
	
	void KillSmokeParticles();
	
	void KillFireFlyParticles();

	int GetActiveParticleAmmount();

	int GetInstanceCount();

	int ShaderForDraw = -1;

private:

	D3DGlobalContext *D3DGC_Local;

	D3DClass *EngineLight;
	PointLight *TempLight;

	bool InitializeParticleSystem();

	ID3D11ShaderResourceView *FireTexture;
	ID3D11ShaderResourceView *SmokeTexture;
	ID3D11ShaderResourceView *HeartTexture;
	ID3D11ShaderResourceView *ParticlesTexture;

	// ������ ������ ��� ������������� ����������� ������
	float m_particleSize; 

	// ����� ����� ����������� ��������
	int LightIndex;
	float TimeToUpdateLight;

	int RealFramesAmount;	// ��������
	
	int UX_Amount;			// ������ �������� Width X U
	int VY_Amount;			// ������ �������� Height Y V
	float OneFrameWidth;	// ������ ������ ����� � ���������� ����������� 0 - 1
	float OneFrameHeight;	// ������ ������ ����� � ���������� ����������� 0 - 1
	float FireFlyWidth;		// ������ ������ �������� FireFly � �������� �� 0 - 1
	float FireFlyHeight;	// ������ ������ �������� FireFly � �������� �� 0 - 1

	int NumOfAnimeFrams;	// ������� ����� ������ � ��������
	int FireFlyFrameNumber;	// ���� �� �������� ��������� FireFly
	int FireFlyCutWidth;	// ������ ������� ����� ��� FireFly �� ������
	int FireFlyCutHeight;	// ������ ������� ����� ��� FireFly �� ������
XMFLOAT4 FireFlyFrameCoord;	// ���������� ����� �� �������� ���������� FireFly ( ��� ��������� )

	float PlayFPSFlame;	// � ����� ��������� � ������� �������������� ����� 
//	float PlayFPSSmoke;	// � ����� ��������� � ������� �������������� ����� 

	float TimePassedFlames;	// ������� ������� ������ ��� ��������� ����� �������� FLAMES
//	float TimePassedSmoke;	// ������� ������� ������ ��� ��������� ����� �������� SMOKE
//	float TimeAliveSmoke;	// ������� ������� �������� �������

	float TimeToEmitFlames;	// ����� ��������� ������ FLAMES
	float UpdateTimeFlames;	// ������� ������� ������ ��� ���������� ���������� FLAMES

	float TimeToEmitSmoke;	// ����� ��������� ������ SMOKE
	float UpdateTimeSmoke;	// ������� ������� ������ ��� ���������� ���������� SMOKE

	float TimeToEmitFireFly;// ����� ��������� ������ FIREFLY
	float UpdateTimeFireFly;// ������� ������� ������ ��� ���������� ���������� FIREFLY

	// ������ ���������� ��������� ���������� ������ ��������
	float *TextcordTop;
	float *TextcordLeft;
	// ������ ���������� ��������� ���������� ������ FireFly
	float *FireFlyTop;
	float *FireFlyLeft;

// ���������� 
	XMFLOAT3 TorchPosition;			// ������� ������ ������ �������� �� ����������
	XMFLOAT3 FlameStartDeviation;	// ������� ��������� ������� ������ Flame
	XMFLOAT3 SmokeStartDeviation;	// ������� ��������� ������� ������ Smoke
	XMFLOAT3 FireFlyStartDeviation;	// ������� ��������� ������� ������ FireFly
	float VelocityVariation;		// ������� ��������

	// ��������� ����� ����� �������� �������
	// � ��� FLAME, ��� SMOKE, ��� FireFly
	ID3D11Buffer *IndexBuffer;

// ����� � ���
	int FlameInstancesAmount;	// ������������ ���������� Instance �������
	int FlameInstanceActive;	// ���������� �������� ������ Flame
	int FireFlyInstancesAmount;	// ���������� ������� ������ ����
	int FireFlyInstancesActive; // ���������� �������� ������
	int SmokeInstancesAmount;	// ������������ ���������� Instance ���� 
	int SmokeInstanceActive;	// ���������� �������� ������ ����


//	int *FlameInstIndNumber;	// ������ �������� ��� ���������� �� �������� �� ������
//	float *FlameInstDistance;	// ������ ���������� ��� ���������� �� �������� �� ������
//	int *SmokeInstIndNumber;	// ������ �������� ��� ���������� �� ���������� �� ������
//	float *SmokeInstDistance;	// ������ ��������� Instance �� ������

	int All_Instances_Amount;	// ����� ���� ������ ������� ����� ����������

	float DistanceAtStart;		// ���������� ����� ���������� ( �� ������� ������������ �������� ���� )
								// �� ������ �� �������� ��� ������
	float BetweenNextTimeFlame;	// ����� ����� �������� ����� ������� FLAME
	float BetweenNextTimeSmoke; // ����� ����� �������� ����� ������� SMOKE
	float BetweenNextTimeFireFly;// ����� ����� �������� ����� ������� FIREFLY

	int FlameStartFrame;		// ��������� ���� �������� FLAME
	int FlameFramesEnd;			// ������� ����� ������ FLAME
	int SmokeStartFrame;		// ��������� ���� �������� SMOKE
	int SmokeFramesEnd;			// ������� ����� ������ SMOKE
	int SmokeFramesAmount;		// ������� ������ � �������� SMOKE

	XMFLOAT4 InitAlpha;			// ��������� ������������ �������� ��� ���������

	// �������������� �������������� ��� ����������� ��������
	Torch_FireSmoke1* TorchFireFlamesAddData;
	Torch_FireSmoke1* TorchFireSmokeAddData;
	Torch_FireSmoke1* TorchFireFlyAddData;
	// ��� Vertex Buffer
	Vertex_FlatObject *FlameVertices;
	// ��� Instance Buffer
	ParticleShaderInstance_TORCH_FIRE *FlameInstances;
	ParticleShaderInstance_TORCH_FIRE *SmokeInstances;
	ParticleShaderInstance_TORCH_FIRE *FireFlyInstances;
	ParticleShaderInstance_TORCH_FIRE *ShaderInstanceAll;
	// ��� Inctance Buffer
	ID3D11Buffer *FlameInstanceBuffer;
	// ���������� ����� ����� �������� �������
	ID3D11Buffer *FlameVertexBuffer;

	void UpdateTorchPos (XMFLOAT3& TorchPosition);

// ����� --------------------------------------------------

	RCube_VecFloat34 Camera;
	RCube_VecFloat34 Object;

	// ������� ���������� �� ������� �� ������
	void DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );

	// ���������� �� ����������� ��������� ������� �� �������
	template <class T> void QuickDepthSortAscending( T* indices, float* depths, int lo, int hi );

	template <class T> void QuickDepthSortDescending( T* indices, float* depths, int lo, int hi );

	// ���������� XYZ
	void Gen_XYZ( XMFLOAT3 &Deviation, XMFLOAT3 &Result );

	// �������� ������� Instance
	void UpdatePosition( XMFLOAT3 &Direction, XMFLOAT3 &Result );

	// �������� ������� Instance
	void UpdateRotation( XMFLOAT3 &Direction, XMFLOAT3 &Result );

	// ��������� ������� FLAME
	void FlamesUpdateData  ( int &i );
	void SmokeUpdateData   ( int &i );
	void FireFlyUpdateData ( int &i );

	void SetInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 );
	
	// ��������� ����� �� ����� 8 * 16 ��� Firefly
	void SetFireFlyInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 );

};

#endif

