#pragma once
#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

// You need to define the version of Direct Input you are using in the header or the compiler will generate annoying messages that it is defaulting to version 8.

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

// The following two libraries need to be linked for Direct Input to work.

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Xinput9_1_0.lib")

// This is the required header for Direct Input.
#define DEFAULT_KEYS_AMOUNT 27
//////////////
// INCLUDES //
//////////////
#include <conio.h>
#include <dinput.h>
#include <Xinput.h>
#include "DxInputStruct.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////

// Начальные кнопки набор 1
static const UINT DefaultButtons[DEFAULT_KEYS_AMOUNT] = { 
	DIK_Q, DIK_W, DIK_A, DIK_S, DIK_Z, DIK_X, DIK_R, DIK_E, 
	DIK_F, DIK_D, DIK_V, DIK_C, DIK_T, DIK_Y, DIK_G, DIK_H, 
	DIK_B, DIK_N, DIK_0, DIK_SUBTRACT, DIK_NUMPADPLUS, DIK_MULTIPLY, DIK_DIVIDE, DIK_ESCAPE,
	DIK_8, DIK_9, DIK_SPACE };

// Начальные кнопки набор 2
static const UINT DefaultButtonsSet2[DEFAULT_KEYS_AMOUNT] = { 
	GP1_BACK, GP1_RT_UP, GP1_RT_LEFT, GP1_RT_DOWN, GP1_RIGHT_STICK, GP1_LEFT_STICK, DIK_R, DIK_E, DIK_F,
	GP1_RT_RIGHT, DIK_V, DIK_C, DIK_T, DIK_Y, DIK_G, DIK_H, DIK_B, DIK_N,
	DIK_0, DIK_SUBTRACT, DIK_NUMPADPLUS, DIK_MULTIPLY, DIK_DIVIDE, GP1_START,
	DIK_8, DIK_9, DIK_SPACE };

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int& screenWidth, int& screenHeight);
	bool InitKeyboard(HWND hwnd);
	bool InitMouse(HWND hwnd);
	bool InitJoystick1(HWND hwnd);
	bool InitJoystick2(HWND hwnd);

	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);
	void GetMouseDelta(int& mouseX, int& mouseY, int& mouseZ);
	void GetMouseState(DIMOUSESTATE2& mouse_state);
	void GetDXInput(DXINPUTSTRUCT& inputStruct);

	vector < RcubeVKAction > ActionKey;

	// Получить код нажатой клавиши для переопределния кнопок в игре
	int GetPressedKey();
	// определение виртуальных клавиш к действиям игрока
	void BindKey(UINT Index, UINT ButtonActionCode, bool SecondSlot);

	// Получить текстовое обозначение привязанной виртуальной кнопки
	char* GetVKeyText( UINT Index, bool SecondSlot );

	DXINPUTSTRUCT DxInputStruct;

private:
	bool ReadKeyboard();
	bool ReadMouse();
	bool ReadXBoxController1();
	bool ReadXBoxController2();

	void ProcessInput();

	// начальная установка виртуальных клавиш к действиям игрока	
	void BindDefaultKeys();

	// запихиваем прочитанные нажатия в виртуальный массив кнопок
	void MapInputToVK();

	// Ищем ЛЮБУЮ, ПЕРВУЮ нажатую клавишу в списке виртуальных кнопок в соответствии с критериями поиска
	bool GetPressedKey( KeyPressed&, int AsType );

// The first three private member variables are the interfaces to Direct Input, the keyboard device, and the mouse device.

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	IDirectInputDevice8* m_Joystick;
	IDirectInputDevice8* m_Joystick2;

// The next two private member variables are used for recording the current state of the keyboard and mouse devices.

	HRESULT result;
	   HWND wnd;
		int m_screenWidth;
		int m_screenHeight;
// XBox 360 Controllers presense
	   bool XBoxCon1;
	   bool XBoxCon2;
// XBox 360 Controllers IDs
	    int XBoxCon1ID;
	    int XBoxCon2ID;
};

#endif

