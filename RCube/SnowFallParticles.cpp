#pragma once
#include "stdafx.h"
#include "SnowFallParticles.h"
#include "D3dClass.h"

SnowFallParticles::SnowFallParticles()
{
	FireTexture = nullptr;
	m_particleList = nullptr;
//	FireInstIndNumber = nullptr;
//	FireInstDistance = nullptr;
	m_vertices = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	m_instanceBuffer = nullptr;
	// -----------------   Instancing   --------------------------------
	TextcordTop = nullptr;
	TextcordLeft = nullptr;

	CurentFrame = 0;
	PlayFPS = 30.0f;
	TimePassed = 0.0f;

	CreatedParticlesCount = 0;

	Camera.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	Object.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };


}


SnowFallParticles::~SnowFallParticles()
{
}


bool SnowFallParticles::Initialize( HWND hwnd,
									 D3DGlobalContext *D3DGC,
									 ID3D11ShaderResourceView* _FireTexture,
									 int _UX_Amount,							// Количество строк в текстуре анимации
									 int _VY_Amount							// Количестко столбцов в текстуре анимации
//									 int FramesAmount						// Реальное количество анимаций в текстуре с верха , слева направо
									 )
{
	bool result;

	D3DGC_Local = D3DGC;

	FireTexture = _FireTexture;

	UX_Amount = _UX_Amount;
	VY_Amount = _VY_Amount;

	OneFrameWidth = 1.0f / VY_Amount;
	OneFrameHeight = 1.0f / UX_Amount;

	NumOfAnimeFrams = UX_Amount * VY_Amount;

	TextcordTop = new float[VY_Amount];
	TextcordLeft = new float[UX_Amount];

//	if ( FramesAmount != 0 )
//		NumOfAnimeFrams = FramesAmount;

	// Уменьшаем количество кадров, потому что их индексы начинаются с НУЛЯ 0 - 63
	--NumOfAnimeFrams;

	int UX = 0;
	int VY = 0;
	OneFrameWidth = 1.0f / UX_Amount;
	OneFrameHeight = 1.0f / VY_Amount;

	while ( VY < VY_Amount )
	{
		TextcordTop[VY] = static_cast<float>( VY * OneFrameHeight );
		++VY;
	}

	while ( UX <  UX_Amount )
	{
		TextcordLeft[UX] = static_cast<float>( UX * OneFrameWidth );
		++UX;
	}


	// Initialize the particle system.
	InitializeParticleSystem();

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers( D3DGC_Local->DX_device );
	if ( !result )
	{
		return false;
	}

	return true;
}



void SnowFallParticles::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	RCUBE_ARR_DELETE( TextcordTop );
	RCUBE_ARR_DELETE( TextcordLeft );

	return;
}


void SnowFallParticles::Frame( FPSTimers& Timers )
{
	//	srand( Timers.Seed );
	// Release old particles.
	KillParticles();

	// Emit new particles.
	EmitParticles( Timers );

	// Update the position of the particles.
	UpdateParticles( Timers );

/*
	D3D11_MAPPED_SUBRESOURCE  mapResource;
	HRESULT result = D3DGC_Local->DX_deviceContext->Map( m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource );
	if ( FAILED( result ) )
	{
		MessageBox( 0, L"Ошибка доступа к буферу частиц FireParticles.", L"Can't ->Map", 0 );
	}

	ParticleShaderInstance_FIRE * Sortedinstance;

	Sortedinstance = ( ParticleShaderInstance_FIRE * ) mapResource.pData;

	for ( int i = 0; i < CreatedParticlesCount; ++i )
	{
		*Sortedinstance = instances[FireInstIndNumber[i]];
		++Sortedinstance;
	}

	D3DGC_Local->DX_deviceContext->Unmap( m_instanceBuffer, 0 );
*/
	D3DGC_Local->DX_deviceContext->UpdateSubresource( m_instanceBuffer, NULL, NULL, instances, NULL, NULL );
}


void SnowFallParticles::Render()
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
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3DGC_Local->DX_deviceContext->IASetVertexBuffers( 0, 2, bufferPointers, strides, offsets );

	// Set the index buffer to active in the input assembler so it can be rendered.
	D3DGC_Local->DX_deviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set the type of primitive that should be rendered from this vertex buffer.
//	D3DGC_Local->DX_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set shader texture resource in the pixel shader.
	D3DGC_Local->DX_deviceContext->PSSetShaderResources( 0, 1, &FireTexture );

	// Render the triangle.
	D3DGC_Local->DX_deviceContext->DrawIndexedInstanced( 6, CreatedParticlesCount/*m_instanceCount*/, 0, 0, 0 );
}


void SnowFallParticles::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	ParticlesPositionDeviationX = 30.5f;
	ParticlesPositionDeviationY = 0.1f;
	ParticlesPositionDeviationZ = 30.0f;

	// Set the speed and speed variation of particles.
	ParticleVelocity = 0.95f;
	ParticleVelocityVariation = 0.9f;

	// Set the physical size of the particles.
	ParticleSize = 0.05f;

	// Set the number of particles to emit per second.
	ParticlesPerSecond = 500.0f;

	// Set the maximum number of particles allowed in the particle system.
	MaxParticles = 5000;

	maxActiveParticles = 5000;
	// Create the particle list.
	m_particleList = new Particles_Data[MaxParticles];

	// Создаём массив индексов элементо в массивах для сортировки по удалению
	// от камеры. Для отрисовки по мере приближения к камере.
//	FireInstIndNumber = new int[MaxParticles];
	//	memset( FireInstIndNumber , 0xffffffff, sizeof( int ) * MaxParticles );
	// Массив растояний до камеры
//	FireInstDistance = new float[MaxParticles];
	//	memset( FireInstDistance, 0, sizeof( float ) * MaxParticles );

	// Initialize the particle list.
	for ( int i = 0; i<MaxParticles; ++i )
	{
		m_particleList[i].Active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	CreatedParticlesCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	AccumulatedTime = 0.0f;

}


void SnowFallParticles::ShutdownParticleSystem()
{

	RCUBE_ARR_DELETE( m_particleList );
//	RCUBE_ARR_DELETE( FireInstIndNumber );
//	RCUBE_ARR_DELETE( FireInstDistance );

	return;
}


bool SnowFallParticles::InitializeBuffers( ID3D11Device* device )
{
	HRESULT result;

	D3D11_BUFFER_DESC BillBordVertBufferDesc, BillBordVertexesIndBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;
	m_instanceCount = MaxParticles;
	// -----------------   Instancing   --------------------------------

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new Vertex_FlatObject[4];


//	unsigned long indices[6] = { 0, 1, 2, 0, 3, 1 };

	// Set up the description of the static index buffer.
	BillBordVertexesIndBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BillBordVertexesIndBufferDesc.ByteWidth = sizeof( unsigned long ) * 6;
	BillBordVertexesIndBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BillBordVertexesIndBufferDesc.CPUAccessFlags = 0;
	BillBordVertexesIndBufferDesc.MiscFlags = 0;
	BillBordVertexesIndBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = FlatObjectIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer( &BillBordVertexesIndBufferDesc, &indexData, &m_indexBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}

	//	delete[] indices;

	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	instances = new BB_Particle_Instance[m_instanceCount];

	memset( instances, 0, ( sizeof( BB_Particle_Instance ) * m_instanceCount ) );

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof( BB_Particle_Instance ) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer( &instanceBufferDesc, &instanceData, &m_instanceBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}
	// -----------------   Instancing   --------------------------------

	// Initialize vertex array to zeros at first.
	memset( m_vertices, 0, ( sizeof( Vertex_FlatObject ) * 4 ) );

	// Top left.
	m_vertices[0].Position = XMFLOAT3( -1.0f * ParticleSize, 1.0f * ParticleSize, 0.0f );  // Top left
	m_vertices[0].TexCoord = XMFLOAT2( 0.0f, 0.0f );

	// Bottom right.
	m_vertices[1].Position = XMFLOAT3( 1.0f * ParticleSize, -1.0f * ParticleSize, 0.0f );  // Bottom right
	m_vertices[1].TexCoord = XMFLOAT2( 1.0f, 1.0f );

	// Bottom left.
	m_vertices[2].Position = XMFLOAT3( -1.0f * ParticleSize, -1.0f * ParticleSize, 0.0f );  // Bottom left.
	m_vertices[2].TexCoord = XMFLOAT2( 0.0f, 1.0f );

	// Top right.
	m_vertices[3].Position = XMFLOAT3( 1.0f * ParticleSize, 1.0f * ParticleSize, 0.0f );  // Top right.
	m_vertices[3].TexCoord = XMFLOAT2( 1.0f, 0.0f );


	// Set up the description of the dynamic vertex buffer.
	BillBordVertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BillBordVertBufferDesc.ByteWidth = sizeof( Vertex_FlatObject ) * 4;
	BillBordVertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BillBordVertBufferDesc.CPUAccessFlags = 0;
	BillBordVertBufferDesc.MiscFlags = 0;
	BillBordVertBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer( &BillBordVertBufferDesc, &vertexData, &m_vertexBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] m_vertices;

	return true;
}


void SnowFallParticles::ShutdownBuffers()
{
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	RCUBE_RELEASE( m_instanceBuffer );
	// -----------------   Instancing   --------------------------------
	RCUBE_RELEASE( m_indexBuffer );
	RCUBE_RELEASE( m_vertexBuffer );

	RCUBE_ARR_DELETE( instances );
//	RCUBE_ARR_DELETE( m_vertices );

	return;
}


int SnowFallParticles::GetActiveParticleAmmount()
{
	return ( CreatedParticlesCount );
}


void SnowFallParticles::EmitParticles( FPSTimers& Timers )
{
	bool emitParticle, found;
	//  Обновление данные системы частиц 
	Particles_Data	Data;
	// Позиция и цвет частицы обновляется в массиве который отправляется в шейдер
	BB_Particle_Instance Data1;

	//	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index;//, i, j;

	float X, Y, Z;
	float R, G, B;

	// Increment the frame time.
	if ( Timers.FrameTime < 1.0f )
		AccumulatedTime += Timers.FrameTime ;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if ( AccumulatedTime >( 1.0f / ParticlesPerSecond ) )
	{
		AccumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ( ( emitParticle == true ) && ( CreatedParticlesCount < ( MaxParticles ) ) )
	{
		++CreatedParticlesCount;

		// Now generate the randomized particle properties.
		X = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * ParticlesPositionDeviationX;// + 5;
		Y = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * ParticlesPositionDeviationY + 15;
		Z = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * ParticlesPositionDeviationZ;

		Data1.position = { X,Y,Z };

		Data.Velocity = ParticleVelocity + ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * ParticleVelocityVariation;

		R = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) + 0.5f;
		G = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) + 0.5f;
		B = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) + 0.5f;

		Data1.color = { R,G,B,0.0f };

		Data1.Dummy = 1.0f;

		int FrameNumber = int( ( ( float ) rand() / RAND_MAX ) * NumOfAnimeFrams );
		Data.CurentFrame = FrameNumber;
		SetInstanceStartFrame( FrameNumber, Data1.NewTexCord );


		Data.Active = true;
		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while ( !found )
		{
			if ( ( m_particleList[index].Active == false ) )//|| ( instances[index].position.z < Data1.position.z )
			{
				found = true;
			}
			else
			{
				++index;
			}
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index] = Data;

		instances[index] = Data1;

	}

	return;
}


void SnowFallParticles::UpdateParticles( FPSTimers& Timers )
{
	float j = 0;
	bool UpdateFrame = false;

	if ( Timers.FpsRate > 0 )
		TimePassed += 1.0f / float( Timers.FpsRate );

	if ( TimePassed > 1.0f / PlayFPS ) //  Скорость смены кадров 24 кадра в секунду по умолчанию 
	{
		TimePassed = 0.0f;
		UpdateFrame = true;
	}

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (int i = 0; i < CreatedParticlesCount; ++i )
	{
		if ( j > XM_2PI )
			j = 0;
		Particles_Data *Part = &m_particleList[i];
		BB_Particle_Instance *Part2 = &instances[i];
		Part2->position.y = Part2->position.y - Part->Velocity * ( Timers.FrameTime );
		Part2->position.x = float( Part2->position.x + cos( j ) * 0.006 );
		//		m_particleList[i].positionZ = m_particleList[i].positionZ + sin(j) * m_particleList[i].velocity * 0.05;
		j += 0.0174533f;

		// Вставляем свет
//		EngineLight->ChangeLightPos( Part->LightIndex, Part2->position );
		// -- Вставляем свет ---------------------------------------------

		// Время обновлять кадры пришло ? - Обновляем кадры для всех Inctance.
		if ( UpdateFrame )
		{
			// Если кадр последний, то обнуляем счётчик кадров
			if ( Part->CurentFrame == NumOfAnimeFrams )
				Part->CurentFrame = 0;
			else
				++Part->CurentFrame;

			SetInstanceStartFrame( Part->CurentFrame, Part2->NewTexCord );
		}

		// Заполняем массив расстояний частиц от камеры
//		DistanceCalk( Part2->position, D3DGC_Local->CameraPosition, FireInstDistance[i] );
		// Сохраняем индексы Instance в массиве индексов
//		FireInstIndNumber[i] = i;
	}
//	if ( CreatedParticlesCount > 1 )
	{
//		QuickDepthSortDescending( FireInstIndNumber, FireInstDistance, 0, CreatedParticlesCount - 1 );

	}

	return;
}


void SnowFallParticles::KillParticles()
{
	//	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for ( int i = 0; i < MaxParticles; ++i )
	{
		Particles_Data* Part = &m_particleList[i];
		BB_Particle_Instance* Part2 = &instances[i];
		if ( ( Part->Active == true ) && ( Part2->position.y < -3.0f ) )
		{
			Part->Active = false;
			Part2->Dummy = 0.0f;
			--CreatedParticlesCount;

			// Вставляем свет
//			EngineLight->FreeLightSource( Part->LightIndex );
			// -- Вставляем свет ---------------------------------------------------------------------------------------------------------------------
		}
	}

	return;
}


void SnowFallParticles::SetInstanceStartFrame( int FrameNumber, XMFLOAT4& Data2 )
{
//	XMFLOAT4 InstanceCoords;
	int Column;
	int Line;

	Line = FrameNumber / UX_Amount;

	Line > 0 ? Column = FrameNumber - Line * UX_Amount : Column = FrameNumber;

	float Top = TextcordTop[Line];
	float Left = TextcordLeft[Column];

	Data2 =
	{
		Top,
		Left,
		Top + OneFrameHeight,
		Left + OneFrameWidth
	};
}


void SnowFallParticles::DistanceCalk( XMFLOAT3 &ObjPos, XMFLOAT3 &Camerapos, float &DistanceStore )
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


template <class T> void SnowFallParticles::QuickDepthSortAscending( T* indices, float* depths, int lo, int hi )
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


template <class T> void SnowFallParticles::QuickDepthSortDescending( T* indices, float* depths, int lo, int hi )
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
		while ( depths[i] > x ) i++;
		while ( depths[j] < x ) j--;
		if ( i <= j )
		{
			h = depths[j]; depths[j] = depths[i]; depths[i] = h;
			index = indices[j]; indices[j] = indices[i]; indices[i] = index;
			i++; j--;
		}
	}
	while ( i <= j );

	//  recursion
	if ( lo < j ) QuickDepthSortDescending( indices, depths, lo, j );
	if ( i < hi ) QuickDepthSortDescending( indices, depths, i, hi );


}