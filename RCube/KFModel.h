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
#include "D3dClass.h"
#include "DirectXMath.h"
#include "Material.h"
#include "VertexBuffers_def.h"	// �������� ����� ���������� �������
#include "SceneObject.h"

using std::ios;

using namespace DirectX ;

class KFModel{
public :
	 KFModel ();
	~KFModel ();

	// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
	void* operator new(size_t i)
	{
		return _mm_malloc ( i, 16 );
	}

	void operator delete(void* p)
	{
		_mm_free ( p );
	}
	// ------------------------------------

	RCudeObjDesc ThisObjDesc;

	HRESULT Init ( HWND hwnd, std::wstring filename, ID3D11Device *d3d11Device,
	ID3D11DeviceContext * DevCon, UINT Shaders, int m_instanceCount, D3DClass * ActiveLightClass );

	void LightRender ();
	void Render ();
	HRESULT Frame ( int frames );


	bool UpdateInstancingPos ();
	void UpdateInstancingDrawData ();

protected:

struct CameraBufferType
{
	XMFLOAT3 cameraPosition;
	float padding;
};

int Instance = 0; // ���������� ��� �������� ������ � ���������� ��������� ������� ��������� �� ������� � ������

HWND g_hwnd; // ���������� ���� ����������� ��� ������ ������

D3DClass * g_ActiveLightClass; // ���� ����� �� ��������� ��� �������� �� ������

ID3D11DeviceContext * d3d11DevCon ; // ������ �������� ������

ID3D11Device *d3d11Device; // ������ ������

ID3D11InputLayout* vertLayout; // ������ ������� ��� �������� ������

ID3D11Buffer* m_posesBuffer; // ������� ���������

bool LoadKFObject(std::wstring FileName) ; // ������� ��  ��� ����� �����

};

#endif