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
#include "dinput.h"
#include "FrustumClass.h"
#include "RenderTextureClass.h"
#include "Light_def.h"
// ++++++++++++++++++++++++  PARTICLE SYSTEM  ++++++++++
#include "FireParticleSystem.h"
#include "SnowFallParticles.h"
#include "TorchFireParticles.h"
// ++++++++++++   �������� �������������� ����   +++++++++++++
#include "RCubeProfiling.h"

// +++++++++++++++++++++++++��� ������++++++++++++++++++
#include "ScrollBarClass.h"
#include "MenuControler.h"
#include "KF2DTextureAnimation.h"

#include "RenderClass.h"
// -------------------------��� ������-------------------
#include <thread>
#include <process.h>

using namespace std;
/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 5000.0f;
const float SCREEN_NEAR = 1.0f;
const float MATRIX_SCALING = SCREEN_DEPTH * 2;


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

	char* Global; // ����� ��� ������� �������������� ������
	char* Str1;

	float rotation ;
	float time;

	float TempVal1;
	float TempVal2;

	int ShadowFrameCounts;

	int CubeMapIndex;
	// ��� �������
	RCube_VecFloat34 CamLook;

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

	bool Initialize(HWND , int&, int&, int& , int&, InputClass* _Input, TimerClass* _Timer);
//	void Shutdown();
	bool Frame(FPSTimers& , DXINPUTSTRUCT&);
	bool Render(int& mouseX, int& mouseY);

	void UpdateCamera();
	void UpdateConstantBuffer();	// �������� ����������� ����� ��� ��������
	// �������� ���������� ���� ���� 
	// http://stackoverflow.com/questions/1950993/how-do-i-get-the-position-of-a-control-relative-to-the-windows-client-rect
	void GetWindowPos( HWND hWnd, int &x, int &y );

	void SnowThread( FPSTimers& );
	// ���������� �� ���������� ������ ���������� ������
	// ����� ��� ������������� CUDA � PhysX � EngineKernal
	D3DGlobalContext *GraphicsClass::GetD3DGC();

	ResourceManager * MyManager;

	RenderClass * RCubeRender;

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

	vector <char*> Strings; // ��� ������������ StringsList

				  D3DClass* m_D3D;
			   CameraClass* m_Camera;
			   
			   FontClass* TempFont;

//	   ParticleSystemClass* m_ParticleSystem;
	   FireParticleSystem  *FireParticles;
	   TorchFireParticles  *TorchFire;
	   SnowFallParticles   *SnowParticles;

			RCubeProfiling* Profile;
			  FrustumClass* m_Frustum;
//				LightClass* m_Light;

// +++++++++++++++   ���� ������ ����   ++++++++++++++
		 FlatObjectClass* m_Bitmap;
	  
		MenuControrerClass* MainMenu ;
	   MenuControrerClass * Hud;

// ++++++++++++   ��������   +++++++++++++
//				 KFTerrain* KFTerrain1;

// +++++++++++++++++++     ��������     ++++++++++++++++++++++++++++++		
	  KF2DTextureAnimation* AnimTexture;
// -------------------     ��������     ------------------------------
	//+++++++++++++++++++++++++++++++++++++++++++++++++++ �� �� ���� ��� ����
		RenderTextureClass* m_RenderTexture;
	//--------------------------------------------------- �� �� ���� ��� ����

	int Exp;

	FPSTimers fpstimers;
	bool IsSphereStartRender ;

	int screenWidth;
	int screenHeight;
	int g_WindowPosX ;
	int g_WindowPosY ;

	bool IsSettingsMenuOn ;

};
#endif
