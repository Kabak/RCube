#pragma once
#include "stdafx.h"
#include <vector>
#include "Mesh_Data.h"

static const UINT MAX_MESH_IN_MODEL = 50;

class _3DModel
{
public:

	_3DModel ();
   ~_3DModel ();

	vector <MeshData*>	Meshes;

	VertexBuffer <PositionType>* InstancePositionsBuffer;	// ����� ������� ���� Instance ������� �������� � ������

	PositionType*       Positions;		// ������� ���� ���������
	PositionType*       PositionsDraw;	// ������� ������� ������ � ���������

	UINT				FrustumType;	// ��� ��������� ��� ������� �� ��� ���������� ���� (����� �������� �������) 0 - ���, 1 - ���, 2 - �����, 3 - ��� �� ������ ���

	int                 InstanceCount; // ���������� ��������� ������������� � ���������
	int                 InstanceFrustumPassedAmount; // ���������� ��������� ���������� ����������

//	bool                IsObjDraw = true; // True - ���� ����� ��������� � ��������� False ���� ���

	BoundingBox         ObjectBox; // ��������� ������� ������� ������

	int					_3D_Obj_Index;		// ����������� ������ � ������� 3D �������
// ��� ������������ ��� ������. ��������, ����� ����� �������
	int                 ObjectVertexesCout = 0; // ����� ���������� ������ � ������ ������� (�.� �� ���� ��� �����)

	bool				Visible;
	bool				CastShadow;
	bool				ReceiveLight;

	int					LayOutType;	// ��� Layout ��� Render

};