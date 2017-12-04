#include "stdafx.h"

#ifndef _KFMODEL_H_
#define _KFMODEL_H_

#include <fstream>
#include "DirectXMath.h"
#include <sstream>
#include <d3d11.h>
#include "D3DGlobalContext.h"
#include <WICTextureLoader.h>
#include "D3dClass.h"
#include "DirectXMath.h"
#include "Material.h"
#include "Buffers_def.h"	// �������� ����� ���������� �������
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

	HRESULT Init ( std::wstring filename, D3DGlobalContext * D3DGC,
		UINT Shaders, int m_instanceCount, D3DClass * ActiveLightClass );

	void LightRender ();
	void Render ();

	bool UpdateInstancingPos ();
	void UpdateInstancingDrawData ();

protected:

//int Instance = 0; // ���������� ��� �������� ������ � ���������� ��������� ������� ��������� �� ������� � ������

D3DClass * g_ActiveLightClass; // ���� ����� �� ��������� ��� �������� �� ������

ID3D11DeviceContext * d3d11DevCon ; // ������ �������� ������

ID3D11Device *d3d11Device; // ������ ������

ID3D11Buffer* m_posesBuffer; // ������� ���������

bool LoadKFObject(std::wstring FileName) ; // ������� ��  ��� ����� �����

};

#endif