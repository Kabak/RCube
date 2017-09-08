#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


// The class constructor initializes the Direct Input interface variables to null.

InputClass::InputClass()
{
 m_directInput = nullptr;
	m_keyboard = nullptr;
	   m_mouse = nullptr;
	m_Joystick = nullptr;
   m_Joystick2 = nullptr;
	  XBoxCon1 = false;
	  XBoxCon2 = false;
	XBoxCon1ID = -1;
	XBoxCon2ID = -1;
	ActionKey.reserve(DEFAULT_KEYS_AMOUNT);

}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
	ActionKey.clear();
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int& screenWidth, int& screenHeight)
{
	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	wnd = hwnd;

	// Initialize the location of the mouse on the screen.
//	DxInputStruct.m_mouseX = 0;
///	DxInputStruct.m_mouseY = 0;
//	DxInputStruct.m_mouseZ = 0;

	ZeroMemory( &DxInputStruct, sizeof( DXINPUTSTRUCT ) );

// This function call will initialize the interface to Direct Input. Once you have a Direct Input object you can initialize other input devices.

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the input DirectInput8Create.", L"Error", MB_OK);
		return false;
	}

	InitKeyboard(hwnd);
	InitMouse(hwnd);

// xbox ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	ZeroMemory(&DxInputStruct.m_XboxControllState1, sizeof(XINPUT_STATE));
//	ZeroMemory(&DxInputStruct.m_XboxControllState2, sizeof(XINPUT_STATE));
	
	if (XInputGetState(0, &DxInputStruct.m_XboxControllState1) == ERROR_SUCCESS)
	{
 
		for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
		{
		if ( !XBoxCon1 )
			if (XInputGetState(i, &DxInputStruct.m_XboxControllState1) == ERROR_SUCCESS)
			{
				XBoxCon1 = true;
				XBoxCon1ID = i;

				result = m_directInput->CreateDevice(GUID_Joystick, &m_Joystick, NULL);
				if(FAILED(result))
				{
				MessageBox(hwnd, L"Could not initialize the input CreateDevice Joystick.", L"Error", MB_OK);
				return false;
				}

				result = m_Joystick->SetDataFormat(&c_dfDIJoystick);
				if(FAILED(result))
				{
				MessageBox(hwnd, L"Could not initialize the input SetDataFormat Joystick.", L"Error", MB_OK);
				return false;
				}
				result = m_Joystick->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ); //DISCL_FOREGROUND | DISCL_NONEXCLUSIVE 
				if(FAILED(result))
				{
					MessageBox(hwnd, L"Could not initialize the input mouse SetCooperativeLevel Joystick.", L"Error", MB_OK);
					return false;
				}
				do
				{
					result = m_Joystick->Acquire();
				}
				while (FAILED(result));

/*				if(FAILED(result))
				{
				MessageBox(hwnd, L"Could not initialize the input Acquire Joystick.", L"Error", MB_OK);
				return false;
				}
*/				
				goto SKEEP;
			}
		if ( !XBoxCon2 )
			if (XInputGetState(i, &DxInputStruct.m_XboxControllState2) == ERROR_SUCCESS)
			{
				XBoxCon2 = true;
				XBoxCon2ID = i;

				result = m_directInput->CreateDevice(GUID_Joystick, &m_Joystick2, NULL);
				if(FAILED(result))
				{
				MessageBox(hwnd, L"Could not initialize the input CreateDevice Joystick2.", L"Error", MB_OK);
				return false;
				}

				result = m_Joystick2->SetDataFormat(&c_dfDIJoystick);
				if(FAILED(result))
				{
				MessageBox(hwnd, L"Could not initialize the input SetDataFormat Joystick2.", L"Error", MB_OK);
				return false;
				}
				result = m_Joystick2->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ); //DISCL_FOREGROUND | DISCL_NONEXCLUSIVE 
				if(FAILED(result))
				{
					MessageBox(hwnd, L"Could not initialize the input mouse SetCooperativeLevel Joystick2.", L"Error", MB_OK);
					return false;
				}
				do
				{
					result = m_Joystick2->Acquire();
				}
				while (FAILED(result));

/*				if(FAILED(result))
				{
				MessageBox(hwnd, L"Could not initialize the input Acquire Joystick2.", L"Error", MB_OK);
				return false;
				}
*/			
			}
SKEEP:;
		}
	}

// ---------------------------------------------------------------------------

// назначаем клавиши по умолчанию
	ZeroMemory(&RVK, sizeof(RVK));
	BindDefaultKeys();

	return true;
}


bool InputClass::InitKeyboard(HWND hwnd)
{
	if ( m_keyboard )
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
	}

	// The first input device we will initialize will be the keyboard.

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the input CreateDevice.", L"Error", MB_OK);
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the input SetDataFormat.", L"Error", MB_OK);
		return false;
	}

	// Setting the cooperative level of the keyboard is important in both what it does and how you use the device from that point forward. In this case we will set it to not share with other programs (DISCL_EXCLUSIVE). This way if you press a key only your application can see that input and no other application will have access to it. However if you want other applications to have access to keyboard input while your program is running you can set it to non-exclusive (DISCL_NONEXCLUSIVE). Now the print screen key works again and other running applications can be controlled by the keyboard and so forth. Just remember that if it is non-exclusive and you are running in a windowed mode then you will need to check for when the device loses focus and when it re-gains that focus so it can re-acquire the device for use again. This focus loss generally happens when other windows become the main focus over your window or your window is minimized.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.idirectinputdevice8.idirectinputdevice8.setcooperativelevel%28v=vs.85%29.aspx
	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);//DISCL_FOREGROUND | DISCL_EXCLUSIVE
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the keyboard input SetCooperativeLevel.", L"Error", MB_OK);
		return false;
	}

	// Once they keyboard is setup we then call Acquire to finally get access to the keyboard for use from this point forward.
	do
	{
		result = m_keyboard->Acquire();
	} while (FAILED(result));

	return true;
}


bool InputClass::InitMouse(HWND hwnd)
{
	if ( m_mouse )
	{
		m_mouse->Unacquire();
		m_mouse->Release();
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the input CreateDevice Mouse.", L"Error", MB_OK);
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the input SetDataFormat Mouse.", L"Error", MB_OK);
		return false;
	}

	// We use non-exclusive cooperative settings for the mouse. We will have to check for when it goes in and out of focus and re-acquire it each time.

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); //DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the input mouse SetCooperativeLevel Mouse.", L"Error", MB_OK);
		return false;
	}

	// Once the mouse is setup we acquire it so that we can begin using it.

	// Acquire the mouse.
	do
	{
		result = m_mouse->Acquire();
	} while (FAILED(result));

	return true;
}


bool InputClass::InitJoystick1(HWND hwnd)
{

	return true;
}


bool InputClass::InitJoystick2(HWND hwnd)
{

	return true;
}


// The Shutdown function releases the two devices and the interface to Direct Input. Notice that the devices are always un-acquired first and then released.
void InputClass::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		RCUBE_RELEASE ( m_mouse );
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		RCUBE_RELEASE( m_keyboard );
	}
	if(m_Joystick)
	{
		m_Joystick->Unacquire();
		RCUBE_RELEASE( m_Joystick );
	}

	if(m_Joystick2)
	{
		m_Joystick2->Unacquire();
		RCUBE_RELEASE( m_Joystick2 );
	}
	// Release the main interface to direct input.
	if(m_directInput)
	{
		RCUBE_RELEASE( m_directInput );
	}

	return;
}

// The Frame function for the InputClass will read the current state of the devices into state buffers we setup. After the state of each device is read it then processes the changes.

bool InputClass::Frame()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// Read XBoxController 1
	if ( XBoxCon1 )
	{
	result = ReadXBoxController1();
		if (!result)
		{
			return false;
		}
	}
	// Read XBoxController 2
	if ( XBoxCon2 )
	{
	result = ReadXBoxController2();
		if (!result)
		{
			return false;
		}
	}
	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

// ReadKeyboard will read the state of the keyboard into the m_keyboardState variable. The state will show any keys that are currently pressed or not pressed. If it fails reading 
// the keyboard then it can be for one of five different reasons. The only two that we want to recover from are if the focus is lost or if it becomes un-acquired. If this is the case 
// we call acquire each frame until we do get control back. The window may be minimized in which case Acquire will fail, but once the window comes to the foreground again then Acquire 
// will succeed and we will be able to read the keyboard state. The other three error types I don't want to recover from in this tutorial.

bool InputClass::ReadKeyboard()
{
	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(DxInputStruct.m_keyboardState), (LPVOID)&DxInputStruct.m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
		
	return true;
}

// ReadMouse will read the state of the mouse similar to how ReadKeyboard read in the state of the keyboard. However the state of the mouse is just changes in the position of the mouse 
// from the last frame. So for example updates will look like the mouse has moved 5 units to the right, but it will not give you the actual position of the mouse on the screen. 
// This delta information is very useful for different purposes and we can maintain the position of the mouse on the screen ourselves.

bool InputClass::ReadMouse()
{
	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&DxInputStruct.m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	// получаем координаты мышки на экране
	POINT MouseNowPos ;
	GetCursorPos(&MouseNowPos);

//	DxInputStruct.m_mouseX = MouseNowPos.x;
//	DxInputStruct.m_mouseY = MouseNowPos.y;
	
	// ѕереводим координаты мышки в наше D3D окно
	if (ScreenToClient( wnd, &MouseNowPos ))
	{
		DxInputStruct.MousePos = MouseNowPos;
		DxInputStruct.m_mouseX = (int)MouseNowPos.x;
		DxInputStruct.m_mouseY = (int)MouseNowPos.y;
	}

	return true;
}


bool InputClass::ReadXBoxController1()
{
	XInputGetState(XBoxCon1ID, &DxInputStruct.m_XboxControllState1);

	return true;
}


bool InputClass::ReadXBoxController2()
{
	XInputGetState(XBoxCon2ID, &DxInputStruct.m_XboxControllState2);

	return true;
}

// The ProcessInput function is where we deal with the changes that have happened in the input devices since the last frame. For this tutorial we will just do a simple mouse location update similar to how Windows keeps track of where the mouse cursor is. To do so we use the m_mouseX and m_mouseY variables that were initialized to zero and simply add the changes in the mouse position to these two variables. This will maintain the position of the mouse based on the user moving the mouse around.
// Note that we do check to make sure the mouse location never goes off the screen. Even if the user keeps moving the mouse to the left we will just keep the cursor at the zero position until they start moving it to the right again.

void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	DxInputStruct.m_mouseX += DxInputStruct.m_mouseDX = DxInputStruct.m_mouseState.lX;
	DxInputStruct.m_mouseY += DxInputStruct.m_mouseDY = DxInputStruct.m_mouseState.lY;
	DxInputStruct.m_mouseZ += DxInputStruct.m_mouseDZ = DxInputStruct.m_mouseState.lZ;

	MapInputToVK();
// провер€ем нажата ли хоть одна кнопка из опредлЄнных в игре
	int size = (int)ActionKey.size();
	for ( int i = 0; i < size ; ++i )
	{
		RcubeVKAction* Keys = &ActionKey[i];
		if ( RVK[Keys->ButtonActionCode] & 0x80 || RVK[Keys->ButtonActionCode2] & 0x80)
//		if ( RVK[ActionKey[i].ButtonActionCode] & 0x80 || RVK[ActionKey[i].ButtonActionCode2] & 0x80 )
			Keys->Pressed = true;
		else
			Keys->Pressed = false;

	}
	// Ensure the mouse location doesn't exceed the screen width or height.
//	if(DxInputStruct.m_mouseX < 0)  { DxInputStruct.m_mouseX = 0; }
//	if(DxInputStruct.m_mouseY < 0)  { DxInputStruct.m_mouseY = 0; }
	
//	if(DxInputStruct.m_mouseX > m_screenWidth)  { DxInputStruct.m_mouseX = m_screenWidth; }
//	if(DxInputStruct.m_mouseY > m_screenHeight) { DxInputStruct.m_mouseY = m_screenHeight; }
	
	return;
}


int InputClass::GetPressedKey ()
{
	Frame();

	for ( int i = 0; i < 310 ; ++i ) // MAX_VK_KEYMAP - 20 ѕотому что у нас только 310 кнопок  
	{
		if ( RVK[i] & 0x80 )
			return i;
	}
	return -1;
}


void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = DxInputStruct.m_mouseX;
	mouseY = DxInputStruct.m_mouseY;
	return;
}

void InputClass::GetMouseDelta(int& mouseDX, int& mouseDY, int& mouseDZ)
{
	mouseDX = DxInputStruct.m_mouseState.lX;
	mouseDY = DxInputStruct.m_mouseState.lY;
	mouseDZ = DxInputStruct.m_mouseState.lZ;
	return;
}

void InputClass::GetMouseState(DIMOUSESTATE2& mouse_state)
{
	mouse_state = DxInputStruct.m_mouseState;
}

void InputClass::GetDXInput(DXINPUTSTRUCT& inputStruct)
{
inputStruct = DxInputStruct;	
}


// https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.idirectinputdevice8.idirectinputdevice8.getdevicestate%28v=vs.85%29.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.xinput_gamepad%28v=vs.85%29.aspx

// запихиваем прочитанные нажати€ в виртуальный массив кнопок
void InputClass::MapInputToVK()
{
//		ZeroMemory(&RVK, sizeof(RVK));

	UCHAR* Source = &DxInputStruct.m_keyboardState[0];
	UCHAR* Dest = &RVK[0];

	for ( int i = 0 ; i < 256 ; ++i ) // 144 - количество кнопок в определении DIK_xxx
	{
		if ( *Source & 0x80 )
			 *Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
		++Source;
	}

	 Source = &DxInputStruct.m_mouseState.rgbButtons[0];
//	UINT k = 256; // 256 - 263 = mouse buttons
	for ( int i = 0 ; i < 8 ; ++i )
	{
		if ( *Source & 0x80 )
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
		++Source;
	}

	if ( XBoxCon1 )
	{

//	UINT k = 264; // 264 + 32 = 1-st joystick
	for ( int i = 0 ; i < 14 ; ++i )
	{
		if ( DxInputStruct.m_XboxControllState1.Gamepad.wButtons == XButtons[i] )
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
	}
// Right Triger
		if (DxInputStruct.m_XboxControllState1.Gamepad.bLeftTrigger > MAX_TRIGER)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Triger
		if (DxInputStruct.m_XboxControllState1.Gamepad.bRightTrigger > MAX_TRIGER)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick Up
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLY > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick Down
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLY < MIN_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick Right
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLX > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick Left
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLX < MIN_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick UpLeft
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLY > MAX_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbLX < MIN_STICK )
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick UpRight
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLY > MAX_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbLX > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick DownRight
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLY < MIN_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbLX > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Left Stick DownLeft
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbLY < MIN_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbLX < MIN_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick Up
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRY > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick Down
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRY < MIN_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick Right
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRX > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick Left
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRX < MIN_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick UpLeft
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRY > MAX_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbRX < MIN_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick UpRight
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRY > MAX_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbRX > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick DownRight
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRY < MIN_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbRX > MAX_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
// Right Stick DownLeft
		if (DxInputStruct.m_XboxControllState1.Gamepad.sThumbRY < MIN_STICK &&
			DxInputStruct.m_XboxControllState1.Gamepad.sThumbRX < MIN_STICK)
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;

	}
	if ( XBoxCon2 )
	{
	UINT k = 296; // 296 + 32 = 2-st joystick
	for ( UINT i = 0 ; i < 14 ; i++ )
	{
		if ( DxInputStruct.m_XboxControllState2.Gamepad.wButtons == XButtons[i] )
			*Dest = 0x80;
		else
			*Dest = 0x00;
		++Dest;
	}
	// Right Triger
	if (DxInputStruct.m_XboxControllState2.Gamepad.bLeftTrigger > MAX_TRIGER)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Triger
	if (DxInputStruct.m_XboxControllState2.Gamepad.bRightTrigger > MAX_TRIGER)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick Up
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLY > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick Down
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLY < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick Right
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLX > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick Left
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLX < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick UpLeft
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLY > MAX_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbLX < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick UpRight
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLY > MAX_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbLX > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick DownRight
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLY < MIN_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbLX > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Left Stick DownLeft
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbLY < MIN_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbLX < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick Up
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRY > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick Down
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRY < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick Right
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRX > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick Left
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRX < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick UpLeft
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRY > MAX_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbRX < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick UpRight
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRY > MAX_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbRX > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick DownRight
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRY < MIN_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbRX > MAX_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	// Right Stick DownLeft
	if (DxInputStruct.m_XboxControllState2.Gamepad.sThumbRY < MIN_STICK &&
		DxInputStruct.m_XboxControllState2.Gamepad.sThumbRX < MIN_STICK)
		*Dest = 0x80;
	else
		*Dest = 0x00;
	++Dest;
	}

}

// начальна€ установка виртуальных клавиш к действи€м игрока
void InputClass::BindDefaultKeys()
{
	RcubeVKAction Action;

		ActionKey.clear();

		for ( UINT i = 0; i < DEFAULT_KEYS_AMOUNT; ++i )
		{
			Action.ButtonActionCode  = DefaultButtons[i];
			Action.ButtonActionCode2 = DefaultButtonsSet2[i];//0xff;
			strcpy_s(Action.ButtonKeyName,MAX_BUTTON_STR_SIZE,ButtonsNames[Action.ButtonActionCode]);
			strcpy_s(Action.ButtonKeyName2,MAX_BUTTON_STR_SIZE,ButtonsNames[Action.ButtonActionCode2]);
			Action.Pressed = false;
			ActionKey.emplace_back( Action );
		}
}

// определение виртуальных клавиш к действи€м игрока
void InputClass::BindKey(UINT Index, UINT buttonActionCode, bool SecondSlot )
{
	UINT Size = ( UINT ) ActionKey.size ();

	if ( Index < Size )
	if ( SecondSlot )
	{
		ActionKey[Index].ButtonActionCode2 = buttonActionCode;
		strcpy_s(ActionKey[Index].ButtonKeyName2,MAX_BUTTON_STR_SIZE,ButtonsNames[buttonActionCode]);
	}
	else
	{
		ActionKey[Index].ButtonActionCode = buttonActionCode;
		strcpy_s(ActionKey[Index].ButtonKeyName,MAX_BUTTON_STR_SIZE,ButtonsNames[buttonActionCode]);
	}
}


char* InputClass::GetVKeyText( UINT Index, bool SecondSlot )
{
	UINT Size = (UINT)ActionKey.size();
	
	if ( Index < Size )
	if ( SecondSlot )
	{
		return ActionKey[Index].ButtonKeyName2;
	}
	else
	{
		return ActionKey[Index].ButtonKeyName;
	}

	return "";
}


// http://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
bool InputClass::GetPressedKey( KeyPressed&, int AsType )
{
	bool Found = false;
	KeyPressed KeyPressed;

	int KeyCode = 0;

	UCHAR* Source = &RVK[0];

	switch (AsType)
	{
		case 0:
			
			for (int i = 0; i < 328; ++i) 
			{
				if ( *Source & 0x80 )
				{
					// ≈сли кнопка имеет код как ASCII, то возвращаем еЄ ASCII код
					if ( i < 256 )
						KeyPressed.ASCII = char( i );
					else 
						KeyPressed.ASCII = 0;

					KeyPressed.Name = ButtonsNames[i];
					
					Found = true;
					break;
				}
				
				++Source;
			}
			break;

		case 1:
			KeyCode = _getch();
/*
			Source = &RVK[2];
			for (int i = 2; i < 12; ++i)
			{
				if (*Source & 0x80)
				{
					KeyPressed.ASCII = char( i );
					KeyPressed.Name = ButtonsNames[i];

					Found = true;
					break;
				}

				++Source;
			}
*/
			break;

		case 2:

			break;

		case 3:

		default:;
	}
	return false;
}

