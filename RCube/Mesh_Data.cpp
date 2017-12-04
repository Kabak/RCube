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


MeshData::~MeshData ()// деконструктор класса в нем я описал релиз всех буферов ранее созданных для данной структуры т.е ее не надо удалять руками 
{
	RCUBE_DELETE	( Material )
	RCUBE_RELEASE	( TextureResurce );		// !!!!!!!!!!!!!
	RCUBE_RELEASE	( TextureSRV );			// !!!!!!!!!!!!!
	RCUBE_RELEASE	( VertBuff );			// !!!!!!!!!!!!!
	RCUBE_ARR_DELETE( VertexArray );
}