#pragma once
#include "stdafx.h"
#include <d3d11.h>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
#include "ParticleSystem_def.h"
#include "FPSTimers.h"
#include "D3DClass.h"
#include "Light_def.h"
#include "Buffers_def.h"
#include <vector>

#ifndef _SNOWFALL_PARTICLES_H_
#define _SNOWFALL_PARTICLES_H_

using namespace std;
using namespace DirectX;

class SnowFallParticles
{
public:
	SnowFallParticles();
	~SnowFallParticles();

	// ���� ���������� ������ ������ ��� ��� ��������, �� FramesAmount ������ ���� �� ����
	// , � ����� ���������� �������� ������. ���� ����� �������� ��� ��������,�� FramesAmount = 0
	bool Initialize( HWND hwnd,
					 D3DGlobalContext *D3DGC_Local,
					 ID3D11ShaderResourceView* texture,
					 int _UX_Amount,							// ���������� ����� � �������� ��������
					 int _VY_Amount							// ���������� �������� � �������� ��������
//					 int FramesAmount					// �������� ���������� �������� � �������� � ����� , ����� �������
					 );


	void Shutdown();
	void Frame( FPSTimers &Timers );
	void Render();

	bool InitializeBuffers( ID3D11Device* );
	void ShutdownBuffers();

	void EmitParticles( FPSTimers& Timers );
	void UpdateParticles( FPSTimers& Timers );
	void KillParticles();

	int GetActiveParticleAmmount();

	int ShaderForDraw = -1;

private:

	D3DGlobalContext *D3DGC_Local;

	void InitializeParticleSystem();
	void ShutdownParticleSystem();

	ID3D11ShaderResourceView *FireTexture;


	int UX_Amount;			// ������ �������� Width X U
	int VY_Amount;			// ������ �������� Height Y V

	float ParticlesPositionDeviationX, ParticlesPositionDeviationY, ParticlesPositionDeviationZ;
	float ParticleVelocity, ParticleVelocityVariation;
	float ParticleSize, ParticlesPerSecond;
	int MaxParticles;

	//  int Amount_Active;
	int CreatedParticlesCount;
	int ActiveParticlesCount;
	int maxActiveParticles;
	float AccumulatedTime;

	Particles_Data* m_particleList;

	void SetInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 );

	Vertex_FlatObject* m_vertices;

	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	BB_Particle_Instance* instances;
	ID3D11Buffer* m_instanceBuffer;
	int m_instanceCount;
	// -----------------   Instancing   --------------------------------

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

	// ������ �������� ��� ���������� �� ���������� �� ������
//	int *FireInstIndNumber;
	// ������ ��������� Instance �� ������
//	float *FireInstDistance;

	//	ParticleShaderInstance_FIRE* Unsortedinstance;

	RCube_VecFloat234 Camera;
	RCube_VecFloat234 Object;

	// ������� ���������� �� ������� �� ������
	void DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );

	// ���������� �� ����������� ��������� ������� �� �������
	template <class T> void QuickDepthSortAscending( T* indices, float* depths, int lo, int hi );

	template <class T> void QuickDepthSortDescending( T* indices, float* depths, int lo, int hi );


};

#endif