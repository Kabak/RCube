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
#include "VertexBuffers_def.h"	// Описание типов вертексных буферов
#include "SceneObject.h"

using std::ios;

using namespace DirectX ;

class KFModel{
public :
	 KFModel ();
	~KFModel ();

	// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
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

int Instance = 0; // необходима для перевося данных о количество прошедшик кулинги инстэнсов из обдейта в рендел

HWND g_hwnd; // глобальный швнд используемы для вывода ошибок

D3DClass * g_ActiveLightClass; // лайт класс он необходим для орисовки со свотом

ID3D11DeviceContext * d3d11DevCon ; // девайс контекст движка

ID3D11Device *d3d11Device; // девайс движка

ID3D11InputLayout* vertLayout; // лэйаут обьекта для передачи данных

ID3D11Buffer* m_posesBuffer; // позиции инстэнсов

bool LoadKFObject(std::wstring FileName) ; // достает из  кфо файла обьет

};

#endif