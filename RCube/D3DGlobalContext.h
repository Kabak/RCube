#pragma once
#include "stdafx.h"
#ifndef _D3DGLOBALCONTEXT_H_
#define _D3DGLOBALCONTEXT_H_

#include <d3d11.h>
#include <d2d1.h>
#include <DWrite.h>
#include "DirectXMath.h"
#include "InputClass.h"
#include <vector>
#include "Text_def.h"

#include"Buffers_def.h"
#ifdef RCube_DX11
#include "DX11Buffers.h"
typedef ID3D11Device D3D_device;
#endif //RCube_DX11

using namespace DirectX;

// ����������� ����  0 - ������� World, View, Projection, Ortho, Camera Position ��� �����
// ����������� ����  1 - ���� LightClass
// ����������� ���� 13 - ���� ������ ��� ���������
// ����������� ���� 12 - FXAA ����������

// ������������ ��� �������� IDXGIAdapter1,ID3D11Device � �.�. �� D3D �� ��� ������ ��� ���������
// ������ D3DGlobalContext.h
	struct D3DGlobalContext 
	{
					HWND  hwnd;
		  IDXGIAdapter1*  Adapter;
			 D3D_device*  DX_device;
	ID3D11DeviceContext*  DX_deviceContext;
		 IDXGISwapChain*  DX_swapChain;
		  ID3D10Device1*  D10_device;
		ID3D11Texture2D*  BackBuffer2DT;	// ���������� BackBuffer
 ID3D11RenderTargetView*  BackBuffer_RTV;	// RenderTargetView �� backBuffer
ID3D11ShaderResourceView* BackBuffer_SRV;	// ShaderResourceView �� backBuffer
ID3D11UnorderedAccessView* BackBuffer_UAV;	// UnorderedAccessView �� backBuffer. ���������� ������� ��� ������������� MSAA ( ����� ��������� ��� ResolveSubresources )
// Fonts
		 ID3D11Texture2D* sharedTex11;		// �������� �� ������� �������� ������ DX10,DX11 Key_Mutex ��������
ID3D11ShaderResourceView* sharedTex11_SRV;	// SRV �������� �� ������� �������� ������ DX10,DX11 Key_Mutex ��������
		 ID3D11Texture2D* sharedTex11_MAPED;// �������� ��� ����������� sharedTex11, ����� ����� ���� ������� MAP ����� sharedTex11
		 IDXGIKeyedMutex* keyedMutex11;
		 IDXGIKeyedMutex* keyedMutex10;
	   ID2D1RenderTarget* D2DRenderTarget;	// D2D ������ ����� � �� �� ���� ��������. ���������� ��� - sharedTex11
			ID2D1Factory* D2DFactory;
					UINT  sharedTex11_Width;
					UINT  sharedTex11_Height;



		 IDWriteFactory* DWriteFactory;
#if defined( DEBUG ) || defined( _DEBUG )
			ID3D11Debug* DebugDevice;
#endif
					 int ScreenWidth;
					 int ScreenHeight;
					 int WindowsPosX;
					 int WindowsPosY;
				   float ScreenRatio; // ScreenWidth / ScreenHeight
				   float NearPlane;
				   float FarPlane;
				XMFLOAT2 ScreenDimentions;	// Width , Height
				XMMATRIX WorldMatrix;
			    XMMATRIX ViewMatrix;
			    XMMATRIX OrthoMatrix;
				XMMATRIX ProjectionMatrix;
				XMMATRIX ViewProjection;
				XMFLOAT3 CameraPosition;
//				XMVECTOR CameraLookAt;
// �������� MSAA
					bool EnableMSAA;
// ��������: 2x 4x 8x Quality Support
					UINT MSAAQualityCount;
// �������� �������� ��� 2x 4x 8x
					UINT MSAAQualityChoosen;
	   ID3D11BlendState* m_alphaEnableBlendingState ;
	   ID3D11BlendState* m_alphaDisableBlendingState;
 ID3D11DepthStencilView* m_depthStencilView;
// Shadows
 ID3D11DepthStencilView* DSV_ShadowMap3D;
// Shadows

// ��� Clustering
		ID3D11Texture2D* m_depthStencilBuffer;
  ID3D11RasterizerState* DefaultRasterizerState;
  ID3D11RasterizerState* WireFrameRasterizerState;
ID3D11DepthStencilState* m_depthStencilState;
ID3D11DepthStencilState* CubeMap_DepthStencilState;
 	   ID3D11BlendState* mGeometryBlendState;
// ��� ������� ������, ����� ��� ������ �����������
	   ID3D11RasterizerState* RasterStateCullNone;
	   ID3D11BlendState* m_alphaParticleBlendingState;
// ��� ��������� ��������� � ������� �� ��������
	   ID3D11BlendState* m_alpha_TOnT_BlendingState;
//  ++++++++++++++++++   ��� Shadows   +++++++++++++++++++++++++
					 bool ShadowsOn;
					 bool SoftShadowsOn;
					float PCF_Amount;
					float PCF_Step;
					float ShadowCLAMP;
					float Shadow_Divider;

// Engine default samplers
	  ID3D11SamplerState* Wrap_Model_Texture;
	  ID3D11SamplerState* CLight_ShadowMap_Sampler;
	  ID3D11SamplerState* CLight_SM_PCF_Sampler;
	  ID3D11SamplerState* FlatObject_Sampler;

//  ------------------   ��� Shadows   -------------------------

// ++++    FXAA �����������  � ��  ++++
 				     bool EnableFXAA;

 ConstantBuffer<CB_FXAA>*				g_pcbFXAA;			// FXAA ���������
 ConstantBuffer<ConstantBufferData>*	Global_VS_ConstantsBuffer;	// ��������� ��� ���������� ��������
// Shadows
 ConstantBuffer<CB_ShadowMap>*			ShadowMapLightView;	// ������� ���� � ������� �����
 ID3D11RasterizerState*					LightRender_RS;
 ID3D11DepthStencilState*				LightRender_DS;
 // Shadows
	     ID3D11Texture2D* BackBuffer_ProxyTexture;			// ��������� �������� ��� ��������� ����� ��� �����
ID3D11ShaderResourceView* BackBuffer_ProxyTextureSRV;		// �������� , StringsList ���� �������� � ���������
  ID3D11RenderTargetView* BackBuffer_ProxyTextureRTV;
	     ID3D11Texture2D* BackBuffer_CopyResolveTexture;
ID3D11ShaderResourceView* BackBuffer_CopyResolveTextureSRV;
ID3D11UnorderedAccessView*BackBuffer_CopyResolveTextureUAV; // ���������� ������� ��� ������������� MSAA ( ����� ��������� ��� ResolveSubresources .  ��� �� � �������)
	  ID3D11SamplerState* FXAA_Sampler;						// Sampler for FXAA input texture
// ��� ����������
	     ID3D11Texture2D* ScreenShootTexture;
//	   ID3D11ShaderResourceView* SRV_ScreenShootTexture;

// ----------    FXAA    ----------

// ++++++++++ DirectX 10 , DirectWrite , FONTS    ++++++++++++++++
	   const float ZeroColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // ������������ ������� StringsList ��� ������� �������� ��� ��������� ����� ������


// ---------- DirectX 10 , DirectWrite , FONTS    ----------------

			 InputClass* m_EngineInputClass;	// ����� ��� MenuControllerClass ��� ���������� ������ ��� ���������� ������ � ���� 

	};


// ������������ � D3DClass ��� �������� ������ ��������� � ������� ������� ������� ����������� ( ��������, ��, L�D )
	struct Display_Mode
	{
		UINT Number; // ����� ������ � ������
		UINT Hieght; 
		UINT Width;
		UINT Freqency;
	 wchar_t Resolution[15]; // ������ ��� ���� ( ��������: 1024 x 768 )
	};




// �������� ���������� �������� ����� ������ 
enum GLOBAL_TEXT_INDEXES{
	INGAME_TEXT	= 0,
	HUD			= 1,
	MENU		= 2,
	STRINGLIST1 = 3
	};

	//  +++++++++++++++++++++++++++    MENU    +++++++++++++++++++++++++++++++++++++++++++++
	// ���� �������� ������� ����� ���� ������� �� ���� KFButton
enum {
	   BUTTON	= 0,
	   CHECKBOX = 1,
	   LABEL	= 2,
	   EDIT		= 3
	};
/*
enum {
		MIDLE = 0,
		UP	  = 1,
		DOWN  = 2,
		LEFT  = 3,
		RIGHT = 4,
		FREE  = 5
	};
*/

enum {
	   ANYSYMBOL  = 1,
	   AS_INTEGER = 2,
	   AS_FLOAT   = 3
	 };


	typedef enum {
//		NOBACKGROUND,
		STATIC_TEXTURE,
		ANIMATED_TEXTURE
	} BackGroundType;


// ������� �������� ��� ������ ScrollBox
enum {
	  NO_FLIP					= 0,
	  FLIP_HORIZONTAL			= 1,
	  FLIP_VERTICAL				= 2,
	  ROTATE90					= 3,
	  ROTATE_90					= 4,
	  FLIP_HORIZONTAL_ROTATE_90 = 5
	};


	typedef enum {
		SCROLL_FROM_POINT_INSENTER = 0,
	} MenuShowMethod;


	struct KFButton_Elements {
	XMFLOAT4 _ObjParam;	// ���������� ������� X,Y, � ������� Z - Height, W - Width
						// ���� Z ��� W = 0.0f , �� ������� ������� �������� ��� ���������� �������
		char *Label;	// ��������� �����
		int  Type;		// BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3
		int  EditType;	// ANYSYMBOL = 1, AS_INTEGER = 2, AS_FLOAT = 3   ���� ����� NULL ��� �������� �� ����������, �� ������ �� ����� �������� ��� Edit
		int  SentenceIndex; // ����� ����������� �� ������ TextClass ( ����������� ������ ��� Label � Edit  ��� ��������� ����� -1 )
		SENTENCE_INIT_DATA* Data;
		bool Checked;	// ��� CheckBox - � ������� �� ��������� ������
		bool Enabled;	// �������� �� ������ ��� �������� �������� ������������� � ������ �� ��������� �� ���� � ������
// Label
		bool ActiveStringsList;		// ��� ��������� ������ ����� � ����. �� ������� ����� ������ � ��� �������������� ��� ������� �����. ��� ������� ������ ������� ����� ������ ��������� � ���� Label
		bool WaitingForKeyPress;	// ��� Label ������� ������������ ��� �������� ����������� ������ � ���� ( ���� False, �� Label ������ ��� ������ ������ - �� ��������� �� ������� � ������ ����� � ������ )
		int  VirtualKeyIndex;		// �������� Label � ����������� ������ � ������. ( ��� ��������������� ������ � ���� )
		bool SecondSlot;			// ���� ��� ����������� ������ tckb true, �� �������� �� 2-�� �����
// Label
		ID3D11Resource * OsnTextureResource;
		ID3D11ShaderResourceView * OsnTexture;
		ID3D11ShaderResourceView * IsClickTexture;
		ID3D11ShaderResourceView * IsMouseOnButtonTexture;
		ID3D11ShaderResourceView * IsNotEnalbledTexture;
	};

	struct KFButtons_OutPut {
		bool IsPress;
		bool IsMouseOnButton;
	};

	struct KFScrollBar_Elements
	{
		bool ShowButtons;
		XMFLOAT4 ObjParam;	// ���������� ������� X,Y, � ������� Z - Width, W - Height
							// ���� Z ��� W = 0.0f , �� ������� ������� �������� ��� ���������� �������
		XMFLOAT4 Values;	// Min, Max, Current, Step
		bool Horizontal;	// �������������� �� ScrollBar
		bool UpSideDown;	// ���� ScrollBar ������������, �� ������ ����� ������� �������� - ������. ��� ��������� �� �������� ������� ��������
		ID3D11Resource * OsnTextureResource;
		ID3D11ShaderResourceView * BodyTexture;
		ID3D11ShaderResourceView * ButtonsTexture;
		ID3D11ShaderResourceView * TravellerTexture;
	
		ID3D11ShaderResourceView * MouseOnButtonTexture;
		ID3D11ShaderResourceView * MouseOnTravellerTexture;

		ID3D11ShaderResourceView * BodyNotEnalbledTexture;
		ID3D11ShaderResourceView * ButtonNotEnalbledTexture;
		ID3D11ShaderResourceView * TravellerNotEnalbledTexture;

		ID3D11ShaderResourceView * ButtonPressTexture;
		ID3D11ShaderResourceView * TravellerPressTexture;
	};

	struct StringsList_Elements
	{
			bool	_ShowStringsList;
			bool	Enabled;
		XMFLOAT4	ObjParam;		// ���������� ������� X,Y, � ������� Z - Width, W - Height
			 int	SentencesIndex; // ������ ������ ��������� ����� � ������ ���� �����, ����� ��� ������ ��� ����������� StringsList ���������� ������������
			 int	FontIndex;		// ����� ������ ������� �������� �����	
			 int	StringsSpacing; // ��������� ����� ��������
		   float	Scaling;		// ��� �������� ������ � ���� StringsList
			 int	StringsMAXLength;// ������������ ���������� char � �������
			 int	ScrollSpeed;	// �������� ���������
  vector <char*>    Strings;		// ������ ����� �� �������� ������� ������������ ������
	};


//  +++++++++++++++++++++++++++    MENU    +++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++    PARTICLE SYSTEM     +++++++++++++++++++++++++++++++++

struct TorchFireSmoke
{
	XMFLOAT3 TorchPosition;	// ������� ������
	int UX_Amount;			// ���������� ����� � �������� ��������
	int VY_Amount;			// ���������� �������� � �������� ��������

	int FireFrameStart;		// ��������� ���� Fire
	int SmokeFrameStart;	// ��������� ���� Smoke

	int FireFlyFrameNumber;	// ���� �� �������� ���������� FireFly
	int FireFlyCutWidth;	// ������ ������� ����� ��� FireFly �� ������
	int FireFlyCutHeight;	// ������ ������� ����� ��� FireFly �� ������

	int FireFramesAmount;	// �������� ���������� �������� � �������� Fire
	int SmokeFramesAmount;	// �������� ���������� �������� � �������� Smoke
};

// ���������� � ������ ������ �� ����� ( ��� ��������� ��������� ������ )
struct Model_data
{
	int Index;		// ������ ������ � ����� ������
  float CamDistance;// ���������� �� ������
   bool Render;		// ��������
   bool InFrustum;	// � ���� ��������� ������
// bool Ocluded;	// ������ ������ ��������
};

struct CustomSampler
{
#ifdef RCube_DX11
	ID3D11SamplerState* Sampler;

#endif
#ifdef RCube_DX12

#endif // RCube_DX12
		
			   wchar_t* Name;
};


struct CustomRasterizer
{
#ifdef RCube_DX11
	ID3D11RasterizerState * Rasterizer;

#endif
#ifdef RCube_DX12

#endif // RCube_DX12

	wchar_t* Name;
};


struct CustomDepthStencil
{
#ifdef RCube_DX11
	ID3D11DepthStencilState * DepthStencilState;

#endif
#ifdef RCube_DX12

#endif // RCube_DX12

	wchar_t* Name;
};


enum DirectXInitVer{
	DX11,
	DX12
};

#endif