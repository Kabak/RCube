#pragma once
#include "stdafx.h"
#include <d3d11.h>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
#include "ParticleSystemDefinitions.h"
#include "FPSTimers.h"
#include "LightClass.h"
#include <vector>

#ifndef _FIRE_PARTICLE_SYSTEM_H_
#define _FIRE_PARTICLE_SYSTEM_H_

using namespace std;
using namespace DirectX;

class FireParticleSystem
{
public:
	FireParticleSystem();
   ~FireParticleSystem();

   // ���� ���������� ������ ������ ��� ��� ��������, �� FramesAmount ������ ���� �� ����
   // , � ����� ���������� �������� ������. ���� ����� �������� ��� ��������,�� FramesAmount = 0
   bool Initialize( HWND hwnd, 
	   D3DGlobalContext *D3DGC_Local, 
					ID3D10Blob* Blob, 
   ID3D11ShaderResourceView* texture, 
					   int _UX_Amount,							// ���������� ����� � �������� ��������
					   int _VY_Amount,							// ���������� �������� � �������� ��������
					   int FramesAmount,					// �������� ���������� �������� � �������� � ����� , ����� �������
			LightClass *_EngineLight 
	);


			bool InitFireSystem( HWND hwnd, 
				   D3DGlobalContext *D3DGC, 
						  ID3D10Blob* Blob, 
	ID3D11ShaderResourceView *_FireTexture, 
//	ID3D11ShaderResourceView *_SmokeTexture,
//	ID3D11ShaderResourceView *_HeartTexture,
//	ID3D11ShaderResourceView *_ParticlesTexture,
//						 int  InstanceAmount,
				  LightClass *_EngineLight 
			);

	void Shutdown();
	void Frame( FPSTimers &Timers );
	void Render();

	bool InitializeBuffers( ID3D11Device*, ID3D10Blob* Blob );
	void ShutdownBuffers();

	void EmitParticles( FPSTimers& Timers );
	void UpdateParticles( FPSTimers& Timers );
	void KillParticles();

	int GetActiveParticleAmmount();

	int GetInstanceCount();

	int GetMaxParticles();

	void ChangeActiveParticleAmount(int Amount);

private:

	D3DGlobalContext *D3DGC_Local;
	
	LightClass *EngineLight;
	LightClass::PointLight *TempLight;

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	ID3D11ShaderResourceView *FireTexture;
	ID3D11ShaderResourceView *SmokeTexture;
	ID3D11ShaderResourceView *HeartTexture;
	ID3D11ShaderResourceView *ParticlesTexture;

	int UX_Amount;			// ������ �������� Width X U
	int VY_Amount;			// ������ �������� Height Y V

	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;

	int maxActiveParticles; // ������������ ���������� ������ �� ������ ( ��� ����������� �������������� ���������� ������ )

	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;
	
	FireType1* m_particleList;

	void SetInstanceStartFrame(int FrameNumber, XMFLOAT4& Data2 );

	BillBordVertexes* m_vertices;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	ParticleShaderInstance_FIRE* instances;
	ID3D11Buffer* m_instanceBuffer;
	int m_instanceCount;
	// -----------------   Instancing   --------------------------------


// ��������
	int RealFramesAmount;
	// ������ ������ ����� � ���������� ����������� 0 - 1
	float OneFrameWidth;
	// ������ ������ ����� � ���������� ����������� 0 - 1
	float OneFrameHeight;

	// ������� ����� ������ � ��������
	int NumOfAnimeFrams;
	// ������� ����
	int CurentFrame;
	// � ����� ��������� � ������� �������������� ����� 
	float PlayFPS;
	// ������� ������� ������
	float TimePassed;

	// ������ ���������� ��
	float *TextcordTop;
	float *TextcordLeft;
	
	// ���������� �������� ���� 
	int FlameInstances;

	// ������ �������� ��� ���������� �� ���������� �� ������
	int *FireInstIndNumber;
	// ������ ��������� Instance �� ������
	float *FireInstDistance;
	
//	ParticleShaderInstance_FIRE* Unsortedinstance;

	RCube_VecFloat34 Camera;
	RCube_VecFloat34 Object;

	// ������� ���������� �� ������� �� ������
	void DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );

	// ���������� �� ����������� ��������� ������� �� �������
	template <class T> void QuickDepthSortAscending( T* indices, float* depths, int lo, int hi );

	template <class T> void QuickDepthSortDescending( T* indices, float* depths, int lo, int hi );


};

#endif