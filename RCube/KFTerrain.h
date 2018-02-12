#pragma once
#include "stdafx.h"

#ifndef _KFTERRAIN_H_
#define _KFTERRAIN_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "LightClass.h"

using namespace DirectX;


const int TEXTURE_REPEAT = 8;

class KFTerrain {
public:

	// эта структура указывает террэйну как он должен создаватьс€ если char* filename = nullptr; то он генерит террэйн рандомно
	struct KFLandscapeParam
	{

		//*********************** дл€ рандомной генерации вершин*****************************
		int HeightesCout, LowlandsCout;
		int MaxHeight, MinHeight;
		int MaxRadius, MinRadius;
		//*********************** дл€ рандомной генерации вершин*****************************

		//*********************** дл€ генерации по зананной текстуре ************************
		char* filename;
		//*********************** дл€ генерации по зананной текстуре ************************

	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct Point
	{
		int x, y;
	};

	int g_Rows, g_Columns;

private :
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct  VertexType
	{

		XMFLOAT3 Position;
//		float pading;
		XMFLOAT2 TextCord;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;

	};

	struct PositionType
	{
		XMFLOAT3 position;
		float padding;
		XMFLOAT4 rotation;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	struct VectorType
	{
		float x, y, z;
	};

	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	//ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_posesBuffer;
	LightClass* g_ActiveLight;

	bool GenersteRandTerrain( KFLandscapeParam* LandParam, float VertixesIndent);
	bool ReadTerrainFromTexture(KFLandscapeParam* LandParam, float VertixesIndent);
	bool CalculateNormals();
	void CalculateTextureCoordinates();
	bool InitializeBuffers(ID3D11Device* device, int ClustersWidth, int ClusterHeight);
	void GenerateLandScape(KFLandscapeParam* LandParam, HeightMapType* m_heightMap);
	int GenerateRandRadius(int MaxRadius, int MinRadius);
	KFTerrain::Point GenerateRandPoint();
	void CheckDrawLenghtParam(HWND hwnd);
	void CheckClustersVertixes(HWND hwnd);
	int PreviosIndex;

	int ComeputeIndex(int X, int Z);

	int m_vertexCount;
	int m_indexCount;
	int g_ClusterHeight , g_ClusterWidth;
	int NumXVerticesInCluster, NumZVerticesInCluster;
	int NumOfVerticesOnCluster;
	int g_DrawLength;
	int NumOfDrawingClusters;
	VertexType ** ClusterVertices;
	int NumOfClusters;
	ID3D11Device* g_Device;
	int * ClusterNumOfinitElements;
	int VertixesDrawLengthX, VertixesDrawLengthZ;
	int NumOfDrawingXVertixes, NumOfDrawingZVertixes;
	int NumOfIndexesOnCluster;
	HWND g_hwnd;
	
	void LandParamChecker(KFLandscapeParam* LandParam);

public:
	KFTerrain();
	~KFTerrain();

	HRESULT Initialize(HWND hwnd, ID3D11Device* device, HWND, ID3D10Blob * vertexShaderBuffer, ID3D11DeviceContext* devCon,
		ID3D11ShaderResourceView* TerrTexture, float& Rows, float& Columns, KFLandscapeParam* LandParam
		, float VertixesIndent, int ClustersWidth, int ClusterHeight, int DrawLength, LightClass* ActiveLight);
	void LightRender();
	void Render();
	void Frame(bool CameraReplaseData, XMVECTOR &CameraPosition);

	ID3D11DeviceContext *  g_deviceContext;
	ID3D11ShaderResourceView * g_Texture;
	LightBufferType LightDesc;
	HeightMapType * m_heightMap; // двумерный массив высот [x кордината][y кордината]
	ID3D11Buffer * m_vertexBuffer , *m_indexBuffer;
	float g_VertixesIndent;


};



#endif