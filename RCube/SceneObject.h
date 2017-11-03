#pragma once
#include "stdafx.h"
#include "DirectXMath.h"
#include "d3d11.h"
#include "Buffers_def.h"
#include "Material.h"

using namespace DirectX;


struct BoundingBox
{
	void CreateBoundStruct ( RCube_VecFloat34 minValues, RCube_VecFloat34 maxValues );

	RCube_VecFloat34 MinValues; // ����������� �������� XYZ �������������� �������
	RCube_VecFloat34 MaxValues; // ������������ �������� XYZ �������������� �������

	float SphereRadius; // ������ ����� �������������� ������ �� �� �� �� ��� ��������

	RCube_VecFloat34 CentralPoint; // ������� ����������� ����� ��� ������� ������� ���������
};


// ��������� ����������� ��� 3D ������
struct MeshData {

	~MeshData ()// ������������� ������ � ��� � ������ ����� ���� ������� ����� ��������� ��� ������ ��������� �.� �� �� ���� ������� ������ 
	{
		RCUBE_RELEASE ( TextureResurce );
		RCUBE_RELEASE ( Texture );
		RCUBE_RELEASE ( VertBuff );
		RCUBE_ARR_DELETE ( VertArr );
		RCUBE_ARR_DELETE ( Indexes );
	}

	UINT VertexBufferSize; // ������ ����������� ������
	UINT IndexBufferSize; // ������ ���������� ������
	Vertex_Model3D * VertArr = nullptr;  // ���������� �����
	UINT * Indexes = nullptr; // ��������� ������ 
							   
	ID3D11Resource * TextureResurce = nullptr;		// ����������� ������ ��� ����� ����***
	ID3D11ShaderResourceView * Texture = nullptr;
	// ����������� ������ ��� ����� ����***

	ID3D11Buffer* VertBuff = nullptr; // ���������� ����� ����� ����

	Material::SurfaceMaterial material; // ������ � ��������� ��� ������� ����

	BoundingBox MeshBox; // ��������� ������� ������� ��� � ������


};


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



class SceneObject
{
public:
	~SceneObject ()// ������������� ������ � ��� � ������ ����� ���� ������� ����� ��������� ��� ������ ��������� �.� �� �� ���� ������� ������ 
	{
		RCUBE_RELEASE ( TextureResurce );
		RCUBE_RELEASE ( Texture );
		RCUBE_RELEASE ( VertBuff );
		RCUBE_ARR_DELETE ( VertArr );
		RCUBE_ARR_DELETE ( Indexes );
	}

	UINT VertexBufferSize; // ������ ����������� ������
	UINT IndexBufferSize; // ������ ���������� ������
	Vertex_Model3D * VertArr = nullptr;  // ���������� �����
	UINT * Indexes = nullptr; // ��������� ������ 

	ID3D11Resource * TextureResurce = nullptr;		// ����������� ������ ��� ����� ����***
	ID3D11ShaderResourceView * Texture = nullptr;
	// ����������� ������ ��� ����� ����***

	ID3D11Buffer* VertBuff = nullptr; // ���������� ����� ����� ����

	Material::SurfaceMaterial material; // ������ � ��������� ��� ������� ����

	BoundingBox MeshBox; // ��������� ������� ������� ��� � ������

/*
XMFLOAT3 Position;
//	XMMATRIX Rotation; //	XMVECTOR Rotation;
bool	 Visible;
bool	 CastShadow;
bool	 ReceiveLight;
*/
};
