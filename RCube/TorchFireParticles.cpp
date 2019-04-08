#pragma once
#include "stdafx.h"
#include "TorchFireParticles.h"

TorchFireParticles::TorchFireParticles()
{
	FireTexture = nullptr;
	SmokeTexture = nullptr;
	HeartTexture = nullptr;
	ParticlesTexture = nullptr;

	FlameVertices = nullptr;
	FlameInstances = nullptr;
	SmokeInstances = nullptr;
	FireFlyInstances = nullptr;

	ShaderInstanceAll = nullptr;

	TextcordTop = nullptr;
	TextcordLeft = nullptr;

	FireFlyTop = nullptr;
	FireFlyLeft = nullptr;

// FLAMES
	// Количество Instance пламени FLAMES
	FlameInstancesAmount = 8;
	FlameInstanceActive = 0;
	// Количество Instance дыма
	SmokeInstancesAmount = 80;
	SmokeInstanceActive = 0;
	// Количество горящих частиц
	FireFlyInstancesAmount = 20;
	FireFlyInstancesActive = 0;

	All_Instances_Amount = FlameInstancesAmount + SmokeInstancesAmount + FireFlyInstancesAmount;

	PlayFPSFlame = 24.0f;	// Количество смен кадров анимаций пламени FLAMES в секунду
//	PlayFPSSmoke = 0.0001f;	// Количество смен кадров анимаций пламени SMOKE в секунду

	BetweenNextTimeFlame = 0.5f;	// Сколько времени в секундах должно пройти перед выпуском новой частицы FLAME
	BetweenNextTimeSmoke = 0.2f;	// Сколько времени в секундах должно пройти перед выпуском новой частицы SMOKE
	BetweenNextTimeFireFly = 0.3f;	// Сколько времени в секундах должно пройти перед выпуском новой частицы FIREFLY

	VelocityVariation = 0.9;	// Разброс скорости частиц при создании

	TimeToEmitFlames = 0.0f;
	TimeToEmitSmoke = 0.0f;
	TimeToEmitFireFly = 0.0f;

	UpdateTimeFlames = 0.0f;	// Как часто нужно обновлять параметры частицам
	UpdateTimeSmoke = 0.0f;
	UpdateTimeFireFly = 0.0f;

	FireFlyFrameNumber = 0;
//	CurentFrame = 0;
	TimePassedFlames = 0.0f;
//	TimePassedSmoke = 0.0f;
//	TimeAliveSmoke = 0.3f;


	// Разброс позиции при создании частицы FLAME
	FlameStartDeviation   =	{ 0.3f, 0.0f, 0.5f };
	SmokeStartDeviation	  =	{ 0.3f, 1.0f, 0.5f };
	FireFlyStartDeviation =	{ 0.5f, 1.0f, 0.5f };

	InitAlpha = { 1.0f, 1.0f, -1.723598f, 0.05f }; //-1.523598f

//	FlameInstIndNumber = nullptr;
//	FlameInstDistance = nullptr;
	FlameInstanceBuffer = nullptr;

	TorchFireFlamesAddData = nullptr;
	TorchFireSmokeAddData = nullptr;
	TorchFireFlyAddData = nullptr;
	FlameVertexBuffer = nullptr;
	
	IndexBuffer = nullptr;
	
	// Размер текстур FLAMES
	ParticleSize = 1.0f;

// SMOKE
//	SmokeInstIndNumber = nullptr;
//	SmokeInstDistance = nullptr;
//	SmokeInstanceBuffer = nullptr;

//	SmokeVertices = nullptr;

//	SmokeVertexBuffer = nullptr;
//	SmokeInstances = nullptr;

	Camera.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	Object.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	FireFlyFrameCoord = { 0.0f, 0.0f, 0.0f, 0.0f };

	TempLight = nullptr;
	TimeToUpdateLight = 0.0f;
	LightIndex = 0;		// Индекс света
//	AttenuationEnd = 0;	// Граница света
//	LightColor = { 0.0f, 0.0f, 0.0f };
}


TorchFireParticles::~TorchFireParticles()
{
}


bool TorchFireParticles::Initialize( HWND hwnd,
									 D3DGlobalContext *D3DGC,
									 ID3D11ShaderResourceView* _FireTexture,
									 TorchFireSmoke TorchFireSmokeInit,
									 D3DClass *_EngineLight
									 )
{
	bool result;

	D3DGC_Local = D3DGC;

	FireTexture = _FireTexture;

	TorchPosition = TorchFireSmokeInit.TorchPosition;

	UX_Amount = TorchFireSmokeInit.UX_Amount;
	VY_Amount = TorchFireSmokeInit.VY_Amount;

	NumOfAnimeFrams = UX_Amount * VY_Amount;
	// Уменьшаем количество кадров, потому что их индексы начинаются с НУЛЯ 0 - 63
  --NumOfAnimeFrams;

	FlameStartFrame = TorchFireSmokeInit.FireFrameStart;
	FlameFramesEnd = TorchFireSmokeInit.FireFrameStart + TorchFireSmokeInit.FireFramesAmount;

	SmokeStartFrame = TorchFireSmokeInit.SmokeFrameStart;
	SmokeFramesAmount = TorchFireSmokeInit.SmokeFramesAmount;
	SmokeFramesEnd = TorchFireSmokeInit.SmokeFrameStart + TorchFireSmokeInit.SmokeFramesAmount;

	FireFlyFrameNumber = TorchFireSmokeInit.FireFlyFrameNumber;
	FireFlyCutWidth = TorchFireSmokeInit.FireFlyCutWidth;
	FireFlyCutHeight = TorchFireSmokeInit.FireFlyCutHeight;

	TextcordTop = new float[ VY_Amount ];
	TextcordLeft = new float[ UX_Amount ];

	// Размеры для нарезки FireFly
	FireFlyTop = new float[ FireFlyCutHeight ];
	FireFlyLeft = new float[ FireFlyCutWidth ];

	int UX = 0; 
	int VY = 0;
	OneFrameWidth = 1.0f / UX_Amount;
	OneFrameHeight = 1.0f / VY_Amount;

	while ( VY < VY_Amount )
	{
		TextcordTop[ VY ] = static_cast<float>( VY * OneFrameHeight );
		++VY;
	}
	
	while ( UX <  UX_Amount )
	{
		TextcordLeft[ UX ] = static_cast<float>( UX * OneFrameWidth );
		++UX;
	}

	SetInstanceStartFrame( FireFlyFrameNumber, FireFlyFrameCoord );

// FireFly
	UX = 0; 
	VY = 0;
	FireFlyWidth = ( TextcordLeft[1] / FireFlyCutWidth );// Берём ширину ячейки
	FireFlyHeight = ( TextcordTop[1] / FireFlyCutHeight );	// Берём высоту ячейки

	while ( VY < FireFlyCutHeight )	 // Количество по высоте
	{
		FireFlyTop[ VY ] = static_cast<float>( VY * FireFlyHeight );// Ширина элемента
		++VY;
	}

	while ( UX < FireFlyCutWidth )	// Количество по ширине
	{
		FireFlyLeft[ UX ] = static_cast<float>( UX * FireFlyWidth ); // Высота элемента
		++UX;
	}
// FireFly ----------------------------------------------------------------------

	// Получаем указатель на класс LIght в движке, для манипуляцией источниками света для частиц
	EngineLight = _EngineLight;

	// Создаём буферы для DX
	result = InitializeBuffers( D3DGC_Local->DX_device );
	if ( !result )
	{
		return false;
	}

	// Инициализируем саму систему частиц
	result = InitializeParticleSystem();
	if ( !result )
	{
		return false;
	}


	TempLight = new PointLight;

	TempLight->attenuationBegin = 1.0f;
	TempLight->attenuationEnd = 7.5f;
	TempLight->color = { 0.86f, 0.43f, 0.129f };

	XMFLOAT3 TempPos = TorchFireSmokeInit.TorchPosition;
	TempPos.y += 1.0f;
	TempLight->position = TempPos;//;{ 0.0f, 10.0f, 00.0f }
	//	TempLight->direction = { 0.0f, 0.0f, 0.0f };//;{ 0.0f, -0.3f, -1.0f }
	TempLight->angel = 0.0f;
	TempLight->ShadowMapSliceNumber = -1;
	TempLight->Dummy = 1;
	TempLight->HaveShadow = false;

	LightIndex = EngineLight->AddLightSource( *TempLight );

	return true;
}



void TorchFireParticles::Shutdown()
{
	ShutdownBuffers();

	RCUBE_DELETE( TempLight );

	RCUBE_ARR_DELETE( TextcordTop );
	RCUBE_ARR_DELETE( TextcordLeft );
	RCUBE_ARR_DELETE( FireFlyTop );
	RCUBE_ARR_DELETE( FireFlyLeft );
}


void TorchFireParticles::Frame( FPSTimers& Timers )
{
	//	srand( Timers.Seed );

	KillSmokeParticles();
	KillFireFlyParticles();

	EmitFlameParticles	( Timers );
	EmitSmokeParticles	( Timers );
	EmitFireFlyParticles( Timers );

	UpdateFlameParticles	( Timers );
	UpdateSmokeParticles	( Timers );
	UpdateFireFlyParticles	( Timers );

	UpdateLight( Timers );

	D3D11_MAPPED_SUBRESOURCE  mapResource;
	HRESULT result = D3DGC_Local->DX_deviceContext->Map( FlameInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource );
	if ( FAILED( result ) )
	{
		MessageBox( 0, L"Ошибка доступа к буферу частиц TorchFireParticles.", L"Can't ->Map", 0 );
	}

	BB_Particle_Instance *Sortedinstance;
	Sortedinstance = ( BB_Particle_Instance * ) mapResource.pData;

	// FLAMES
	for ( int i = 0; i < FlameInstancesAmount; ++i )
	{
//		Particles_Data *PointerFlamesAddData = &TorchFireFlamesAddData[i];

		if( TorchFireFlamesAddData[i].Active )
		{
			*Sortedinstance = FlameInstances[i]; //FlameInstances[FlameInstIndNumber[i]];
			++Sortedinstance;
		}

	}
	// SMOKE
	for ( int i = 0; i < SmokeInstancesAmount; ++i )
	{
		//		Particles_Data *PointerFlamesAddData = &TorchFireFlamesAddData[i];

		if ( TorchFireSmokeAddData[i].Active )
		{
			*Sortedinstance = SmokeInstances[i]; //SmokeInstances[SmokeInstIndNumber[i]];
			++Sortedinstance;
		}

	}
	// FIREFLY
	for ( int i = 0; i < FireFlyInstancesAmount; ++i )
	{
		//		Particles_Data *PointerFlamesAddData = &TorchFireFlamesAddData[i];

		if ( TorchFireFlyAddData[i].Active )
		{
			*Sortedinstance = FireFlyInstances[i]; //SmokeInstances[SmokeInstIndNumber[i]];
			++Sortedinstance;
		}

	}
	D3DGC_Local->DX_deviceContext->Unmap( FlameInstanceBuffer, 0 );


}


void TorchFireParticles::Render()
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set vertex buffer stride and offset.
	strides[0] = sizeof( Vertex_FlatObject );
	strides[1] = sizeof( BB_Particle_Instance );

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = FlameVertexBuffer;
	bufferPointers[1] = FlameInstanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3DGC_Local->DX_deviceContext->IASetVertexBuffers( 0, 2, bufferPointers, strides, offsets );

	// Set the index buffer to active in the input assembler so it can be rendered.
	D3DGC_Local->DX_deviceContext->IASetIndexBuffer( IndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set the type of primitive that should be rendered from this vertex buffer.
//	D3DGC_Local->DX_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set shader texture resource in the pixel shader.
	D3DGC_Local->DX_deviceContext->PSSetShaderResources( 0, 1, &FireTexture );

	// Render the triangle.
	D3DGC_Local->DX_deviceContext->DrawIndexedInstanced( 6, FlameInstanceActive + SmokeInstanceActive + FireFlyInstancesActive, 0, 0, 0 );

}


bool TorchFireParticles::InitializeParticleSystem()
{

// FLAMES
	TorchFireFlamesAddData = new Particles_Data[FlameInstancesAmount];
	// Создаём массив индексов элементо в массивах для сортировки по удалению
	// от камеры. Для отрисовки по мере приближения к камере.
//	FlameInstIndNumber = new int[FlameInstancesAmount];
	//	memset( FlameInstIndNumber, 0, sizeof( int ) * FlameInstancesAmount );
//	FlameInstDistance = new float[FlameInstancesAmount];	// Массив растояний до камеры
	//	memset( FlameInstDistance, 0, sizeof( float ) * FlameInstancesAmount );
// FLAMES --------------------------------------------------------------------------------

// SMOKE
	TorchFireSmokeAddData = new Particles_Data[SmokeInstancesAmount];
//	SmokeInstIndNumber = new int[SmokeInstancesAmount];
//	SmokeInstDistance = new float[SmokeInstancesAmount];
// SMOKE ---------------------------------------------------
// FIREFLY
	TorchFireFlyAddData = new Particles_Data[FireFlyInstancesAmount];
// FIREFLY -------------------------------------------------
	// Генерим произвольный начальный кадр анимации для всех FLAMES
/*	for ( int i = 0; i < FlameInstancesAmount; ++i )
	{
		TorchFireFlamesAddData[i].Active = false;
	}
*/
	memset ( TorchFireFlamesAddData,  0, sizeof( Particles_Data ) * FlameInstancesAmount );
	memset ( TorchFireSmokeAddData,   0, sizeof( Particles_Data ) * SmokeInstancesAmount );
	memset ( TorchFireFlyAddData, 0, sizeof( Particles_Data ) * FireFlyInstancesAmount );
	return true;
}


bool TorchFireParticles::InitializeBuffers( ID3D11Device* device )
{
	HRESULT result;

	D3D11_BUFFER_DESC BillBordVertBufferDesc, BillBordVertexesIndBufferDesc;
	D3D11_SUBRESOURCE_DATA FlameVertexData;
//	D3D11_SUBRESOURCE_DATA SmokeVertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;
	// -----------------   Instancing   --------------------------------

	// Create the vertex array for the particles that will be rendered.
	FlameVertices = new Vertex_FlatObject[4];
//	SmokeVertices = new BillBordVertexes[4];

	unsigned long indices[6] = { 0, 1, 2, 0, 3, 1 };

	// Set up the description of the static index buffer.
	BillBordVertexesIndBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BillBordVertexesIndBufferDesc.ByteWidth = sizeof( unsigned long ) * 6;
	BillBordVertexesIndBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BillBordVertexesIndBufferDesc.CPUAccessFlags = 0;
	BillBordVertexesIndBufferDesc.MiscFlags = 0;
	BillBordVertexesIndBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer( &BillBordVertexesIndBufferDesc, &indexData, &IndexBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}


	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	FlameInstances = new BB_Particle_Instance[FlameInstancesAmount];
	SmokeInstances = new BB_Particle_Instance[SmokeInstancesAmount];
	FireFlyInstances = new BB_Particle_Instance[FireFlyInstancesAmount];

	ShaderInstanceAll = new BB_Particle_Instance[All_Instances_Amount];

	memset( ShaderInstanceAll, 0, ( sizeof( BB_Particle_Instance ) * All_Instances_Amount ) );
//	memset( SmokeInstances, 0, ( sizeof( ParticleShaderInstance_SMOKE ) * MaxSmokeInstanse ) );

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof( BB_Particle_Instance ) * All_Instances_Amount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = ShaderInstanceAll;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer( &instanceBufferDesc, &instanceData, &FlameInstanceBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}
/*
	// SMOKE
	instanceBufferDesc.ByteWidth = sizeof( ParticleShaderInstance_SMOKE ) * MaxSmokeInstanse;
	instanceData.pSysMem = SmokeInstances;
	result = device->CreateBuffer( &instanceBufferDesc, &instanceData, &SmokeInstanceBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}
*/
	// -----------------   Instancing   --------------------------------

	// Initialize vertex array to zeros at first.
	memset( FlameVertices, 0, ( sizeof( Vertex_FlatObject ) * 4 ) );

	// Top left.
	FlameVertices[0].Position = XMFLOAT3( -1.0f * ParticleSize, 1.0f * ParticleSize, 0.0f );  // Top left
	FlameVertices[0].TexCoord = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Bottom right.
	FlameVertices[1].Position = XMFLOAT3( 1.0f * ParticleSize, -1.0f * ParticleSize, 0.0f );  // Bottom right
	FlameVertices[1].TexCoord = XMFLOAT4( 1.0f, 1.0f, 0.0f, 0.0f );

	// Bottom left.
	FlameVertices[2].Position = XMFLOAT3( -1.0f * ParticleSize, -1.0f * ParticleSize, 0.0f );  // Bottom left.
	FlameVertices[2].TexCoord = XMFLOAT4( 0.0f, 1.0f, 0.0f, 0.0f );

	// Top right.
	FlameVertices[3].Position = XMFLOAT3( 1.0f * ParticleSize, 1.0f * ParticleSize, 0.0f );  // Top right.
	FlameVertices[3].TexCoord = XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f );
/*
	memset( SmokeVertices, 0, ( sizeof( BillBordVertexes ) * 4 ) );

	// Top left.
	SmokeVertices[0].position = XMFLOAT3( -1.0f * ParticleSize, 1.0f * ParticleSize, 0.0f );  // Top left
	SmokeVertices[0].texture = XMFLOAT2( 0.0f, 0.0f );

	// Bottom right.
	SmokeVertices[1].position = XMFLOAT3( 1.0f * ParticleSize, -1.0f * ParticleSize, 0.0f );  // Bottom right
	SmokeVertices[1].texture = XMFLOAT2( 1.0f, 1.0f );

	// Bottom left.
	SmokeVertices[2].position = XMFLOAT3( -1.0f * ParticleSize, -1.0f * ParticleSize, 0.0f );  // Bottom left.
	SmokeVertices[2].texture = XMFLOAT2( 0.0f, 1.0f );

	// Top right.
	SmokeVertices[3].position = XMFLOAT3( 1.0f * ParticleSize, 1.0f * ParticleSize, 0.0f );  // Top right.
	SmokeVertices[3].texture = XMFLOAT2( 1.0f, 0.0f );
*/
	// Set up the description of the dynamic vertex buffer.
	BillBordVertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BillBordVertBufferDesc.ByteWidth = sizeof( Vertex_FlatObject ) * 4;
	BillBordVertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BillBordVertBufferDesc.CPUAccessFlags = 0;
	BillBordVertBufferDesc.MiscFlags = 0;
	BillBordVertBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	FlameVertexData.pSysMem = FlameVertices;
	FlameVertexData.SysMemPitch = 0;
	FlameVertexData.SysMemSlicePitch = 0;

	// Вертексный буфер для FLAMES
	result = device->CreateBuffer( &BillBordVertBufferDesc, &FlameVertexData, &FlameVertexBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}

	return true;
}


void TorchFireParticles::ShutdownBuffers()
{
	RCUBE_RELEASE( FlameInstanceBuffer )
	RCUBE_RELEASE( FlameVertexBuffer )

	
	RCUBE_ARR_DELETE( FlameVertices )
//	RCUBE_ARR_DELETE( SmokeVertices )
	
	RCUBE_RELEASE( IndexBuffer )

	
//	RCUBE_ARR_DELETE( FlameInstIndNumber )
//	RCUBE_ARR_DELETE( FlameInstDistance )
	
//	RCUBE_ARR_DELETE( SmokeInstIndNumber )
//	RCUBE_ARR_DELETE( SmokeInstDistance )

	RCUBE_ARR_DELETE( TorchFireFlamesAddData )
	RCUBE_ARR_DELETE( FlameInstances )
	RCUBE_ARR_DELETE( SmokeInstances )
	RCUBE_ARR_DELETE( FireFlyInstances )

	RCUBE_ARR_DELETE( ShaderInstanceAll )

	RCUBE_ARR_DELETE( TorchFireSmokeAddData )
	RCUBE_ARR_DELETE( TorchFireFlyAddData )
//	RCUBE_RELEASE( SmokeInstanceBuffer )
//	RCUBE_RELEASE( SmokeVertexBuffer )
//	RCUBE_RELEASE( SmokeIndexBuffer )

}


int TorchFireParticles::GetActiveParticleAmmount()
{
	return ( FlameInstanceActive );
}


// SMOKE ---------------------------------------------------------------------------
void TorchFireParticles::EmitSmokeParticles( FPSTimers& Timers )
{
	RCube_VecFloat234 StartPos;
	RCube_VecFloat234 GenPos;

	StartPos.Fl3 = TorchPosition;
	//	StartPos.Fl4.w = 0.0f;
	//	GenPos.Fl4.w = 0.0f;

	bool emitSmoke = false;

	//  Обновление данные системы частиц 
	Particles_Data *InstanceAddData = &TorchFireSmokeAddData[0];
	// Позиция и цвет частицы обновляется в массиве который отправляется в шейдер
	BB_Particle_Instance *ShaderInstance;


	if ( Timers.FrameTime < 1.0f )
	{
		TimeToEmitSmoke += Timers.FrameTime;
	}

	// Прошло ли время паузы между генерацией SMOKE
	if ( TimeToEmitSmoke > BetweenNextTimeSmoke )
	{
		TimeToEmitSmoke = 0.0f;
		emitSmoke = true;
	}

	// Создаём SMOKE
	if ( ( emitSmoke == true ) && ( SmokeInstanceActive < SmokeInstancesAmount ) )
	{
		++SmokeInstanceActive;
		int index = 0;
		bool found = false;
		while ( !found )
		{
			if ( InstanceAddData->Active == false )
				found = true;
			else
			{
				++index;
				++InstanceAddData;
			}
		}

		ShaderInstance = &SmokeInstances[index];
		// Записываем позицию новой частицы
		Gen_XYZ( SmokeStartDeviation, GenPos.Fl3 );
		GenPos.Vec += StartPos.Vec;
		ShaderInstance->position = GenPos.Fl3;

		// координаты кадры анимации
		int FrameNumber = int( ( ( float ) rand() / RAND_MAX ) * SmokeFramesAmount ) + SmokeStartFrame;
		InstanceAddData->CurentFrame = FrameNumber;
		SetInstanceStartFrame( FrameNumber, ShaderInstance->NewTexCord );

		XMFLOAT4 Cord;
		SetFireFlyInstanceStartFrame( 2, Cord );
		// Активируем частицу
		InstanceAddData->Active = true;

		// Записываем оставшееся время для затухания яркости частицы перед уничтожением
		InstanceAddData->LifeTime = 255.0f;

		// Время начала жизни частицы
//		InstanceAddData->TimePased = 0.0f;

//		InstanceAddData->Smoke = true;

		ShaderInstance->color = InitAlpha;
	}
}


void TorchFireParticles::EmitFireFlyParticles( FPSTimers& Timers )
{
	RCube_VecFloat234 StartPos;
	RCube_VecFloat234 GenPos;

	StartPos.Fl3 = TorchPosition;
	
	bool emitFireFly = false;

	Particles_Data *FireFlyInstanceAddData = &TorchFireFlyAddData[0];
	BB_Particle_Instance *FireFlyShaderInstance;

	if ( Timers.FrameTime < 1.0f )
	{
		TimeToEmitFireFly += Timers.FrameTime ;
	}

	if ( TimeToEmitFireFly > BetweenNextTimeFireFly )
	{
		TimeToEmitFireFly = 0.0f;
		emitFireFly = true;
	}

	// Создаём FYREFLY
	if ( ( emitFireFly == true ) && ( FireFlyInstancesActive < FireFlyInstancesAmount ) )
	{
		++FireFlyInstancesActive;

		int index = 0;
		bool found = false;
		while ( !found )
		{
			if ( FireFlyInstanceAddData->Active == false )
				found = true;
			else
			{
				++index;
				++FireFlyInstanceAddData;
			}
		}

		// Создаём указатели на объект ( для ускорения )
		//		InstanceAddData = &TorchFireFlamesAddData[index];
		FireFlyShaderInstance = &FireFlyInstances[index];

		// Записываем позицию новой частицы
		Gen_XYZ( FireFlyStartDeviation, GenPos.Fl3 );
		GenPos.Vec += StartPos.Vec;
		FireFlyShaderInstance->position = GenPos.Fl3;

		// координаты кадры анимации
		int FireFlyNumber = int( ( ( float ) rand() / RAND_MAX ) * 127 ); // Разбиваем кадр для нарезки 
																		// ещё на 128 кусков это произвольные FireFly 8 * 16
		FireFlyInstanceAddData->CurentFrame = FireFlyNumber;
		SetFireFlyInstanceStartFrame( FireFlyNumber, FireFlyShaderInstance->NewTexCord );

		FireFlyInstanceAddData->Angel = (( float ) rand() / RAND_MAX ) * XM_2PI;

		// Активируем частицу
		FireFlyInstanceAddData->Active = true;

		// Записываем оставшееся время для затухания яркости частицы перед уничтожением
		FireFlyInstanceAddData->LifeTime = 255.0f;

		// Время начала жизни частицы
		//		InstanceAddData->TimePased = 0.0f;

		FireFlyShaderInstance->color = InitAlpha;
		FireFlyShaderInstance->color.x *= 0.02f;
		FireFlyShaderInstance->color.w = 0.9f;
	}
}


void TorchFireParticles::UpdateFireFlyParticles( FPSTimers& Timers )
{
	bool UpdateFrame = false;
	bool UpdateData = false;

	if ( Timers.FpsRate > 0 )
	{
		float a = 1.0f / float( Timers.FpsRate );
		UpdateTimeFireFly += a;
	}
	// Скорость обновления данных для FIREFLY
	if ( UpdateTimeFireFly > 1.0f / 45 )
	{
		UpdateTimeFireFly = 0.0f;
		UpdateData = true;
	}

	for ( int i = 0; i < FireFlyInstancesAmount; ++i )
	{
		Particles_Data *PointerFireFlyAddData = &TorchFireFlyAddData[i];
		BB_Particle_Instance *ShaderInstance = &FireFlyInstances[i];

		if ( PointerFireFlyAddData->Active == true )
		{
			// Время обновлять данные FLAMES пришло ?
			if ( UpdateData )
			{
				if ( PointerFireFlyAddData->Angel > XM_2PI )
					PointerFireFlyAddData->Angel = ( ( float ) rand() / RAND_MAX ) * XM_2PI;

				// Изменяем позицию частицы 
				ShaderInstance->position.x += float( cos( PointerFireFlyAddData->Angel ) ) * 0.05f;
				ShaderInstance->position.z += float( sin( PointerFireFlyAddData->Angel ) ) * 0.05f;
				ShaderInstance->position.y += 0.1f;

				PointerFireFlyAddData->Angel += 0.0174533f * 3;

				--PointerFireFlyAddData->LifeTime -= 4.5;

				if ( PointerFireFlyAddData->LifeTime < 60.0f )
					ShaderInstance->color.w -= 0.05;
			}
		}
	}
}


void TorchFireParticles::KillFireFlyParticles()
{
	for ( int i = 0; i < FireFlyInstancesAmount; ++i )
	{
		Particles_Data *InstanceAddData = &TorchFireFlyAddData[i];
//		BB_Particle_Instance *ShaderInstance = &FireFlyInstances[i];

		if ( ( InstanceAddData->Active == true ) && ( InstanceAddData->LifeTime < 0.0f ) )
		{
			InstanceAddData->Active = false;
			--FireFlyInstancesActive;
		}
	}
}


void TorchFireParticles::SetInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 )
{
	int Column;
	int Line;

	Line = FrameNumber / UX_Amount;

	Line > 0 ? Column = FrameNumber - Line * UX_Amount : Column = FrameNumber;

	float Top = TextcordTop[ Line ];
	float Left = TextcordLeft[ Column ];

	Data2 =
	{
		Top,
		Left,
		Top + OneFrameHeight,
		Left + OneFrameWidth
	};
}


void TorchFireParticles::SetFireFlyInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 )
{
	int Column;
	int Line;

	Line = FrameNumber / FireFlyCutWidth; // Количество элементов в строке - 16
	 
	Line > 0 ? Column = FrameNumber - Line * FireFlyCutWidth :	Column = FrameNumber;

	float Top = FireFlyTop[ Line ];
	float Left = FireFlyLeft[ Column ];

	float x = FireFlyFrameCoord.x + Left;
	float y = FireFlyFrameCoord.y + Top;

	Data2 =
	{
		x,
		y,
		x + FireFlyHeight,
		y + FireFlyWidth
	};
}


void TorchFireParticles::EmitFlameParticles( FPSTimers& Timers )
{
	RCube_VecFloat234 StartPos;
	RCube_VecFloat234 GenPos;

	StartPos.Fl3 = TorchPosition;
//	StartPos.Fl4.w = 0.0f;
//	GenPos.Fl4.w = 0.0f;

	bool emitFlame = false;

	//  Обновление данные системы частиц 
	Particles_Data *InstanceAddData = &TorchFireFlamesAddData[0];
	// Позиция и цвет частицы обновляется в массиве который отправляется в шейдер
	BB_Particle_Instance *ShaderInstance;

	if ( Timers.FrameTime < 1.0f )
	{
		TimeToEmitFlames += Timers.FrameTime;
	}

	// Прошло ли время паузы между генерацией FLAMES
	if ( TimeToEmitFlames > BetweenNextTimeFlame )
	{
		TimeToEmitFlames = 0.0f;
		emitFlame = true;
	}

	// Создаём FLAME
	if ( ( emitFlame == true ) && ( FlameInstanceActive < FlameInstancesAmount ) )
	{
		++FlameInstanceActive;
		
		int index = 0;
		bool found = false;
		while ( !found )
		{
			if ( InstanceAddData->Active == false )
				found = true;
			else
			{
				++index;
				++InstanceAddData;
			}
		}

		// Создаём указатели на объект ( для ускорения )
//		InstanceAddData = &TorchFireFlamesAddData[index];
		ShaderInstance = &FlameInstances[index];
		
		// Записываем позицию новой частицы
		Gen_XYZ( FlameStartDeviation, GenPos.Fl3 );
		GenPos.Vec += StartPos.Vec;
		ShaderInstance->position = GenPos.Fl3;

		// координаты кадры анимации
		InstanceAddData->CurentFrame = FlameStartFrame;
		SetInstanceStartFrame( FlameStartFrame, ShaderInstance->NewTexCord );
		
		// Активируем частицу
		InstanceAddData->Active = true;
		
		// Записываем оставшееся время для затухания яркости частицы перед уничтожением
		InstanceAddData->LifeTime = 1.0f;

		// Время начала жизни частицы
//		InstanceAddData->TimePased = 0.0f;
		
		ShaderInstance->color = InitAlpha;
	}
}


void TorchFireParticles::UpdateSmokeParticles( FPSTimers& Timers )
{
//	int i;
//	float j = 0;
	bool UpdateFrame = false;
	bool UpdateData = false;

	if ( Timers.FpsRate > 0 )
	{
		float a = 1.0f / float( Timers.FpsRate );
//		TimePassedSmoke += a;
		UpdateTimeSmoke += a;
//		TimeAliveSmoke += a;
	}

	// Скорость обновления данных для FLAMES
	if ( UpdateTimeSmoke > 1.0f / 45 )
	{
		UpdateTimeSmoke = 0.0f;
		UpdateData = true;
	}
/*
	if ( TimePassedSmoke > 1.0f / PlayFPSSmoke ) //  Скорость смены кадров 24 кадра в секунду по умолчанию 
	{
		TimePassedSmoke = 0.0f;
		UpdateFrame = true;
	}
*/
	XMFLOAT3 FlyDirection = { 0.1f, 0.3f, 0.0f };

	for (int i = 0; i < SmokeInstancesAmount; ++i )
	{
		Particles_Data *PointerSmokeAddData = &TorchFireSmokeAddData[i];
		BB_Particle_Instance *ShaderInstance = &SmokeInstances[i];

		if ( PointerSmokeAddData->Active == true )
		{
			// Время обновлять данные FLAMES пришло ?
			if ( UpdateData )
			{
				// Изменяем позицию частицы 
				ShaderInstance->position.y += 0.03;

				--PointerSmokeAddData->LifeTime;

				if ( PointerSmokeAddData->LifeTime > 240.0f )
				{
					// Alpha W
					ShaderInstance->color.w += 0.005;
				}

				if ( PointerSmokeAddData->LifeTime < 240.0f )
				{
					// Size X
					ShaderInstance->color.x += 0.01;
				}


				if ( PointerSmokeAddData->LifeTime < 40.0f )
					ShaderInstance->color.w -= 0.005;


				//	if ( PointerSmokeAddData->CurentFrame > 32 && PointerSmokeAddData->CurentFrame < 36 )
				{
					ShaderInstance->position.y += 0.03;
					// Rotate Y
					ShaderInstance->color.z += -0.017453292;
				}


				// Изменяем размер BillBoard
				if ( PointerSmokeAddData->CurentFrame > 33 )
				{
					ShaderInstance->position.y += 0.02;
					// РАЗМЕР
					//				ShaderInstance->Alpha.x += 0.05;
					// ПОВОРОТ
					ShaderInstance->color.z += 0.017453292 / 2;
				}
			}
			// Заполняем массив расстояний частиц от камеры
//			DistanceCalk( ShaderInstance->position, D3DGC_Local->CameraPosition, SmokeInstDistance[i] );
			// Сохраняем индексы Instance в массиве индексов
//			SmokeInstIndNumber[i] = i;
		}
	}

	//	if ( FlameInstanceActive > 2 )
	//		QuickDepthSortDescending( FlameInstIndNumber, FlameInstDistance, 0, FlameInstancesAmount - 1 );

}


void TorchFireParticles::UpdateFlameParticles( FPSTimers& Timers )
{
//	int i;
//	float j = 0;
	bool UpdateFrame = false;
	bool UpdateData = false;

	if ( Timers.FpsRate > 0 )
	{
		float a = 1.0f / float( Timers.FpsRate );
		TimePassedFlames += a;
		UpdateTimeFlames += a;
	}

	// Скорость обновления данных для FLAMES
	if ( UpdateTimeFlames > 1.0f / 45 )
	{
		UpdateTimeFlames = 0.0f;
		UpdateData = true;
	}

	if ( TimePassedFlames > 1.0f / PlayFPSFlame ) //  Скорость смены кадров 24 кадра в секунду по умолчанию 
	{
		TimePassedFlames = 0.0f;
		UpdateFrame = true;
	}

	for (int i = 0; i < FlameInstancesAmount; ++i )
	{
		Particles_Data *PointerFlamesAddData = &TorchFireFlamesAddData[i];
		BB_Particle_Instance *ShaderInstance = &FlameInstances[i];

		if ( PointerFlamesAddData->Active == true )
		{
			// Время обновлять данные FLAMES пришло ?
			if ( UpdateData )
			{
				// Изменяем позицию частицы 
				ShaderInstance->position.y += 0.005;

				// Изменяем прозрачность частицы
				if ( PointerFlamesAddData->CurentFrame > 2 )
				{
					// Alpha 
					ShaderInstance->color.w += 0.05;
				}
				// РАЗМЕР
				ShaderInstance->color.x += 0.01;



				if ( PointerFlamesAddData->CurentFrame > 3 && PointerFlamesAddData->CurentFrame < 21 )
				{
					ShaderInstance->position.y += 0.01;
					ShaderInstance->color.z += -0.017453292 / 2;	//  1
				}


				// Изменяем размер BillBoard
				if ( PointerFlamesAddData->CurentFrame > 20 )
				{
					ShaderInstance->position.y += 0.02;
					// РАЗМЕР
					//				ShaderInstance->Alpha.x += 0.05;
					// ПОВОРОТ
					ShaderInstance->color.z += 0.017453292 / 2;
				}
			}
			// Время обновлять кадры пришло ? - Обновляем кадры для всех Inctance.
			if ( UpdateFrame )
			{
				// Если кадр последний, то обнуляем счётчик кадров
				if ( PointerFlamesAddData->CurentFrame == FlameFramesEnd )
				{
					PointerFlamesAddData->Active = false;
					--FlameInstanceActive;
				}
				else
				{
					++PointerFlamesAddData->CurentFrame;
					SetInstanceStartFrame( PointerFlamesAddData->CurentFrame, ShaderInstance->NewTexCord );
				}
			}

			// Заполняем массив расстояний частиц от камеры
//			DistanceCalk( ShaderInstance->position, D3DGC_Local->CameraPosition, FlameInstDistance[i] );
			// Сохраняем индексы Instance в массиве индексов
//			FlameInstIndNumber[i] = i;
		}
	}

//	if ( FlameInstanceActive > 2 )
//		QuickDepthSortDescending( FlameInstIndNumber, FlameInstDistance, 0, FlameInstancesAmount - 1 );

}


void TorchFireParticles::SmokeUpdateData( int &i )
{

}


void TorchFireParticles::UpdateLight( FPSTimers& Timers )
{
	bool UpdateLight = false;

	if ( Timers.FpsRate > 0 )
	{
		TimeToUpdateLight += 1.0f / float( Timers.FpsRate );
	}

	if ( TimeToUpdateLight > 1.0f / 10 )
	{
		TimeToUpdateLight = 0.0f;
		UpdateLight = true;
	}

	if ( UpdateLight )
	{
		float Attenuation;
		XMFLOAT3 Color;

		float X = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * 0.2f;

		Color = TempLight->color;

		Color.x += X;
		Color.y += X;
		Color.z += X;

		Attenuation = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * 1.2f;

		Attenuation += TempLight->attenuationEnd;

		EngineLight->ChangeLightAttnuationEnd( LightIndex, Attenuation );
		EngineLight->ChangeLightColor( LightIndex, Color );
	}

}


void TorchFireParticles::KillSmokeParticles()
{
	for ( int i = 0; i < SmokeInstancesAmount; ++i )
	{
		Particles_Data *InstanceAddData = &TorchFireSmokeAddData[i];
		BB_Particle_Instance *ShaderInstance = &SmokeInstances[i];

		if ( ( InstanceAddData->Active == true ) && ( InstanceAddData->LifeTime < 0.0f ) )
		{
			InstanceAddData->Active = false;
			--SmokeInstanceActive;
		}
	}
}


int TorchFireParticles::GetInstanceCount()
{
	return FlameInstanceActive;//CreatedParticlesCount;//m_instanceCount;
}


void TorchFireParticles::DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore )
{
	// http://stackoverflow.com/questions/10291862/what-is-the-best-way-to-get-distance-between-2-points-with-directxmath

	RCube_VecFloat234 Result;

	Camera.Fl3 = Camerapos;
	Object.Fl3 = ObjPos;

	Result.Vec = XMVector3Length( XMVectorSubtract( Camera.Vec, Object.Vec ) );

	if ( Result.Fl3.x < 0.0f )
		DistanceStore = -Result.Fl3.x;
	else
		DistanceStore = Result.Fl3.x;
}


template <class T> void TorchFireParticles::QuickDepthSortAscending( T* indices, float* depths, int lo, int hi )
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
	if ( lo < j ) QuickDepthSortAscending( indices, depths, lo, j );
	if ( i < hi ) QuickDepthSortAscending( indices, depths, i, hi );


}


template <class T> void TorchFireParticles::QuickDepthSortDescending( T* indices, float* depths, int lo, int hi )
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
		while ( depths[i] >= x ) i++;
		while ( depths[j] < x ) j--;
		if ( i <= j )
		{
			h = depths[i]; depths[i] = depths[j]; depths[j] = h;
			index = indices[i]; indices[i] = indices[j]; indices[j] = index;
			i++; j--;
		}
	}
	while ( i <= j );

	//  recursion
	if ( lo < j ) QuickDepthSortAscending( indices, depths, lo, j );
	if ( i < hi ) QuickDepthSortAscending( indices, depths, i, hi );
}


/*
void quick_sort( int arr[20], int low, int high )
{
int pivot, j, temp, i;
if ( low<high )
{
pivot = low;
i = low;
j = high;

while ( i<j )
{
while ( ( arr[i] <= arr[pivot] ) && ( i<high ) )
{
i++;
}

while ( arr[j]>arr[pivot] )
{
j--;
}

if ( i<j )
{
temp = arr[i];
arr[i] = arr[j];
arr[j] = temp;
}
}

temp = arr[pivot];
arr[pivot] = arr[j];
arr[j] = temp;
quick_sort( arr, low, j - 1 );
quick_sort( arr, j + 1, high );
}
}
*/


void TorchFireParticles::UpdateTorchPos( XMFLOAT3& TorchPosition )
{
//	float X, Y, Z;

	BB_Particle_Instance* Instanse = &FlameInstances[0];

	RCube_VecFloat234 StartPos;
	RCube_VecFloat234 GenPos;

	StartPos.Fl3 = TorchPosition;
	StartPos.Fl4.w = 0.0f;
	GenPos.Fl4.w = 0.0f;
/*
//1
	Instanse->position = TempPos;
	++Instanse;
	return;
//2
	Instanse->position.x = TempPos.x - 0.1f;
	Instanse->position.z = TempPos.z - 0.1f;
	Instanse->position.y = TempPos.y;
	++Instanse;
//3
	Instanse->position.z = TempPos.z + 0.2f;
	Instanse->position.y = TempPos.y;
	Instanse->position.x = TempPos.x;
	++Instanse;
//4
	Instanse->position.x = TempPos.x + 0.2f;
	Instanse->position.y = TempPos.y;
	Instanse->position.z = TempPos.z;
	++Instanse;
//5
	Instanse->position.z = TempPos.z - 0.2f;
	Instanse->position.y = TempPos.y;
	Instanse->position.x = TempPos.x;
*/
}


void TorchFireParticles::Gen_XYZ ( XMFLOAT3 &Deviation, XMFLOAT3 &Result )
{
	Result.x = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * Deviation.x;
	Result.y = 0.0f;//( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * Deviation.y;
	Result.z = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * Deviation.z;
}


void TorchFireParticles::UpdatePosition ( XMFLOAT3 &Direction, XMFLOAT3 &Result )
{
	Result.x += ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * Direction.x;
	Result.y += ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * Direction.y;
	Result.z += ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * Direction.z;
}


void TorchFireParticles::UpdateRotation( XMFLOAT3 &Direction, XMFLOAT3 &Result )
{
	Result.x = ( ( ( float ) rand() ) / RAND_MAX ) * Direction.x;
	Result.y = ( ( ( float ) rand() ) / RAND_MAX ) * Direction.y;
	Result.z = ( ( ( float ) rand() ) / RAND_MAX ) * Direction.z;
}

