#include "stdafx.h"

#ifndef _TEXTFONTSCLASS_H_
#define _TEXTFONTSCLASS_H_

// Text
#include "D3DGlobalContext.h"
#include <D3D10_1.h>
#include "D3D11.h"
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <string>
#include <dwrite.h>
#include "DirectXMath.h"
#include "fontclass.h"
#include "fontshaderclass.h"
#include "D3Dcompiler.h"
//#include <stdlib.h>

using namespace DirectX;

class TextFonts {

public:
	TextFonts::TextFonts();
	TextFonts::~TextFonts();

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

bool Initialize(HWND hwnd, D3DGlobalContext* D3DGC,wchar_t *FontName,UINT FontSize,XMVECTOR &color);
bool InitD2D_D3D101_DWrite(HWND hwnd, IDXGIAdapter1 *Adapter, UINT width, UINT height, ID3D11Device *g_pd3dDevice, wchar_t *FontName, UINT FontSize, XMFLOAT4A &FontColor);
bool InitD2DScreenTexture(ID3D11Device *g_pd3dDevice);

bool ChangeFontAtr(wchar_t *FontName,UINT FontSize);
bool ChangeDrawPos(float x, float y, float z);

void RenderInt(wchar_t *text, int Number);
void RenderFloat(wchar_t *text, float Number);
void RenderXMVector(wchar_t *text, XMVECTOR& Vector);
void RenderXMatrix(wchar_t *text, XMMATRIX& Matrix);
void RenderText(wchar_t &text);
void SetRenderDimantions(UINT Wigth, UINT Height);
void Shutdown();

private:

	HRESULT hr;

	XMMATRIX WVP;
	
	int ScreenWidth;
	int ScreenHeight;

	UINT stride;
	UINT offset;
	UINT FontSize;

struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x,y,z), texCoord(u, v){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

XMFLOAT4A FontDrawColor;


D3D11_INPUT_ELEMENT_DESC layout[2];
UINT numElements;

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
};

cbPerObject cbPerObj;

ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;

ID3D11InputLayout* g_pTextVertexLayout;

ID3D11SamplerState* g_pSampleType;

ID3D10Device1 *d3d101Device;	

IDXGIKeyedMutex *keyedMutex11;
IDXGIKeyedMutex *keyedMutex10;	

ID2D1RenderTarget *D2DRenderTarget;	

ID2D1SolidColorBrush *Brush;

ID3D11DeviceContext* m_ActiveDeviceContext;

ID3D11BlendState* Transparency;

ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;

ID3D11Texture2D *sharedTex11;	

ID3D11Buffer *d2dVertBuffer;
ID3D11Buffer *d2dIndexBuffer;

ID3D11Buffer* cbPerObjectBuffer;

ID3D11ShaderResourceView *d2dTexture;

IDWriteFactory *DWriteFactory;

IDWriteTextFormat *TextFormat;

wchar_t *printText;
wchar_t *Numbertext;
char *FloatNumber;
};

#endif