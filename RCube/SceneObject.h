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

	RCube_VecFloat34 MinValues; // минимальные значения XYZ ограничивающие коробку
	RCube_VecFloat34 MaxValues; // максимальные значения XYZ ограничивающие коробку

	float SphereRadius; // радиус сферы ограничивающей обьект ка бы он не бых повернут

	RCube_VecFloat34 CentralPoint; // позиция центральной точки для легкого расчета фраттрума
};


// структура описывабщая мэш 3D модели
struct MeshData {

	~MeshData ()// деконструктор класса в нем я описал релиз всех буферов ранее созданных для данной структуры т.е ее не надо удалять руками 
	{
		RCUBE_RELEASE ( TextureResurce );
		RCUBE_RELEASE ( Texture );
		RCUBE_RELEASE ( VertBuff );
		RCUBE_ARR_DELETE ( VertArr );
		RCUBE_ARR_DELETE ( Indexes );
	}

	UINT VertexBufferSize; // размер вертексного масива
	UINT IndexBufferSize; // размер индексного масива
	Vertex_Model3D * VertArr = nullptr;  // вертексный масив
	UINT * Indexes = nullptr; // индексный массив 
							   
	ID3D11Resource * TextureResurce = nullptr;		// тексттурные данные для этого мэша***
	ID3D11ShaderResourceView * Texture = nullptr;
	// тексттурные данные для этого мэша***

	ID3D11Buffer* VertBuff = nullptr; // вертексный буфер этого мэша

	Material::SurfaceMaterial material; // данные о материале для данного мэша

	BoundingBox MeshBox; // параметры коробки огранич мэш в обькте


};


struct RCudeObjDesc // структура описывающая объкт как набор мэшей и описание к ним
{
	RCudeObjDesc ()
	{
		Meshes = nullptr;
		Positions = nullptr;
		PositionsDraw = nullptr;
	}

	~RCudeObjDesc () {

		RCUBE_ARR_DELETE ( Meshes );//  удаляю мэши все атрубуты удаляются автоматически в деконструкторе класса
		RCUBE_ARR_DELETE ( Positions );
		RCUBE_ARR_DELETE ( PositionsDraw );

	}

	UINT                 MeshesCount;	// количество мэшей
										//		int					TexturesCount;	// количество текстур
	MeshData*           Meshes;			// тут находятся все мэши для модела

	PositionType*       Positions;		// пизиции всех инстэнсов
	PositionType*       PositionsDraw;	// позиции который уходит в отрисовку

	UINT                FrustumType; // тип фраструма для обьекта не для отдельного мэша (важно понимать разницу) 0 - шар, 1 - куб, 2 - точка, 3 - ещё не решили что

	int                 Shaders; // номер в спаске связок шейдеров

	int                 InstanceCount; // количество инстэнсов присуствующих в отрисовке
	int                 InstanceFrustumPassedAmount; // количество инстэнсов отрезанных фраструмом

	bool                IsObjDraw = true; // True - если обькт участвует в отрисовке False если нет

	BoundingBox         ObjectBox; // параметры коробки огранич обьект

	int                 ObjectVertexesCout = 0; // общее количество вершин в данном обьекте (т.е во всех его мэшах)

	bool				Visible;
	bool				CastShadow;
	bool				ReceiveLight;
	
	int					LayOutType;	// Тип Layout для Render
	void*				ObjectVertexType;
	void*				InstancesVertexType;

};



class SceneObject
{
public:
	~SceneObject ()// деконструктор класса в нем я описал релиз всех буферов ранее созданных для данной структуры т.е ее не надо удалять руками 
	{
		RCUBE_RELEASE ( TextureResurce );
		RCUBE_RELEASE ( Texture );
		RCUBE_RELEASE ( VertBuff );
		RCUBE_ARR_DELETE ( VertArr );
		RCUBE_ARR_DELETE ( Indexes );
	}

	UINT VertexBufferSize; // размер вертексного масива
	UINT IndexBufferSize; // размер индексного масива
	Vertex_Model3D * VertArr = nullptr;  // вертексный масив
	UINT * Indexes = nullptr; // индексный массив 

	ID3D11Resource * TextureResurce = nullptr;		// тексттурные данные для этого мэша***
	ID3D11ShaderResourceView * Texture = nullptr;
	// тексттурные данные для этого мэша***

	ID3D11Buffer* VertBuff = nullptr; // вертексный буфер этого мэша

	Material::SurfaceMaterial material; // данные о материале для данного мэша

	BoundingBox MeshBox; // параметры коробки огранич мэш в обькте

/*
XMFLOAT3 Position;
//	XMMATRIX Rotation; //	XMVECTOR Rotation;
bool	 Visible;
bool	 CastShadow;
bool	 ReceiveLight;
*/
};
