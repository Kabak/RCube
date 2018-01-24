#include "stdafx.h"
#include "KFTerrain.h"


KFTerrain::KFTerrain() 
{

	m_indexBuffer = nullptr;
	m_vertexBuffer = nullptr;
	m_heightMap = nullptr;
	PreviosIndex = -1;
	m_posesBuffer = nullptr;

}

HRESULT KFTerrain::Initialize(HWND hwnd, D3DGlobalContext *D3DGC, 
	ID3D11ShaderResourceView* TerrTexture  , 
							   float& Rows, 
							   float& Columns, 
							   KFLandscapeParam* LandParam, 
							   float VertixesIndent,  
							   int ClustersWidth, 
							   int ClusterHeight, 
							   int DrawLength, 
							   D3DClass* ActiveLight) {

	Local_D3DGC = D3DGC;
	g_ActiveLight = ActiveLight;
	HRESULT result;

	g_hwnd = hwnd;

	g_DrawLength = DrawLength;
	g_Rows = (int)Rows;
	g_Columns = (int)Columns;

	g_ClusterHeight = ClusterHeight;
	g_ClusterWidth = ClustersWidth;

	CheckClustersVertixes(hwnd);
	CheckDrawLenghtParam(hwnd);

	g_Texture = TerrTexture;
	g_VertixesIndent = VertixesIndent;


	bool bool_result;


	// Load in the height map for the terrain.
	if (LandParam->filename == nullptr)
		bool_result = GenersteRandTerrain(LandParam, VertixesIndent);
	else
		bool_result = ReadTerrainFromTexture(LandParam, VertixesIndent);
	if (!bool_result)
	{
		return E_FAIL;
	}

	// Normalize the height of the height map.
	//NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	bool_result = CalculateNormals();
	if (!bool_result)
	{
		return E_FAIL;
	}


		// Calculate the texture coordinates.
		CalculateTextureCoordinates();

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
		bool_result = InitializeBuffers( Local_D3DGC->DX_device,  ClustersWidth, ClusterHeight);
	if (!bool_result)
	{
		return E_FAIL;
	}

	D3D11_BUFFER_DESC posesBufferDesc;
	D3D11_SUBRESOURCE_DATA posesData;

	posesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	posesBufferDesc.ByteWidth = sizeof(PositionType);

	posesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	posesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	posesBufferDesc.MiscFlags = 0;
	posesBufferDesc.StructureByteStride = 0;

	TerrainPositionType ThisObjDesc;

	ThisObjDesc.position = XMFLOAT3(0.0f , 0.0f , 0.0f);
	ThisObjDesc.rotation = XMFLOAT4(0.0f , 0.0f , 0.0f , 0.0f);

	// Give the subresource structure a pointer to the instance data.
	posesData.pSysMem = &ThisObjDesc;
	posesData.SysMemPitch = 0;
	posesData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = Local_D3DGC->DX_device->CreateBuffer(&posesBufferDesc, &posesData, &m_posesBuffer);
	if (FAILED(result))
	{
		return result;
	}

	return S_OK;
}


bool KFTerrain::InitializeBuffers(ID3D11Device* device, int ClustersWidth, int ClusterHeight)
{
//	Vertex_Model3D* vertices;
	unsigned int* indices;
	int index, i, j;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index1, index2, index3, index4;
	int c = 0;
	NumOfClusters = ClustersWidth * ClusterHeight;
	NumXVerticesInCluster = g_Rows / ClustersWidth;
	NumZVerticesInCluster = g_Columns / ClusterHeight;
	NumOfVerticesOnCluster = NumXVerticesInCluster * NumZVerticesInCluster;
	NumOfDrawingClusters = (1 + g_DrawLength * 2) * (1 + g_DrawLength * 2);
	NumOfDrawingXVertixes = (1 + g_DrawLength * 2) * NumXVerticesInCluster;
	NumOfDrawingZVertixes = (1 + g_DrawLength * 2) * NumZVerticesInCluster;



	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = NumOfVerticesOnCluster * NumOfDrawingClusters;

	// Set the index count to the same as the vertex count.
	m_indexCount = (NumOfDrawingXVertixes - 1) * (NumOfDrawingZVertixes - 1) * 6;

	NumOfIndexesOnCluster = (NumXVerticesInCluster - 1) * (NumZVerticesInCluster - 1) * 6;

	// Create the vertex array.
/*	vertices = new Vertex_Model3D[m_vertexCount];
	if (!vertices)
	{
		return false;
	}
*/
	// Create the index array.
	indices = new unsigned int[m_indexCount];
	if (!indices)
	{
		return false;
	}

	ClusterNumOfinitElements = new int [NumOfClusters];
	ClusterVertices = new Vertex_Model3D*[NumOfClusters];
	if (!ClusterVertices)
	{
		return false;
	}

	c = 0;
	while (c < NumOfClusters) {

		ClusterVertices[c] = new Vertex_Model3D[NumOfVerticesOnCluster];
		ClusterNumOfinitElements[c] = 0;

		++c;
	}


	// Initialize the index to the vertex buffer.
	index = 0;
	int XClusterIndex, ZClusterIndex ,  ClusterIndex;

		// Load the vertex and index array with the terrain data.
		for (j = 0; j<(g_Columns); j++)
		{
			for (i = 0; i<(g_Rows); i++)
			{
				
				ZClusterIndex = j / (g_Columns / ClusterHeight);
				XClusterIndex = i / (g_Rows / ClustersWidth);
				
				ClusterIndex = (ZClusterIndex * ClustersWidth) + XClusterIndex;

				//float HV = ((float)i / (float)m_terrainWidth) * 1.0f;
				//float Hv2 = ((float)j / (float)m_terrainWidth) * 1.0f;

				ClusterVertices[ClusterIndex][ClusterNumOfinitElements[ClusterIndex]].Position = XMFLOAT3(m_heightMap[index].x, m_heightMap[index].y, m_heightMap[index].z);
				ClusterVertices[ClusterIndex][ClusterNumOfinitElements[ClusterIndex]].TexCoord = XMFLOAT2(((float)i / (float)g_Rows) * 1.0f, ((float)j / (float)g_Columns) * 1.0f);
				ClusterVertices[ClusterIndex][ClusterNumOfinitElements[ClusterIndex]].Normal = XMFLOAT3(m_heightMap[index].nx, m_heightMap[index].ny, m_heightMap[index].nz);
				ClusterVertices[ClusterIndex][ClusterNumOfinitElements[ClusterIndex]].Tangent = XMFLOAT3(0.0f , 0.0f , 0.0f);
                ++ClusterNumOfinitElements[ClusterIndex];


				index++;
			}
		}



	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof( Vertex_Model3D ) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, NULL/*&vertexData*/, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// CreateIndexBuffer

	// Load the vertex and index array with the terrain data.
	index = 0;
	for (j = 0; j<(NumOfDrawingZVertixes - 1); ++j)
	{
		for (i = 0; i<(NumOfDrawingXVertixes - 1); ++i)
		{

			index1 = ComeputeIndex(i, j);          // Bottom left.
			index2 = ComeputeIndex(i, j + 1);      // Bottom right.
			index3 = ComeputeIndex(i + 1, j);      // Upper left.
			index4 = ComeputeIndex(i + 1, j + 1);  // Upper right.

			// �������� ��� ������� �� �������� �� �������� 0_0
			/*indices[index++] = index1;
			indices[index++] = index2;
			indices[index++] = index4;
			indices[index++] = index1;
			indices[index++] = index4;
			indices[index++] = index3;*/

			indices[index++] = index1;
			indices[index++] = index4;
			indices[index++] = index2;
			indices[index++] = index1;
			indices[index++] = index3;
			indices[index++] = index4;


			//ResultIndex = ComeputeIndex(i , j);
			

		}
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
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
//	delete[] vertices;
//	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;
}




bool KFTerrain::GenersteRandTerrain( KFLandscapeParam* LandParam, float VertixesIndent) {

	int  i, j, k, index = 0;
	float RealPixelVertexX, RealPixelVertexY;

	RealPixelVertexX = -(VertixesIndent * ((g_Rows - 1) / 2));
	RealPixelVertexY = -(VertixesIndent * ((g_Columns - 1) / 2));

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[g_Rows * g_Columns];
	if (!m_heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

//	int sj = -(g_Columns / 2), si = -(g_Rows / 2);

	// Read the image data into the height map.
	for (j = 0; j<g_Columns; j++)
	{
//		si = -(g_Rows / 2);
		RealPixelVertexX = -(VertixesIndent * ((g_Rows - 1) / 2));
		for (i = 0; i<g_Rows; i++)
		{
			//height = -10000000.0f;

//			si++;
			index = (g_Rows * j) + i;

			m_heightMap[index].x = RealPixelVertexX;
			m_heightMap[index].y = 0.0f;
			m_heightMap[index].z = RealPixelVertexY;

			//++index;
			RealPixelVertexX += VertixesIndent;

			k += 3;
		}

		RealPixelVertexY += VertixesIndent;
//		sj++;
	}

	GenerateLandScape(LandParam, m_heightMap);

	return true;

}


bool  KFTerrain::ReadTerrainFromTexture(KFLandscapeParam* LandParam, float VertixesIndent) {

	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize;
	unsigned char* bitmapImage;
	int  i, j, index = 0 , index2 = 0;
	float LandX = 0.0f, LandY = 0.0f; // ��������� � ��������
	
	float RealPixelVertexX, RealPixelVertexY;

	RealPixelVertexX = -(VertixesIndent * ((g_Rows - 1) / 2));
	RealPixelVertexY = -(VertixesIndent * ((g_Columns - 1) / 2));

	int TextureWidth, TextureHeight;

	// Open the height map file in binary.
	error = fopen_s(&filePtr, LandParam->filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (UINT)fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = (UINT)fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	TextureWidth = bitmapInfoHeader.biWidth;
	TextureHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = TextureWidth * TextureHeight * 4;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = (UINT)fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	int sj = -(g_Columns / 2), si = -(g_Rows / 2);

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[g_Rows * g_Columns];
	if (!m_heightMap)
	{
		return false;
	}


	// Read the image data into the height map.
	for (j = 0; j<g_Columns; j++)
	{
		si = -(g_Rows / 2);
		RealPixelVertexX = -(VertixesIndent * ((g_Rows - 1) / 2));
		for (i = 0; i<g_Rows; i++)
		{
			//height = -10000000.0f;

			si++;

			LandX = (float)((float)i / g_Rows) * TextureWidth;
			LandY = (float)((float)j / g_Columns) * TextureHeight;

			// ������ � ������� m_heightMap
			index = (g_Rows * j) + i;
			// ������ � ��������
			index2 = (TextureWidth * (int)LandY) + (int)LandX;

			m_heightMap[index].x = RealPixelVertexX;
			m_heightMap[index].y = (float)bitmapImage[index2 * 4] - 127;
			m_heightMap[index].z = RealPixelVertexY;

			//	index2 += 3;
			//++index;
			RealPixelVertexX += VertixesIndent;

		}

		RealPixelVertexY += VertixesIndent;
		sj++;
	}

	delete[]bitmapImage;

	return true;
}

void KFTerrain::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)g_Rows;

	// Calculate how many times to repeat the texture.
	incrementCount = g_Rows / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j<g_Columns; j++)
	{
		for (i = 0; i<g_Rows; i++)
		{
			// Store the texture coordinate in the height map.
			m_heightMap[(g_Columns * i) + j].tu = tuCoordinate;
			m_heightMap[(g_Columns * i) + j].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}


void KFTerrain::LightRender() 
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set vertex buffer stride and offset.
	strides[0] = sizeof( Vertex_Model3D );
	strides[1] = sizeof(PositionType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_posesBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	Local_D3DGC->DX_deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the index buffer to active in the input assembler so it can be rendered.
	Local_D3DGC->DX_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set shader texture resource in the pixel shader.
//	Local_D3DGC->DX_deviceContext->PSSetShaderResources(0, 1, &g_Texture);

	// Render the triangle.
	g_ActiveLight->RenderIndextedClustered(g_Texture, m_indexCount, 1);

}


void KFTerrain::Render() 
{

	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set vertex buffer stride and offset.
	strides[0] = sizeof( Vertex_Model3D );
	strides[1] = sizeof(PositionType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_posesBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	Local_D3DGC->DX_deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the index buffer to active in the input assembler so it can be rendered.
	Local_D3DGC->DX_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set shader texture resource in the pixel shader.
	Local_D3DGC->DX_deviceContext->PSSetShaderResources(0, 1, &g_Texture);

	// Render the triangle.
	Local_D3DGC->DX_deviceContext->DrawIndexed(m_indexCount, 0, 0);

}

bool KFTerrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	TerrainVectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new TerrainVectorType[(g_Columns - 1) * (g_Rows - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(g_Columns - 1); j++)
	{
		for (i = 0; i<(g_Rows - 1); i++)
		{
			index1 = (j * g_Rows) + i;
			index2 = (j * g_Rows) + (i + 1);
			index3 = ((j + 1) * g_Rows) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (g_Rows - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j<g_Columns; j++)
	{
		for (i = 0; i<g_Rows; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (g_Rows - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (g_Rows - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (g_Rows - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (g_Columns - 1)))
			{
				index = (j * (g_Rows - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (g_Rows - 1)) && (j < (g_Columns - 1)))
			{
				index = (j * (g_Rows - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * g_Rows) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}


KFTerrain::~KFTerrain() {

	RCUBE_ARR_DELETE ( ClusterNumOfinitElements );

	int c = 0;

	if (m_posesBuffer != nullptr)
		m_posesBuffer->Release();

	while (c < NumOfClusters) {

		delete[] ClusterVertices[c];

		++c;
	}

	delete[] ClusterVertices;

	RCUBE_ARR_DELETE(m_heightMap)

	RCUBE_RELEASE(m_indexBuffer);
	RCUBE_RELEASE(m_vertexBuffer)

}

void KFTerrain::GenerateLandScape(KFLandscapeParam* LandParam, HeightMapType* m_heightMap) {
	// ��� ����������� ������
	int c = 0, f, e;
	Point PosePoint;
	float HeighValue;
	int index;
	float MinXRadiusComute, MinYRadiusComute, MaxXRadiusComute, MaxYRadiusComute;
	float Radius, distFromPointToCent;

	LandParamChecker(LandParam);

	while (c < LandParam->LowlandsCout) {
		PosePoint = GenerateRandPoint();

		HeighValue = -(float)(rand() % LandParam->MinHeight);
		index = (g_Rows * PosePoint.y) + PosePoint.x;
		m_heightMap[index].y = HeighValue;

		Radius = (float)GenerateRandRadius(LandParam->MaxRadius, LandParam->MinRadius);

		MinXRadiusComute = PosePoint.x - Radius;
		MinYRadiusComute = PosePoint.y - Radius;
		MaxXRadiusComute = PosePoint.x + Radius;
		MaxYRadiusComute = PosePoint.y + Radius;

		f = (int)MinYRadiusComute;
		while (f < MaxYRadiusComute) {
			e = (int)MinXRadiusComute;
			while (e < MaxXRadiusComute) {

				if ((e > 0 && e < g_Rows) && (f > 0 && f < g_Columns)) {

					distFromPointToCent = (float)sqrt((f - PosePoint.y)*(f - PosePoint.y) + (e - PosePoint.x)*(e - PosePoint.x));
					if (distFromPointToCent <= Radius) {
						index = (g_Rows * f) + e;
						m_heightMap[index].y = (HeighValue - (distFromPointToCent * HeighValue) / Radius) * g_VertixesIndent;

					}

				}
				++e;
			}

			++f;
		}


		c++;
	}

	c = 0;
	while (c < LandParam->HeightesCout) {
		PosePoint = GenerateRandPoint();
		HeighValue = (float)(rand() % LandParam->MaxHeight);

		index = (g_Rows * PosePoint.y) + PosePoint.x;
		m_heightMap[index].y = HeighValue;

		Radius = (float)GenerateRandRadius(LandParam->MaxRadius, LandParam->MinRadius);

		MinXRadiusComute = PosePoint.x - Radius;
		MinYRadiusComute = PosePoint.y - Radius;
		MaxXRadiusComute = PosePoint.x + Radius;
		MaxYRadiusComute = PosePoint.y + Radius;

		f = (int)MinYRadiusComute;
		while (f < MaxYRadiusComute) {
			e = (int)MinXRadiusComute;
			while (e < MaxXRadiusComute) {

				if ((e > 0 && e < g_Rows) && (f > 0 && f < g_Columns)) {

					distFromPointToCent = (float)sqrt((f - PosePoint.y)*(f - PosePoint.y) + (e - PosePoint.x)*(e - PosePoint.x));
					if (distFromPointToCent <= Radius) {
						index = (g_Rows * f) + e;
						m_heightMap[index].y = (HeighValue - (distFromPointToCent * HeighValue) / Radius) * g_VertixesIndent;

					}

				}
				++e;
			}

			++f;
		}


		c++;
	}

}

int KFTerrain::GenerateRandRadius(int MaxRadius, int MinRadius) {
	int Result = MinRadius - 1;

	while (Result < MinRadius)
		Result = rand() % MaxRadius;

	return Result;
}

Point KFTerrain::GenerateRandPoint() {
	Point Result;

	Result.x = rand() % g_Rows;
	Result.y = rand() % g_Columns;

	return Result;
}

int KFTerrain::ComeputeIndex(int X , int Z) {


	int ZClusterIndex = Z / (NumOfDrawingZVertixes / (1 + g_DrawLength * 2));
	int XClusterIndex = X / (NumOfDrawingXVertixes / (1 + g_DrawLength * 2));

	int ClusterIndex = (ZClusterIndex * (1 + g_DrawLength * 2)) + XClusterIndex;

	int VertexXInCluster = X - (XClusterIndex * NumXVerticesInCluster);
	int VertexZInCluster = Z - (ZClusterIndex * NumZVerticesInCluster);

	int VertIndexIncluster = (VertexZInCluster * NumXVerticesInCluster) + VertexXInCluster;


	return VertIndexIncluster + (NumOfVerticesOnCluster * ClusterIndex);


}

void KFTerrain::Frame(bool CameraReplaseData, XMVECTOR &CameraPosition) {
	if (CameraReplaseData) {

		int ClusterX, ClusterZ;
		int * DrawingClustersIndexes = new int[NumOfDrawingClusters];
		int CamX = (int)((g_Rows / 2) + (XMVectorGetX(CameraPosition) / g_VertixesIndent));
		int CamZ = (int)((g_Columns / 2) + (XMVectorGetZ(CameraPosition) / g_VertixesIndent));

		ClusterZ = CamZ / (g_Columns / g_ClusterHeight);
		ClusterX = CamX / (g_Rows / g_ClusterWidth);



		ClusterZ < g_DrawLength ? ClusterZ = g_DrawLength : ClusterZ;

		ClusterZ > g_ClusterHeight - g_DrawLength - 1 ? ClusterZ = g_ClusterHeight - g_DrawLength - 1 : ClusterZ;
		
		ClusterX < g_DrawLength ? ClusterX = g_DrawLength : ClusterX;
		
		ClusterX > g_ClusterWidth - g_DrawLength - 1 ? ClusterX = g_ClusterWidth - g_DrawLength - 1 : ClusterZ;

		int c = ClusterX - g_DrawLength;
		int f = ClusterZ - g_DrawLength;

		c = 0;
		f = 0;
		int Index = 0;
		Index = (ClusterZ * g_ClusterWidth) + ClusterX;

		if (PreviosIndex != Index) {

			PreviosIndex = Index;
			D3D11_MAPPED_SUBRESOURCE mapperResurse;
			Vertex_Model3D * vertixesPtr;

			Local_D3DGC->DX_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapperResurse);

			vertixesPtr = (Vertex_Model3D*)mapperResurse.pData;

			size_t Vertex_Type_Size = sizeof( Vertex_Model3D );

			f = ClusterZ - g_DrawLength;
			while (f < ClusterZ + g_DrawLength + 1) 
			{
				c = ClusterX - g_DrawLength;
				while (c < ClusterX + g_DrawLength + 1) 
				{

					Index = (f * g_ClusterWidth) + c;

					int Elevent = ClusterNumOfinitElements[Index];

					memcpy(vertixesPtr, ClusterVertices[Index], Vertex_Type_Size * Elevent );
					vertixesPtr += Elevent;

					++c;
				}
				++f;
			}

			Local_D3DGC->DX_deviceContext->Unmap(m_vertexBuffer, 0);

		}

		delete[] DrawingClustersIndexes;
	}
}

void KFTerrain::CheckDrawLenghtParam(HWND hwnd) {

	if (g_DrawLength < 1) {

		MessageBox(hwnd, L"��������� ���������� ������ ���������� ��������� ������������ �������� 1.Terrain", L"Error", MB_OK);
		g_DrawLength = 1;
		return;

	}
	

	int MinimalClusteringSize;

	if (g_ClusterHeight > g_ClusterWidth)
		MinimalClusteringSize = g_ClusterHeight;
	else
		MinimalClusteringSize = g_ClusterWidth;

	if ((g_DrawLength > (MinimalClusteringSize / 2) - 1) && g_DrawLength != 1) {

		MessageBox(hwnd, L"��������� ���������� ������ ����������� ��������� ������������ ������������ ��������.Terrain", L"Error", MB_OK);

		if (MinimalClusteringSize != 3)
			g_DrawLength = (MinimalClusteringSize / 2) - 1;
		else
			g_DrawLength = 1;

	}

}

void KFTerrain::CheckClustersVertixes(HWND hwnd) {

	bool Check;

	if (g_Rows % g_ClusterWidth != 0.0f) {
		MessageBox(hwnd, L"����� ������ �������� ����� ��������� �� ��� X, ��������� �������������� ����������.Terrain", L"Error", MB_OK);
		Check = false;

		while (!Check)
		{
			g_Rows++;
			if (g_Rows % g_ClusterWidth == 0.0f)
				Check = true;
		}
	}


	if (g_Columns % g_ClusterHeight != 0.0f) {
		MessageBox(hwnd, L"����� ������ �������� ����� ��������� �� ��� Y, ��������� �������������� ����������.Terrain", L"Error", MB_OK);
		Check = false;

		while (!Check)
		{
			g_Columns++;
			if (g_Columns % g_ClusterHeight == 0.0f)
				Check = true;
		}
	}

}

void KFTerrain::LandParamChecker(KFLandscapeParam* LandParam) {

	if (LandParam->MinRadius < 1)
	{

		MessageBox(g_hwnd, L"����������� ������ �����/����� ������ 1, ������� � 1.Terrain", L"Error", MB_OK);
		LandParam->MinRadius = 1;

	}

	if (LandParam->MaxRadius <= LandParam->MinRadius)
	{

		MessageBox(g_hwnd, L"������������ ������ ��� ������ ���� ������ ��� �����������.������� � ��������� ���������.Terrain", L"Error", MB_OK);
		LandParam->MaxRadius = LandParam->MinRadius + 1;

	}

	if (LandParam->HeightesCout < 0) {

		MessageBox(g_hwnd, L"���������� ����� �� ����� ���� ������ 0. ������� � 0.Terrain", L"Error", MB_OK);
		LandParam->HeightesCout = 0;

	}

	if (LandParam->LowlandsCout < 0) {

		MessageBox(g_hwnd, L"���������� ����� �� ����� ���� ������ 0. ������� � 0.Terrain", L"Error", MB_OK);
		LandParam->LowlandsCout = 0;

	}

	if (LandParam->MaxHeight < 0) {

		MessageBox(g_hwnd, L"������������ �������� ������ ��� �� ����� ���� ������ 0. ������� � 0.Terrain", L"Error", MB_OK);
		LandParam->MaxHeight = 0;

	}

	if (LandParam->MinHeight < 0) {

		MessageBox(g_hwnd, L"����������� �������� ������� ����� �� ����� ���� ������ 0. ������� � 0.Terrain", L"Error", MB_OK);
		LandParam->MinHeight = 0;

	}
}