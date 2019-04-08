#pragma once
#include "stdafx.h"
#ifndef _D3DGLOBALCONTEXT_H_
#define _D3DGLOBALCONTEXT_H_

#include"Buffers_def.h"
#ifdef RCube_DX11
#include <d3d11.h>
#include "DX11Buffers.h"
#endif //RCube_DX11

#include <d2d1.h>
#include <DWrite.h>
#include "DirectXMath.h"
#include "InputClass.h"
#include <vector>
#include "Text_def.h"

using namespace DirectX;

// ����������� ���� VS  0 - ������� World, View, Projection, Ortho, Camera Position ��� �����
// ����������� ���� VS  1 - ���� LightClass
// ����������� ���� VS 13 - ���� ������ ��� ���������   �� ������������ ���

// ����������� ���� PS 1  - ���� D3DClass Light PerFrameConstants  ( DEBUG Constants )
// ����������� ���� PS 12 - FXAA ����������
/*
Samplers Slots
0 - 3D Model Samspling
1 - Shadow Map Sampling
2 - Shadow Map PCF Filter Sampling
3 - Flat Object Sampling (Text, Menu, 2DAnimation, Particles, CubMap)
*/

// ������������ ��� �������� IDXGIAdapter1,ID3D11Device � �.�. �� D3D �� ��� ������ ��� ���������
// ������ D3DGlobalContext.h

	struct D3DGlobalContext 
	{
					HWND  hwnd;
		  IDXGIAdapter1*  Adapter;
				 Device*  DX_device;
		  DeviceContext*  DX_deviceContext;
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
				XMFLOAT4 ScreenScale;	// Width, Height, Width in %, Height in %  Engine Screen resolution autoadapting scale system
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
ID3D11DepthStencilState* depthStencil_State;
ID3D11DepthStencilState* CubeMap_DepthStencil_State;
ID3D11DepthStencilState* depthStencil_Disabled_State;
ID3D11DepthStencilState* depthStencil_NoWrite_Particles;
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
					  int ClustersAmount;

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


	enum FLAT_OBJ_TYPE
	{
		ANIMATION   = -1,
		REGULAR     = 0
	};

// �������� ���������� �������� ����� ������ 
	enum GLOBAL_TEXT_INDEXES
	{
		INGAME_TEXT = 0,
		HUD			= 1,
		MENU		= 2,
		STRINGLIST1 = 3
	};

	//  +++++++++++++++++++++++++++    MENU    +++++++++++++++++++++++++++++++++++++++++++++

	struct MenuItem
	{
		MenuItem ()
		{
						ObjX = 0; 
						ObjY = 0; 
					ObjWidth = 0; 
				   ObjHeight = 0; 
					   Group = -1;  
	    PreviousItemDistance = 0;
//			NextItemDistanceY = 0; 
//					  Plased = false;
		}

		int ObjX;		// Object X pos
		int ObjY;		// Object Y pos
		int ObjWidth;	// Object Width
		int ObjHeight;	// Object Height

		int PreviousItemDistance; // Distance to next MenuItem X direction
//		int NextItemDistanceY; // Distance to next MenuItem Y direction

//		bool Plased;

		int Index;
		int Type;
		int IndexInType; // Index among Buttons or ScrollBars...
		int Group; // Screen Zone where Item is located
//		XMFLOAT4 MenuDimention;
	};

//	vector <int*> ItemIndex;

	struct GroupItem
	{
		GroupItem ()
		{
			GroupX = 0; GroupY = 0; GroupWidth = 0; GroupHeight = 0; DistanceBetweenElements = 0;
			ItemIndexes.reserve ( 10 );
		}

		~GroupItem () { ItemIndexes.clear (); };

		int GroupX;		// Group X pos
		int GroupY;		// Group Y pos
		int GroupWidth;	// Group Width
		int GroupHeight;	// Group Height
		float ScaleX;	// 
		float ScaleY;	// 
		bool Direction; // If X = true, if false = Y
		int DistanceBetweenElements;
		vector <int> ItemIndexes; // Objects indexes that fits into group

	};
	
enum _X_Y_ {
	_X_ = true,
	_Y_ = false
};
// ���� �������� ������� ����� ���� ������� �� ���� KFButton
enum MENU_OBJ_STYLE {
	   BUTTON		= 0,
	   CHECKBOX		= 1,
	   LABEL		= 2,
	   EDIT			= 3,
	   COLOR_PICK	= 4,
	   SCROLLBAR	= 5,
	   STR_LIST		= 6,
	   TEXTSTRING	= 7
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

enum EDIT_INPUT_STYLE {
	   ANYSYMBOL  = 1,
	   AS_INTEGER = 2,
	   AS_FLOAT   = 3
	 };

/*
enum COLOR_PICKER_STYLE
{
	PANTHON = 1,
	COLOR_PICKER = 2,
	ALPHA_PICKER = 3
};
*/

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
	  FLIP_HORIZONTAL_ROTATE_90 = 5,
	  ANIMATION_TEXTURE			= 6,
	  COLOR_BUTTON				= 7,
	  COLOR_PICKER				= 8,
	  ALPHA_PICKER_HORIZONTAL	= 9,
	  ALPHA_PICKER_VERTICAL		= 10
	};


	typedef enum {
		SCROLL_FROM_POINT_INSENTER = 0,
	} MenuShowMethod;


	struct Font_Param {
	
		 bool Outlined;
		  int OutlineWidth;
		float FontSize;
		D2D1_GRADIENT_STOP gradientStops[3];
		XMFLOAT4 GradientPos;
		XMFLOAT4 OutlineColor;
		wchar_t *Font_Path_Name;

	};

// MENU definitions
	const int MIN_PLACE_ACCURACY = 0;
	// Button
	const float MIN_BUTTON_WIDTH = 20.0f;
	const float MIN_BUTTON_HEIGHT = 20.0f;
	const float MIN_BUTTON_SIZE   = 5.0f;
	// ScrollBar
	const float MIN_TRAVELER_SIZE = 20.0f; // � �������� 20
	// ColorPicker
//	const float MIN_SELECTOR_SIZE = 11.0f; // � �������� ( ������, ��� ��� �������� ��������� 11 pix
	const float MIN_PANTHON_SIZE = 5.0f; // � �������� 
	const float MAX_PANTHON_SIZE = 50.0f; // � �������� 
	const float MIN_COLORPICKER_SIZE = 50.0f; // In pixel
	const float MIN_DISTANCE_FROM_BODY = 5.0f; // MIN distance from ColorPicker to Panton & Alpha


	struct KFButton_Elements {
	XMFLOAT4 _ObjParam;	// ���������� ������� X,Y, � ������� Z - Width, W - Height
						// ���� Z ��� W = 0.0f , �� ������� ������� �������� ��� ���������� �������
	XMFLOAT4 Colour;	// If the button is COLOR square type
		char *Label;	// ��������� �����
		int  Type;		// BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3, COLOR = 4
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
		ID3D11ShaderResourceView * OsnTexture;
		ID3D11ShaderResourceView * IsClickTexture;
		ID3D11ShaderResourceView * IsMouseOnButtonTexture;
		ID3D11ShaderResourceView * IsNotEnalbledTexture;
	};

	struct KFButtons_OutPut {
		bool IsPress;
		bool IsMouseOnButton;
	};

	struct ScrollBar_Elements
	{
		bool ShowButtons;
		XMFLOAT4 ObjParam;	// ���������� ������� X,Y, � ������� Z - Width, W - Height
							// ���� Z ��� W = 0.0f , �� ������� ������� �������� ��� ���������� �������
		XMFLOAT4 Values;	// Min, Max, Current, Step
		bool Horizontal;	// �������������� �� ScrollBar
		bool UpSideDown;	// ���� ScrollBar ������������, �� ������ ����� ������� �������� - ������. ��� ��������� �� �������� ������� ��������
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


	struct ColorPicker_Horisontal_Layout
	{
		XMFLOAT4 Body			= {   0.0f,   0.0f, 300.0f, 300.0f };
		XMFLOAT4 Selector		= {  10.0f,   5.0f,  20.0f, 255.0f };

	};


	struct ColorPicker_Vertical_Layout
	{
		XMFLOAT4 Body			= {   0.0f,   0.0f, 300.0f, 300.0f };
		XMFLOAT4 Selector		= {  10.0f,   5.0f,  20.0f, 255.0f };
	};


	struct ColorPicker_Elements
	{
		XMFLOAT4 ObjParam;	// Object location X,Y  , size : Z - Width, W - Height
							// If Z or W = 0.0f Will be used for create ColorPicker dementions
							// otherwise dementions will be calculated using BodyTexture size
		XMFLOAT4 ActiveColorValue;
		float _Panthon_Alpha_Thickness;	// Desired width of elements
		bool Horizontal;				// Horizontal or Vertical ColorPicker layout
		ID3D11ShaderResourceView * PanthonTexture;
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