#pragma once
#include "stdafx.h"

#ifdef RCube_DX11
#include "DX11Buffers.h"
#endif 

#ifndef _TEXTURES_H_
#define _TEXTURES_H_

enum TextureTypes
{
	_1D = 1,
	_2D,
	_3D
};


struct TextureData
{
	TextureData () : 
		Type ( 2 ),
		Format ( DXGI_FORMAT_R8G8B8A8_UNORM ),
		Usage ( D3D11_USAGE_DEFAULT ),
		MipMapLevels (1),
		ArraySize (1),
		ShaderResource (true)
		{};
	int Type;	// 1D 2D 3D
	UINT Format;// DXGI_FORMAT
	UINT Width;	// ������
	UINT Height;// ������
	UINT Usage; // ������ CPU
	UINT MipMapLevels;
	UINT ArraySize;
	DXGI_SAMPLE_DESC SampleDesc;
	bool ShaderResource;
	bool RenderTarget;
	bool DepthStensil;
	bool Unordered_Access;
	bool TextureCube;
	bool Shared_KeyMutex;
	bool GenerateMipMaps;
};


class Texture
{

public:
	Texture ();
   ~Texture ();

	ID3D11ShaderResourceView* SRV;		// ShaderResource
	ID3D11DepthStencilView* DSV;		// DepthStensilView
	ID3D11UnorderedAccessView* UAV;		// UnorderedAccessView
	ID3D11RenderTargetView*	RTV;		// RenderTirgetView
	ID3D11Resource*			 Resource;	// Texture
	ID3D11Texture1D*		 Texture1D;
	ID3D11Texture2D*		 Texture2D;
	ID3D11Texture3D*		 Texture3D;
	WCHAR* Name;// ��� �������� / ����� �� �������� ������� ��������
	int Index;	// ����� � ������� ��������� �������
	int Type;	// 1D 2D 3D
	UINT Format;// DXGI_FORMAT
	UINT Width;	// ������
	UINT Height;// ������
	UINT Usage; // ������ CPU
//	DXGI_SAMPLE_DESC SampleDesc;
	bool ShaderResource;
	bool RenderTarget;
	bool DepthStensil;
	bool Unordered_Access;
	bool TextureCube;
	bool Shared_KeyMutex;
	
};
#endif