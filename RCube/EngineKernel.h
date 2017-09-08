#pragma once
#include "stdafx.h"

#ifndef _ENGINEKERNEL_H_
#define _ENGINEKERNEL_H_

// Here we define WIN32_LEAN_AND_MEAN. We do this to speed up the build process, it reduces the size of the Win32 header files by excluding some of the less used APIs.
///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

// Windows.h is included so that we can call the functions to create/destroy windows and be able to use the other useful win32 functions.
//////////////
// INCLUDES //
//////////////
#include <windows.h>

//We have included the headers to the other two classes in the frame work at this point so we can use them in the system class.
///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "InputClass.h"
#include "GraphicsClass.h"
#include "FpsClass.h"
#include "CpuClass.h"
#include "TimerClass.h"
#include "SoundClass.h"
#include "DxInputStruct.h"
#include "FPSTimers.h"
#include "KFPicking.h"
#include "RCubeNetwork.h"
#include "KF_PXControler.h"

#include <process.h>

using namespace std;
using namespace DirectX;


// The definition of the class is fairly simple. We see the Initialize, Shutdown, and Run function that was called in WinMain defined here. There are also some private functions that will be called inside those functions. We have also put a MessageHandler function in the class to handle the windows system messages that will get sent to the application while it is running. And finally we have some private variables m_Input and m_Graphics which will be pointers to the two objects that will handle graphics and input.
////////////////////////////////////////////////////////////////////////////////
// Class name: EngineKernel
////////////////////////////////////////////////////////////////////////////////
class EngineKernel
{
public:
	EngineKernel();
	EngineKernel(const EngineKernel&);
	~EngineKernel();

// Переопределяем операторы New и delete для правильного размещения XMMATRIX в памяти
    void* operator new(size_t i)
    {
        return _mm_malloc(i,16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	int screenWidth, screenHeight, WindowPosX , WindowPosY;

// Вращающиеся объекты движка для теста
	float Speed; // Скорость вращения объекта в об/мин
	float Speed2;// Скорость вращения объекта2 в об/мин
	float rot;
	float rot2;
	float MouseSpeed;
	float KeySpeed;

private:

	int MenPXIndex; // индекс человека в физике 
	PxQuat MenWorkPxQ; // кватернион человека в физике 
	PxVec3 MenPosition; // позиция человека
	XMVECTOR XMVecMenPosition; //  позиция чкловека в XMVectore

// Глобальный таймер прошедшего времени в секундах
// для каждого Frame
	LARGE_INTEGER StartCounter;
	LARGE_INTEGER StopCounter;
			float EngineFrameTime;

	float DZ;
	float x;
	float y;
	float z;//-20.0f;

	float CamX;
	float CamY;
	float CamZ;

	float SpecPower;
	float DifPower;
	float Ambi;
	// Используем для отладки теней в и шейдера
	float DiffuseX;
	float DiffuseY;
	float DepthBias;
	float SlopeScaledDepthBias;


	bool Start;

	bool done ;

	int AnyMenuWidth , AnyMenuHeigth ;
	bool Frame();
	void InitializeWindows(int& screenWidth, int& screenHeight , int& WindowPosX , int& WindowPosY);
	void ShutdownWindows();
// Выполняем дейтствия, если кнопки нажаты
	void KeysPressed();

	// Отображаются ли меню
	bool IsMenuOn;
//	bool IsOptionsOn;
	// фиксатор нажатия ESCAPE для меню
	bool IsEscapeDown;

//	bool SphereHit[25];
	std::vector<XMFLOAT3> bottleVertPosArray;
	std::vector<DWORD> bottleVertIndexArray ;

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	   InputClass* m_Input;
	GraphicsClass* m_Graphics;
	   SoundClass* m_Sound;
	     CpuClass* m_Cpu;
	   TimerClass* m_Timer;
	   KFPicking * Pic ;
	   RCubeNetwork* RNetWork;

// +++++  Системные переменные работы программы
		 FPSTimers Counters;
// Для передачи указателя на массив кодов нажатых клавиш в Graphics.cpp
	 DXINPUTSTRUCT InputStruct;
// -----  Системные переменные работы программы

//	++++++++++++++++++++++++++++++   PhysX Initialisation   ++++++++++++++++++++++++++++++++++++++++++
	 KF_PXControler * PxControl;
 //	------------------------------	 PhysX Initialisation   -------------------------------------------

};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static EngineKernel* ApplicationHandle = 0;


#endif
