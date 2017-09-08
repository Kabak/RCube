#include "stdafx.h"

#ifndef _KFTEXTUREMANEGER_H_
#define _KFTEXTUREMANEGER_H_

#include <Windows.h>
#include "SquareObjectClass.h"
#include <D2D1.h>
#include <dwrite.h>
#include "D3Dcompiler.h"

struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x,y,z), texCoord(u, v){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

struct cbPerObject
{
	XMMATRIX  WVP;
};

class KFTextureManeger{
public :
	KFTextureManeger() ;
	~KFTextureManeger() ;
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

	HRESULT Init(HWND hwnd, float TextureWidth, float TextureHaigth, ID3D11Device* device,
		float ScreenWidth, float ScreenHeigth, int ScreenPosX, int ScreenPosY, ID3D11DeviceContext* deviceContext
		, ID3D11RenderTargetView *ActiveRenderTargetView, ID3D11BlendState * ActiveBlendState
		, ID3D11ShaderResourceView * texture);

    void DrawTextureOnTextute(ID3D11ShaderResourceView * FirstTexture , ID3D11ShaderResourceView * SecondTexture , float SecondTexturePosX , float SecondTexturePosY , float Width , float Height, XMMATRIX & OrthoMatix); 
	void Render(float PosX , float PosY , XMMATRIX & OrhtoMatrix) ;

	void PreparationToAnimate(int NumVerse, int NumColumniation, ID3D11ShaderResourceView * animTexture);
	void DrawAnimate(int SpeedFrames, XMMATRIX & OrhtoMatrix);

	void PreparationToTextRendering(IDXGIAdapter1 *Adapter,wchar_t *FontName,UINT FontSize,XMFLOAT4 &color); 
	void DrawTextOnTexture(ID3D11ShaderResourceView * texture, float TextPosX, float TextPosY, wchar_t &Text, XMMATRIX & OrthoMatix);

	HRESULT SetTextureSize(float width , float height);

	ID3D11ShaderResourceView* GetTexture();

	void ResSetWindowParam(int ScreenWidth , int ScreenHeigth , int WindowPosX , int WindowPosY);
private:
	bool InitD2D_D3D101_DWrite(HWND hwnd, IDXGIAdapter1 *Adapter, UINT width, UINT height, ID3D11Device *g_pd3dDevice, wchar_t *FontName, UINT FontSize, XMFLOAT4A &FontColor);
	bool InitD2DScreenTexture(ID3D11Device *g_pd3dDevice);

	float g_ScereenWidth , g_ScreenHeight ;
	HWND g_hwnd ;
	// PreviosFirstTextureIndex и PreviosSecondTextureIndex нужны для провеки какие текстуры запихнуты в память помогает сэкономить время
	int  PreviosSecondTextureIndex , NumOfAnimeFrams ; 
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView , * g_ActiveRenderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	// BacroundTexture текстура заднего вида
	// OnTexture текстура переднего обьекта
	SquareObjectClass *BacroundTexture , *OnTexture , *DrawingTexture , *AnimeTextures ;
	ID3D11DeviceContext* g_deviceContext ;
	ID3D11Device* g_device ;
	InterfaceVertexType * VestexTypes ;
	float *TextcordTop , TextcordWidht , *TextcordLeft , TextcordHeight ;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	// тут все что я включил только для текста+++++++++++++++++++++++++++++++++++++++++++++++++
	IDXGIKeyedMutex *keyedMutex11;
    IDXGIKeyedMutex *keyedMutex10;	
	ID3D11Texture2D *sharedTex11;	
	ID2D1RenderTarget *D2DRenderTarget;	
	XMFLOAT4A FontDrawColor;
	ID2D1SolidColorBrush *Brush;
	IDWriteTextFormat *TextFormat;
	ID3D11BlendState* Transparency;
	ID3D11Buffer *d2dVertBuffer;
    ID3D11Buffer *d2dIndexBuffer;
	IDWriteFactory *DWriteFactory;
	UINT stride;
	UINT offset;
	ID3D11Buffer* cbPerObjectBuffer;
	ID3D11ShaderResourceView *d2dTexture;
	ID3D11InputLayout* g_pTextVertexLayout;
	cbPerObject cbPerObj;
	ID3D10Blob* VS_Buffer;
    ID3D10Blob* PS_Buffer;
    ID3D11VertexShader* VS;
    ID3D11PixelShader* PS;
	UINT numElements;
	ID3D11SamplerState* g_pSampleType;
	ID3D11RasterizerState* CCWcullMode;
    ID3D11RasterizerState* CWcullMode;
	ID3D10Device1 *d3d101Device;	
	bool TextInit ;
	// тут все что я включил только для текста!!!!??????----------------------------------------------------
	// хрени для альфа блендинга
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;
	ID3D11BlendState * g_BlendState ;
};

#endif