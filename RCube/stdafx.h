// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <DirectXMath.h>
#include <cmath>

//RCube_DX11, RCube_DX12
#define RCube_DX11

//RCube_English, RCube_Russian
#define RCube_English

/*
// https://msdn.microsoft.com/en-us/library/hh973459.aspx
// Windows Runtime Library for managing COM objects without pain
#include <wrl.h>

using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
*/

using namespace DirectX;

struct RCube_XY_POS
{
	RCube_XY_POS () {};
	int X;
	int Y;
};

union RCube_VecFloat234
{
	RCube_VecFloat234(){}; //MyData.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT2 Fl2;
	XMFLOAT3 Fl3;
	XMFLOAT4 Fl4;
	XMVECTOR Vec;
};


union RCubeMatrix
{
	RCubeMatrix(){};
	XMMATRIX XMM;
	XMFLOAT4X4A XMF;
};


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define	RCUBE_RELEASE(x)	if( x != nullptr ){ x->Release(); x = nullptr; }
#define	RCUBE_SHUTDOWN(x)	if( x != nullptr ){ x->Shutdown(); delete x; x = nullptr; }
#define	RCUBE_DELETE(x)		if( x != nullptr ){ delete x; x = nullptr; }
#define	RCUBE_ARR_DELETE(x)	if( x != nullptr ){ delete [] x; x = nullptr; }
#define	PXSAFE_RELEASE(x)	if( x != nullptr ){ x->release(); x = nullptr; }

#define RCUBE_VECTOR_DELETE(x) { size_t c = 0; while ( c < x.size() ){ RCUBE_DELETE ( x[c] ); ++c; } x.clear (); }
#define RCUBE_VECTOR_RELEASE(x) { size_t c = 0; while ( c < x.size() ){ RCUBE_RELEASE ( x[c] ); ++c; } x.clear (); }
#define RCUBE_VECTOR_ARR_DELETE(x) { size_t c = 0; while ( c < x.size() ){ RCUBE_ARR_DELETE ( x[c] ); ++c; } x.clear (); }


#define KF_MAX_MODEL_ARR_SIZE 10000	// Максимальное количество моделей на сцене

#define PVD_HOST "127.0.0.1"

#define MAX_OBJ 300	// Максимальное количество кубиков на сцене

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
