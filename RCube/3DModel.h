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

	VertexBuffer <PositionType>* InstancePositionsBuffer;	// Буфер позиций всех Instance объекта уходящий в шейдер

	PositionType*       Positions;		// пизиции всех инстэнсов
	PositionType*       PositionsDraw;	// позиции который уходит в отрисовку

	UINT				FrustumType;	// тип фраструма для обьекта не для отдельного мэша (важно понимать разницу) 0 - шар, 1 - куб, 2 - точка, 3 - ещё не решили что

	int                 InstanceCount; // количество инстэнсов присуствующих в отрисовке
	int                 InstanceFrustumPassedAmount; // количество инстэнсов отрезанных фраструмом

//	bool                IsObjDraw = true; // True - если обькт участвует в отрисовке False если нет

	BoundingBox         ObjectBox; // параметры коробки огранич обьект

	int					_3D_Obj_Index;		// Собственный индекс в массиве 3D моделей
// Это использвется для физики. Возможно, можно будет удалить
	int                 ObjectVertexesCout = 0; // общее количество вершин в данном обьекте (т.е во всех его мэшах)

	bool				Visible;
	bool				CastShadow;
	bool				ReceiveLight;

	int					LayOutType;	// Тип Layout для Render

};