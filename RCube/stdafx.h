// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <DirectXMath.h>
#include <cmath>

using namespace DirectX;

union RCube_VecFloat34
{
	RCube_VecFloat34(){}; //MyData.Vec = { 0.0f, 0.0f, 0.0f, 1.0f };
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

#define KF_MAX_MODEL_ARR_SIZE 10000	// ������������ ���������� ������� �� �����

#define PVD_HOST "127.0.0.1"

#define MAX_OBJ 300	// ������������ ���������� ������� �� �����

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
