#pragma once
#include "stdafx.h"
#include "Buffers_def.h"
#include "d3d11.h"
#include "Material.h"
#include "BoundingBox.h"
#include <vector>




// ��������� ����������� ��� 3D ������
struct MeshData {

//public:
	 MeshData ();
	~MeshData ();// ������������� ������ � ��� � ������ ����� ���� ������� ����� ��������� ��� ������ ��������� �.� �� �� ���� ������� ������ 

	UINT VertexBufferSize;		// ������ ����������� ������
	UINT IndexBufferSize;		// ������ ���������� ������
	
// ����� ������� ��� ����� ������ 
	Vertex_Model3D * VertexArray;// ������ ��������� � ������ ��� ���������
	ID3D11Buffer* VertBuff; // ���������� ����� ����� ����

	ID3D11Resource * TextureResurce;// ����������� ������ ��� ����� ����***
	ID3D11ShaderResourceView * TextureSRV;

	int TextureIndex;			// ����� �������� � ������� ������� ������
	int BufferIndex;			// ����� VertexBuffer � ������� ������� ������
	// ����������� ������ ��� ����� ����***

	Material* Material; // ������ � ��������� ��� ������� ����

	BoundingBox MeshBox; // ��������� ������� ������� ��� � ������


};