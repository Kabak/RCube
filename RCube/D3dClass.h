#pragma once
#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Filename: D3dClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

#include "D3DGlobalContext.h"
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include "DirectXMath.h"
#include <vector>

using namespace std;
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
   ~D3DClass();

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

	bool Initialize(HWND, int, int, bool, bool, float, float);
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();
	
	  D3DGlobalContext* D3DGC;

	//++++++++++++++++++++++++++++ моя функция для получения рендертаргета
ID3D11RenderTargetView* GetRenderTargetView();
	//----------------------------моя функция для получения рендертаргета

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();
	void SetDefaultResterizeState();
	void SetCullNoneRState();

	void BlurScene(ID3D11ComputeShader* Horizont, ID3D11ComputeShader* Vertical, int BlurAmont);

	void TurnZBufferOn();
	void TurnZBufferOff();

//We have two new functions for turning on and off alpha blending.

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();
	void TurnOnParticleBlending();
	void TurnOnTextOnTextureBlending();

	void GetDisplaySupportedModes( UINT resolution_min ); // resolution_min - минимальное разрешение на котором должна работать программа

	char* GetVideoCardString();

	bool SaveTextureToPNG( ID3D11ShaderResourceView* );


// Video Card data
	int m_videoCardMemory;
	char* m_videoCardDescription;
// FXAA фильтрация сцены
	void FXAAScene();

private:
	HRESULT hr, result;
	bool m_vsync_enabled;
	UINT ActiveAdapter;

	vector < IDXGIAdapter1* > Adapters;
	vector < Display_Mode*  > ActualDisplayModes;

		  IDXGIAdapter1* FoundAdapter1;

  ID3D11RasterizerState* m_rasterStateNone;
ID3D11DepthStencilState* m_depthDisabledStencilState;

	bool FxaaIntegrateResource(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc);
	
	bool InitD2D_D3D101_DWrite();
	
	void AdaptersRelease();
};

#endif
