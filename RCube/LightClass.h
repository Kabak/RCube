#pragma once
#include "stdafx.h"

#include <DirectXTex.h>
// NOTE: Complementary Z buffer: clear to 0 (far)!
// float NearPlane = 0.05f;
// float FarPlane = 1000.0f;
// XMVECTOR mCameraNearFarG = XMVECTOR{ NearPlane, FarPlane, 0.0f, 0.0f };
// desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
// camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, Width/Height, FarPlane, NearPlane);
// d3dDeviceContext->ClearDepthStencilView(mDepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "D3DGlobalContext.h"
#include "DirectXMath.h"
#include "ClusterShadingFragmentFactory.h"
#include "LightGrid.h"
#include "Buffer.h"
#include "FrustumClass.h"
#include "D3Dcompiler.h"

using namespace std;
using namespace DirectX;
////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
private:
	// ���������� ��������� �� ������� ����� �����
	int ClustersAmount;

public:

	static const int MAX_LIGHTS = 4096;
	static const int MAX_LIGHTS_WITH_SHADOW = 4096;
	// �������� ���������� ������
	int LightsWanted = 128;

	// ������� ������ �� �����
	int mActiveLights = 0;
	int mVisibleLights = 0;
	int mActiveLightsWithShadows = 0;

	//  ++++++++++++++++++   ��� Shadows   +++++++++++++++++++++++++
	int SMWidth; // ������ Shadow Map
	int SMHeight;// ������ Shadow Map
	D3D11_VIEWPORT LightViewPort;
	//  ------------------   ��� Shadows   -------------------------

	// �������� � �������������������� � �������� Texture2DMS
	// http://www.gamedev.ru/code/forum/?id=211108
	// http://www.gamedev.net/topic/660879-using-texture2dms-in-shader/
	// http://www.gamedev.net/topic/636068-msaa-issues/
	// http://www.gamedev.net/topic/571238-directx-10-view-dimensions-problem/

	struct PointLight
	{
		XMFLOAT3 position;			// ������� ��������� ����� �� �����
		float attenuationBegin;		// ������ ������ ������� �����
		XMFLOAT3 color;				// ���� ��������
		float attenuationEnd;		// ������ ��������� ������� �����
		XMFLOAT3 direction;			// ����������� �����, ���� ���� - SpotLight
		float angel;				// ���� ��������, ���� ���� - SpotLight   ( ���� ���� ����� 0, �� ���� - PointLight )
		XMFLOAT4X4 qtwvp;			// ������� ViewProjection �����
//		XMFLOAT4 RotQuat;			// ���������� ��������
		bool HaveShadow;			// ����� �� ���� ����
		int ShadowMapSliceNumber;	// ����� �������� ShadowMap � ������� Shadow Maps
		int LightID;				// ����� ����� � ������� ������ ( ����� ��� ��������� ������������� Shadow Map �� ���� ������ � ������
		int Dummy;					// �������� ��� �������� https://developer.nvidia.com/pc-gpu-performance-hot-spots   ��������� ��� �������� ������������ ������� ����� � ������� ������ � ������������ � �������
	};

	int PointLightSize;	// ������ ��������� PointLight
	// ��� ������
	PointLight TempL;

	// ��� � ���� ������ ������
	vector <PointLight*> mPointLightParameters;

	// ������ �������� SpotLights ������� ����� ���� ��� ��������� ShadowMap
	vector <int> SpotLightsWithShadowsIndexes;

	// ������ �������������� �������� ������. �������������� ������ �����������, 
	// ����� ���� ������ �� ����� � ��� ����� �������, �� ������ ������ ��� ���� ��� ����� � �������.
	vector <int> FreeLightIndex;

	// ��������� ���������� Clustering Lite ��������
	struct UIConstants
	{
		UINT lightingOnly;
		UINT shadowsOn;
		UINT clusteredGridScale;
		UINT softshadowsOn;
	   float PCF_Amount;
	   float PCF_Step;
	   float ShadowCLAMP;
	   float Shadow_Divider;
	};

	struct PerFrameConstants
	{
		// ������ ���������� � ������ �����
		XMFLOAT4 AmbientColor;			// r,g,b,  a - �� ������������
		XMFLOAT4 DiffuseColor;			// r,g,b,  a - ���� �������� 0 - 1 - max
		XMFLOAT3 DiffuseDirection;
		float DiffusePower;
		XMFLOAT4 SpecularColor;			// r,g,b,  a - ���� �������� �� 0 �� 1 - max
		XMFLOAT3 SpecularDirection;
		float SpecularPower;

		XMVECTOR mCameraNearFar;

		unsigned int mFramebufferDimensionsX;
		unsigned int mFramebufferDimensionsY;
		unsigned int mFramebufferDimensionsZ;
		unsigned int mFramebufferDimensionsW;

		UIConstants mUI;
	};
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++    ���������� ������    +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	RCube_VecFloat34 LightPos, LightPosCopy, Temp, Temp2;

	// ��� ��������� ������ ������� ������
	XMVECTOR One;
	XMVECTOR HalfOne;

	XMFLOAT2 clipMinFast;
	XMFLOAT2 clipMaxFast;
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ++++++++++++++++++++++    ���������� ������    +++++++++++++++++++++
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	D3DGlobalContext* D3DGC_Light;

	FrustumClass* Frustum;

	FragmentFactory fragmentFactory;

	// ����� ������ ��� �������
	ID3D11ShaderResourceView *lightBufferSRV;
	// ����� ��������� ��� �������
	ID3D11ShaderResourceView *LightGridBufferSRV;

	XMMATRIX LightProjection; // ����������� ������� ��� ���������������� ����� � ������� �����
	ID3D11Texture2D* ShadowMap3D; // �������� ��� �������� ����
	ID3D11ShaderResourceView* SRV_ShadowMap3D;
	//ID3D11BlendState* LightRender_BS;

	// ������ ����� ��� �����
	PerFrameConstants* Light;
	// ����������� ����� ��� �������
	//		ID3D11Buffer* cb_LightBuffer;

	HRESULT hr;


	// Clustered Shading state
	LightGridBuilder mLightGridBuilder;

	int mLightGridBufferSize;

	// Flat framebuffer RGBA16-encoded
	struct FramebufferFlatElement
	{
		unsigned int rb;
		unsigned int ga;
	};

	// ID3D11SamplerState* mDiffuseSampler;
	// ID3D11SamplerState* SampleTypeClamp;
	// ID3D11SamplerState* cmpSampler;

	//ID3D11RasterizerState* mRasterizerState;

	D3D11_VIEWPORT viewport;

	// ������ ������ ������������ � ������ ��� �����������
	StructuredBuffer<PointLight>* mLightBuffer;
	// ������ ����� ������ ������������ � ������
	StructuredBuffer<LightGridEntry>* mLightGridBuffer;

	ID3D11Buffer* mPerFrameConstants;

	// ID3D11ShaderResourceView* RTV;
	// ID3D11Texture2D* DSBmDepthBuffer;
	// ID3D11Texture2D *BackBuffer;

	//ID3D11VertexShader* mGeometryVS;
	//ID3D11PixelShader* mClusteredPS;
	//ID3D10Blob* ClustaredVS;
	//ID3D10Blob* ClustaeredPS;

	// XMVECTOR mCameraNearFarG;

inline	void LightClass::UpdateClipRegion (
		float lc,          // Light x/y coordinate (view space)
		float& lz,          // Light z coordinate (view space)
		float& lightRadius,
  const float& cameraScale, // Project scale for coordinate (_11 or _22 for x/y respectively)
		float& clipMin,
		float& clipMax );

inline	XMVECTOR LightClass::ComputeClipRegion (
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



public:
	LightClass ();
	~LightClass ();

	// �������������� ��������� New � delete ��� ����������� ���������� XMMATRIX � ������
	void* operator new( size_t i )
	{
		return _mm_malloc ( i,16 );
	}

		void operator delete( void* p )
	{
		_mm_free ( p );
	}
	// ------------------------------------

	bool Init ( HWND hwnd, D3DGlobalContext* D3DGC, FrustumClass* Frustum );

	// ��������� ����������� ����� ����� ��� �����
	void Frame ();
	// ���������� ������� �����
	ID3D11ShaderResourceView * FrameLights ( ID3D11DeviceContext* d3d11DevCon, const UIConstants* ui );

	void RenderClustered(ID3D11ShaderResourceView *ModelTexture,
		int vertexDrawAmount, int InstancCout);

	void RenderIndextedClustered(ID3D11ShaderResourceView *ModelTexture,
		int indexDrawAmount, int InstancCout);


	bool CreatingShadowMapsArray ( int Width, int Height, UINT MsaaCount, UINT MsaaQuality, int ArraySize );

	void DeleteShadowMapsArray ( ID3D11Texture2D* Texture, ID3D11ShaderResourceView* SRV );


	// �������� PointLight ��� SpotLight �������� �����
	int AddLightSource ( PointLight &NewLight );

	// ���������� ���� �� �����
	void FreeLightSource ( int LightNumber );

	// �� �������� ���� Dummy -1
	void LightRender(int LightNumber, bool Yes);

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

	int GetActiveLghtNumber();

	void Shutdown ();
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

	enum LightCullTechnique
	{
		CULL_FORWARD_NONE = 0,
		CULL_FORWARD_PREZ_NONE,
		CULL_DEFERRED_NONE,
		CULL_QUAD,
		CULL_QUAD_DEFERRED_LIGHTING,
		CULL_COMPUTE_SHADER_TILE,
		CULL_CLUSTERED,
	};

};

#endif
