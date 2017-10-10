#pragma once
#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARTICLESYSTEMCLASS_H_
#define _PARTICLESYSTEMCLASS_H_

// Время обновления от 50 до 200 мкс

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include "DirectXMath.h"
#include "D3DGlobalContext.h"
#include "ParticleSystemDefinitions.h"
#include "FPSTimers.h"
#include "D3DClass.h"
#include "Light_def.h"
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
//#include "textureclass.h"

using namespace std;

static const int PARTICLES_SYS_MAX = 100;

typedef enum
	{
		DROP_DOWN = 0, // падение
	} RenderTechnique;


class ParticleSystemClass
{

public:
	ParticleSystemClass();
   ~ParticleSystemClass();

	bool Initialize(HWND hwnd, D3DGlobalContext *D3DGC_Local, ID3D10Blob* Blob, ID3D11ShaderResourceView* texture, D3DClass *_EngineLight);
	void Shutdown();
	void Frame( FPSTimers &Timers );
	void Render();

	int GetActiveParticleAmmount();

	int GetIndexCount();
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	int GetInstanceCount();
	// -----------------   Instancing   --------------------------------

private:
	D3DGlobalContext *D3DGC_Local;
	
	D3DClass *EngineLight;
	PointLight *TempLight;

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*, ID3D10Blob* Blob );
	void ShutdownBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	void RenderBuffers(ID3D11DeviceContext*);

public:
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;

	ID3D11ShaderResourceView* m_Texture;
	BasicType* m_particleList;
	int m_vertexCount, m_indexCount;
	Vertex_FlatObject* m_vertices;
	ID3D11InputLayout* m_layout;
//	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	ParticleShaderInstance_BASIC* instances;
	ID3D11Buffer* m_instanceBuffer;
	int m_instanceCount;
// -----------------   Instancing   --------------------------------
};

#endif