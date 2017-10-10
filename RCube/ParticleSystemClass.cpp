#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ParticleSystemClass.h"


ParticleSystemClass::ParticleSystemClass()
{
	m_Texture		= nullptr;
	m_particleList	= nullptr;
	m_vertices		= nullptr;
	m_vertexBuffer	= nullptr;
	m_indexBuffer	= nullptr;

//	m_sampleState	= nullptr;
	m_layout		= nullptr;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	m_instanceBuffer= nullptr;
	// -----------------   Instancing   --------------------------------

	TempLight		= nullptr;

}


ParticleSystemClass::~ParticleSystemClass()
{
}


bool ParticleSystemClass::Initialize(HWND hwnd, D3DGlobalContext *D3DGC, ID3D10Blob* Blob, ID3D11ShaderResourceView* texture, D3DClass *_EngineLight )
{
	bool result;

	D3DGC_Local = D3DGC;
	m_Texture = texture;

	// Получаем указатель на класс LIght в движке, для манипуляцией источниками света для частиц
	EngineLight = _EngineLight;

	// Initialize the particle system.
	result = InitializeParticleSystem();
	if(!result)
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers( D3DGC_Local->D11_device, Blob);
	if(!result)
	{
		return false;
	}

	TempLight = new PointLight;

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


void ParticleSystemClass::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	RCUBE_DELETE( TempLight );

	return;
}


void ParticleSystemClass::Frame( FPSTimers& Timers )
{
	srand( Timers.Seed );
	// Release old particles.
	KillParticles();

	// Emit new particles.
	EmitParticles( Timers.FrameTime);
	
	// Update the position of the particles.
	UpdateParticles( Timers.FrameTime);

	// Update the dynamic vertex buffer with the new position of each particle.
	D3DGC_Local->D11_deviceContext->UpdateSubresource ( m_instanceBuffer, NULL, NULL, instances, NULL, NULL );

}


void ParticleSystemClass::Render()
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers( D3DGC_Local->D11_deviceContext );

	return;
}

int ParticleSystemClass::GetIndexCount()
{
	return m_indexCount;
}


bool ParticleSystemClass::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 30.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 30.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.9f;

	// Set the physical size of the particles.
	m_particleSize = 0.05f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 250.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 3000;

	// Create the particle list.
	m_particleList = new BasicType[m_maxParticles];
	if(!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for(int i=0; i<m_maxParticles; ++i)
	{
		m_particleList[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}


void ParticleSystemClass::ShutdownParticleSystem()
{
	// Release the particle list.
	if(m_particleList)
	{
		delete [] m_particleList;
		m_particleList = 0;
	}

	return;
}


bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device, ID3D10Blob* Blob )
{
	HRESULT result;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;
	m_instanceCount = m_maxParticles;
	// -----------------   Instancing   --------------------------------

	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = 4;

	// Set the maximum number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new Vertex_FlatObject[m_vertexCount];


	unsigned long indices[6] = { 0, 1, 2, 0, 3, 1 };

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

//	delete[] indices;

	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	instances = new ParticleShaderInstance_BASIC[m_instanceCount];

	memset(instances, 0, (sizeof( ParticleShaderInstance_BASIC ) * m_instanceCount));

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof( ParticleShaderInstance_BASIC ) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}
	// -----------------   Instancing   --------------------------------

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof( Vertex_FlatObject ) * m_vertexCount));

	// Top left.
	m_vertices[0].Position = XMFLOAT3 ( -1.0f * m_particleSize, 1.0f * m_particleSize, 0.0f );  // Top left
	m_vertices[0].TexCoord = XMFLOAT2 ( 0.0f, 0.0f );

	// Bottom right.
	m_vertices[1].Position = XMFLOAT3 ( 1.0f * m_particleSize, -1.0f * m_particleSize, 0.0f );  // Bottom right
	m_vertices[1].TexCoord = XMFLOAT2 ( 1.0f, 1.0f );

	// Bottom left.
	m_vertices[2].Position = XMFLOAT3 ( -1.0f * m_particleSize, -1.0f * m_particleSize, 0.0f );  // Bottom left.
	m_vertices[2].TexCoord = XMFLOAT2 ( 0.0f, 1.0f );

	// Top right.
	m_vertices[3].Position = XMFLOAT3 ( 1.0f * m_particleSize, 1.0f * m_particleSize, 0.0f );  // Top left.
	m_vertices[3].TexCoord = XMFLOAT2 ( 1.0f, 0.0f );


	// Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex_FlatObject ) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] m_vertices;

// LAYOUT
	D3D11_INPUT_ELEMENT_DESC BASIC_Layout[4] =
	{
		{ "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 , 0 , D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD" , 0 , DXGI_FORMAT_R32G32_FLOAT , 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA , 0 },
		{ "POSITION" , 1 , DXGI_FORMAT_R32G32B32_FLOAT , 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 },
		{ "COLOR" , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT , 1 , D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA , 1 }
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
/*
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

*/
	return true;
}


void ParticleSystemClass::ShutdownBuffers()
{
	// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
	RCUBE_RELEASE ( m_instanceBuffer );
	// -----------------   Instancing   --------------------------------
	RCUBE_RELEASE ( m_indexBuffer );
	RCUBE_RELEASE ( m_vertexBuffer );

//	RCUBE_RELEASE( m_sampleState);
	RCUBE_RELEASE( m_layout);

	delete[] instances;

	return;
}


int ParticleSystemClass::GetActiveParticleAmmount()
{
	return ( m_currentParticleCount );
}


void ParticleSystemClass::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	//  Обновление данные системы частиц 
	BasicType	Data;
	// Позиция и цвет частицы обновляется в массиве который отправляется в шейдер
	ParticleShaderInstance_BASIC Data1;

//	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	float X,Y,Z;
	float R,G,B;

	// Increment the frame time.
	if (frameTime < 1.0f )
	m_accumulatedTime += frameTime * 60;

	// Set emit particle to false for now.
	emitParticle = false;
	
	// Check if it is time to emit a new particle or not.
	if(m_accumulatedTime > (1.0f / m_particlesPerSecond ))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		++m_currentParticleCount;

		// Now generate the randomized particle properties.
		X = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationX;// + 5;
		Y = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationY + 15;
		Z = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationZ;

		Data1.position = {X,Y,Z};

		Data.Velocity = m_particleVelocity + (((float)rand()-(float)rand())/RAND_MAX) * m_particleVelocityVariation;

		R   = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		G = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		B  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		
		Data1.color = {R,G,B,0.0f};

		Data.active = true;
		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while(!found)
		{
			if((m_particleList[index].active == false) || ( instances[index].position.z < Data1.position.z))
			{
				found = true;
			}
			else
			{
				++index;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while(i != index)
		{
			m_particleList[i] = m_particleList[j];
			instances[i] = instances[j];
			--i;
			--j;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index] = Data;

// Вставляем свет
		TempLight->position = Data1.position;
		RCube_VecFloat34 TempColor;
		TempColor.Fl4 = Data1.color;
		TempLight->color = TempColor.Fl3;

		m_particleList[index].LightIndex = EngineLight->AddLightSource( *TempLight );
// -- Вставляем свет ---------------------------------------------------------------------------------------------------------------------

		instances[i] = Data1;

	}

	return;
}


void ParticleSystemClass::UpdateParticles(float frameTime)
{
	int i;
	float j = 0;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for(i=0; i<m_currentParticleCount; ++i)
	{
		if ( j > XM_2PI)
			j = 0;
		BasicType* Part = &m_particleList[i];
		ParticleShaderInstance_BASIC* Part2 = &instances[i];
		Part2->position.y = Part2->position.y - Part->Velocity * (frameTime * 60);
		Part2->position.x = float(Part2->position.x + cos(j) * 0.006);
//		m_particleList[i].positionZ = m_particleList[i].positionZ + sin(j) * m_particleList[i].velocity * 0.05;
		j += 0.0174533f;

// Вставляем свет
		EngineLight->ChangeLightPos( Part->LightIndex , Part2->position );
// -- Вставляем свет ---------------------------------------------------------------------------------------------------------------------
	}

	return;
}


void ParticleSystemClass::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for( i=0; i < m_maxParticles ; ++i )
	{
		BasicType* Part = &m_particleList[i];
		ParticleShaderInstance_BASIC* Part2 = &instances[i];
		if(( Part->active == true) && ( Part2->position.y < -3.0f))
		{
			Part->active = false;
			--m_currentParticleCount;

// Вставляем свет
			EngineLight->FreeLightSource( Part->LightIndex );
// -- Вставляем свет ---------------------------------------------------------------------------------------------------------------------

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for( j=i; j < m_maxParticles-1 ; ++j )
			{
				m_particleList[j] = m_particleList[j+1];
					 instances[j] = instances[j+1];
			}
			memset( &instances[m_maxParticles - 1] ,0 , sizeof ( ParticleShaderInstance_BASIC ) );
		}
	}

	return;
}



void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set vertex buffer stride and offset.
	strides[0] = sizeof( Vertex_FlatObject );
	strides[1] = sizeof( ParticleShaderInstance_BASIC );

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources( 0, 1, &m_Texture );

	// Set the vertex input layout.
	deviceContext->IASetInputLayout( m_layout );

	// Set the vertex and pixel shaders that will be used to render this triangle.
	// deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	// deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
//	deviceContext->PSSetSamplers( 0, 1, &m_sampleState );

	// Render the triangle.
	deviceContext->DrawIndexedInstanced( 6, m_instanceCount, 0, 0, 0 );

	return;
}

int ParticleSystemClass::GetInstanceCount()
{
	return m_instanceCount;
}

