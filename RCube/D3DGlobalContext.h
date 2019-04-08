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

// Константный слот VS  0 - матрицы World, View, Projection, Ortho, Camera Position для кадра
// Константный слот VS  1 - свет LightClass
// Константный слот VS 13 - цвет шрифта для рисования   НЕ ИСПОЛЬЗУЕТСЯ УЖЕ

// Константный слот PS 1  - свет D3DClass Light PerFrameConstants  ( DEBUG Constants )
// Константный слот PS 12 - FXAA фильтрация
/*
Samplers Slots
0 - 3D Model Samspling
1 - Shadow Map Sampling
2 - Shadow Map PCF Filter Sampling
3 - Flat Object Sampling (Text, Menu, 2DAnimation, Particles, CubMap)
*/

// Используется для передачи IDXGIAdapter1,ID3D11Device и т.д. из D3D во все классы для отрисовки
// смотри D3DGlobalContext.h

	struct D3DGlobalContext 
	{
					HWND  hwnd;
		  IDXGIAdapter1*  Adapter;
				 Device*  DX_device;
		  DeviceContext*  DX_deviceContext;
		 IDXGISwapChain*  DX_swapChain;
		  ID3D10Device1*  D10_device;
		ID3D11Texture2D*  BackBuffer2DT;	// Собственно BackBuffer
 ID3D11RenderTargetView*  BackBuffer_RTV;	// RenderTargetView из backBuffer
ID3D11ShaderResourceView* BackBuffer_SRV;	// ShaderResourceView из backBuffer
ID3D11UnorderedAccessView* BackBuffer_UAV;	// UnorderedAccessView из backBuffer. Невозможно создать при использовании MSAA ( нужно создавать для ResolveSubresources )
// Fonts
		 ID3D11Texture2D* sharedTex11;		// Текстура на которой рисуются шрифты DX10,DX11 Key_Mutex текстура
ID3D11ShaderResourceView* sharedTex11_SRV;	// SRV Текстуры на которой рисуются шрифты DX10,DX11 Key_Mutex текстура
		 ID3D11Texture2D* sharedTex11_MAPED;// Текстура для копирования sharedTex11, чтобы можно было сделать MAP копии sharedTex11
		 IDXGIKeyedMutex* keyedMutex11;
		 IDXGIKeyedMutex* keyedMutex10;
	   ID2D1RenderTarget* D2DRenderTarget;	// D2D рисует текст и всё на этой текстуре. Фактически это - sharedTex11
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
// Включаем MSAA
					bool EnableMSAA;
// Желаемый: 2x 4x 8x Quality Support
					UINT MSAAQualityCount;
// ответное значение для 2x 4x 8x
					UINT MSAAQualityChoosen;
	   ID3D11BlendState* m_alphaEnableBlendingState ;
	   ID3D11BlendState* m_alphaDisableBlendingState;
 ID3D11DepthStencilView* m_depthStencilView;
// Shadows
 ID3D11DepthStencilView* DSV_ShadowMap3D;
// Shadows

// Для Clustering
		ID3D11Texture2D* m_depthStencilBuffer;
  ID3D11RasterizerState* DefaultRasterizerState;
  ID3D11RasterizerState* WireFrameRasterizerState;
ID3D11DepthStencilState* depthStencil_State;
ID3D11DepthStencilState* CubeMap_DepthStencil_State;
ID3D11DepthStencilState* depthStencil_Disabled_State;
ID3D11DepthStencilState* depthStencil_NoWrite_Particles;
 	   ID3D11BlendState* mGeometryBlendState;
// Для системы частиц, чтобы был эффект кристаликов
	   ID3D11RasterizerState* RasterStateCullNone;
	   ID3D11BlendState* m_alphaParticleBlendingState;
// Для рисования текстурой с текстом на текстуре
	   ID3D11BlendState* m_alpha_TOnT_BlendingState;
//  ++++++++++++++++++   Для Shadows   +++++++++++++++++++++++++
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

//  ------------------   Для Shadows   -------------------------

// ++++    FXAA сглаживание  и те  ++++
 				     bool EnableFXAA;

 ConstantBuffer<CB_FXAA>*				g_pcbFXAA;			// FXAA константы
 ConstantBuffer<ConstantBufferData>*	Global_VS_ConstantsBuffer;	// Константы для вертексных шейдеров
// Shadows
 ConstantBuffer<CB_ShadowMap>*			ShadowMapLightView;	// Матрица вида с позиции света
 ID3D11RasterizerState*					LightRender_RS;
 ID3D11DepthStencilState*				LightRender_DS;
 // Shadows
	     ID3D11Texture2D* BackBuffer_ProxyTexture;			// Временная текстура для рисования всего что нужно
ID3D11ShaderResourceView* BackBuffer_ProxyTextureSRV;		// Например , StringsList сюда рисуется и скролится
  ID3D11RenderTargetView* BackBuffer_ProxyTextureRTV;
	     ID3D11Texture2D* BackBuffer_CopyResolveTexture;
ID3D11ShaderResourceView* BackBuffer_CopyResolveTextureSRV;
ID3D11UnorderedAccessView*BackBuffer_CopyResolveTextureUAV; // Невозможно создать при использовании MSAA ( нужно создавать для ResolveSubresources .  ЧТО МЫ И СДЕЛАЛИ)
	  ID3D11SamplerState* FXAA_Sampler;						// Sampler for FXAA input texture
// Для скриншотов
	     ID3D11Texture2D* ScreenShootTexture;
//	   ID3D11ShaderResourceView* SRV_ScreenShootTexture;

// ----------    FXAA    ----------

// ++++++++++ DirectX 10 , DirectWrite , FONTS    ++++++++++++++++
	   const float ZeroColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // Используется классом StringsList для очистки текстуры для написания строк текста


// ---------- DirectX 10 , DirectWrite , FONTS    ----------------

			 InputClass* m_EngineInputClass;	// Нужно для MenuControllerClass для управления вводом при назначении клавиш в игре 

	};


// Используется в D3DClass для создания списка доступных и рабочих режимов системы отображения ( монитора, ТВ, LСD )
	struct Display_Mode
	{
		UINT Number; // Номер режима в списке
		UINT Hieght; 
		UINT Width;
		UINT Freqency;
	 wchar_t Resolution[15]; // Строка для меню ( например: 1024 x 768 )
	};


	enum FLAT_OBJ_TYPE
	{
		ANIMATION   = -1,
		REGULAR     = 0
	};

// Перечень глобальных индексов строк текста 
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
// Типы объектов которые могут быть созданы на базе KFButton
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


// Поворот текстуры для кнопок ScrollBox
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
	const float MIN_TRAVELER_SIZE = 20.0f; // В пикселях 20
	// ColorPicker
//	const float MIN_SELECTOR_SIZE = 11.0f; // В пикселях ( потому, что мой текстура селектора 11 pix
	const float MIN_PANTHON_SIZE = 5.0f; // В пикселях 
	const float MAX_PANTHON_SIZE = 50.0f; // В пикселях 
	const float MIN_COLORPICKER_SIZE = 50.0f; // In pixel
	const float MIN_DISTANCE_FROM_BODY = 5.0f; // MIN distance from ColorPicker to Panton & Alpha


	struct KFButton_Elements {
	XMFLOAT4 _ObjParam;	// Координаты объекта X,Y, и размеры Z - Width, W - Height
						// Если Z или W = 0.0f , то берутся размеры текстуры для начального размера
	XMFLOAT4 Colour;	// If the button is COLOR square type
		char *Label;	// начальный текст
		int  Type;		// BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3, COLOR = 4
		int  EditType;	// ANYSYMBOL = 1, AS_INTEGER = 2, AS_FLOAT = 3   если равно NULL или отличное от указанного, то кнопка не будет работать как Edit
		int  SentenceIndex; // Номер предложения из класса TextClass ( обязательно только для Label и Edit  для остальных можно -1 )
		SENTENCE_INIT_DATA* Data;
		bool Checked;	// Для CheckBox - в нажатом ли состоянии кнопка
		bool Enabled;	// Доступен ли объект или рисуется текстура недоступности и объект не реагирует на мышь и прочее
// Label
		bool ActiveStringsList;		// Для активного списка строк в меню. На которые можно нажать и они подсвечиваются при проходе мышки. При нажатии должна вернуть номер строки связанный с этим Label
		bool WaitingForKeyPress;	// Для Label который используется для привязки виртуальных кнопок в игре ( если False, то Label просто как строка текста - не реагирует на нажатие и пролёты мышки и прочее )
		int  VirtualKeyIndex;		// Привязка Label к виртуальной кнопке в движке. ( Для переопределения кнопки в меню )
		bool SecondSlot;			// Слот для определения кнопок tckb true, то привязка ко 2-му слоту
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
		XMFLOAT4 ObjParam;	// Координаты объекта X,Y, и размеры Z - Width, W - Height
							// Если Z или W = 0.0f , то берутся размеры текстуры для начального размера
		XMFLOAT4 Values;	// Min, Max, Current, Step
		bool Horizontal;	// Горизонтальный ли ScrollBar
		bool UpSideDown;	// Если ScrollBar вертикальный, то удобно когда большее значение - вверху. Для прокрутки на странице удобнее наоборот
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
		XMFLOAT4	ObjParam;		// Координаты объекта X,Y, и размеры Z - Width, W - Height
			 int	SentencesIndex; // Индекс группы текстовых строк в списке всех строк, чтобы все строки для конкретного StringsList рисовались одновременно
			 int	FontIndex;		// номер шрифта которым рисуется текст	
			 int	StringsSpacing; // Растояние между строками
		   float	Scaling;		// Для подгонки текста в окно StringsList
			 int	StringsMAXLength;// Максимальное количество char в строках
			 int	ScrollSpeed;	// Скорость прокрутки
  vector <char*>    Strings;		// массив строк из которого берутся отображаемые строки
	};


//  +++++++++++++++++++++++++++    MENU    +++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++    PARTICLE SYSTEM     +++++++++++++++++++++++++++++++++

// Информация о каждой модели на сцене ( Для обработки отрисовки модели )
struct Model_data
{
	int Index;		// Индекс модели в общем списке
  float CamDistance;// Расстояние до камеры
   bool Render;		// Рисуется
   bool InFrustum;	// В зоне видимости камеры
// bool Ocluded;	// Закрыт другим объектом
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