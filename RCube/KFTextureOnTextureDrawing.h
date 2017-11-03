#pragma once
#include "stdafx.h"

#include <Windows.h>
#include "FlatObjectClass.h"
#include <D2D1.h>
#include <dwrite.h>
#include "D3Dcompiler.h"
#include "FlatObjectClass.h"
#include "D3DGlobalContext.h"

#ifndef _KFTEXTUREONTEXTURE_H_
#define _KFTEXTUREONTEXTURE_H

class KFTextureOnTextureDrawing{
private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView,* g_ActiveRenderTargetView;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ID3D11ShaderResourceView* m_shaderResourceView;
	FlatObjectClass * osnBackGroun;
	FlatObjectClass * Obj;
	ID3D11DeviceContext * g_deviceContext;
	void DrawTextres();
	bool IsSomeStingreset;
	ID3D11DepthStencilView * m_ActiveDepthStensil;
	std::vector<FlatObjectClass*> ObjsArray;
	HWND g_hwnd;
	int g_ScreenWidth, g_ScreenHeigth;
	int g_ScreenX, g_ScreenY;
	int OsnTextureWidth, OsnTextureHeigth;
	ID3D11Device * g_device;
	ID3D11DeviceContext * g_DevCon;
public:

	KFTextureOnTextureDrawing();
	~KFTextureOnTextureDrawing();
	HRESULT Init(HWND hwnd, D3DGlobalContext* D3DGC, UINT StTextureWidth, UINT StTextureHeigth, int screenPosX, int ScreenPosY, ID3D11ShaderResourceView * BackGroundTexture,
		ID3D11RenderTargetView * ActiveRenderTargetView, float PosX, float PosY , 
		ID3D11DepthStencilView * ActiveDepthStensil );
	void Draw();
	void AddTexture(D3DGlobalContext* D3DGC, float TexPosX, float TexPosY, float texWidth, float texHeigth
		, ID3D11ShaderResourceView * Texture);

};

#endif