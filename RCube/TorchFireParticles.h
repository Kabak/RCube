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

	// Если количество кадров меньше чем вся текстура, то FramesAmount должно быть не НУЛЬ
	// , а ровно количеству реальных кадров. Если кадры занимают всю текстуру,то FramesAmount = 0
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

	// Размер частиц при инициализации Вертексного буферы
	float m_particleSize; 

	// Номер света присвоенный системой
	int LightIndex;
	float TimeToUpdateLight;

	int RealFramesAmount;	// Анимация
	
	int UX_Amount;			// Ширина текстуры Width X U
	int VY_Amount;			// Высота текстуры Height Y V
	float OneFrameWidth;	// Ширина одного кадра в текстурных координатах 0 - 1
	float OneFrameHeight;	// Высота одного кадра в текстурных координатах 0 - 1
	float FireFlyWidth;		// Ширина одного элемента FireFly в единицах от 0 - 1
	float FireFlyHeight;	// Высота одного элемента FireFly в единицах от 0 - 1

	int NumOfAnimeFrams;	// Сколько всего кадров в анимации
	int FireFlyFrameNumber;	// Кадр из которого генерятся FireFly
	int FireFlyCutWidth;	// Размер нарезки куска для FireFly по ширине
	int FireFlyCutHeight;	// Размер нарезки куска для FireFly по высоте
XMFLOAT4 FireFlyFrameCoord;	// Координаты кадра из которого нарезаются FireFly ( для ускорения )

	float PlayFPSFlame;	// С какой скоростью в секунду прокручиваются кадры 
//	float PlayFPSSmoke;	// С какой скоростью в секунду прокручиваются кадры 

	float TimePassedFlames;	// Сколько времени прошло для обноления кадра анимации FLAMES
//	float TimePassedSmoke;	// Сколько времени прошло для обноления кадра анимации SMOKE
//	float TimeAliveSmoke;	// Сколько времени рисуется частица

	float TimeToEmitFlames;	// Время создавать частиы FLAMES
	float UpdateTimeFlames;	// Сколько времени прошло для обновления параметров FLAMES

	float TimeToEmitSmoke;	// Время создавать частиы SMOKE
	float UpdateTimeSmoke;	// Сколько времени прошло для обновления параметров SMOKE

	float TimeToEmitFireFly;// Время создавать частиы FIREFLY
	float UpdateTimeFireFly;// Сколько времени прошло для обновления параметров FIREFLY

	// Массив указателей координат текстурных кадров анимаций
	float *TextcordTop;
	float *TextcordLeft;
	// Массив указателей координат текстурных кадров FireFly
	float *FireFlyTop;
	float *FireFlyLeft;

// Глобальные 
	XMFLOAT3 TorchPosition;			// Позиция ЦЕНТРА вокруг которого всё происходит
	XMFLOAT3 FlameStartDeviation;	// Разброс стартовой позиции частиц Flame
	XMFLOAT3 SmokeStartDeviation;	// Разброс стартовой позиции частиц Smoke
	XMFLOAT3 FireFlyStartDeviation;	// Разброс стартовой позиции частиц FireFly
	float VelocityVariation;		// Разброс скорости

	// Индексный буфер одной сущности частицы
	// И для FLAME, для SMOKE, для FireFly
	ID3D11Buffer *IndexBuffer;

// ПЛАМЯ и ДЫМ
	int FlameInstancesAmount;	// Максимальное Количество Instance пламени
	int FlameInstanceActive;	// количество активных частиц Flame
	int FireFlyInstancesAmount;	// Количество летучих частиц огня
	int FireFlyInstancesActive; // Количество активных частиц
	int SmokeInstancesAmount;	// Максимальное Количество Instance дыма 
	int SmokeInstanceActive;	// количество активных частиц дыма


//	int *FlameInstIndNumber;	// Массив индексов для сортировки по удалению от камеры
//	float *FlameInstDistance;	// Массив расстояний для сортировки по удалению от камеры
//	int *SmokeInstIndNumber;	// Массив индексов для сортировки по расстоянию от камеры
//	float *SmokeInstDistance;	// Массив растояний Instance от камеры

	int All_Instances_Amount;	// Сумма всех частиц которые нужно обработать

	float DistanceAtStart;		// Расстояние между вертексами ( на которые натягивается текстура огня )
								// от центра до вертекса при старте
	float BetweenNextTimeFlame;	// Время между выпуском новой частицы FLAME
	float BetweenNextTimeSmoke; // Время между выпуском новой частицы SMOKE
	float BetweenNextTimeFireFly;// Время между выпуском новой частицы FIREFLY

	int FlameStartFrame;		// Начальный кадр анимации FLAME
	int FlameFramesEnd;			// Сколько всего кадров FLAME
	int SmokeStartFrame;		// Начальный кадр анимации SMOKE
	int SmokeFramesEnd;			// Сколько всего кадров SMOKE
	int SmokeFramesAmount;		// Сколько кадров в анимации SMOKE

	XMFLOAT4 InitAlpha;			// Начальная прозрачность текстуры при появлении

	// Дополнительные характеристики для манипуляции пламенем
	Torch_FireSmoke1* TorchFireFlamesAddData;
	Torch_FireSmoke1* TorchFireSmokeAddData;
	Torch_FireSmoke1* TorchFireFlyAddData;
	// Тип Vertex Buffer
	Vertex_FlatObject *FlameVertices;
	// Тип Instance Buffer
	ParticleShaderInstance_TORCH_FIRE *FlameInstances;
	ParticleShaderInstance_TORCH_FIRE *SmokeInstances;
	ParticleShaderInstance_TORCH_FIRE *FireFlyInstances;
	ParticleShaderInstance_TORCH_FIRE *ShaderInstanceAll;
	// Сам Inctance Buffer
	ID3D11Buffer *FlameInstanceBuffer;
	// Вертексный буфер одной сущности частицы
	ID3D11Buffer *FlameVertexBuffer;

	void UpdateTorchPos (XMFLOAT3& TorchPosition);

// ПЛАМЯ --------------------------------------------------

	RCube_VecFloat34 Camera;
	RCube_VecFloat34 Object;

	// Находим расстояние от объекта до камеры
	void DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore );

	// Сортировка ПО ВОЗРАСТАНИЮ элементов массива по глубине
	template <class T> void QuickDepthSortAscending( T* indices, float* depths, int lo, int hi );

	template <class T> void QuickDepthSortDescending( T* indices, float* depths, int lo, int hi );

	// Генерируем XYZ
	void Gen_XYZ( XMFLOAT3 &Deviation, XMFLOAT3 &Result );

	// Изменяем позицию Instance
	void UpdatePosition( XMFLOAT3 &Direction, XMFLOAT3 &Result );

	// Изменяем поворот Instance
	void UpdateRotation( XMFLOAT3 &Direction, XMFLOAT3 &Result );

	// Обновляем частицы FLAME
	void FlamesUpdateData  ( int &i );
	void SmokeUpdateData   ( int &i );
	void FireFlyUpdateData ( int &i );

	void SetInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 );
	
	// Нарезалка кадра на куски 8 * 16 для Firefly
	void SetFireFlyInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 );

};

#endif

