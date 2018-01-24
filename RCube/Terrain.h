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

struct TerrainParam
{

	//*********************** ��� ��������� ��������� ������*****************************
	int HeightesCout, LowlandsCout;
	int MaxHeight, MinHeight;
	int MaxRadius, MinRadius;
	//*********************** ��� ��������� ��������� ������*****************************

	//*********************** ��� ��������� �� �������� �������� ************************
	char* filename;
	//*********************** ��� ��������� �� �������� �������� ************************

};


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

  float QuadVertexStep; // ���������� ����� ���������� � ���������
	int QuadsPerCluster;// ���������� ���������� � ����� ������� 7 * 7 , 16 * 16 , 31 * 31 , 63 * 63 , 127 * 127 , 255 * 255
	int ClustersAmount;	// ���������� �������� � Terrain  X * Z
	int ClustersX_ROW;	// ���������� �������� �� X - ROW
	int	ClustersZ_COL;	// ���������� �������� �� Z - COLUMN
	int VertexInX_Row;	// ���������� ��������� � ����
	int VertexInZ_Col;	// ���������� ��������� � �������
	int TotalVertex;	// ����� ���������� ��������� � �������
	int TotalIndex;		// ����� ���������� �������� � �������
	
	Vertex_Model3D* VB_Data;// ������ ���� ��������� � ����������� ������������ � ���������
	Index_Type* IB_Data;	// ��������� �����

	HeightMapType * m_heightMap; // ��������� ������ ����� [x ���������][y ���������]
	RCube_VecFloat34 First_Vertex_Data;	// ��������� � ������ 1-�� ��������, �� ������� �� ������

};

#endif