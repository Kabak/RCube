#pragma once
#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: GraphicsClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x800
#endif

//////////////
// INCLUDES //
#include "D3DGlobalContext.h"
#include "DxInputStruct.h"
#include <DWrite.h>
#include <D2D1.h>
#include <vector>

#include "FPSTimers.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "ResourceManager.h"
#include "TextClass.h"
#include "dinput.h"
#include "FrustumClass.h"
#include "RenderTextureClass.h"
#include "Light_def.h"
// ++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++
#include "FireParticleSystem.h"
#include "SnowFallParticles.h"
#include "TorchFireParticles.h"
// ++++++++++++   Измеряем быстродействие кода   +++++++++++++
#include "RCubeProfiling.h"
// ++++++++++++   Свет сцены   +++++++++++
//#include "LightClass.h"
// ++++++++++++   Ландшафт   +++++++++++++
#include "Terrain.h"
// +++++++++++++++++++++++++мои классы++++++++++++++++++
#include "ScrollBarClass.h"
#include "MenuControler.h"
#include "CubeMappingClass.h"
#include "KFModel.h"
#include "KFModelList.h"
//#include "KFClusterMap.h"
#include "KF2DTextureAnimation.h"
#include "KFShadowWork.h"

#include "RenderClass.h"
// -------------------------мои классы-------------------
#include <thread>
#include <process.h>

using namespace std;
/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 1.0f;
/*
union _Float34_Vector
{
	_Float34_Vector() {}; //MyData.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT3 Fl3;
	XMFLOAT4 Fl4;
	XMVECTOR Vec;
};
*/

// We'll need these four globals to start with.
////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:

	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	char* Global; // Нужно для отладки быстродействия движка

	float rotation ;
	float time;

	float TempVal1;
	float TempVal2;

	int ShadowFrameCounts;

	// Для отладки
	RCube_VecFloat34 CamLook;

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

	bool Initialize(HWND , int&, int&, int& , int&, InputClass* _Input );
//	void Shutdown();
	bool Frame(FPSTimers& , DXINPUTSTRUCT&);
	bool Render(int& mouseX, int& mouseY);

	void UpdateCamera();
	void UpdateConstantBuffer();	// Обновить константный буфер для шейдеров
	// Получаем координаты окна игры 
	// http://stackoverflow.com/questions/1950993/how-do-i-get-the-position-of-a-control-relative-to-the-windows-client-rect
	void GetWindowPos( HWND hWnd, int &x, int &y );

	void SnowThread( FPSTimers& );
	// Возвращает на глобальный массив переменных движка
	// Нужно для инициализации CUDA в PhysX в EngineKernal
	D3DGlobalContext *GraphicsClass::GetD3DGC();

//    KFButtons_OutPut ButtonsOut[5] ;
	ResourceManager * MyManager;
	KFModelList * ModelList;
	KFShadowWork * ShadowWork;

	RenderClass * RCubeRender;

//	FlatObjectClass *Picture;
private:
	HRESULT hr;

	HWND ParentWindow;
// The m_beginCheck variable is used to determine if the user has clicked on the screen or not.
	bool m_beginCheck;

public:
// And the second change is the new private pointer to the D3DClass which we have called m_D3D. In case you were wondering I use the prefix m_ on all class variables. 
// That way when I'm coding I can remember quickly what variables are members of the class and which are not.
//	vector < IDWriteTextFormat* > TextFormat;
	vector < FontOnTextureData* > FontList;

	vector <char*> Strings; // Для тестирования StringsList

				  D3DClass* m_D3D;
			   CameraClass* m_Camera;
	   
//	   ParticleSystemClass* m_ParticleSystem;
	   FireParticleSystem  *FireParticles;
	   TorchFireParticles  *TorchFire;
	   SnowFallParticles   *SnowParticles;

			RCubeProfiling* Profile;
			  FrustumClass* m_Frustum;
//				LightClass* m_Light;

// +++++++++++++++   Свой курсор мыши   ++++++++++++++
		 FlatObjectClass* m_Bitmap;
	  
		MenuControrerClass* MainMenu ;
	   MenuControrerClass * Hud;

// ++++++++++++   Ландшафт   +++++++++++++
				 Terrain* KFTerrain;
// +++++++++++++++++++  Текстовые классы для вывода текста на экран  +++++++++++++++++++++++
				 TextClass* m_Text;
// ----------------------------------------------------------------------------------------
// +++++++++++++++++++     Анимация     ++++++++++++++++++++++++++++++		
	  KF2DTextureAnimation* AnimTexture;
// -------------------     Анимация     ------------------------------
	//+++++++++++++++++++++++++++++++++++++++++++++++++++ то чо надо для меню
		RenderTextureClass* m_RenderTexture;
	//--------------------------------------------------- то чо надо для меню
			CubeMapping * Mapping ;

	int Exp;

	FPSTimers fpstimers;
	bool IsSphereStartRender ;

	int screenWidth;
	int screenHeight;
	int g_WindowPosX ;
	int g_WindowPosY ;

	bool IsSettingsMenuOn ;
	KFModel * Cube;
	KFModel * Sphere;
	KFModel * Scene;


//	KFClusterMap * ClusterMap;
};
#endif
