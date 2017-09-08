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

	RCube_VecFloat34 MinValues; // ����������� �������� XYZ �������������� �������
	RCube_VecFloat34 MaxValues; // ������������ �������� XYZ �������������� �������

	float SphereRadius; // ������ ����� �������������� ������ �� �� �� �� ��� ��������

	RCube_VecFloat34 CentralPoint; // ������� ����������� ����� ��� ������� ������� ���������
};

// ��������� ����������� ���
struct MeshData {

	~MeshData(); // ������������� ������ � ��� � ������ ����� ���� ������� ����� ��������� ��� ������ ��������� �.� �� �� ���� ������� ������ 

	int VertexBufferSize; // ������ ����������� ������
	int IndexBufferSize; // ������ ���������� ������
	Vertex * VertArr = nullptr;  // ���������� �����
	DWORD * Indexes = nullptr; // ��������� ������ 

	// ����������� ������ ��� ����� ����***
	ID3D11Resource * TextureResurse = nullptr; 
	ID3D11ShaderResourceView * Texture = nullptr;
	// ����������� ������ ��� ����� ����***

	ID3D11Buffer* VertBuff = nullptr; // ���������� ����� ����� ����

	Material::SurfaceMaterial material; // ������ � ��������� ��� ������� ����

	BoundingBox MeshBox; // ��������� ������� ������� ��� � ������


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
int Instance = 0; // ���������� ��� �������� ������ � ���������� ��������� ������� ��������� �� ������� � ������

HWND g_hwnd; // ���������� ���� ����������� ��� ������ ������

LightClass * g_ActiveLightClass; // ���� ����� �� ��������� ��� �������� �� ������

ID3D11DeviceContext * d3d11DevCon ; // ������ �������� ������

ID3D11Device *d3d11Device; // ������ ������

ID3D11InputLayout* vertLayout; // ������ ������� ��� �������� ������

ID3D11Buffer* m_posesBuffer; // ������� ���������

bool LoadKFObject(std::wstring FileName) ; // ������� ��  ��� ����� �����



public :
	KFModel() ;
	~KFModel() ;

	struct RCudeObjDesc // ��������� ����������� ����� ��� ����� ����� � �������� � ���
	{
		~RCudeObjDesc();

		int                 MeshesCount;	// ���������� �����
//		int					TexturesCount;	// ���������� �������
		MeshData*           Meshes;			// ��� ��������� ��� ���� ��� ������

		PositionType*       Positions; // ����� ������� ���� ���������
		PositionType*       PositionsDraw; // ����� ������� ������� ������ � ���������

		UINT                FrustumType; // ��� ��������� ��� ������� �� ��� ���������� ���� (����� �������� �������) 0 - ���, 1 - ���, 2 - �����, 3 - ��� �� ������ ���

        int                 Shaders; // ����� � ������ ������ ��������

		int                 instanceCount; // ���������� ��������� ������������� � ���������
		int                 InstanceFrustumPassedAmount; // ���������� ��������� ���������� ����������

		bool                IsObjDraw = true; // True - ���� ����� ��������� � ��������� False ���� ���

		BoundingBox         ObjectBox; // ��������� ������� ������� ������

		int                 ObjectVertexesCout = 0; // ����� ���������� ������ � ������ ������� (�.� �� ���� ��� �����)

	} ThisObjDesc;

	// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
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