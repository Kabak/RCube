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

	int FXAAShaderIndex;	// ������ ������� � ������� ������� ����������� FXAA �����������
	int BlureHorizComputeShaderIndex;
	int BlureVertComputeShaderIndex;
	int ShadowMapShaderIndex;	// ������ ������� ��� ��������� ShadowMap 
	int LightShaderForDraw;		// ������ ������� ��� ��������� �����

// Video Card data
	int m_videoCardMemory;
	char* m_videoCardDescription;
// FXAA ���������� �����
	void FXAAScene();

	int CreateCustomSampler ();						// �������� ������������� �������� �������������

	int CreateCustomRasterizerState ( D3D11_RASTERIZER_DESC& Desc );				// �������� ������������� RSState
   bool ChangeCustomRasterizerState ( int Number, D3D11_RASTERIZER_DESC& Desc );	// Change
   ID3D11RasterizerState* GetCustomRasterizerState ( int Number );

	int CreateCustomDepthStencilState ();			// �������� ������������� DepthStencilState

	bool Rander3DObject ();

	D3D11_VIEWPORT viewport;	// Engine render ViewPort

// + LIGHTS

	// �������� ���������� ������
	int LightsWanted = 128;

	// ������� ������ �� �����
	int mActiveLights = 0;
	int mVisibleLights = 0;
	int mActiveLightsWithShadows = 0;

	//  ++++++++++++++++++   ��� Shadows   +++++++++++++++++++++++++
	int SMWidth; // ������ Shadow Map
	int SMHeight;// ������ Shadow Map
	D3D11_VIEWPORT LightViewPort;	// ShadowMap ViewPort
	//  ------------------   ��� Shadows   -------------------------

	int PointLightSize;	// ������ ��������� PointLight
						// ��� ������

	// ��� � ���� ������ ������
	vector <PointLight*> mPointLightParameters;

	// ������ �������� SpotLights ������� ����� ���� ��� ��������� ShadowMap
	vector <int> SpotLightsWithShadowsIndexes;

	// ������ �������������� �������� ������. �������������� ������ �����������, 
	// ����� ���� ������ �� ����� � ��� ����� ������� �� ���������.
	vector <int> FreeLightIndex;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++    ���������� ������    +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	RCube_VecFloat34 LightPos, LightPosCopy, Temp, Temp2;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++    ���������� ������    +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	FrustumClass* Frustum;

	FragmentFactory fragmentFactory;

	// ����� ������ ��� �������
	ID3D11ShaderResourceView *lightBufferSRV;
	// ����� ��������� ��� �������
	ID3D11ShaderResourceView *LightGridBufferSRV;

	ID3D11Texture2D* ShadowMap3D; // �������� ��� �������� ����
	ID3D11ShaderResourceView* SRV_ShadowMap3D;

	// ������ ����� ��� �����
	PerFrameConstants* Light;

	// Clustered Shading state
	LightGridBuilder mLightGridBuilder;

	int mLightGridBufferSize;

	// ������ ������ ������������ � ������ ��� �����������
	StructuredBuffer<PointLight>* mLightBuffer;
	// ������ ����� ������ ������������ � ������
	StructuredBuffer<LightGridEntry>* mLightGridBuffer;
	// ��������� ��� ������� ��������
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

	// ��������� ����������� ����� ����� ��� �����
	void Frame ();
	// ���������� ������� �����
	ID3D11ShaderResourceView * FrameLights ( ID3D11DeviceContext* d3d11DevCon, const UIConstants* ui );

	void RenderClustered ( ID3D11ShaderResourceView *ModelTexture,
		int vertexDrawAmount, int InstancCout );

	void RenderIndextedClustered ( ID3D11ShaderResourceView *ModelTexture,
		int indexDrawAmount, int InstancCout );


	bool CreatingShadowMapsArray ( int Width, int Height, UINT MsaaCount, UINT MsaaQuality, int ArraySize );

	void DeleteShadowMapsArray ( ID3D11Texture2D* Texture, ID3D11ShaderResourceView* SRV );


	// �������� PointLight ��� SpotLight �������� �����
	int AddLightSource ( PointLight &NewLight );

	// ���������� ���� �� �����
	void FreeLightSource ( int LightNumber );

	// �� �������� ���� Dummy -1
	void LightRender ( int LightNumber, bool Yes );

	// ����������� ����� ������
	void ResetLightsBuffer ();

	// ������� PointLight ��� SpotLight �������� �����
	bool DeleteLightSource ( int LightNumber );

	// ������� ��� ��������� �����
	void DeleteAllLights ();

	// �������� ������� �����
	bool ChangeLightPos ( int LightNumber, XMFLOAT3& pos );
	bool ChangeLightPos ( int LightNumber, XMVECTOR& pos );

	// �������� ������� ������ �����
	bool ChangeLightAttnuationBegin ( int LightNumber, float& attBegin );

	// �������� ������� ��������� �����
	bool ChangeLightAttnuationEnd ( int LightNumber, float& attEnd );

	// �������� ���� �����
	bool ChangeLightColor ( int LightNumber, XMFLOAT3& color );

	// ��������� ����������� �����
	bool ChangeLightDirection ( int LightNumber, XMFLOAT3& direction );
	bool ChangeLightDirection ( int LightNumber, XMVECTOR& direction );
	bool ChangeLightDirection ( int LightNumber, XMVECTOR&Axis, float Angel );

	// �������� ����������� �����
	bool GetLightDirection ( int LightNumber, XMFLOAT3& direction );
	bool GetLightDirection ( int LightNumber, XMVECTOR& direction );

	// �������� ���������� ��������� �����
	bool GetLightPos ( int LightNumber, XMFLOAT3& pos );
	bool GetLightPos ( int LightNumber, XMVECTOR& pos );

	// ������������ � ����� ����������� ������ �����
	void LightsOnly ();
	// ������������ � ����� ����������� ������� �� ��������
	void LightsOnlyOFF ();
	// ��������� ���������� ��������� �������� �����
	// ���������� �������� 4 8 16 24 32
	void SetClustersAmount ( int amount );
	// �������� ����
	void ShadowsON ( bool Value );
	// �������� ������ ����
	void SoftShadowsON ( bool Value );

	int GetActiveLghtNumber ();

	// ������������� Ambient, Diffuse, Specular �� ���������
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


	// �������� ������� ��������� �����
	void UpdatePointLightPos ( int& Index, XMFLOAT3& Position );

	// �������� ��������� �������� ��������� �����
	void UpdatePointLightPower ( int& Index, float& Distance );	// ��� �� ��������� - ���������� �� ������� ����� ���������������� ���� ����

																// �������� ���� ��������� ��������� �����
	void UpdatePointLightColor ( int& Index, XMFLOAT4& );


	// http://ru.directx.wikia.com/wiki/DirectX_11_%D1%88%D0%B0%D0%B3_%D0%B7%D0%B0_%D1%88%D0%B0%D0%B3%D0%BE%D0%BC:%D0%A2%D0%BE%D1%87%D0%B5%D1%87%D0%BD%D0%BE%D0%B5_%D0%BE%D1%81%D0%B2%D0%B5%D1%89%D0%B5%D0%BD%D0%B8%D0%B5
	// ������� � ������� ��� ������� : Attenuation = att0 + (att1 * d) + (att2 * d?)
	// XMFLOAT4 Attenuation : X - att0 , Y - att1 , Z - att2 , d - PointLightPower - Distance
	// �������� ���������� att � ������� ������� ���� �������� ��������� ����� ������� ��������� ��������
	//	void UpdatePointLightAttenuation (int& Index, XMFLOAT4&);

	void GetAmbientColor ( XMFLOAT4& );
	void GetDiffuseColor ( XMFLOAT4& );
	void GetDiffuseDirection ( XMFLOAT3& );
	void GetSpecularColor ( XMFLOAT4& );
	void GetSpecularPower ( float& );
	float GetSpecularPower ();
	void GetSpecularDirection ( XMFLOAT3& );


private:
	// ���������� ��������� �� ������� ����� �����
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
	vector < Display_Mode*  > ActualDisplayModes;	// ������ �������������� ���������� ������ � �������

	IDXGIAdapter1* FoundAdapter1;

	bool FxaaIntegrateResource ( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );

	bool InitD2D_D3D101_DWrite ();

	void AdaptersRelease ();

#ifdef RCube_DX11

	vector < CustomSampler >	CustomSamplers;			// Samplers ��������� �������������
	vector < CustomRasterizer > CustomRasterizers;		// Rasterizers ��������� �������������
	vector < CustomDepthStencil > CustomDepthStencils;	// DipthStencils ��������� �������������

	ID3D11DepthStencilState* m_depthDisabledStencilState;

#endif // RCube_DX11

#ifdef RCube_DX12


#endif // RCube_DX12


};

#endif
