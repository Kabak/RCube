#pragma once
#include "stdafx.h"

#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <DirectXMath.h>
#include "Buffers_def.h"
#include "TerrainCluster.h"
#include "DX11Buffers.h"
#include <vector>


using namespace DirectX;
using namespace std;


class Terrain
{
public:
	Terrain ();
   ~Terrain ();

	const int TEXTURE_REPEAT = 8;

	int TerrainBuffersIndex;	// ����� ������ � ������ �������
	int TerrainTextureIndex;	// ����� �������� ������������� �� Terrain
	int Terrain_Object_Index;	// ����� ������ ���� � ������

	VertexBuffer <PositionType>* InstancePositionsBuffer;	// ����� ������� ���� Instance ������� �������� � ������

	vector <TerrainCluster*> Clusters;	// ������ ��������� �� ������� ������� Terrain
	vector <TerrainCluster*> PassedFrustumClasters;// ������ �������� ��������� ��������� Frustum

  float QuadVertexStep; // ���������� ����� ���������� � ���������
	int QuadsPerCluster;// ���������� ���������� � ����� ������� X * X , 7 * 7 , 16 * 16 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
	int ClustersAmount;	// ���������� ��������� � Terrain  X * Z
	int ClustersX_ROW;	// ���������� ��������� �� X - ROW
	int	ClustersZ_COL;	// ���������� ��������� �� Z - COLUMN
	int VertexInX_Row;	// ���������� ��������� � ���� ����� Terrain
	int VertexInZ_Col;	// ���������� ��������� � ������� ����� Terrain
	int Total_Vertexs_In_Terrain;	// ����� ���������� ��������� � Terrain
	int Total_Indexs_In_Terrain;	// ����� ���������� �������� � Terrain
	int Total_Indexs_In_Cluster;	// ����� ���������� �������� � ����� �������� �������
	int Row_Col_VertexAmountInClasterAnyDir;// ����� ���������� ��������� � �������� � ����� ��� ���� Row ��� Col

	Vertex_Model3D* VB_Data;// ������ ���� ��������� � ����������� ������������ � ���������
	Index_Type* IB_Data;	// ��������� �����

	RCube_VecFloat234 First_Vertex_Data;	// ��������� � ������ 1-�� ��������, �� ������� �� ������

	bool CastShadow;	// �������� �� ���� �� ����� ������� ?
	bool ClusterRender; // �������� ���������� ? ( �������� �������� Frustum )
};

#endif