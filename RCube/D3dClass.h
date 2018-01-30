#pragma once
#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Filename: D3dClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

#include "D3DGlobalContext.h"
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include "DirectXMath.h"
#include <vector>

#include "Buffers_def.h"
#ifdef RCube_DX11
#include "DX11Buffers.h"
#endif

#include "Light_def.h"
#include "ClusterShadingFragmentFactory.h"
#include "LightGrid.h"
#include "FrustumClass.h"
 
//#include "KFModelList.h"
//#include "Terrain.h"

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
   ~D3DClass();

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
/*
// + Shadow Works
	struct ObjectUsingShadows {

		KFModelList * ModelList;
		Terrain * Terrain;

	} DrawShadowsObjects;
// - Shadow Works
*/
	bool Initialize(HWND, int, int, bool, bool, float, float, FrustumClass* frustum );
	void Shutdown();
	
	void BeginScene(XMFLOAT4&);
	void EndScene();
	
	  D3DGlobalContext* D3DGC;

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void SetDefaultResterizeState();
	void SetCullNoneResterizeState();
	void RenderWireFrameResterizeState ();

	void BlurScene(ID3D11ComputeShader* Horizont, ID3D11ComputeShader* Vertical, int BlurAmont);

	void TurnZBufferOn();
	void TurnZBufferOff();

//We have two new functions for turning on and off alpha blending.

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();
	void TurnOnParticleBlending();
	void TurnOnTextOnTextureBlending();

	char* GetVideoCardString();

	bool SaveTextureToPNG( ID3D11ShaderResourceView* );

	int FXAAShaderIndex;	// Индекс шейдера в массиве которым выполняется FXAA сглаживание
	int BlureHorizComputeShaderIndex;
	int BlureVertComputeShaderIndex;
	int ShadowMapShaderIndex;	// Индекс шейдера для отрисовки ShadowMap 
	int LightShaderForDraw;		// Индекс шейдера для отрисовки света

// Video Card data
	int m_videoCardMemory;
	char* m_videoCardDescription;
// FXAA фильтрация сцены
	void FXAAScene();

	int CreateCustomSampler ();						// Создание произвольного сэмплера пользователем

	int CreateCustomRasterizerState ( D3D11_RASTERIZER_DESC& Desc );				// Создание произвольного RSState
   bool ChangeCustomRasterizerState ( int Number, D3D11_RASTERIZER_DESC& Desc );	// Change
   ID3D11RasterizerState* GetCustomRasterizerState ( int Number );

	int CreateCustomDepthStencilState ();			// Создание произвольного DepthStencilState

	bool Rander3DObject ();

	D3D11_VIEWPORT viewport;	// Engine render ViewPort

// + LIGHTS

	// Желаемое количество светов
	int LightsWanted = 128;

	// Сколько светов на сцене
	int mActiveLights = 0;
	int mVisibleLights = 0;
	int mActiveLightsWithShadows = 0;

	//  ++++++++++++++++++   Для Shadows   +++++++++++++++++++++++++
	int SMWidth; // Ширина Shadow Map
	int SMHeight;// Высота Shadow Map
	D3D11_VIEWPORT LightViewPort;	// ShadowMap ViewPort
	//  ------------------   Для Shadows   -------------------------

	int PointLightSize;	// Размер структуры PointLight
						// ДЛЯ ТЕСТОВ

	// Это и есть массив светов
	vector <PointLight*> mPointLightParameters;

	// Массив индексов SpotLights которые имеют тень для отрисовки ShadowMap
	vector <int> SpotLightsWithShadowsIndexes;

	// Массив освободившихся индексов светов. Освободившейся индекс добавляется, 
	// когда свет больше не нужен и его можно удалить из отрисовки.
	vector <int> FreeLightIndex;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++    Ускорители частиц    +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	RCube_VecFloat34 LightPos, LightPosCopy, Temp, Temp2;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++    Ускорители частиц    +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	FrustumClass* Frustum;

	FragmentFactory fragmentFactory;

	// Буфер светов для шейдера
	ID3D11ShaderResourceView *lightBufferSRV;
	// Буфер кластеров для шейдера
	ID3D11ShaderResourceView *LightGridBufferSRV;

	ID3D11Texture2D* ShadowMap3D; // текстура где хранятся тени
	ID3D11ShaderResourceView* SRV_ShadowMap3D;

	// Массив света для сцены
	PerFrameConstants* Light;

	// Clustered Shading state
	LightGridBuilder mLightGridBuilder;

	int mLightGridBufferSize;

	// Массив светов передающихся в шейдер для отображения
	StructuredBuffer<PointLight>* mLightBuffer;
	// Массив сетки светов передаваемый в шейдер
	StructuredBuffer<LightGridEntry>* mLightGridBuffer;
	// Константы для отладки шейдеров
	ConstantBuffer<PerFrameConstants>* PerFrameConstants_Debug;

	inline	void UpdateClipRegion (
		float lc,          // Light x/y coordinate (view space)
		float& lz,          // Light z coordinate (view space)
		float& lightRadius,
		const float& cameraScale, // Project scale for coordinate (_11 or _22 for x/y respectively)
		float& clipMin,
		float& clipMax );

	inline	XMVECTOR ComputeClipRegion (
		XMFLOAT3& lightPosView,
		float& lightRadius,
		const float& float11,
		const float& float22,
		const float& mCameraNearFar );

	inline	void GenerateLightFragments (
		const FragmentFactory& fragmentFactory,
		LightGridBuilder* builder,
		XMMATRIX& ProjMatrix,
		XMMATRIX& CameraMatrix,
		PointLight* light,
		int lightIndex );

	// Обновляем константный буфер света для сцены
	void Frame ();
	// Обновление массива света
	ID3D11ShaderResourceView * FrameLights ( ID3D11DeviceContext* d3d11DevCon, const UIConstants* ui );

	void RenderClustered ( ID3D11ShaderResourceView *ModelTexture,
		int vertexDrawAmount, int InstancCout );

	void RenderIndextedClustered ( ID3D11ShaderResourceView *ModelTexture,
		int indexDrawAmount, int InstancCout );


	bool CreatingShadowMapsArray ( int Width, int Height, UINT MsaaCount, UINT MsaaQuality, int ArraySize );

	void DeleteShadowMapsArray ( ID3D11Texture2D* Texture, ID3D11ShaderResourceView* SRV );


	// Добавить PointLight или SpotLight источник света
	int AddLightSource ( PointLight &NewLight );

	// Освободить слот от света
	void FreeLightSource ( int LightNumber );

	// Не рисовать свет Dummy -1
	void LightRender ( int LightNumber, bool Yes );

	// Пересоздать буфер светов
	void ResetLightsBuffer ();

	// Удалить PointLight или SpotLight источник света
	bool DeleteLightSource ( int LightNumber );

	// Удалить все источники света
	void DeleteAllLights ();

	// Изменить позицию света
	bool ChangeLightPos ( int LightNumber, XMFLOAT3& pos );
	bool ChangeLightPos ( int LightNumber, XMVECTOR& pos );

	// Изменить границу начала света
	bool ChangeLightAttnuationBegin ( int LightNumber, float& attBegin );

	// Изменить границу окончания света
	bool ChangeLightAttnuationEnd ( int LightNumber, float& attEnd );

	// Изменить цвет света
	bool ChangeLightColor ( int LightNumber, XMFLOAT3& color );

	// Изменение направления света
	bool ChangeLightDirection ( int LightNumber, XMFLOAT3& direction );
	bool ChangeLightDirection ( int LightNumber, XMVECTOR& direction );
	bool ChangeLightDirection ( int LightNumber, XMVECTOR&Axis, float Angel );

	// Получить направление света
	bool GetLightDirection ( int LightNumber, XMFLOAT3& direction );
	bool GetLightDirection ( int LightNumber, XMVECTOR& direction );

	// Получаем координаты положения света
	bool GetLightPos ( int LightNumber, XMFLOAT3& pos );
	bool GetLightPos ( int LightNumber, XMVECTOR& pos );

	// Переключение в режим отображения только света
	void LightsOnly ();
	// Переключение в режим отображения текстур на объектах
	void LightsOnlyOFF ();
	// Изменения количества кластеров световой сцены
	// Допустимые значения 4 8 16 24 32
	void SetClustersAmount ( int amount );
	// Включаем тени
	void ShadowsON ( bool Value );
	// Включаем мягкие тени
	void SoftShadowsON ( bool Value );

	int GetActiveLghtNumber ();

	// Устанавливаем Ambient, Diffuse, Specular по умолчанию
	void SetAllLightDefault ();

	void SetAmbientColor ( XMFLOAT4& );

	void SetPointColor ( XMFLOAT4& );
	void SetPointPos ( XMFLOAT3& );
	void SetPointAttenuation ( XMFLOAT4& );
	void SetPointPower ( float& );

	void SetDiffuseColor ( XMFLOAT4& );
	void SetDiffuseDirection ( XMFLOAT3& );
	void SetDiffusePower ( float& Power );

	void SetSpecularColor ( XMFLOAT4& );
	void SetSpecularPower ( float& );
	void SetSpecularDirection ( XMFLOAT3& );


	// Обновить позицию источника света
	void UpdatePointLightPos ( int& Index, XMFLOAT3& Position );

	// Обновить дальность действия источника света
	void UpdatePointLightPower ( int& Index, float& Distance );	// она же дистанция - расстояние на которое может распростарниться этот свет

																// Обновить цвет точечного источника света
	void UpdatePointLightColor ( int& Index, XMFLOAT4& );


	// http://ru.directx.wikia.com/wiki/DirectX_11_%D1%88%D0%B0%D0%B3_%D0%B7%D0%B0_%D1%88%D0%B0%D0%B3%D0%BE%D0%BC:%D0%A2%D0%BE%D1%87%D0%B5%D1%87%D0%BD%D0%BE%D0%B5_%D0%BE%D1%81%D0%B2%D0%B5%D1%89%D0%B5%D0%BD%D0%B8%D0%B5
	// формула в шейдере для расчёта : Attenuation = att0 + (att1 * d) + (att2 * d?)
	// XMFLOAT4 Attenuation : X - att0 , Y - att1 , Z - att2 , d - PointLightPower - Distance
	// Обновить переменные att в формуле расчёта силы действия точечного света включая дальность действия
	//	void UpdatePointLightAttenuation (int& Index, XMFLOAT4&);

	void GetAmbientColor ( XMFLOAT4& );
	void GetDiffuseColor ( XMFLOAT4& );
	void GetDiffuseDirection ( XMFLOAT3& );
	void GetSpecularColor ( XMFLOAT4& );
	void GetSpecularPower ( float& );
	float GetSpecularPower ();
	void GetSpecularDirection ( XMFLOAT3& );


private:
	// Количество кластеров на которое побит экран
	int ClustersAmount;

// - LIGHTS

// + Shadow Works
/*
public:

// - Shadow Works	

	ID3D11VertexShader* ShadowMapShader;

//	int LightRender_RS_Number = -1;

//	void Init ( HWND hwnd, D3DGlobalContext * D3DGC, ResourceManager * Maneger, D3DClass * Light, FrustumClass* Frustum
//		, ObjectUsingShadows Objects );
	void RenderSpotLightsSadowMaps ( std::vector <int> SpotLightsWithShadowsIndexes );

	void InitRasterizerState ( int DepthBias, float SlopeScaledDepthBias );


private:

	ID3D11DepthStencilView* DSV_ShadowMap3D; // 
	ID3D11RasterizerState* LightRender_RS;
	ID3D11DepthStencilState* LightRender_DS;
//	D3DClass * g_Light;
	D3D11_DEPTH_STENCIL_VIEW_DESC DSD2;
	XMVECTOR Up;
	FrustumClass* g_Frustum;

	void DrawObjectUsingShadows ( XMVECTOR DrawPosition, bool ReplaseData );

	struct cbShadowObject
	{

		XMMATRIX ViewProjection;

	};

	RCube_VecFloat34 LightPosition, LightTarget;
*/

	HRESULT hr, result;
	bool m_vsync_enabled;
	UINT ActiveAdapter;

	vector < IDXGIAdapter1* > Adapters;
	vector < Display_Mode*  > ActualDisplayModes;	// Массив поддерживаемых разрешений экрана в системе

	IDXGIAdapter1* FoundAdapter1;

	bool FxaaIntegrateResource ( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );

	bool InitD2D_D3D101_DWrite ();

	void AdaptersRelease ();

#ifdef RCube_DX11

	vector < CustomSampler >	CustomSamplers;			// Samplers созданные пользователем
	vector < CustomRasterizer > CustomRasterizers;		// Rasterizers созданные пользователем
	vector < CustomDepthStencil > CustomDepthStencils;	// DipthStencils созданные пользователем

	ID3D11DepthStencilState* m_depthDisabledStencilState;

#endif // RCube_DX11

#ifdef RCube_DX12


#endif // RCube_DX12


};

#endif
