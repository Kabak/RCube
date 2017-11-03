#pragma once
#include "stdafx.h"
#include <d3d11.h>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
#include "ParticleSystemDefinitions.h"
#include "FPSTimers.h"
#include "D3dClass.h"
#include "Light_def.h"
#include "Buffers_def.h"
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

   // Если количество кадров меньше чем вся текстура, то FramesAmount должно быть не НУЛЬ
   // , а ровно количеству реальных кадров. Если кадры занимают всю текстуру,то FramesAmount = 0
   bool Initialize( HWND hwnd, 
	   D3DGlobalContext *D3DGC_Local, 
   ID3D11ShaderResourceView* texture, 
					   int _UX_Amount,							// Количество строк в текстуре анимации
					   int _VY_Amount,							// Количестко столбцов в текстуре анимации
					   int FramesAmount,					// Реальное количество анимаций в текстуре с верха , слева направо
				D3DClass *_EngineLight
	);


			bool InitFireSystem( HWND hwnd, 
				   D3DGlobalContext *D3DGC, 
						  ID3D10Blob* Blob, 
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

	void EmitParticles( FPSTimers& Timers );
	void UpdateParticles( FPSTimers& Timers );
	void KillParticles();

	int GetActiveParticleAmmount();

	int GetInstanceCount();

	int GetMaxParticles();

	int ShaderForDraw = -1;

	void ChangeActiveParticleAmount(int Amount);

private:

	D3DGlobalContext *D3DGC_Local;
	
	D3DClass *EngineLight;
	PointLight *TempLight;

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	ID3D11ShaderResourceView *FireTexture;
	ID3D11ShaderResourceView *SmokeTexture;
	ID3D11ShaderResourceView *HeartTexture;
	ID3D11ShaderResourceView *ParticlesTexture;

	int UX_Amount;			// Ширина текстуры Width X U
	int VY_Amount;			// Высота текстуры Height Y V

	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;

	int maxActiveParticles; // Максимальное количество частиц на экране ( для регулировки одновременного количества частиц )

	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;
	
	FireType1* m_particleList;

	void SetInstanceStartFrame(int FrameNumber, XMFLOAT4& Data2 );

	Vertex_FlatObject* m_vertices;

	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	ParticleShaderInstance_FIRE* instances;
	ID3D11Buffer* m_instanceBuffer;
	int m_instanceCount;
	// -----------------   Instancing   --------------------------------


// Анимация
	int RealFramesAmount;
	// ШИрина одного кадра в текстурных координатах 0 - 1
	float OneFrameWidth;
	// Высота одного кадра в текстурных координатах 0 - 1
	float OneFrameHeight;

	// Сколько всего кадров в анимации
	int NumOfAnimeFrams;
	// Текущий кадр
	int CurentFrame;
	// С какой скоростью в секунду прокручиваются кадры 
	float PlayFPS;
	// Сколько времени прошло
	float TimePassed;

	// Массив указателей на
	float *TextcordTop;
	float *TextcordLeft;
	
	// Количество инстансе огня 
	int FlameInstances;

	// Массив индексов для сортировки по расстоянию от камеры
	int *FireInstIndNumber;
	// Массив растояний Instance от камеры
	float *FireInstDistance;
	
//	ParticleShaderInstance_FIRE* Unsortedinstance;

	RCube_VecFloat34 Camera;
	RCube_VecFloat34 Object;

	// Находим расстояние от объекта до камеры
	void DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );

	// Сортировка ПО ВОЗРАСТАНИЮ элементов массива по глубине
	template <class T> void QuickDepthSortAscending( T* indices, float* depths, int lo, int hi );

	template <class T> void QuickDepthSortDescending( T* indices, float* depths, int lo, int hi );


};

#endif