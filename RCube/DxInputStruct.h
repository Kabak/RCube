#pragma once
#include "stdafx.h"

#ifndef _DXINPUTSTRUCT_H_
#define _DXINPUTSTRUCT_H_

#include <dinput.h>
#include <Xinput.h>
#include <vector>

// https://msdn.microsoft.com/en-us/library/windows/desktop/ee418641%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ee418674%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.dimousestate%28v=vs.85%29.aspx
// https://katyscode.wordpress.com/2013/08/30/xinput-tutorial-part-1-adding-gamepad-support-to-your-windows-game/
// https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.idirectinputdevice8.idirectinputdevice8.getdevicestate%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinput_gamepad%28v=vs.85%29.aspx

// Максимальное значение Для джойстиков
#define MAX_STICK 30000
#define MIN_STICK -30000
// Максимальное значение для Тригеров
#define MAX_TRIGER 128
#define MIN_TRIGER 128
// Кнопки мышки сразу после клавиатуры с 256 по 263 включительно ( 8 кнопок )
#define MB1 256
#define MB2 257
#define MB3 258
#define MB4 259
#define MB5 260
#define MB6 261
#define MB7 262
#define MB8 263
// GamePad 1 сразу после мышки 264 - 296 включительно ( 32 кнопки )
#define GP1_DPAD_UP 264
#define GP1_DPAD_DOWN 265
#define GP1_DPAD_LEFT 266
#define GP1_DPAD_RIGHT 267
#define GP1_START 268
#define GP1_BACK 269
#define GP1_RIGHT_STICK 270
#define GP1_LEFT_STICK 271
#define GP1_LEFT_THUMB 272
#define GP1_RIGHT_THUMB 273
#define GP1_A 274
#define GP1_B 275
#define GP1_X 276
#define GP1_Y 277
#define GP1_LTP 278
#define GP1_RTP 279
#define GP1_LT_UP 280
#define GP1_LT_DOWN 281
#define GP1_LT_RIGHT 282
#define GP1_LT_LEFT 283
#define GP1_LT_ULEFT 284
#define GP1_LT_UPRIGHT 285
#define GP1_LT_DOWNRIGHT 286
#define GP1_LT_DOWNLEFT 287
#define GP1_RT_UP 288
#define GP1_RT_DOWN 289
#define GP1_RT_RIGHT 290
#define GP1_RT_LEFT 291
#define GP1_RT_UPLEFT 292
#define GP1_RT_UPRIGHT 293
#define GP1_RT_DOWNRIGHT 294
#define GP1_RT_DOWNLEFT 295


// Gamepad 2 сразу после GamePad 1  296 - 329 включительно ( 32 кнопки )
#define GP2_DPAD_UP 296
#define GP2_DPAD_DOWN 297
#define GP2_DPAD_LEFT 298
#define GP2_DPAD_RIGHT 299
#define GP2_START 300
#define GP2_BACK 301
#define GP2_RIGHT_STICK 302
#define GP2_LEFT_STICK 303
#define GP2_LEFT_THUMB 304
#define GP2_RIGHT_THUMB 305
#define GP2_A 306
#define GP2_B 307
#define GP2_X 308
#define GP2_Y 309
#define GP2_LTP 310
#define GP2_RTP 311
#define GP2_LT_UP 312
#define GP2_LT_DOWN 313
#define GP2_LT_RIGHT 314
#define GP2_LT_LEFT 315
#define GP2_LT_ULEFT 316
#define GP2_LT_UPRIGHT 317
#define GP2_LT_DOWNRIGHT 318
#define GP2_LT_DOWNLEFT 319
#define GP2_RT_UP 319
#define GP2_RT_DOWN 320
#define GP2_RT_RIGHT 321
#define GP2_RT_LEFT 322
#define GP2_RT_UPLEFT 323
#define GP2_RT_UPRIGHT 324
#define GP2_RT_DOWNRIGHT 325
#define GP2_RT_DOWNLEFT 326



// +++++++++++++++++++   Старое описание   ++++++++++++++++++++++++++++++++++
// Кнопки мышки сразу после клавиатуры с 144 по 151 включительно ( 8 кнопок )
// GamePad 1 сразу после мышки 152 - 184  ( 32 кнопки )
// Gamepad 2 сразу после GamePad 1  185 - 217  ( 32 кнопки )
// -------------------   Старое описание   ----------------------------------

struct KeyPressed
{
	char ASCII;
	const char* Name;
};

// максимальное количество виртуальных кнопок в движке RCUBE
const UINT MAX_VK_KEYMAP = 330;
const UINT MAX_BUTTON_STR_SIZE = 20;

using namespace std;
// Объявление глобальной структуры в которую сохраняются значения ввода с клавиатуры мыши и XBox360 Controllers
// для передачи в Graphics.h - обработка нажатых кнопок
struct DXINPUTSTRUCT{

		unsigned char m_keyboardState[256];

		DIMOUSESTATE2 m_mouseState;

		int m_mouseX;
		int m_mouseY;
		int m_mouseZ;	

		POINT MousePos;

		int m_mouseDX;
		int m_mouseDY;
		int m_mouseDZ;

		DIJOYSTATE m_joystick1;
		DIJOYSTATE m_joystick2;
		XINPUT_STATE m_XboxControllState1;
		XINPUT_STATE m_XboxControllState2;
		XINPUT_VIBRATION m_XboxControllVibra1;
		XINPUT_VIBRATION m_XboxControllVibra2;

//		DxInputStruct.m_XboxControllVibra1.wLeftMotorSpeed = 6500;
//		DxInputStruct.m_XboxControllVibra1.wRightMotorSpeed = 1000;
//		XInputSetState(0, &DxInputStruct.m_XboxControllVibra1);
//		(InputStruct.m_XboxControllState1.Gamepad.wButtons & XINPUT_GAMEPAD_START ) != 0
/*
https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinput_gamepad%28v=vs.85%29.aspx
// GAMEPAD ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if ( InputStruct.m_XboxControllState1.Gamepad.bRightTrigger != 0 )
{
		InputStruct.m_XboxControllVibra1.wLeftMotorSpeed = 6500;
		InputStruct.m_XboxControllVibra1.wRightMotorSpeed = InputStruct.m_XboxControllState1.Gamepad.bLeftTrigger * 254 ;
		XInputSetState(0, &InputStruct.m_XboxControllVibra1);
}
if ( (InputStruct.m_XboxControllState1.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) != 0 )
{
		InputStruct.m_XboxControllVibra1.wLeftMotorSpeed = 6500;
		InputStruct.m_XboxControllVibra1.wRightMotorSpeed = 1000;
		XInputSetState(0, &InputStruct.m_XboxControllVibra1);
}
else
{
		InputStruct.m_XboxControllVibra1.wLeftMotorSpeed = 0;
		InputStruct.m_XboxControllVibra1.wRightMotorSpeed = 0;
		XInputSetState(0, &InputStruct.m_XboxControllVibra1);
}
*/
};

struct RcubeVKAction
{
	UINT ButtonActionCode;
	UINT ButtonActionCode2;
	 char ButtonKeyName[MAX_BUTTON_STR_SIZE];
	 char ButtonKeyName2[MAX_BUTTON_STR_SIZE];
	 bool Pressed;
};
// Виртуальная карта нажатых кнопок на всех устройствах
static UCHAR RVK[ MAX_VK_KEYMAP ];

// Имена кнопок на мыши для отображения в меню переопределения кнопок/действий в игре
/*
static const char *MouseBNames[8] ={
"MB1"
"MB2"
"MB3"
"MB4"
"MB5"
"MB6"
"MB7"
"MB8"
};

// Имена кнопок на GamePad для отображения в меню переопределения кнопок/действий в игре
static const char *PadBNames[32] = {
"PAD_A",
"PAD_B",
"PAD_X",
"PAD_Y",
"PAD_RS",
"PAD_LS",
"PAD_LT",
"PAD_RT",

"PAD_UP",
"PAD_DOWN",
"PAD_LEFT",
"PAD_RIGHT",
"PAD_START",
"PAD_BACK",
"PAD_LTP",
"PAD_RTP",

"PAD_LT_UP",
"PAD_LT_DOWN",
"PAD_LT_RIGHT",
"PAD_LT_LEFT",
"PAD_LT_ULEFT",
"PAD_LT_UPRIGHT",
"PAD_LT_DOWNRIGHT",
"PAD_LT_DOWNLEFT",

"PAD_RT_UP",
"PAD_RT_DOWN",
"PAD_RT_RIGHT",
"PAD_RT_LEFT ",
"PAD_RT_UPLEFT",
"PAD_RT_UPRIGHT",
"PAD_RT_DOWNRIGHT", 
"PAD_RT_DOWNLEFT",
};
*/

// Таблица переменных кнопок мыши для быстрой проверки нажатий
static const UINT MouseButtons [8] = { 
	MB1,
	MB2,
	MB3,
	MB4,
	MB5,
	MB6,
	MB7,
	MB8 };

// Таблица переменных кнопок GamePad для быстрой проверки нажатий
static const UINT XButtons[14] = {
								XINPUT_GAMEPAD_DPAD_UP,
								XINPUT_GAMEPAD_DPAD_DOWN,
								XINPUT_GAMEPAD_DPAD_LEFT,
								XINPUT_GAMEPAD_DPAD_RIGHT,
								XINPUT_GAMEPAD_START,
								XINPUT_GAMEPAD_BACK,
								XINPUT_GAMEPAD_LEFT_THUMB,
								XINPUT_GAMEPAD_RIGHT_THUMB,
								XINPUT_GAMEPAD_LEFT_SHOULDER,
								XINPUT_GAMEPAD_RIGHT_SHOULDER,
								XINPUT_GAMEPAD_A,
								XINPUT_GAMEPAD_B,
								XINPUT_GAMEPAD_X,
								XINPUT_GAMEPAD_Y,
								};


static const char *ButtonsNames[] = {
"",
"ESC",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"0",
"-",
"=",
"BACK SPACE",
"TAB",
"Q",
"W",
"E",
"R",
"T",
"Y",
"U",
"I",
"O",
"P",
"{",
"}",
"ENTER",
"LEFT CTRL",
"A",
"S",
"D",
"F",
"G",
"H",
"J",
"K",
"L",
";",
"'",
"`",
"L-SHIFT",
"\\",
"Z",
"X",
"C",
"V",
"B",
"N",
"M",
",",
".",
"/",
"R-SHIFT",
"NUM *",
"L-ALT",
"SPACE",
"CAPSLOCK",
"F1",
"F2",
"F3",
"F4",
"F5",
"F6",
"F7",
"F8",
"F9",
"F10",
"NUMLOCK",
"SCROLL",
"NUMPAD7",
"NUMPAD8",
"NUMPAD9",
"NUM-SUB",
"NUMPAD4",
"NUMPAD5",
"NUMPAD6",
"NUM-ADD",
"NUMPAD1",
"NUMPAD2",
"NUMPAD3",
"NUMPAD0",
"NUM DEL","","",
"OEM_102",/* <> or \| on RT 102-key keyboard (Non-U.S.) */
"F11",
"F12","","","","","","","","","","","",
"F13",/*                     (NEC PC98) */
"F14",/*                     (NEC PC98) */
"F15",/*                     (NEC PC98) */
"","","","","","","","","",
"KANA","","",//                         0x81000470    /* (Japanese keyboard)            */
"ABNT_C1","","","","","",//             0x81000473    /* /? on Brazilian keyboard */
"CONVERT","",//                         0x81000479    /* (Japanese keyboard)            */
"NOCONVERT","",//						0x8100047B    /* (Japanese keyboard)            */
"YEN",//                                0x8100047D    /* (Japanese keyboard)            */
"ABNT_C2","","","","","","","","","",// 0x8100047E    /* Numpad . on Brazilian keyboard */
"","","","","",
"NUMPADEQUALS","","",//	                0x8100048D    /* = on numeric keypad (NEC PC98) */
"PREVTRACK",//							0x81000490    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
"AT",//		                            0x81000491    /*                     (NEC PC98) */
"COLON",//		                        0x81000492    /*                     (NEC PC98) */
"UNDERLINE",//		                    0x81000493    /*                     (NEC PC98) */
"KANJI",//		                        0x81000494    /* (Japanese keyboard)            */
"STOP",//		                        0x81000495    /*                     (NEC PC98) */
"AX",//		                            0x81000496    /*                     (Japan AX) */
"UNLABELED","",//	                    0x81000497    /*                        (J3100) */
"NEXTTRACK","","",//	                0x81000499    /* Next Track */
"NUMPADENTER",//		                0x8100049C    /* Enter on numeric keypad */
"R-CTRL","","",//	                    0x8100049D
"MUTE",//								0x810004A0    /* Mute */
"CALCULATOR",//		                    0x810004A1    /* Calculator */
"PLAYPAUSE","",//	                    0x810004A2    /* Play / Pause */
"MEDIASTOP","","","","","","","","","",//0x810004A4    /* Media Stop */
"VOLUMEDOWN","",//		                    0x810004AE    /* Volume - */
"VOLUMEUP","",//	                    0x810004B0    /* Volume + */
"WEBHOME",//		                    0x810004B2    /* Web home */
"NUMPADCOMMA","",//		                0x810004B3    /* , on numeric keypad (NEC PC98) */
"NUM DIV","",//                         0x810004B5    /* / on numeric keypad */
"SYSRQ",//		                        0x810004B7
"R-ALT","","","","","","","",//         0x810004B8    /* right Alt */
"","","","","",
"PAUSE","",//	                        0x810004C5    /* Pause */
"HOME",//		                        0x810004C7    /* Home on arrow keypad */
"UP",//		                            0x810004C8    /* UpArrow on arrow keypad */
"PRIOR","",//	                        0x810004C9    /* PgUp on arrow keypad */
"LEFT","",//	                        0x810004CB    /* LeftArrow on arrow keypad */
"RIGHT","",//	                        0x810004CD    /* RightArrow on arrow keypad */
"END",//		                        0x810004CF    /* End on arrow keypad */
"DOWN",//		                        0x810004D0    /* DownArrow on arrow keypad */
"NEXT",//		                        0x810004D1    /* PgDn on arrow keypad */
"INSERT",//		                        0x810004D2    /* Insert on arrow keypad */
"DELETE","","","","","","","",//        0x810004D3    /* Delete on arrow keypad */
"LWIN",//		                        0x810004DB    /* Left Windows key */
"RWIN",//		                        0x810004DC    /* Right Windows key */
"APPS",//		                        0x810004DD    /* AppMenu key */
"POWER",//		                        0x810004DE    /* System Power */
"SLEEP","","","",//                     0x810004DF    /* System Sleep */
"WAKE","",//	                        0x810004E3    /* System Wake */
"WEBSEARCH",//		                    0x810004E5    /* Web Search */
"WEBFAVORITES",//		                0x810004E6    /* Web Favorites */
"WEBREFRESH",//		                    0x810004E7    /* Web Refresh */
"WEBSTOP",//	                        0x810004E8    /* Web Stop */
"WEBFORWARD",//		                    0x810004E9    /* Web Forward */
"WEBBACK",//	                        0x810004EA    /* Web Back */
"MYCOMPUTER",//		                    0x810004EB    /* My Computer */
"MAIL",//		                        0x810004EC    /* Mail */
"MEDIASELECT",//		                0x810004ED    /* Media Select */
"","","","","","","","","","","","","","","","","","",
"MB1",//								0x00000100
"MB2",
"MB3",
"MB4",
"MB5",
"MB6",
"MB7",
"MB8",//								0x00000107

"PAD1_A",//					264			
"PAD1_B",
"PAD1_X",
"PAD1_Y",
"PAD1_RS",
"PAD1_LS",
"PAD1_LT",
"PAD1_RT",

"PAD1_UP",
"PAD1_DOWN",
"PAD1_LEFT",
"PAD1_RIGHT",
"PAD1_START",
"PAD1_BACK",
"PAD1_LTP",
"PAD1_RTP",

"PAD1_LT_UP",
"PAD1_LT_DOWN",
"PAD1_LT_RIGHT",
"PAD1_LT_LEFT",
"PAD1_LT_ULEFT",
"PAD1_LT_UPRIGHT",
"PAD1_LT_DOWNRIGHT",
"PAD1_LT_DOWNLEFT",

"PAD1_RT_UP",
"PAD1_RT_DOWN",
"PAD1_RT_RIGHT",
"PAD1_RT_LEFT ",
"PAD1_RT_UPLEFT",
"PAD1_RT_UPRIGHT",
"PAD1_RT_DOWNRIGHT", 
"PAD1_RT_DOWNLEFT",

"PAD2_A",//					296			
"PAD2_B",
"PAD2_X",
"PAD2_Y",
"PAD2_RS",
"PAD2_LS",
"PAD2_LT",
"PAD2_RT",

"PAD2_UP",
"PAD2_DOWN",
"PAD2_LEFT",
"PAD2_RIGHT",
"PAD2_START",
"PAD2_BACK",
"PAD2_LTP",
"PAD2_RTP",

"PAD2_LT_UP",
"PAD2_LT_DOWN",
"PAD2_LT_RIGHT",
"PAD2_LT_LEFT",
"PAD2_LT_ULEFT",
"PAD2_LT_UPRIGHT",
"PAD2_LT_DOWNRIGHT",
"PAD2_LT_DOWNLEFT",

"PAD2_RT_UP",
"PAD2_RT_DOWN",
"PAD2_RT_RIGHT",
"PAD2_RT_LEFT ",
"PAD2_RT_UPLEFT",
"PAD2_RT_UPRIGHT",
"PAD2_RT_DOWNRIGHT", 
"PAD2_RT_DOWNLEFT",
};

#endif