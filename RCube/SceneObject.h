#pragma once
#include "DirectXMath.h"
#include "d3d11.h"
#include "VertexBuffers_def.h"
#include "Material.h"

using namespace DirectX;


struct BoundingBox
{
	void CreateBoundStruct ( RCube_VecFloat34 minValues, RCube_VecFloat34 maxValues );

	RCube_VecFloat34 MinValues; // минимальные значени€ XYZ ограничивающие коробку
	RCube_VecFloat34 MaxValues; // максимальные значени€ XYZ ограничивающие коробку

	float SphereRadius; // радиус сферы ограничивающей обьект ка бы он не бых повернут

	RCube_VecFloat34 CentralPoint; // позици€ центральной точки дл€ легкого расчета фраттрума
};


// структура описывабща€ мэш 3D модели
struct MeshData {

	~MeshData ()// деконструктор класса в нем € описал релиз всех буферов ранее созданных дл€ данной структуры т.е ее не надо удал€ть руками 
	{
		RCUBE_RELEASE ( TextureResurce );
		RCUBE_RELEASE ( Texture );
		RCUBE_RELEASE ( VertBuff );
		RCUBE_ARR_DELETE ( VertArr );
		RCUBE_ARR_DELETE ( Indexes );
	}

	int VertexBufferSize; // размер вертексного масива
	int IndexBufferSize; // размер индексного масива
	Vertex_Model3D * VertArr = nullptr;  // вертексный масив
	DWORD * Indexes = nullptr; // индексный массив 

							   // тексттурные данные дл€ этого мэша***
	ID3D11Resource * TextureResurce = nullptr;
	ID3D11ShaderResourceView * Texture = nullptr;
	// тексттурные данные дл€ этого мэша***

	ID3D11Buffer* VertBuff = nullptr; // вертексный буфер этого мэша

	Material::SurfaceMaterial material; // данные о материале дл€ данного мэша

	BoundingBox MeshBox; // параметры коробки огранич мэш в обькте


};


struct RCudeObjDesc // структура описывающа€ объкт как набор мэшей и описание к ним
{
	RCudeObjDesc ()
	{
		Meshes = nullptr;
		Positions = nullptr;
		PositionsDraw = nullptr;
	}

	~RCudeObjDesc () {

		RCUBE_ARR_DELETE ( Meshes );//  удал€ю мэши все атрубуты удал€ютс€ автоматически в деконструкторе класса
		RCUBE_ARR_DELETE ( Positions );
		RCUBE_ARR_DELETE ( PositionsDraw );

	}

	int                 MeshesCount;	// количество мэшей
										//		int					TexturesCount;	// количество текстур
	MeshData*           Meshes;			// тут наход€тс€ все мэши дл€ модела

	PositionType*       Positions;		// пизиции всех инстэнсов
	PositionType*       PositionsDraw;	// позиции который уходит в отрисовку

	UINT                FrustumType; // тип фраструма дл€ обьекта не дл€ отдельного мэша (важно понимать разницу) 0 - шар, 1 - куб, 2 - точка, 3 - ещЄ не решили что

	int                 Shaders; // номер в спаске св€зок шейдеров

	int                 InstanceCount; // количество инстэнсов присуствующих в отрисовке
	int                 InstanceFrustumPassedAmount; // количество инстэнсов отрезанных фраструмом

	bool                IsObjDraw = true; // True - если обькт участвует в отрисовке False если нет

	BoundingBox         ObjectBox; // параметры коробки огранич обьект

	int                 ObjectVertexesCout = 0; // общее количество вершин в данном обьекте (т.е во всех его мэшах)

	bool				Visible;
	bool				CastShadow;
	bool				ReceiveLight;
	
	int					LayOutType;	// “ип Layout дл€ Render
	void*				ObjectVertexType;
	void*				InstancesVertexType;

};


/*
class SceneObject
{
public:
XMFLOAT3 Position;
//	XMMATRIX Rotation; //	XMVECTOR Rotation;
bool	 Visible;
bool	 CastShadow;
bool	 ReceiveLight;
};
*/