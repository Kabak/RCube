#pragma once
#include "stdafx.h"
#include "DirectXMath.h"
#include "Mesh_Data.h"

using namespace DirectX;

struct RCudeObjDesc // ��������� ����������� ����� ��� ����� ����� � �������� � ���
{
	RCudeObjDesc ()
	{
		Meshes = nullptr;
		Positions = nullptr;
		PositionsDraw = nullptr;
	}

	~RCudeObjDesc () {

		RCUBE_ARR_DELETE ( Meshes );//  ������ ���� ��� �������� ��������� ������������� � �������������� ������
		RCUBE_ARR_DELETE ( Positions );
		RCUBE_ARR_DELETE ( PositionsDraw );

	}

	UINT                 MeshesCount;	// ���������� �����
										//		int					TexturesCount;	// ���������� �������
	MeshData*           Meshes;			// ��� ��������� ��� ���� ��� ������

	PositionType*       Positions;		// ������� ���� ���������
	PositionType*       PositionsDraw;	// ������� ������� ������ � ���������

	UINT                FrustumType; // ��� ��������� ��� ������� �� ��� ���������� ���� (����� �������� �������) 0 - ���, 1 - ���, 2 - �����, 3 - ��� �� ������ ���

	int                 Shaders; // ����� � ������ ������ ��������

	int                 InstanceCount; // ���������� ��������� ������������� � ���������
	int                 InstanceFrustumPassedAmount; // ���������� ��������� ���������� ����������

	bool                IsObjDraw = true; // True - ���� ����� ��������� � ��������� False ���� ���

	BoundingBox         ObjectBox; // ��������� ������� ������� ������

	int                 ObjectVertexesCout = 0; // ����� ���������� ������ � ������ ������� (�.� �� ���� ��� �����)

	bool				Visible;
	bool				CastShadow;
	bool				ReceiveLight;
	
	int					LayOutType;	// ��� Layout ��� Render
	void*				ObjectVertexType;
	void*				InstancesVertexType;

};
