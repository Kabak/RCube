#pragma once
#include "stdafx.h"

#include "resource.h"



#ifdef RCube_DX11

#pragma comment(lib,"D3D11.lib")

#endif // RCube_DX11


#ifdef RCube_DX12
// Link necessary d3d12 libraries.

#pragma comment(lib, “D3D12.lib”)

#endif // RCube_DX12


// Required always
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3DCompiler.lib")


#pragma comment(lib,"D3D10.lib")
#pragma comment(lib,"D3D10_1.lib")
// Direct Write
#pragma comment(lib,"D2D1.lib")
#pragma comment(lib,"DWrite.lib")
#pragma comment(lib,"dxguid.lib")
// DXInput
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"Xinput9_1_0.lib")

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
