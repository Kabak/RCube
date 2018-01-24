#pragma once
#include "stdafx.h"

#ifndef _KFTERRAIN_H_
#define _KFTERRAIN_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "D3DClass.h"
#include "D3DGlobalContext.h"
#include "Buffers_def.h"

using namespace DirectX;


const int TEXTURE_REPEAT = 8;

class KFTerrain {
public:

	// эта структура указывает террэйну как он должен создаваться если char* filename = nullptr; то он генерит террэйн рандомно
	struct KFLandscapeParam
	{

		//*********************** для рандомной генерации вершин*****************************
		int HeightesCout, LowlandsCout;
		int MaxHeight, MinHeight;
		int MaxRadius, MinRadius;
		//*********************** для рандомной генерации вершин*****************************

		//*********************** для генерации по зананной текстуре ************************
		char* filename;
		//*********************** для генерации по зананной текстуре ************************

	};
/*
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
*/
	int g_Rows, g_Columns;

private :



	D3DGlobalContext* Local_D3DGC;

	ID3D11Buffer* m_posesBuffer;	// Буфер позиций вертексов
	D3DClass* g_ActiveLight;		// Световой класс для отрисовки

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

	int m_vertexCount;	// ОБщее количество вертексов в терейне
	int m_indexCount;	// ОБщее количество индексов в терейне
	int g_ClusterHeight , g_ClusterWidth;	// Размеры кластера на которые побита земля
	int NumXVerticesInCluster, NumZVerticesInCluster;
	int NumOfVerticesOnCluster;
	int g_DrawLength;	// Дальность отрисовки от камеры в кроличестве кластеров
	int NumOfDrawingClusters;
	Vertex_Model3D ** ClusterVertices;
	int NumOfClusters;
	ID3D11Device* g_Device;
	int * ClusterNumOfinitElements;
//	int VertixesDrawLengthX, VertixesDrawLengthZ;
	int NumOfDrawingXVertixes, NumOfDrawingZVertixes;
	int NumOfIndexesOnCluster;
	HWND g_hwnd;
	
	void LandParamChecker(KFLandscapeParam* LandParam);

public:
	KFTerrain();
	~KFTerrain();

	HRESULT Initialize(HWND hwnd, D3DGlobalContext *D3DGC,
		ID3D11ShaderResourceView* TerrTexture, float& Rows, float& Columns, KFLandscapeParam* LandParam
		, float VertixesIndent, int ClustersWidth, int ClusterHeight, int DrawLength, D3DClass* ActiveLight);
	void LightRender();
	void Render();
	void Frame(bool CameraReplaseData, XMVECTOR &CameraPosition);

	ID3D11DeviceContext *  g_deviceContext;
	ID3D11ShaderResourceView * g_Texture;
	HeightMapType * m_heightMap; // двумерный массив высот [x кордината][y кордината]
	ID3D11Buffer * m_vertexBuffer , *m_indexBuffer;
	float g_VertixesIndent;


};



#endif