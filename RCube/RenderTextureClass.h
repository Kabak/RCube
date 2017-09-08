#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
//#include <d3dx11tex.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

	//++++++++++++++++++++++++++++++++++++++++++++ функции для включения - выключения Z буфера
	void TurnZBufferOn(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView) ;
	void TurnZBufferOff(ID3D11DeviceContext* deviceContext) ;
	//------------------------------------------функции для включения - выключения Z буфера
private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
};

#endif