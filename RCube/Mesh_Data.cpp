#include "stdafx.h"
#include "Mesh_Data.h"


MeshData::MeshData ()
{
	TextureResurce = nullptr;
		TextureSRV = nullptr;
		  VertBuff = nullptr;
	   VertexArray = nullptr;
		  Material = nullptr;
}


MeshData::~MeshData ()// ������������� ������ � ��� � ������ ����� ���� ������� ����� ��������� ��� ������ ��������� �.� �� �� ���� ������� ������ 
{
	RCUBE_DELETE	( Material )
	RCUBE_RELEASE	( TextureResurce );		// !!!!!!!!!!!!!
	RCUBE_RELEASE	( TextureSRV );			// !!!!!!!!!!!!!
	RCUBE_RELEASE	( VertBuff );			// !!!!!!!!!!!!!
	RCUBE_ARR_DELETE( VertexArray );
}