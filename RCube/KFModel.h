#include "stdafx.h"

#ifndef _KFMODEL_H_
#define _KFMODEL_H_

#include <vector>
#include <fstream>
#include "DirectXMath.h"
#include <sstream>
#include <d3d11.h>
#include "D3Dcompiler.h"
#include <WICTextureLoader.h>
#include "LightClass.h"
#include "DirectXMath.h"
#include "Material.h"
//#include "D3DX11tex.h"

using std::ios;

using namespace DirectX ;

class KFModel{
public :




protected:
	//Create material structure


struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		float tx, float ty, float tz)
		: pos(x,y,z , 1.0f), texCoord(u, v), normal(nx, ny, nz),
		tangent(tx, ty, tz){}

	XMFLOAT4 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	//XMFLOAT3 biTangent;
};

struct BoundingBox
{
	void CreateBoundStruct(RCube_VecFloat34 minValues , RCube_VecFloat34 maxValues);

	RCube_VecFloat34 MinValues; // минимальные значения XYZ ограничивающие коробку
	RCube_VecFloat34 MaxValues; // максимальные значения XYZ ограничивающие коробку

	float SphereRadius; // радиус сферы ограничивающей обьект ка бы он не бых повернут

	RCube_VecFloat34 CentralPoint; // позиция центральной точки для легкого расчета фраттрума
};

// структура описывабщая мэш
struct MeshData {

	~MeshData(); // деконструктор класса в нем я описал релиз всех буферов ранее созданных для данной структуры т.е ее не надо удалять руками 

	int VertexBufferSize; // размер вертексного масива
	int IndexBufferSize; // размер индексного масива
	Vertex * VertArr = nullptr;  // вертексный масив
	DWORD * Indexes = nullptr; // индексный массив 

	// тексттурные данные для этого мэша***
	ID3D11Resource * TextureResurse = nullptr; 
	ID3D11ShaderResourceView * Texture = nullptr;
	// тексттурные данные для этого мэша***

	ID3D11Buffer* VertBuff = nullptr; // вертексный буфер этого мэша

	Material::SurfaceMaterial material; // данные о материале для данного мэша

	BoundingBox MeshBox; // параметры коробки огранич мэш в обькте


};

struct CameraBufferType
{
	XMFLOAT3 cameraPosition;
	float padding;
};

public:


	struct cbPerObject
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
		XMMATRIX ViewProjection;
		XMMATRIX Ortho;
		XMFLOAT3 cameraPosition;
		float padding;
		XMFLOAT4 TransposedCameraRotation2;

	};



// +++++++++++++++++   Instancing   ++++++++++++++++++++++++++++++++
struct PositionType
{
	RCube_VecFloat34 position;
	RCube_VecFloat34 rotation;
};
// -----------------   Instancing   --------------------------------


protected:
int Instance = 0; // необходима для перевося данных о количество прошедшик кулинги инстэнсов из обдейта в рендел

HWND g_hwnd; // глобальный швнд используемы для вывода ошибок

LightClass * g_ActiveLightClass; // лайт класс он необходим для орисовки со свотом

ID3D11DeviceContext * d3d11DevCon ; // девайс контекст движка

ID3D11Device *d3d11Device; // девайс движка

ID3D11InputLayout* vertLayout; // лэйаут обьекта для передачи данных

ID3D11Buffer* m_posesBuffer; // позиции инстэнсов

bool LoadKFObject(std::wstring FileName) ; // достает из  кфо файла обьет



public :
	KFModel() ;
	~KFModel() ;

	struct RCudeObjDesc // структура описывающая объкт как набор мэшей и описание к ним
	{
		~RCudeObjDesc();

		int                 MeshesCount;	// количество мэшей
//		int					TexturesCount;	// количество текстур
		MeshData*           Meshes;			// тут находятся все мэши для модела

		PositionType*       Positions; // набор пизиции всех инстэнсов
		PositionType*       PositionsDraw; // наюор позиций который уходит в отрисовку

		UINT                FrustumType; // тип фраструма для обьекта не для отдельного мэша (важно понимать разницу) 0 - шар, 1 - куб, 2 - точка, 3 - ещё не решили что

        int                 Shaders; // номер в спаске связок шейдеров

		int                 instanceCount; // количество инстэнсов присуствующих в отрисовке
		int                 InstanceFrustumPassedAmount; // количество инстэнсов отрезанных фраструмом

		bool                IsObjDraw = true; // True - если обькт участвует в отрисовке False если нет

		BoundingBox         ObjectBox; // параметры коробки огранич обьект

		int                 ObjectVertexesCout = 0; // общее количество вершин в данном обьекте (т.е во всех его мэшах)

	} ThisObjDesc;

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }
// ------------------------------------

	HRESULT Init(HWND hwnd, std::wstring filename, ID3D11Device *d3d11Device
		, ID3D11DeviceContext * DevCon, ID3D10Blob * Blob, UINT Shaders, int m_instanceCount, LightClass * ActiveLightClass);

	void LightRender() ;
	void Render();
	HRESULT Frame(int frames) ;


	bool UpdateInstancingPos();
	void UpdateInstancingDrawData();

};

#endif