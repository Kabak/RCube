#pragma once
#include "stdafx.h"
#include "SnowFallParticles.h"

SnowFallParticles::SnowFallParticles()
{
	FireTexture = nullptr;
	SmokeTexture = nullptr;
	HeartTexture = nullptr;
	ParticlesTexture = nullptr;
	m_particleList = nullptr;
	FireInstIndNumber = nullptr;
	FireInstDistance = nullptr;
	m_vertices = nullptr;
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;

	m_sampleState = nullptr;
	m_layout = nullptr;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	m_instanceBuffer = nullptr;
	// -----------------   Instancing   --------------------------------
	TextcordTop = nullptr;
	TextcordLeft = nullptr;

	CurentFrame = 0;
	PlayFPS = 30.0f;
	TimePassed = 0.0f;

	Camera.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	Object.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };

	TempLight = nullptr;
}


SnowFallParticles::~SnowFallParticles()
{
}


bool SnowFallParticles::Initialize( HWND hwnd,
									 D3DGlobalContext *D3DGC,
									 ID3D10Blob* Blob,
									 ID3D11ShaderResourceView* _FireTexture,
									 int _UX_Amount,							// ���������� ����� � �������� ��������
									 int _VY_Amount,							// ���������� �������� � �������� ��������
									 int FramesAmount,						// �������� ���������� �������� � �������� � ����� , ����� �������

									 LightClass *_EngineLight
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

	if ( FramesAmount != 0 )
		NumOfAnimeFrams = FramesAmount;

	// ��������� ���������� ������, ������ ��� �� ������� ���������� � ���� 0 - 63
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

	// �������� ��������� �� ����� LIght � ������, ��� ������������ ����������� ����� ��� ������
	EngineLight = _EngineLight;

	// Initialize the particle system.
	result = InitializeParticleSystem();
	if ( !result )
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers( D3DGC_Local->D11_device, Blob );
	if ( !result )
	{
		return false;
	}

	TempLight = new LightClass::PointLight;

	TempLight->attenuationBegin = 0.1f;
	TempLight->attenuationEnd = 0.5f;
	//	TempLight->color = { 1.0f, 0.7f, 1.0f };
	//	TempLight->position = { 0.0f, 0.0f, 0.0f };//;{ 0.0f, 10.0f, 00.0f }
	//	TempLight->direction = { 0.0f, 0.0f, 0.0f };//;{ 0.0f, -0.3f, -1.0f }
	TempLight->angel = 0.0f;
	TempLight->ShadowMapSliceNumber = -1;
	TempLight->Dummy = 1;
	TempLight->HaveShadow = false;

	return true;
}



void SnowFallParticles::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	RCUBE_DELETE( TempLight );

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
	HRESULT result = D3DGC_Local->D11_deviceContext->Map( m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource );
	if ( FAILED( result ) )
	{
		MessageBox( 0, L"������ ������� � ������ ������ FireParticles.", L"Can't ->Map", 0 );
	}

	ParticleShaderInstance_FIRE * Sortedinstance;

	Sortedinstance = ( ParticleShaderInstance_FIRE * ) mapResource.pData;

	for ( int i = 0; i < m_currentParticleCount; ++i )
	{
		*Sortedinstance = instances[FireInstIndNumber[i]];
		++Sortedinstance;
	}

	D3DGC_Local->D11_deviceContext->Unmap( m_instanceBuffer, 0 );
*/
	D3DGC_Local->D11_deviceContext->UpdateSubresource( m_instanceBuffer, NULL, NULL, instances, NULL, NULL );
}


void SnowFallParticles::Render()
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set vertex buffer stride and offset.
	strides[0] = sizeof( BillBordVertexes );
	strides[1] = sizeof( ParticleShaderInstance_FIRE );

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3DGC_Local->D11_deviceContext->IASetVertexBuffers( 0, 2, bufferPointers, strides, offsets );

	// Set the index buffer to active in the input assembler so it can be rendered.
	D3DGC_Local->D11_deviceContext->IASetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set the type of primitive that should be rendered from this vertex buffer.
	D3DGC_Local->D11_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Set shader texture resource in the pixel shader.
	D3DGC_Local->D11_deviceContext->PSSetShaderResources( 0, 1, &FireTexture );

	// Set the vertex input layout.
	D3DGC_Local->D11_deviceContext->IASetInputLayout( m_layout );

	// Set the vertex and pixel shaders that will be used to render this triangle.
	// deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	// deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	D3DGC_Local->D11_deviceContext->PSSetSamplers( 0, 1, &m_sampleState );

	// Render the triangle.
	D3DGC_Local->D11_deviceContext->DrawIndexedInstanced( 6, m_currentParticleCount/*m_instanceCount*/, 0, 0, 0 );
}


bool SnowFallParticles::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 30.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 30.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 0.95f;
	m_particleVelocityVariation = 0.9f;

	// Set the physical size of the particles.
	m_particleSize = 0.05f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 500.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 5000;

	// Create the particle list.
	m_particleList = new FireType1[m_maxParticles];

	// ������ ������ �������� �������� � �������� ��� ���������� �� ��������
	// �� ������. ��� ��������� �� ���� ����������� � ������.
	FireInstIndNumber = new int[m_maxParticles];
	//	memset( FireInstIndNumber , 0xffffffff, sizeof( int ) * m_maxParticles );
	// ������ ��������� �� ������
	FireInstDistance = new float[m_maxParticles];
	//	memset( FireInstDistance, 0, sizeof( float ) * m_maxParticles );

	// Initialize the particle list.
	for ( int i = 0; i<m_maxParticles; ++i )
	{
		m_particleList[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}


void SnowFallParticles::ShutdownParticleSystem()
{

	RCUBE_ARR_DELETE( m_particleList );
	RCUBE_ARR_DELETE( FireInstIndNumber );
	RCUBE_ARR_DELETE( FireInstDistance );

	return;
}


bool SnowFallParticles::InitializeBuffers( ID3D11Device* device, ID3D10Blob* Blob )
{
	HRESULT result;

	D3D11_BUFFER_DESC BillBordVertBufferDesc, BillBordVertexesIndBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;
	m_instanceCount = m_maxParticles;
	// -----------------   Instancing   --------------------------------

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new BillBordVertexes[4];


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
	result = device->CreateBuffer( &BillBordVertexesIndBufferDesc, &indexData, &m_indexBuffer );
	if ( FAILED( result ) )
	{
		return false;
	}

	//	delete[] indices;

	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	instances = new ParticleShaderInstance_FIRE[m_instanceCount];

	memset( instances, 0, ( sizeof( ParticleShaderInstance_FIRE ) * m_instanceCount ) );

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof( ParticleShaderInstance_FIRE ) * m_instanceCount;
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
	memset( m_vertices, 0, ( sizeof( BillBordVertexes ) * 4 ) );

	// Top left.
	m_vertices[0].position = XMFLOAT3( -1.0f * m_particleSize, 1.0f * m_particleSize, 0.0f );  // Top left
	m_vertices[0].texture = XMFLOAT2( 0.0f, 0.0f );

	// Bottom right.
	m_vertices[1].position = XMFLOAT3( 1.0f * m_particleSize, -1.0f * m_particleSize, 0.0f );  // Bottom right
	m_vertices[1].texture = XMFLOAT2( 1.0f, 1.0f );

	// Bottom left.
	m_vertices[2].position = XMFLOAT3( -1.0f * m_particleSize, -1.0f * m_particleSize, 0.0f );  // Bottom left.
	m_vertices[2].texture = XMFLOAT2( 0.0f, 1.0f );

	// Top right.
	m_vertices[3].position = XMFLOAT3( 1.0f * m_particleSize, 1.0f * m_particleSize, 0.0f );  // Top right.
	m_vertices[3].texture = XMFLOAT2( 1.0f, 0.0f );


	// Set up the description of the dynamic vertex buffer.
	BillBordVertBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BillBordVertBufferDesc.ByteWidth = sizeof( BillBordVertexes ) * 4;
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

	// LAYOUT
	D3D11_INPUT_ELEMENT_DESC BASIC_Layout[5] =
	{
		{ "POSITION"	, 0 , DXGI_FORMAT_R32G32B32_FLOAT	, 0 , 0							   , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "TEXCOORD"	, 0 , DXGI_FORMAT_R32G32_FLOAT		, 0 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA   , 0 },
		{ "POSITION"	, 1 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "COLOR"		, 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
        { "InsTEXCOORD" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
	};

	unsigned int numElements;
	// Get a count of the elements in the layout.
	numElements = sizeof( BASIC_Layout ) / sizeof( BASIC_Layout[0] );

	// Create the vertex input layout.
	result = device->CreateInputLayout( BASIC_Layout, numElements, Blob->GetBufferPointer(), Blob->GetBufferSize(),
										&m_layout );
	if ( FAILED( result ) )
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState( &samplerDesc, &m_sampleState );
	if ( FAILED( result ) )
	{
		return false;
	}


	return true;
}


void SnowFallParticles::ShutdownBuffers()
{
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	RCUBE_RELEASE( m_instanceBuffer );
	// -----------------   Instancing   --------------------------------
	RCUBE_RELEASE( m_indexBuffer );
	RCUBE_RELEASE( m_vertexBuffer );

	RCUBE_RELEASE( m_sampleState );
	RCUBE_RELEASE( m_layout );

	RCUBE_ARR_DELETE( instances );
//	RCUBE_ARR_DELETE( m_vertices );

	return;
}


int SnowFallParticles::GetActiveParticleAmmount()
{
	return ( m_currentParticleCount );
}


void SnowFallParticles::EmitParticles( FPSTimers& Timers )
{
	bool emitParticle, found;
	//  ���������� ������ ������� ������ 
	FireType1	Data;
	// ������� � ���� ������� ����������� � ������� ������� ������������ � ������
	ParticleShaderInstance_FIRE Data1;

	//	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index;//, i, j;

	float X, Y, Z;
	float R, G, B;

	// Increment the frame time.
	if ( Timers.FrameTime < 1.0f )
		m_accumulatedTime += Timers.FrameTime * 60;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if ( m_accumulatedTime >( 1.0f / m_particlesPerSecond ) )
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ( ( emitParticle == true ) && ( m_currentParticleCount < ( m_maxParticles ) ) )
	{
		++m_currentParticleCount;

		// Now generate the randomized particle properties.
		X = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * m_particleDeviationX;// + 5;
		Y = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * m_particleDeviationY + 15;
		Z = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * m_particleDeviationZ;

		Data1.position = { X,Y,Z };

		Data.Velocity = m_particleVelocity + ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) * m_particleVelocityVariation;

		R = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) + 0.5f;
		G = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) + 0.5f;
		B = ( ( ( float ) rand() - ( float ) rand() ) / RAND_MAX ) + 0.5f;

		Data1.color = { R,G,B,0.0f };

		int FrameNumber = int( ( ( float ) rand() / RAND_MAX ) * NumOfAnimeFrams );
		Data.CurentFrame = FrameNumber;
		SetInstanceStartFrame( FrameNumber, Data1.NewTexCord );


		Data.active = true;
		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while ( !found )
		{
			if ( ( m_particleList[index].active == false ) )//|| ( instances[index].position.z < Data1.position.z )
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

		// ��������� ����
		TempLight->position = Data1.position;
		RCube_VecFloat34 TempColor;
		TempColor.Fl4 = Data1.color;
		TempLight->color = TempColor.Fl3;

//		m_particleList[index].LightIndex = EngineLight->AddLightSource( *TempLight );
		// -- ��������� ���� ---------------------------------------------------------------------------------------------------------------------

		instances[index] = Data1;

	}

	return;
}


void SnowFallParticles::UpdateParticles( FPSTimers& Timers )
{
	int i;
	float j = 0;
	bool UpdateFrame = false;

	if ( Timers.FpsRate > 0 )
		TimePassed += 1.0f / float( Timers.FpsRate );

	if ( TimePassed > 1.0f / PlayFPS ) //  �������� ����� ������ 24 ����� � ������� �� ��������� 
	{
		TimePassed = 0.0f;
		UpdateFrame = true;
	}

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for ( i = 0; i < m_currentParticleCount; ++i )
	{
		if ( j > XM_2PI )
			j = 0;
		FireType1 *Part = &m_particleList[i];
		ParticleShaderInstance_FIRE *Part2 = &instances[i];
		Part2->position.y = Part2->position.y - Part->Velocity * ( Timers.FrameTime * 60 );
		Part2->position.x = float( Part2->position.x + cos( j ) * 0.006 );
		//		m_particleList[i].positionZ = m_particleList[i].positionZ + sin(j) * m_particleList[i].velocity * 0.05;
		j += 0.0174533f;

		// ��������� ����
//		EngineLight->ChangeLightPos( Part->LightIndex, Part2->position );
		// -- ��������� ���� ---------------------------------------------

		// ����� ��������� ����� ������ ? - ��������� ����� ��� ���� Inctance.
		if ( UpdateFrame )
		{
			// ���� ���� ���������, �� �������� ������� ������
			if ( Part->CurentFrame == NumOfAnimeFrams )
				Part->CurentFrame = 0;
			else
				++Part->CurentFrame;

			SetInstanceStartFrame( Part->CurentFrame, Part2->NewTexCord );
		}

		// ��������� ������ ���������� ������ �� ������
//		DistanceCalk( Part2->position, D3DGC_Local->CameraPosition, FireInstDistance[i] );
		// ��������� ������� Instance � ������� ��������
//		FireInstIndNumber[i] = i;
	}
//	if ( m_currentParticleCount > 1 )
	{
//		QuickDepthSortDescending( FireInstIndNumber, FireInstDistance, 0, m_currentParticleCount - 1 );

	}

	return;
}


void SnowFallParticles::KillParticles()
{
	//	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for ( int i = 0; i < m_maxParticles; ++i )
	{
		FireType1* Part = &m_particleList[i];
		ParticleShaderInstance_FIRE* Part2 = &instances[i];
		if ( ( Part->active == true ) && ( Part2->position.y < -3.0f ) )
		{
			Part->active = false;
			--m_currentParticleCount;

			// ��������� ����
//			EngineLight->FreeLightSource( Part->LightIndex );
			// -- ��������� ���� ---------------------------------------------------------------------------------------------------------------------
		}
	}

	return;
}


int SnowFallParticles::GetInstanceCount()
{
	return m_currentParticleCount;//m_instanceCount;
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

	RCube_VecFloat34 Result;

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