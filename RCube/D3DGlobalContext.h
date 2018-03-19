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

// Константный слот  0 - матрицы World, View, Projection, Ortho, Camera Position для кадра
// Константный слот  1 - свет LightClass
// Константный слот 13 - цвет шрифта для рисования
// Константный слот 12 - FXAA фильтрация

// Используется для передачи IDXGIAdapter1,ID3D11Device и т.д. из D3D во все классы для отрисовки
// смотри D3DGlobalContext.h
	struct D3DGlobalContext 
	{
					HWND  hwnd;
		  IDXGIAdapter1*  Adapter;
			 D3D_device*  DX_device;
	ID3D11DeviceContext*  DX_deviceContext;
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
				XMFLOAT2 ScreenDimentions;	// Width , Height
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
ID3D11DepthStencilState* m_depthStencilState;
ID3D11DepthStencilState* CubeMap_DepthStencilState;
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




// Перечень глобальных индексов строк текста 
enum GLOBAL_TEXT_INDEXES{
	INGAME_TEXT	= 0,
	HUD			= 1,
	MENU		= 2,
	STRINGLIST1 = 3
	};

	//  +++++++++++++++++++++++++++    MENU    +++++++++++++++++++++++++++++++++++++++++++++
	// Типы объектов которые могут быть созданы на базе KFButton
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


// Поворот текстуры для кнопок ScrollBox
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
	XMFLOAT4 _ObjParam;	// Координаты объекта X,Y, и размеры Z - Height, W - Width
						// Если Z или W = 0.0f , то берутся размеры текстуры для начального размера
		char *Label;	// начальный текст
		int  Type;		// BUTTON = 0, CHECKBOX = 1, LABEL = 2, EDIT = 3
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
		XMFLOAT4 ObjParam;	// Координаты объекта X,Y, и размеры Z - Width, W - Height
							// Если Z или W = 0.0f , то берутся размеры текстуры для начального размера
		XMFLOAT4 Values;	// Min, Max, Current, Step
		bool Horizontal;	// Горизонтальный ли ScrollBar
		bool UpSideDown;	// Если ScrollBar вертикальный, то удобно когда большее значение - вверху. Для прокрутки на странице удобнее наоборот
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

struct TorchFireSmoke
{
	XMFLOAT3 TorchPosition;	// Позиция факела
	int UX_Amount;			// Количество строк в текстуре анимации
	int VY_Amount;			// Количестко столбцов в текстуре анимации

	int FireFrameStart;		// Стартовый кадр Fire
	int SmokeFrameStart;	// Стартовый уадр Smoke

	int FireFlyFrameNumber;	// Кадр из которого нарезаются FireFly
	int FireFlyCutWidth;	// Размер нарезки куска для FireFly по ширине
	int FireFlyCutHeight;	// Размер нарезки куска для FireFly по высоте

	int FireFramesAmount;	// Реальное количество анимаций в текстуре Fire
	int SmokeFramesAmount;	// Реальное количество анимаций в текстуре Smoke
};

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