#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

// The class constructor initializes the vertex and index buffer pointers to null.

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_ShaderTexture = nullptr;
	m_model = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

// The Initialize function now takes as input the file name of the model that should be loaded. 
bool ModelClass::Initialize(HWND hwnd, ID3D11Device* device , ID3D11DeviceContext * devCon, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

// In the Initialize function we now call the new LoadModel function first. It will load the model data from the file name we provide into the new m_model array. Once this model array is filled
// we can then build the vertex and index buffers from it. Since InitializeBuffers now depends on this model data you have to make sure to call the functions in the correct order.

	// Load in the model data,
	result = LoadModel(modelFilename);
	if(!result)
	{
		MessageBox(hwnd, L"Could not LoadModel object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		MessageBox(hwnd, L"Could not Model InitializeBuffers.", L"Error", MB_OK);
		return false;
	}

// The Initialize function now calls a new private function that will load the texture.

	// Load the texture for this model.
	result = LoadTexture(device , devCon, textureFilename);
	if(!result)
	{
		MessageBox(hwnd, L"Could not Model LoadTexture.", L"Error", MB_OK);
		return false;
	}


	return true;
}

// The Shutdown function will call the shutdown functions for the vertex and index buffers.

void ModelClass::Shutdown()
{
// The Shutdown function now calls a new private function to release the texture object that was loaded during initialization.
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

// In the Shutdown function we add a call to the ReleaseModel function to delete the m_model array data once we are done.
	// Release the model data.
	ReleaseModel();

	return;
}

// Render is called from the GraphicsClass::Render function. This function calls RenderBuffers to put the vertex and index buffers on the graphics pipeline so the color shader will 
// be able to render them.

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

// GetIndexCount returns the number of indexes in the model. The color shader will need this information to draw this model.

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

int ModelClass::GetVertexCount()
{
	return m_vertexCount ;
}

// GetTexture returns the model texture resource. The texture shader will need access to this texture to render the model.
ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_ShaderTexture;
}


// The InitializeBuffers function is where we handle creating the vertex and index buffers. Usually you would read in a model and create the buffers from that data file. 
// For this tutorial we will just set the points in the vertex and index buffer manually since it is only a single triangle.

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;
	
// First create two temporary arrays to hold the vertex and index data that we will use later to populate the final buffers with.

	// Set the number of vertices in the vertex array.
//	m_vertexCount = 3;

	// Set the number of indices in the index array.
//	m_indexCount = 3;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

// Loading the vertex and index arrays has changed a bit. Instead of setting the values manually we loop through all the elements in the new m_model array and copy that data from there into 
// the vertex array. The index array is easy to build as each vertex we load has the same index number as the position in the array it was loaded into.

	// Load the vertex array and index array with data.
	for(i=0; i<m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

// With the vertex array and index array filled out we can now use those to create the vertex buffer and index buffer. Creating both buffers is done in the same fashion. 
// First fill out a description of the buffer. In the description the ByteWidth (size of the buffer) and the BindFlags (type of buffer) are what you need to ensure are filled out correctly. 
// After the description is filled out you need to also fill out a subresource pointer which will point to either your vertex or index array you previously created. With the description and 
// subresource pointer you can call CreateBuffer using the D3D device and it will return a pointer to your new buffer.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

// After the vertex buffer and index buffer have been created you can delete the vertex and index arrays as they are no longer needed since the data was copied into the buffers.

	// Release the arrays now that the vertex and index buffers have been created and loaded.

	return true;
}

// The ShutdownBuffers function just releases the vertex buffer and index buffer that were created in the InitializeBuffers function.

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

// RenderBuffers is called from the Render function. The purpose of this function is to set the vertex buffer and index buffer as active on the input assembler in the GPU. 
// Once the GPU has an active vertex buffer it can then use the shader to render that buffer. This function also defines how those buffers should be drawn such as triangles, lines, fans, and so forth.
// In this tutorial we set the vertex buffer and index buffer as active on the input assembler and tell the GPU that the buffers should be drawn as triangles using the 
// IASetPrimitiveTopology DirectX function.

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

// LoadTexture is a new private function that will create the texture object and then initialize it with the input file name provided. This function is called during initialization.
bool ModelClass::LoadTexture(ID3D11Device* device , ID3D11DeviceContext * devcon, WCHAR* filename)
{
//	bool result;


	if (m_ShaderTexture != nullptr)
		m_ShaderTexture->Release();

	HRESULT HR = DirectX::CreateWICTextureFromFile(device, devcon, filename, nullptr, &m_ShaderTexture);
	if (HR == E_FAIL)
		return false;

	return true;
}

// The ReleaseTexture function will release the texture object that was created and loaded during the LoadTexture function.
void ModelClass::ReleaseTexture()
{
	if (m_ShaderTexture != nullptr)
		m_ShaderTexture->Release();
	return;
}

// This is the new LoadModel function which handles loading the model data from the text file into the m_model array variable. It opens the text file and reads in the vertex count first.
// After reading the vertex count it creates the ModelType array and then reads each line into the array. Both the vertex count and index count are now set in this function.
bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);
	
	// If it could not open the file then exit.
	if(fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if(!m_model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for(i=0; i<m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

// The ReleaseModel function handles deleting the model data array.
void ModelClass::ReleaseModel()
{
	if(m_model)
	{
		delete [] m_model;
		m_model = 0;
	}

	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return;
}

void ModelClass::GetVertexType(VertexType * _Vertexes){

		for(int i=0; i<m_vertexCount; i++)
	{
		_Vertexes[i].position = vertices[i].position;
		_Vertexes[i].texture = vertices[i].texture;
		_Vertexes[i].normal = vertices[i].normal;

	}

}

void ModelClass::GetIndexes(unsigned long* Indexes){

		for(int i=0; i<m_vertexCount; i++)
	{
		Indexes[i] = indices[i];

	}

}
