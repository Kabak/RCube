#pragma once
#include "stdafx.h"
#include "Buffers_def.h"
#include "d3d11.h"
#include "Material.h"
#include "BoundingBox.h"
#include <vector>




// структура описывабщая мэш 3D модели
struct MeshData {

//public:
	 MeshData ();
	~MeshData ();// деконструктор класса в нем я описал релиз всех буферов ранее созданных для данной структуры т.е ее не надо удалять руками 

	UINT VertexBufferSize;		// размер вертексного масива
	UINT IndexBufferSize;		// размер индексного масива
	
// МОЖНО УДАЛИТЬ ДЛЯ НОВОЙ МОДЕЛИ 
	Vertex_Model3D * VertexArray;// массив уходящиий в шейдер для отрисовки
	ID3D11Buffer* VertBuff; // вертексный буфер этого мэша

	ID3D11Resource * TextureResurce;// тексттурные данные для этого мэша***
	ID3D11ShaderResourceView * TextureSRV;

	int TextureIndex;			// Номер текстуры в массиве текстур движка
	int BufferIndex;			// Номер VertexBuffer в массиве буферов движка
	// тексттурные данные для этого мэша***

	Material* Material; // данные о материале для данного мэша

	BoundingBox MeshBox; // параметры коробки огранич мэш в обькте


};