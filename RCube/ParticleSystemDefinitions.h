#pragma once
#include "stdafx.h"

#ifndef _PARTICLE_SYSTEM_DEFINITION_H_
#define _PARTICLE_SYSTEM_DEFINITION_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "VertexBuffers_def.h"

using namespace DirectX;

//  +++++++++++++++++++++++++++++++++        Particle System     +++++++++++++++++++++++++++++++++++++++
enum
{
	RANDOM = 0,	// Произвольно
	NORANDOM = 1		// Не произвольно
};


// Типы систем частиц
enum
{
	BASIC		= 0,	// текстурная ситема частицы = BillBords квадратный
	SNOW		= 1,	// текстурная ситема частицы = BillBords квадратный
	PARTICLES	= 2,	// текстурная ситема частицы = BillBords квадратный ) для любых мелких частиц пыли, например, повисшей в воздухе.
	EXPLOSION	= 3,	// несколько систем частиц : SMOKE + FIRE + DIRT
	FIRE		= 4,	// текстурная ситема частицы не только BillBords для реалистичности
	SMOKE		= 5,	// текстурная ситема частицы = BillBords квадратный, возможно анимированный
	GALAXY		= 6,	// текстурная ситема частицы = BillBords квадратный
	STARS		= 7,	// ( MILKYWAY сюда же ) текстурная ситема частицы = BillBords квадратный
	COMETS		= 8,	// текстурная ситема частицы = BillBords квадратный
	WATER_DROPS = 9,	// объектная ситема частицы + BillBords квадратный с эфектами
	LIVES		= 10,	// текстурная ситема частицы = BillBords квадратный
	DIRT		= 11,	// объектная ситема частицы + Объёмные, вертексные объекты на расстоянии BillBords
	ROCKS		= 12,	// объектная ситема частицы + Объёмные, вертексные объекты на расстоянии BillBords
	GROUNDSTEP	= 13	// несколько систем частиц : DIRT + PARTICLES
};

// +++++++++++++++++   Define particles Instancing buffer types for Shaders    ++++++++

// BASIC
// Перечень параметров частицы передаваемых в Vertex шейдер
struct ParticleShaderInstance_BASIC
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};
// BASIC ------------------------------------------------------

// Перечень параметров частицы передаваемых в Vertex шейдер
struct ParticleShaderInstance_FIRE
{
	XMFLOAT3 position;
	float Dummy; // Рисовать ли частицу 0.0f - НЕТ   1.0 - ДА
	XMFLOAT4 color;
	XMFLOAT4 NewTexCord;
};

// TORCH-FIRE 
// Перечень параметров частицы передаваемых в Vertex шейдер
struct ParticleShaderInstance_TORCH_FIRE
{
	XMFLOAT3 position;
	XMFLOAT4 Alpha;		// x - размер BillBoard , z - угол поворота по Z , w - Alpha
	XMFLOAT4 NewTexCord;
};
// TORCH-FIRE -------------------------------------------------------


// SMOKE 
struct ParticleShaderInstance_SMOKE
{
	XMFLOAT3 position;
	XMFLOAT4 Alpha;
	XMFLOAT4 NewTexCord;
};
// SMOKE -------------------------------------------------------

// -----------------   Define particles Instancing buffer types for Shaders   --------------------------------



// ++++++++++++++++   Additional particle parameters   ++++++++++++++++++++++++++++++++

// Дополнительные данные для базовой системы частиц
struct BasicType
{
	float Velocity;
	 bool active;
	 UINT LightIndex;
};


struct FireType1
{
	float Velocity;
	bool active;
	UINT LightIndex;
	int CurentFrame;
};


struct Torch_FireSmoke1
{
	float LifeTime;		// Сколько осталось жить
//	float TimePased;	// Сколько времени прошло с момента создания частицы
	int CurentFrame;
	float Rotation;		// Поворот в воздухе
	float Velocity;		// Скорость полёта
	float Angel;		// Угол для расчёта синусоидальнго полёта частиц FireFly
//	bool Smoke;			// Smoke Instance if true
//	bool Fire;			// Fire Instance if true 
//	bool Flyer;			// Flyer Instance if true 
//	bool Heart;			// Heart Instance if true 
	bool Active;		// Рисуется ли частица
};

/*
struct Smoke1
{
	float LifeTime;		// Сколько осталось жить
	  int CurentFrame;	
	float Rotation;		// Поворот в воздухе
	float Velocity;		// Скорость полёта
};
*/

// Дополнительные данные для Огня
struct FireBillBord
{
	float size;					// размер BillBord
	 UINT LightIndex;			// номер света привязанный к конкретному Instance BillBord
	float LightAttenuationBegin;// размер света начало
	float LightAttenuationEnd;	// размер света конец
 XMFLOAT3 SmokeDirection;		// направление дыма
};


struct AdvancedType
{
	XMFLOAT4 Velocity;
	XMVECTOR Rotation;
	XMFLOAT4 Color;
	XMFLOAT3 Position;
	float Mass;
	float Life;
	float Fade;
	float RotateRate;
	bool active;
};
// ++++++++++++++++   Additional particle parameters   ++++++++++++++++++++++++++++++++



// +++++++++++++++++++++++++++++  System Types Definition   +++++++++++++++++++++++++++++
// BASIC
struct BASIC_ParticlesSystem
{
			 XMFLOAT3   Deviation;			// разброс позиций для Emiter
			 XMFLOAT3	Translation;		// смещение относительно центра системы
				float   VelocityVariation;	// разброс скорости для Emiter
				float   ParticlesSize;		// размер частиц
				float   EmitsPerSecond;		// сколько частиц излучать в секунду
				float   ParticlesMAXAmount;	// Максимальное количество частиц
			BasicType  *ParticlesData;		// Указатель на дополнительные данные для частиц
		   ParticleShaderInstance_BASIC  *ShaderInstances;	// Указатель на массив свойств частиц передающихся в шейдера рисующий частицы   ParticleShaderInstanceType1
					   Vertex_FlatObject	 *VertexStructure;	// Указатель на описание вертекса передаваемого в шейдер						ParticleShaderVertexType1
					  ID3D11InputLayout  *Layout;
					  ID3D11VertexShader *VertexShader;
					  ID3D11PixelShader  *PixelShader;
					  ID3D11SamplerState *SamplerState;
				 void  *Emitter;			// Указатель на излучатель
				 void  *Updater;			// Указатель на расчёт частиц
				 void  *Killer;				// Eказатель на уничтожитель
   ID3D11ShaderResourceView *Texture;		// Указатель на текстуру для частиц
				bool	Active;				// Активна ли система для рисования и просчёта


};


struct ParticleAdvancedType
{
	XMFLOAT3 Position;
	XMFLOAT4 Direction;
	XMFLOAT4 Rotation;
	XMFLOAT4 Color;
	   float Velocity;
	   float Mass;
	   float Life;
	   float Fade;
	   float RotateRate;
	    bool active;
};

// +++++++++++++++++++++++++++++  System Types Definition   +++++++++++++++++++++++++++++

//  +++++++++++++++++++++++++++++++++        Particle System     +++++++++++++++++++++++++++++++++++++++
// LAYOUTS
/* EXAPLE
polygonLayout[0].SemanticName = "POSITION";
polygonLayout[0].SemanticIndex = 0;
polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
polygonLayout[0].InputSlot = 0;
polygonLayout[0].AlignedByteOffset = 0;
polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
polygonLayout[0].InstanceDataStepRate = 0;
*/
/*
// BASIC Layout
D3D11_INPUT_ELEMENT_DESC BASIC_Layout[4] =
{
{ "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , 0 , D3D11_INPUT_PER_VERTEX_DATA , 0 },
{ "TEXCOORD" , 0 , DXGI_FORMAT_R32G32_FLOAT , 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA , 0 },
{ "POSITION" , 1 , DXGI_FORMAT_R32G32B32_FLOAT , 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
{	 "COLOR" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT , 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
};
*/

// LAYOUTS ------------------------------------------------------------------------------
#endif